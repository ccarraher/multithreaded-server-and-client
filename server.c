#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <linux/in.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#define INET_ADDRSTRLEN 16
#define MAX 256

typedef struct
{       
        int sock;
        struct sockaddr address;
        int addr_len;
} connection_t;
	
void * process(void * ptr)
{       
        char buffer[MAX];
        int len= 0;
        int numSpaces = 0;
        int numChars = 0;
        connection_t * conn;
        long addr = 0;
        int n = 0;
        int i = 0;
        int res = 0;
        pid_t tid = syscall(SYS_gettid);                                        // System call for thread id
        if (!ptr) pthread_exit(0); 
        conn = (connection_t *)ptr;
        struct sockaddr_in sa = {0};
        socklen_t sl = sizeof(sa); 
        if(getpeername(conn->sock, (struct sockaddr *)&sa, &sl))
                perror("getpeername() failed");
        else    
                printf("worker %u: established connection with client %s#%d\n", tid, inet_ntoa(sa.sin_addr), ntohs(sa.sin_port));
        while((n = recv(conn->sock, buffer, MAX, 0)) > 0) {
                for(i = 0; i < n; i++) {
                        if(buffer[i] == ' ') {
                                numSpaces++;
                        } else {
                                numChars++;
                        }
                }
                numSpaces++;                                                    // Number of Spaces plus one always equals the number of words unless the user enters nothing
                numChars--;                                                     // Not preferable, however I'm pretty sure numChars contains null character or something similar so -1 is correct
                printf("worker %u: received message from client. # words = %d and # characters = %d\n", tid, numSpaces, numChars);
                send(conn->sock, &numChars, sizeof(int), 0);
                send(conn->sock, &numSpaces, sizeof(int), 0);                   
                memset(&buffer, 0, sizeof(buffer));                             // Clear out the buffer
                numSpaces = 0;                                                  // Clear out spaces count
                numChars = 0;                                                   // Clear out character count
        }
        if(n < 0)
                printf("%s\n", "Read error");
        /* close socket and clean up */
        printf("worker %u: client terminated\n", tid);
        close(conn->sock);
        free(conn);
        pthread_exit(0);
}

int main(int argc, char ** argv)
{
        int sock = -1;
        struct sockaddr_in address;
        int port;
        connection_t * connection;
        pthread_t thread;

        /* check for command line arguments */
	if (argc != 2)
        {
                fprintf(stderr, "usage: %s port\n", argv[0]);
                return -1;
        }

        /* obtain port number */
        if (sscanf(argv[1], "%d", &port) <= 0)
        {
                fprintf(stderr, "%s: error: wrong parameter: port\n", argv[0]);
                return -2;
        }

        /* create socket */
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (sock <= 0)
        {
                fprintf(stderr, "%s: error: cannot create socket\n", argv[0]);
                return -3;
        }

        /* bind socket to port */
        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        if (bind(sock, (struct sockaddr *)&address, sizeof(struct sockaddr_in)) < 0)
        {
                fprintf(stderr, "%s: error: cannot bind socket to port %d\n", argv[0], port);
                return -4;
        }

        /* listen on port */
        if (listen(sock, 5) < 0)
        {
                fprintf(stderr, "%s: error: cannot listen on port\n", argv[0]);
                return -5;
        }

        printf("%s: ready and listening\n", argv[0]);
        while (1)
        {
                /* accept incoming connections */
                connection = (connection_t *)malloc(sizeof(connection_t));
                connection->sock = accept(sock, &connection->address, &connection->addr_len);
                if (connection->sock <= 0)
                {
                        free(connection);
                }
                else
                {
                        /* start a new thread but do not wait for it */
                        pthread_create(&thread, 0, process, (void *)connection);
                        pthread_detach(thread);
                }
        }

        return 0;
}
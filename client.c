#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define BUF 256

int main(int argc, char ** argv)
{
        int port;
        int sock = -1;
        struct sockaddr_in address;
        struct hostent * host;
        int len= 0;
        int n = 0;
        int ret = 0;
        char message[BUF];
        int numWords = 0;
        int numChars = 0;
	/* checking commandline parameter */
        if (argc != 3)
        {
                printf("usage: %s hostname port\n", argv[0]);
                return -1;
        }

        /* obtain port number */
        if (sscanf(argv[2], "%d", &port) <= 0)
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

        /* connect to server */
        address.sin_family = AF_INET;
        address.sin_port = htons(port);
        host = gethostbyname(argv[1]);
        if (!host)
        {
                fprintf(stderr, "%s: error: unknown host %s\n", argv[0], argv[1]);
                return -4;
        }
        memcpy(&address.sin_addr, host->h_addr_list[0], host->h_length);
        if (connect(sock, (struct sockaddr *)&address, sizeof(address)))
        {
                fprintf(stderr, "%s: error: cannot connect to host %s\n", argv[0], argv[1]);
                return -5;
        }
        char ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &address, ip, INET_ADDRSTRLEN);
        printf("client: established connection with: %s\n", ip);

        /* send text to server */
        while(fgets(message, BUF, stdin) != NULL) {
                send(sock, message, strlen(message), 0);
                if(recv(sock, &numWords, sizeof(int), 0) == 0) {
                        perror("The string terminated prematurely");
                        exit(4);
                }
                if(recv(sock, &numChars, sizeof(int), 0) == 0) {
                        perror("The string terminated prematurely");
                        exit(4);
                }
                printf("%d %d\n", numWords, numChars);
        }
        //printf("%d\n", numWords);
        /* close socket */
        close(sock);

        return 0;
}
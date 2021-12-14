# multithreaded-server-and-client
Multithreaded server using Socket API
Made for a linux environment
User enters a message and the server will echo the number of words and characters

# Running
gcc server.c -pthread -o server
gcc client.c -o client

./server <port>
./client <ip of server> <same port as server>
  

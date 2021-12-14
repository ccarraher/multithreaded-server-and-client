# multithreaded-server-and-client
Multithreaded server using Socket API
Made for a linux environment.\
User enters a message and the server will echo the number of words and characters back to the client.

# Running
`gcc server.c -pthread -o server`\
`gcc client.c -o client`

`./server <port>`\
`./client <ip of server> <same port as server>`
  

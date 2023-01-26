# Introduction
This project contains an udp server and a client written in c.

* The client supports **maxRetry** with default value 10 (times). (maxRetry)
* The client contains **timeout** with default value 1 (second). (tv.tv_sec)
* The interval between each retry follows the exponential backoff algorithm with 3 parameters below:
  * multiplier = 2
  * base = 500ms
  * max wait_interval = 8s


# How to run the code?
1. run the server with 2 parameters (server IP, port)  
```console
./server 127.0.0.1 2000
```

2. run the client with 3 parameters (server IP, port, messageFromClient)  
```console
./client 127.0.0.1 2000 'Hello from client!'
```

# Expected Result on Console
## Server Side
```console
Socket created successfully  
Done with binding  
Listening for incoming messages...  
  
Received message from IP: 127.0.0.1 and port: 58549  
Message from client: hello from client!
```

## Client Side
```console
Socket created successfully  
Send the message to server...  
Received message from IP: 127.0.0.1 and port: 2000  
Server's response: hello from client!
```
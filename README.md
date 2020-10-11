# AES-encryption-UDP-clientServer-C

To Run follow the instructions

Compilation:
1. Type the following command to compile Server file:
gcc gmult.c aes.c udpServer.c -lws2_32 -o server

2. Type the following command to compile Client file:
gcc gmult.c aes.c udpClient.c -lws2_32 -o client


Execution:
Step 1: Run the server file by using the command: server
Step 2: Run the client file by using the command: client <ip address>

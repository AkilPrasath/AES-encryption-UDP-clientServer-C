# AES-encryption-UDP-clientServer-C
This program encrypts (using AES) 128 byte data from a txt file and sends to the server where it will be decrypted and stored in a txt file.
#### Step 0:
Put your 128 byte text data into the ```dummy.txt``` file.

## To Run follow the instructions
### Compilation:
#### Step 1: 
Type the following command to compile Server file:
```gcc gmult.c aes.c udpServer.c -lws2_32 -o server```

#### Step 2:
Type the following command to compile Client file:
```gcc gmult.c aes.c udpClient.c -lws2_32 -o client```


### Execution:
#### Step 1: 
Run the server file by using the command: ```server```
#### Step 2:
Run the client file by using the command: ```client <ip address>```
Example: ```client 127.0.0.1```
### Output:
The decrypted text data will be stored in ```decrypt.txt``` file and will be opened automatically.

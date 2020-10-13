/* udpechocli01.c
   Example codes for COMP2650, modified from Stevens's UNPv1.
   Dr. Xiaowen Chu
   To complie: >cl udpechocli01.c ws2_32.lib
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <winsock2.h>
#include <stdint.h>
#include "aes.h"

#define SA struct sockaddr
#define MAXLINE 4096
#define SERV_PORT 4567

void errexit(const char *format, ...)
{
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    WSACleanup();
    exit(1);
}

int countChars(char array[])
{
    for (int i = 0; i < MAXLINE; i++)
    {
        if (array[i] == '\0')
        {
            return i;
        }
    }
}
void string2hexString(char *input, char *output)
{
    int loop;
    int i;

    i = 0;
    loop = 0;

    while (input[loop] != '\0')
    {
        sprintf((char *)(output + i), "%02X", input[loop]);
        loop += 1;
        i += 2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
}
void dg_cli(int sockfd, const SA *pservaddr, int servlen)
{
    int n;
    char sendline[MAXLINE], recvline[MAXLINE + 1];
    // char *sendline, recvline[MAXLINE + 1];
    uint8_t i;
    uint8_t key[] = {
        0x00, 0x01, 0x02, 0x03,
        0x04, 0x05, 0x06, 0x07,
        0x08, 0x09, 0x0a, 0x0b,
        0x0c, 0x0d, 0x0e, 0x0f,
        0x10, 0x11, 0x12, 0x13,
        0x14, 0x15, 0x16, 0x17,
        0x18, 0x19, 0x1a, 0x1b,
        0x1c, 0x1d, 0x1e, 0x1f};

    uint8_t out[16]; // 128
    uint8_t in[16];
    uint8_t *w; // expanded key

    w = aes_init(sizeof(key));
    aes_key_expansion(key, w);

    for (int i = 0; i < MAXLINE; i++)
    {
        sendline[i] = '\0';
    }
    printf("\nPress enter to start encryption..");
    while (fgets(sendline, MAXLINE, stdin) != NULL)
    {
        //file starts
        FILE *fp;
        long lSize;
        char *buffer;

        fp = fopen("dummy.txt", "rb");
        if (!fp)
            perror("dummy.txt"), exit(1);

        fseek(fp, 0L, SEEK_END);
        lSize = ftell(fp);
        rewind(fp);

        /* allocate memory for entire content */
        buffer = calloc(1, lSize + 1);
        if (!buffer)
            fclose(fp), fputs("memory alloc fails", stderr), exit(1);

        /* copy the file into the buffer */
        if (1 != fread(buffer, lSize, 1, fp))
            fclose(fp), free(buffer), fputs("entire read fails", stderr), exit(1);
        //file ends

        int msgSize = strlen(buffer);
        if (msgSize != 16)
        {
            printf("\nFile Size is not 16 bytes!!\n");
            break;
            // for (int i = 0; i < MAXLINE; i++)
            // {
            //     sendline[i] = '\0';
            // }
            // printf("\nEnter 16 char text: ");
            // continue;
        }
        for (int k = 0; k < 16; k++)
        {
            sendline[k] = buffer[k];
        }
        free(buffer);
        sendline[16] = '\0';
        //AES Starts here

        for (i = 0; i < 16; i++)
        {
            in[i] = (uint8_t)sendline[i];
        }

        printf("\nPlaintext message from file \"dummy.txt\": ");
        for (i = 0; i < 4; i++)
        {
            // printf("%02x %02x %02x %02x ", in[4 * i + 0], in[4 * i + 1], in[4 * i + 2], in[4 * i + 3]);
            printf("%c%c%c%c", sendline[4 * i + 0], sendline[4 * i + 1], sendline[4 * i + 2], sendline[4 * i + 3]);
        }
        printf("\n");
        printf("\nHexadecimal Representation: ");
        for (i = 0; i < 4; i++)
        {
            printf("%02x %02x %02x %02x ", in[4 * i + 0], in[4 * i + 1], in[4 * i + 2], in[4 * i + 3]);
        }
        printf("\n");
        aes_cipher(in /* in */, out /* out */, w /* expanded key */);

        printf("\nEncrypted message: ");
        char cipherText[16];
        for (i = 0; i < 4; i++)
        {
            cipherText[4 * i + 0] = (char)out[4 * i + 0];
            cipherText[4 * i + 1] = (char)out[4 * i + 1];
            cipherText[4 * i + 2] = (char)out[4 * i + 2];
            cipherText[4 * i + 3] = (char)out[4 * i + 3];
            printf("%c%c%c%c", cipherText[4 * i + 0], cipherText[4 * i + 1], cipherText[4 * i + 2], cipherText[4 * i + 3]);
        }
        printf("\n");
        printf("\nSending Encrypted message...");
        //AES ends here
        // if (sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen) == SOCKET_ERROR)
        if (sendto(sockfd, cipherText, 16, 0, pservaddr, servlen) == SOCKET_ERROR)
            errexit("sendto error: error number %d\n", WSAGetLastError());

        if ((n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL)) == SOCKET_ERROR)
            errexit("recvfrom error: error number %d\n", WSAGetLastError());

        recvline[n] = '\0';
        // fputs(recvline, stdout);
        for (int i = 0; i < MAXLINE; i++)
        {
            sendline[i] = '\0';
        }
        printf("\nEncrypted message sent to Server successfully.");
        // printf("\n\nEnter 16 char text: ");
        break;
    }
}

int main(int argc, char **argv)
{
    WSADATA wsadata;
    int sockfd;
    struct sockaddr_in servaddr;

    if (argc != 2)
        errexit("usage: udpEchoCli01 <IPaddress>");

    if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
        errexit("WSAStartup failed\n");

    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(SERV_PORT);
    if ((servaddr.sin_addr.s_addr = inet_addr(argv[1])) == INADDR_NONE)
        errexit("inet_addr error: error number %d\n", WSAGetLastError());

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd == INVALID_SOCKET)
        errexit("cannot create socket: error number %d\n", WSAGetLastError());

    /* Call our own function dg_cli() to talk with the server */
    dg_cli(sockfd, (SA *)&servaddr, sizeof(servaddr));

    exit(0);
}

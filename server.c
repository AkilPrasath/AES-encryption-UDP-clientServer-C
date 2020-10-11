/* udpechosrv01.c
   Example codes for COMP2650, modified from Stevens's UNPv1.
   Dr. Xiaowen Chu
   To complie: >cl udpechosrv01.c ws2_32.lib
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
//to compile in gcc use -lws2_32
int main(int argc, char **argv)
{
	uint8_t key[] = {
		0x00, 0x01, 0x02, 0x03,
		0x04, 0x05, 0x06, 0x07,
		0x08, 0x09, 0x0a, 0x0b,
		0x0c, 0x0d, 0x0e, 0x0f,
		0x10, 0x11, 0x12, 0x13,
		0x14, 0x15, 0x16, 0x17,
		0x18, 0x19, 0x1a, 0x1b,
		0x1c, 0x1d, 0x1e, 0x1f};
	uint8_t *w; // expanded key
	w = aes_init(sizeof(key));

	aes_key_expansion(key, w);

	WSADATA wsadata;
	int sockfd;
	struct sockaddr_in servaddr, cliaddr;
	int len, n;
	char mesg[MAXLINE];
	printf("\nStarting Server...\n");
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0)
		errexit("WSAStartup failed\n");

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd == INVALID_SOCKET)
		errexit("cannot create socket: error number %d\n", WSAGetLastError());

	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if (bind(sockfd, (SA *)&servaddr, sizeof(servaddr)) == SOCKET_ERROR)
		errexit("can't bind to port %d: error number %d\n", SERV_PORT, WSAGetLastError());

	printf("Server started successfully\n\n");
	printf("Server Running.....");

	for (;;)
	{
		len = sizeof(cliaddr);
		if ((n = recvfrom(sockfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len)) == SOCKET_ERROR)
			errexit("recvfrom error: error number %d\n", WSAGetLastError());
		uint8_t out[16];
		uint8_t in[16];
		uint8_t i;
		printf("\n\nEncrypted message received");
		printf("\nReceived Encrypted Message: ");
		for (int t = 0; t < 16; t++)
		{
			// printf("%02x ", (uint8_t)mesg[t]);
			printf("%c", mesg[t]);
			out[t] = (uint8_t)mesg[t];
		}
		printf("\n");

		printf("\nDecrypting Cipher...");

		aes_inv_cipher(out, in, w);

		printf("\nDecryption Successful");

		printf("\n\nHexadecimal Representation: ");

		for (i = 0; i < 4; i++)
		{
			printf("%02x %02x %02x %02x ", in[4 * i + 0], in[4 * i + 1], in[4 * i + 2], in[4 * i + 3]);
		}
		printf("\n");
		printf("Decrypted message: ");
		for (i = 0; i < 4; i++)
		{
			// printf("%02x %02x %02x %02x ", in[4 * i + 0], in[4 * i + 1], in[4 * i + 2], in[4 * i + 3]);
			printf("%c%c%c%c", (char)in[4 * i + 0], (char)in[4 * i + 1], (char)in[4 * i + 2], (char)in[4 * i + 3]);
		}

		printf("\n");

		if (sendto(sockfd, mesg, n, 0, (SA *)&cliaddr, len) == SOCKET_ERROR)
			errexit("sendto error: error number %d\n", WSAGetLastError());
		for (int i = 0; i < MAXLINE; i++)
		{
			mesg[i] = '\0';
		}
	}
}

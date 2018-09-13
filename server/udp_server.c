#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include <stdbool.h>
/* You will have to modify the program below */

#define MAXBUFSIZE 100

int main (int argc, char * argv[] )
{


	int sock;                           //This will be our socket
	struct sockaddr_in sin, remote;     //"Internet socket address structure"
	unsigned int remote_length;         //length of the sockaddr_in structure
	int nbytes;                        //number of bytes we receive in our message
	char buffer[MAXBUFSIZE];             //a buffer to store our received message
	char compareBuffer[MAXBUFSIZE];
	char fileName[MAXBUFSIZE];


	FILE* fp;
	// fp = fopen("foo1", "w+");
	// if(fp == NULL) {
	// 	printf("error opening file.\n");
	// 	return -1;
	// }
	// printf("you made it.\n");
	// return -2;


	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}

	/******************
	  This code populates the sockaddr_in struct with
	  the information about our socket
	 ******************/
	bzero(&sin,sizeof(sin));                    //zero the struct
	sin.sin_family = AF_INET;                   //address family
	sin.sin_port = htons(atoi(argv[1]));        //htons() sets the port # to network byte order
	sin.sin_addr.s_addr = INADDR_ANY;           //supplies the IP address of the local machine


	//Causes the system to create a generic socket of type UDP (datagram)
	if ((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
	{
		printf("unable to create socket");
	}


	/******************
	  Once we've created a socket, we must bind that socket to the
	  local address and port we've supplied in the sockaddr_in struct
	 ******************/
	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}

	remote_length = sizeof(remote);
	bzero(&remote, remote_length);

	//waits for an incoming message
	while(true)
	{
		// bzero(buffer,sizeof(buffer));
		nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);
		printf("%s\n", buffer);


		if(!strcmp(buffer, "put")) {

			nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);
			printf("%s\n", buffer);
			sprintf(fileName, "%s", buffer);
			printf("%s\n", buffer);

			fp = fopen("foo1", "w+");
			if(fp != NULL) {
				printf("Error opening file.\n");
				return -1;
			}
			while(true) {
				if(!strcmp(buffer, "Over")) {
					printf("hit\n");
					fclose(fp);
					break;
				}
				nbytes = recvfrom(sock, buffer, MAXBUFSIZE, 0, (struct sockaddr *) &remote, (socklen_t *) &remote_length);
				printf("%s\n", buffer);
				fwrite(buffer, sizeof(char), sizeof(buffer), fp);
			}
		}
		else {
			char msg[] = "Did not understand command.\n\n\n";
			nbytes = sendto(sock, &msg, strlen(msg), 0, (struct sockaddr *) &remote, remote_length);
		}
  }


	fclose(fp);
	close(sock);
}

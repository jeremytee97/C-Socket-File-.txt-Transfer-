/* A simple server in the internet domain using TCP
The port number is passed as an argument


 To compile: gcc server.c -o server
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ctype.h>

#define LENGTH 256
ssize_t total = 0;
void writefile(FILE *fp, int sockfd);
int main(int argc, char **argv)
{
	int sockfd, newsockfd, portno;// clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	socklen_t clilen;
	const char close_conn[] = "GOODBYE-CLOSE-TCP";
	int n;

	if (argc < 2)
	{
		fprintf(stderr,"ERROR, no port provided\n");
		exit(1);
	}

	 /* Create TCP socket */

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockfd < 0)
	{
		perror("ERROR opening socket");
		exit(1);
	}


	bzero((char *) &serv_addr, sizeof(serv_addr));

	portno = atoi(argv[1]);

	/* Create address we're going to listen on (given port number)
	 - converted to network byte order & any IP address for
	 this machine */

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);  // store in machine-neutral format

	 /* Bind address to the socket */

	if (bind(sockfd, (struct sockaddr *) &serv_addr,
			sizeof(serv_addr)) < 0)
	{
		perror("ERROR on binding");
		exit(1);
	}

	/* Listen on socket - means we're ready to accept connections -
	 incoming connection requests will be queued */

	listen(sockfd,5);

	clilen = sizeof(cli_addr);

	/* Accept a connection - block until a connection is ready to
	 be accepted. Get back a new file descriptor to communicate on. */
	newsockfd = accept(	sockfd, (struct sockaddr *) &cli_addr,
						&clilen);

	if (newsockfd < 0)
	{
		perror("ERROR on accept");
		exit(1);
	}
	bzero(buffer,256);

	//Dummy name - can read from buffer to get actual name from client
	FILE *fp = fopen("test1.txt", "wb");
    if (fp == NULL)
    {
        perror("Cannot open file");
        exit(1);
    }

    char addr[INET_ADDRSTRLEN];
    printf("Start receive file");
    writefile(fp, newsockfd);
    printf("Receive Success, NumBytes = %ld\n", total);

    fclose(fp);
    close(newsockfd);

	/* close socket */

	close(sockfd);

	return 0;
}


void writefile(FILE *fp, int sockfd)
{
    ssize_t n;
    char buff[LENGTH];
    while ((n = recv(sockfd, buff, LENGTH, 0)) > 0)
    {
	    total+=n;
        if (n == -1)
        {
            perror("Receive File Error");
            exit(1);
        }

        if (fwrite(buff, sizeof(char), n, fp) != n)
        {
            perror("Write File Error");
            exit(1);
        }
        memset(buff, 0, LENGTH);
    }
}

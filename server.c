#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#define MYPORT 5000 /* Avoid reserved ports */
#define BACKLOG 10 /* pending connections queue size */

int main(int argc, char *argv[])
{
	int sockfd, new_fd; /* listen on sock_fd, new connection on new_fd */
	struct sockaddr_in my_addr; /* my address */
	struct sockaddr_in their_addr; /* connector addr */
	int sin_size;
	/* create a socket */
	if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
	{
		perror("socket");
		exit(1);
	}

	/* set the address info */
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(MYPORT); /* short, network byte order */
	my_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	/* INADDR_ANY allows clients to connect to any one of the host’s IP address. Optionally, use this
	line if you know the IP to use:
	my_addr.sin_addr.s_addr = inet_addr(“127.0.0.1”);
	*/
	memset(my_addr.sin_zero, '\0', sizeof(my_addr.sin_zero));
	/* bind the socket */
	if (bind(sockfd, (struct sockaddr *) &my_addr, sizeof(struct sockaddr)) == -1) 
	{
		perror("bind");
		exit(1);
	}

	if (listen(sockfd, BACKLOG) == -1) {
		perror("listen");
		exit(1);
	}
	while (1) 
	{ /* main accept() loop */
		sin_size = sizeof(struct sockaddr_in);
		if ((new_fd = accept(sockfd, (struct sockaddr*)&their_addr, &sin_size)) == -1) 
		{
			perror("accept");
			continue;
		}
	
		printf("server: got connection from %s\n", inet_ntoa(their_addr.sin_addr));
		close(new_fd);
	}
}

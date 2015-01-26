 #include <stdio.h>
 
 #include <stdlib.h>
 #include <unistd.h>
 #include <string.h>
 #include <sys/socket.h>
 #include <arpa/inet.h>

 int main() 
 {
	char buf[80];
	struct sockaddr_in s_addr;
	int i, sock;

	if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) 
	{
		fprintf(stderr, "Failed to create socket\n");
		exit(1);
	}

	s_addr.sin_family = AF_INET;
	s_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	s_addr.sin_port = htons(55443);

	if (connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr)) < 0) 
	{
		fprintf(stderr, "Failed to connect socket\n");
		exit(1);
	}
	
	for(i=0;i<4;i++)
	{
		sprintf(buf, "M LR 10 10\n");
		write(sock, buf, strlen(buf));
		memset(buf, 0, 80);
		read(sock, buf, 80);
		sleep(2);
		
		sprintf(buf, "M L 25\n");
		write(sock, buf, strlen(buf));
		memset(buf, 0, 80);
		read(sock, buf, 80);
		sleep(2);
	}
}
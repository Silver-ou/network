#include	"unp.h"

char          sendline[MAXLINE+1],recvline[MAXLINE+1];
static void sig_alrm(int);

int
main(int argc, char **argv)
{
	int			sockfd;
	struct sockaddr_in	servaddr;

	if (argc != 2)
		err_quit("usage: tcpcli <IPaddress>");

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (SA *) &servaddr, sizeof(servaddr));
	
	printf("connect successfully\n");	

	str_cli(stdin, sockfd);		/* do it all */

	exit(0);
}
void
str_cli(FILE *fp, int sockfd)
{
	int n,i=0;
	int maxfdp1,stdineof;
	fd_set rset;
	time_t ticks;

	while (Fgets(sendline, MAXLINE, fp) != NULL) {

		Writen(sockfd, sendline, strlen(sendline));
		alarm(10);
		while((n = read(sockfd,recvline,MAXLINE)) > 0){
			recvline[n] = 0;
			if(fputs(recvline,stdout) == EOF){
				err_sys("fputs error");
			}
			break;
		}
		if(i>0){
			printf("---------------------------------------");
			printf("\nselect service type:\n1.print\n2.count\n3.file and photo\n");
			printf("---------------------------------------\n");

		}
		i++;
		if(n ==0){
			printf("%.24s\r\n",ctime(&ticks));
			err_quit("str_cli: server terminated prematurely");
		}

	}
}
static void sig_alrm(int signo){
	return;
}

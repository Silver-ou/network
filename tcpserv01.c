#include	"unp.h"
#include        <stdlib.h>
#include        <string.h>
#include        <sys/select.h>
#include        <time.h>
struct timeval timeout;
char 		buff[MAXLINE];

int
main(int argc, char **argv)
{
	int				listenfd, connfd;
	int                             count = 0;
	pid_t				childpid;
	socklen_t			clilen;

	struct sockaddr_in		cliaddr, servaddr;
	time_t				ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family      = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port        = htons(SERV_PORT);

	Bind(listenfd, (SA *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for ( ; ; ) {
		clilen = sizeof(cliaddr);
		connfd = Accept(listenfd, (SA *) &cliaddr, &clilen);
		printf("connection from %s, port %d\n", Inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));
		ticks = time(NULL);
		union{
			short s;
			char c[sizeof(short)];
		}un;
		un.s = 0x0102;
		printf("%s:\n",CPU_VENDOR_OS);
		if(sizeof(short) ==2){
			if(un.c[0] == 1 && un.c[1] == 2){
				char *temp = "big-endian";
				snprintf(buff,sizeof(buff),"%s\r\n %s",ctime(&ticks),temp);
			}
			else if(un.c[0] == 2 && un.c[1] ==1){
				char *temp = "little-endian";
				snprintf(buff,sizeof(buff),"%s\r\n %s",ctime(&ticks),temp);
			}
		}
		else
			printf("sizeof(short) =%d\n",sizeof(short));
		count++;
		if ( (childpid = Fork()) == 0 && count<3) {	/* child process */				
			Close(listenfd);	/* close listening socket */
			str_echo(connfd);	/* process the request */
			exit(0);
		}
		else if(count>2){
			snprintf(buff,sizeof(buff),"service is too busy");
			Writen(connfd,buff,strlen(buff));				
		}
		Close(connfd);			/* parent closes connected socket */
	}
}

void
str_echo(int sockfd)
{	
	ssize_t		n;
	char 		*temp;
	time_t    	ticks;

	ticks=time(NULL);

	while ( (n = read(sockfd, buff, MAXLINE)) > 0){
		int i=0;
		i=atoi(buff);
		printf("%d %s",i,buff);
		switch(i){
			case 1:
				break;
			case 2:
			{
				long arg0,arg1,arg2;
				char *temp="exit";
				for(; ;){
					if(strcmp(buff,temp)==0)
						break;
					if(sscanf(buff,"%ld%ld%ld",&arg0,arg1,arg2)==3)
						snprintf(buff,sizeof(buff),"%ld %ld %ld %f\n",arg1 + arg2, arg1 - arg2, arg1 * arg2, arg1*1.0/arg2);
					else
						snprintf(buff,sizeof(buff), "input is error\n");
					break;
				}
				break;
			}
			case 3:
			{
				memset(buff,'\0',sizeof(buff));
				break;
		
			}
			continue;
		}
		Writen(sockfd,buff,strlen(buff));
		memset(buff,'\0',sizeof(buff));	
	}

}

#include "./user.h"
#include "./pack.h"
#include "./driver.h"
#include "./online.h"

#define IP "192.168.199.152"
#define PORT 9766

static int id=-1;
static long phone=-1;

void main_menu(int sockfd);
void receive_picture(int sockfd);

void *mypack(void *data,int len,int flag)
{
		struct pack *temp=(struct pack *)malloc(sizeof(struct pack)+len);
		temp->flag=flag;
		temp->len=len;
		memcpy(temp->data,data,len);
		return temp;
}

void system_pause()
{
		getchar();
		puts("Press any key to continue...");
		system("stty raw");
		getchar();
		system("stty cooked");
		system("clear");
}

void show_time()
{
		char showtime[50];
		time_t t;
		struct tm *timeinfo;
		time(&t);
		timeinfo=localtime(&t);
		strftime(showtime,50,"%Y年%m月%d日-%H:%M:%S\n%A",timeinfo);
		printf("%s\n",showtime);
}

void * heartbeat(void *arg)
{
		int sockfd=*((int *)arg);
		struct pack temp;
	//	int flags=fcntl(sockfd,F_GETFL,0);
	//	fcntl(sockfd,F_SETFL,flags|O_NONBLOCK);
		while(1)
		{
			struct pack *p=mypack(&temp,sizeof(temp),3001);
			int ret=send(sockfd,p,sizeof(struct pack),0);
			sleep(5);
		//	while(1);
		}
		printf("heartbeat stop!\n");
		return NULL;
}

void user_register(int sockfd)
{
		struct order temp;
		printf("please input name:");
		scanf("%s",temp.name);
		printf("please input passwd:");
		scanf("%s",temp.passwd);
		printf("please input phone number:");
		scanf("%ld",&temp.phone);
		struct pack *p=mypack(&temp,sizeof(temp),2002);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct order));
		if(ret<=0)
		{
				printf("write failed   %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
		ret=read(sockfd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("read failed   %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
		if(temp.flag==1)
		{
				printf("register success\n");
				printf("your user id is %d\n",temp.id);
		}
		if(temp.flag==0)
				printf("register failed\n");
}

void user_login(int sockfd)
{
		struct order temp;
		int count=0;
		long number,n;
		while(1)
		{
			printf("please input id(5) or phone number(11):");
			scanf("%ld",&number);
			n=number;
			while(n!=0)
			{
					count++;
					n=n/10;
			}
			if(count==11)
			{
					temp.phone=number;
					phone=number;
					temp.flag=11;  // 11为phone的标识
					break;
			}
			if(count==5)
			{
					temp.id=number;
					id=number;
					temp.flag=12;      // 12为id的标识
					break;
			}
			printf("input error!\n");
			printf("count=%d\n",count);
		}
		printf("please input passwd:");
		scanf("%s",temp.passwd);
		struct pack *p=mypack(&temp,sizeof(temp),2003);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct order));
		if(ret<=0)
		{
				printf("write failed   %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
		ret=read(sockfd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("read failed   %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
		if(temp.flag==1)
		{
				printf("sign in success\n");
				main_menu(sockfd);
		}
		if(temp.flag==-1)
		{
				printf("you are logged in!\n");
				return ;
		}
		if(temp.flag==0)
				printf("sign in failed\n");
}
				
void send_hailing(int sockfd)
{
		struct order temp;
		struct online *q=head;
		if(temp.id==-1)
		{
				temp.flag=11;
				temp.phone=phone;
		}
		if(temp.phone==-1)
		{
				temp.flag=12;
				temp.id=id;
		}
		printf("please input your longitute:");
		scanf("%f",&temp.longitude);
		printf("please input your latitude:");
		scanf("%f",&temp.latitude);
		struct pack *p=mypack(&temp,sizeof(temp),2004);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct order));
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
		/*
		while(read(sockfd,&buf,sizeof(struct online1))==sizeof(struct online1))
		{
		if(ret<=0)
		{
				printf("read failed %d\n",__LINE__);
				return ;
		}
				add_online(buf.id,buf.number,buf.confd,buf.type);
			//	q=q->next;
		}
		show_online();
		*/
		receive_picture(sockfd);
		return ;
}

void receive_drive_info(int sockfd,char photo_name[30])
{
		struct driver temp;
		int ret=read(sockfd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed  %d\n",__LINE__);
				exit(0);
		}
		system("clear");
		printf("a driver accepted your order,his information is:\n");
		printf("driver_photo:%s\nname:%s\nphone:%d\ncarNumber:%s\ncarModel:%s\n",photo_name,temp.name,temp.phone,temp.number,temp.model);
		
		system_pause();
		printf("waiting......\n");
		read(sockfd,&temp,sizeof(struct driver));
		system("clear");
		printf("\033[30m thank you for using!\033[11m\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[31m thank you for using!\033[12m\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[32m thank you for using!\033[13m\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[33m thank you for using!\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[34m thank you for using!\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[35m thank you for using!\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[36m thank you for using!\n");
		sleep(1);
		printf("\033[1A");
		printf("\033[K");
		printf("\033[0m thank you for using!\n");
		system_pause();
}


void receive_picture(int sockfd)
{
		int rn;
		printf("receive infortion...\n");
		char photo_name[30];
		chdir("photo");
		srand((unsigned)time(NULL));
		sprintf(photo_name,"driver_photo%d.jpg",rand()%100);
		FILE *fp=fopen(photo_name,"ab");
		chdir("..");
		char buf[15600]="\0";
		while(1)
		{
			rn=read(sockfd,buf,1024);
			if(rn!=1024)
					break;
			fwrite(buf,1,rn,fp);
		}
		fclose(fp);
		receive_drive_info(sockfd,photo_name);
		return ;
}

void view_info(int sockfd)
{
		struct order temp;
		if(temp.id==-1)
		{
				temp.flag=11;
				temp.phone=phone;
		}
		if(temp.phone==-1)
		{
				temp.flag=12;
				temp.id=id;
		}
		struct pack *p=mypack(&temp,sizeof(temp),2001);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct order));
		if(ret<=0)
		{
				printf("write failed   %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				exit(0);
		}
		ret=read(sockfd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("read failed  %d\n",__LINE__);
				exit(0);
		}
		if(temp.flag==0)
		{
				printf("view failed\n");
				exit(0);
		}
		if(temp.flag==1)
		{
				printf("your information:\n");
				printf("id:%d\nname:%s\nphone:%ld\n",temp.id,temp.name,temp.phone);
				system_pause();
		}
}
/*
void send_delete(int sockfd)
{
		struct order temp;
		struct pack *p=mypack(&temp,sizeof(temp),2005);
		int ret=write(sockfd,p,sizeof(struct pack));
		if(ret<=0)
		{
				printf("write failed   %d\n",__LINE__);
				close(sockfd);
				exit(0);
		}
}
*/

void main_menu(int sockfd)
{
		int key;
//		pthread_t thread1;
//		pthread_create(&thread1,NULL,heartbeat,&sockfd);
		while(1)
		{
				show_time();
				printf("1 look information\n");
				printf("2 taxi-hailing\n");
				printf("0 exit\n");
				printf("please input your choice:");
				scanf("%d",&key);
				switch(key)
				{
						case 1:system("clear");
								view_info(sockfd);
								break;
						case 2:system("clear");
								send_hailing(sockfd);
							   break;
						case 0:system("clear");
							  // send_delete(sockfd);
							    id=-1;
								phone-1;
								return;
								break;
				}
		}
}				


void login_menu(int sockfd)
{
		system("clear");
		while(1)
		{
				printf("1 login\n");
				printf("2 register\n");
				printf("0 exit\n");
				printf("please input your choice:");
				int key;
				scanf("%d",&key);
				switch(key)
				{
						case 1:system("clear");
								user_login(sockfd);
								break;
						case 2:system("clear");
								user_register(sockfd);
								break;
						case 0:
								close(sockfd);
								printf("bye!\n");
								exit(0);
								break;
				}
		}
}


int main()
{
		int sockfd=socket(AF_INET,SOCK_STREAM,0);
		printf("sockfd=%d pid=%d\n",sockfd,getpid());
		struct sockaddr_in saddr;
		saddr.sin_family=AF_INET;
		saddr.sin_port=htons(PORT);
		int ret=inet_pton(AF_INET,IP,&saddr.sin_addr.s_addr);
		ret=connect(sockfd,(struct sockaddr *)&saddr,sizeof(saddr));
		if(ret==-1)
		{
				perror("connect failed\n");
				return -1;
		}
		login_menu(sockfd);
}






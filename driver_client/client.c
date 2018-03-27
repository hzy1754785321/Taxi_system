#include "./driver.h"
#include "./online.h"
#include "./pack.h"
#include "./user.h"

#define IP "192.168.199.152"
#define PORT 9766

void main_menu(int sockfd);
static int _id;
static int _flag=0;
static int _show=0;
static int _stop=0;

int set_sockfd()
{
		int sockfd=socket(AF_INET,SOCK_STREAM,0);
	//	printf("sockfd=%d pid=%d\n",sockfd,getpid());
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
		return sockfd;
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

void *mypack(void *data,int len,int flag)
{
		struct pack *temp=(struct pack *)malloc(sizeof(struct pack)+len);
		temp->flag=flag;
		temp->len=len;
		memcpy(temp->data,data,len);
		return temp;
}

void *heart(void *data,int len)
{
		struct head *temp=(struct head *)malloc(sizeof(struct head)+len);
//		temp->flag=flag;
	//	temp->type=type;
		temp->len=len;
		memcpy(temp->data,data,len);
		return temp;
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


int send_url(int sockfd)
{
		char buf[1024];
		sprintf(buf,"http://news.youth.cn/jsxw/201709/W020170916204149324820.jpg");
		int ret=write(sockfd,&buf,sizeof(buf));
		if(ret<=0)
		{
				printf("write failed %d\n",__LINE__);
				printf("errno=%d\n",errno);
				return 0;
		}
		return 1;
}

int driver_register(int sockfd)
{
		struct driver temp;
		printf("please input name:");
		scanf("%s",temp.name);
		printf("please input passwd:");
		scanf("%s",temp.passwd);
		printf("please input phone number:");
		scanf("%d",&temp.phone);
		printf("please input car model:");
		scanf("%s",temp.model);
		printf("please input car number:");
		scanf("%s",temp.number);
		struct pack *p=mypack(&temp,sizeof(temp),1002);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct driver));
		if(ret<=0)
		{
				printf("write failed   %d\n",__LINE__);
				printf("errno=%d\n",errno);
				close(sockfd);
				exit(0);
		}
		ret=read(sockfd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed   %d\n",__LINE__);
				printf("errno=%d\n",errno);
				close(sockfd);
				exit(0);
		}
		if(temp.flag==1)
		{
				printf("register success\n");
				printf("your driver id is %d\n",temp.id);
				return 1;
		}
		if(temp.flag==0)
		{
				printf("register failed\n");
				return 0;
		}
}

int driver_login(int sockfd)
{
		struct driver temp;
		printf("please input id:");
		scanf("%d",&temp.id);
		_id=temp.id;
		printf("please input passwd:");
		scanf("%s",temp.passwd);
		struct pack *p=mypack(&temp,sizeof(temp),1003);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct driver));
		if(ret<=0)
		{
				printf("write failed   %d\n",__LINE__);
				printf("error=%d\n",errno);
				close(sockfd);
				exit(0);
		}
		ret=read(sockfd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed   %d\n",__LINE__);
				printf("error=%d\n",errno);
				close(sockfd);
				exit(0);
		}
		if(temp.flag==1)
		{
				printf("sign in success\n");
				main_menu(sockfd);
				return 1;
		}
		if(temp.flag==-1)
		{
				printf("you are logged in!\n");
				return 0;
		}
		if(temp.flag==0)
		{
				printf("sign in failed\n");
				return 0;
		}
}

int sign_out(int sockfd)
{
		  struct driver temp;
		  struct pack *p=mypack(&temp,sizeof(temp),1004);
		  int ret=write(sockfd,p,sizeof(struct pack));
		  if(ret<=0)
		  {
				  printf("write failed   %d\n",__LINE__);
				  printf("error=%d\n",errno);
				  return 0;
		  }
		  return 1;
}

void order_completion(int sockfd,int user_confd)
{
		struct driver temp;
		printf("user_confd=%d\n",user_confd);
		temp.user_confd=user_confd;
		char ch;
		while(1)
		{
			printf("the order has been completed?(y/n)");
			ch=getchar();
			if(ch=='y')
			{
					printf("order completed!\n");
					write(sockfd,&temp,sizeof(struct driver));
					system_pause();
					return ;
			}
		}
}

void *show_link(void *arg)
{
		struct user_online1 buf;
		struct driver temp;
		int sockfd=*((int *)arg);
		int flag=0;
		while(1)
		{
						if(read(sockfd,&buf,sizeof(struct user_online1))==sizeof(struct user_online1)||_stop==0)
						{
								if(flag==0)
								{

									delete_user_online_all();
									flag++;
								}
								add_user_online(buf.confd,buf.id,buf.name,buf.phone,buf.longitude,buf.latitude);

						}
						if(_stop==1)
						{
								char *t="pthread exit";
								pthread_exit(t);
						}
						if(buf.type==-1)
						{
					//		show_user_online();
							write_file();
							flag=0;
						}
		}
}


int order_receive(int sockfd)
{
		struct user_online1 buf;
		struct driver temp;
		struct pack *p=mypack(&temp,sizeof(temp),1005);
		pthread_t thread1;
		pthread_create(&thread1,NULL,show_link,&sockfd);
		int ret=write(sockfd,p,sizeof(struct pack));
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				printf("errno=%d\n",errno);
				return 0;
		}
		system("gnome-terminal -e ./show");
/*loop:	
		while(1)
		{
				if(read(sockfd,&buf,sizeof(struct user_online1))==sizeof(struct user_online1))
				{
						if(buf.type==-2)
								break;
						add_user_online(buf.confd,buf.id,buf.name,buf.phone,buf.longitude,buf.latitude);
				}
				if(buf.type==-1)
						break;
		}
		while(1)
		{
	//		printf("order list:\n");
	//		show_user_online();
*/
			printf("please input passenger's id(0 exit):");
			scanf("%d",&temp.user_id);
			_stop=1;
/*			if(temp.user_id==0)
			{
					temp.flag=-2;
					write(sockfd,&temp,sizeof(struct driver));
					return 0;
			}
			if(temp.user_id==1)
			{
					temp.flag=-1;
					delete_user_online_all();
					write(sockfd,&temp,sizeof(struct driver));
					goto loop;
			}*/

			temp.flag=0;
		//	write(sockfd,&temp,sizeof(struct driver));
			temp.user_confd=search_user_confd(temp.user_id);
			int user_confd1=temp.user_confd;   //test
	//		printf("user confd=%d\n",temp.user_confd);
			if(temp.user_confd==-1)
			{
					printf("order receiving failed\n");
					temp.flag=-3;
					write(sockfd,&temp,sizeof(struct driver));
					system_pause();
					return 0;
			}
			delete_user_online(temp.user_confd);
			write_file();
			temp.id=_id;
		/*	ret=write(sockfd,&temp,sizeof(struct driver));
			if(ret<=0)
			{
					printf("write failed  %d\n",__LINE__);
					printf("errno=%d\n",errno);
					return 0;
			}
			*/
				temp.flag=-6;    //test
				temp.user_confd=user_confd1;
				write(sockfd,&temp,sizeof(struct driver));  //test
				read(sockfd,&temp,sizeof(struct driver));   //test
			if(temp.flag==-7)
			{
					printf("order receiving failed\n");
					return 0;
			}
			float longitude=search_longitude(temp.user_id);
			float latitude=search_latitude(temp.user_id);
			if(send_url(sockfd))
			{
					printf("order receiving success\n");
				//	printf("please pick up passenger at the %f,%f\n",longitude,latitude);
					system_pause();
					order_completion(sockfd,user_confd1);
					return 1;
			}
		//	else
		
}	



void look_info(int sockfd)
{
		struct driver temp;
		temp.id=_id;
		struct pack *p=mypack(&temp,sizeof(temp),1001);
		int ret=write(sockfd,p,sizeof(struct pack)+sizeof(struct driver));
		printf("ret=%d\n",ret);
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				return ;
		}
		ret=read(sockfd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed  %d\n",__LINE__);
				return ;
		}
		printf("id:%d\nname:%s\nphone:%d\ncarModel:%s\ncarNumber:%s\n",temp.id,temp.name,temp.phone,temp.model,temp.number);
		system_pause();
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




void main_menu(int sockfd)
{
//		pthread_create(&thread2,NULL,show_time,NULL);
//		int sockfd1=set_sockfd();
//		pthread_t thread1;
//		pthread_create(&thread1,NULL,unheart_pack,NULL);
//		pthread_create(&thread1,NULL,heartbeat,&sockfd1);
		system("clear");
		int key;
		while(1)
		{
			show_time();
				printf("1 look information\n");
				printf("2 order receiving\n");
				printf("0 exit\n");
				printf("please input your choice:");
				scanf("%d",&key);
				switch(key)
				{
						case 1:system("clear");
								look_info(sockfd);
								break;
						case 2: system("clear");
								order_receive(sockfd);
								break;
						case 0:system("clear");
								if(sign_out(sockfd)==0)
								{
										close(sockfd);
										exit(0);
								}
							    close(sockfd);
								return ;
								break;
				}
		}
}



void  login_menu(int sockfd)
{
		system("clear");
//		int sockfd=*((int *)arg);
  		while(1)
		{
 		//	sockfd=set_sockfd();
			printf("1 login\n");
			printf("2 register\n");
			printf("0 exit\n");
			printf("please input your chioce:");	
			int key;
			scanf("%d",&key);
			switch(key)
			{
					case 1:system("clear");
							driver_login(sockfd);
							break;
					case 2:system("clear");
							driver_register(sockfd);
							break;
					case 0:system("clear");
					//		sign_out(sockfd);
							close(sockfd);
							printf("bye!\n");
							exit(0);
							break;
			}	
 		 }
}



int main()
{
		/*
		int sockfd=socket(AF_INET,SOCK_STREAM,0);
	//	printf("sockfd=%d pid=%d\n",sockfd,getpid());
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
		*/
//		while(1)
//		{
		//	pthread_t thread;
		int sockfd=set_sockfd();
		login_menu(sockfd);
		//	pthread_create(&thread,NULL,login_menu,&sockfd);
//		}
	//	while(1)
	     
		//		pthread_t thread,thread1;
	//
	//	login_menu(sockfd);
		//pid_t pid;
//		pid=fork();
}









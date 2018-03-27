
#include "./online.h"
#include "driver.h"
#include "./user.h"
#include "./pack.h"


#define ERR_EXIT(m)\
do\
{\
		perror(m);\
		exit(EXIT_FAILURE);\
}while(0)

#define IP "192.168.199.152"
#define PORT 9766

char *user="root";
char *passwd="1997";
char *database="driver_data";
char *server="localhost";

void send_picture(int user_confd,int id,int confd);

static int _flag=0;
static int _people=0;
static int _order=0;
static int _user=0;
static int _send=0;
static int _stop=0;

void hander_info(int confd)
{
		struct driver temp;
		int ret=read(confd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed %d\n",__LINE__);
				return ;
		}
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		sprintf(sql,"select * from driver where id=%d",temp.id);
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		MYSQL_RES *res;
		MYSQL_ROW row;
		res=mysql_use_result(conn);
		row=mysql_fetch_row(res);
		if(row==NULL)
		{
				printf("view fail\n");
				return ;
		}
		else
		{
				strcpy(temp.name,row[1]);
				temp.phone=atoi(row[3]);
				strcpy(temp.model,row[4]);
				strcpy(temp.number,row[5]);
		}
		ret=write(confd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				return ;
		}
}


void hander_register(int confd)
{
		struct driver temp;
		int ret=read(confd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed %d\n",__LINE__);
				return ;
		}
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		sprintf(sql,"insert into driver(name,passwd,phone,carModel,carNumber) values(\'%s\',\'%s\',%d,\'%s\',\'%s\')",temp.name,temp.passwd,temp.phone,temp.model,temp.number);
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		strcpy(sql,"select max(id) as value from driver");
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		MYSQL_RES *res;
		MYSQL_ROW row;
		res=mysql_use_result(conn);
		row=mysql_fetch_row(res);
		if(row==NULL)
		{
				printf("id or passwd error\n");
				temp.flag=0;
		}
		else																{
				temp.id=atoi(row[0]);
				temp.flag=1;
		}
		write(confd,&temp,sizeof(struct driver));
		mysql_free_result(res);
		mysql_close(conn);
}

void hander_login(int confd)
{
		struct driver temp;
		struct online *p=head; 
		int ret=read(confd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read login failed  %d\n",__LINE__);
				return ;
		}
		/*
		while(p!=NULL)
		{
				if(temp.id==p->id)
				{
						if(p->type==0)
						{
								change_online(confd);
								break;
						}
						temp.flag=-1;
				}
				p=p->next;
		}
		write(confd,&temp,sizeof(struct driver));
		if(temp.flag==-1)
				return;
				*/
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		sprintf(sql,"select * from driver where id=%d and passwd=\'%s\'",temp.id,temp.passwd);
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		MYSQL_RES *res;
		MYSQL_ROW row;
		res=mysql_use_result(conn);
		row=mysql_fetch_row(res);
		if(row==NULL)
		{
				printf("id or passwd error\n");
				temp.flag=0;
		}
		else
	{
		if(p==NULL)
		{
				strcpy(temp.name,row[1]);
				temp.phone=atoi(row[3]);
				strcpy(temp.number,row[5]);
				strcpy(temp.model,row[4]);
				temp.flag=1;
				add_online(temp.id,temp.name,temp.model,temp.number,temp.phone,confd,1);
				system("clear");
				show_online();
		}
		while(p!=NULL)
		{
				if(temp.id==p->id&&p->type==1)
				{
						temp.flag=-1;
						break;
				}
				if(temp.id==p->id&&p->type==0)
				{
				//	change_online(confd);
					p->type=1;
					temp.flag=1;
					system("clear");
					show_online();
					break;
				}
				else if(temp.id!=p->id&&p->next==NULL)
				{
					strcpy(temp.name,row[1]);
					temp.phone=atoi(row[3]);
					strcpy(temp.model,row[4]);
					strcpy(temp.number,row[5]);
					temp.flag=1;
					add_online(temp.id,temp.name,temp.model,temp.number,temp.phone,confd,1);
					system("clear");
					show_online();
					break;
				}
				p=p->next;
		}
	}
		write(confd,&temp,sizeof(struct driver));
	/*	
		if(temp.flag==1)
		{
			FILE *fp=fopen("driverpic.png","ab");
			char buf[256]="\0";
			while(1)
			{
					int rn=read(confd,buf,256);
					if(rn==0)
							break;
					fwrite(buf,1,rn,fp);
			}
			fclose(fp);
		}
	*/	
		mysql_free_result(res);
		mysql_close(conn);
}

void *send_link(void *arg)
{
		int confd=*((int *)arg);
		struct user_online1 temp1;
		struct driver temp;
	while(1)
	{
			if(_send==1)
			{
				struct user_online *p=head1;
				while(p!=NULL)
				{
					temp1.id=p->id;
					temp1.confd=p->confd;
					strcpy(temp1.name,p->name);
					temp1.phone=p->phone;
					temp1.type=p->type;
					temp1.longitude=p->longitude;
					temp1.latitude=p->latitude;
					if(p->next==NULL)
							temp1.type=-1;
					int ret=write(confd,&temp1,sizeof(struct user_online1));
					if(ret<=0)
					{
							printf("write failed  %d\n",__LINE__);
							return NULL;
					}
					p=p->next;
				}
					p=head1;
					_send=0;
			}
			if(_stop==1)
			{
					char *t="pthread stop";
					pthread_exit(t);
			}
	}	
}


void download_picture(int confd)
{
		char buf[1024];
		char buf1[1024];
		struct user_online1 temp1;
		struct driver temp;
		struct user_online *p=head1;
		pthread_t thread3;
		pthread_create(&thread3,NULL,send_link,&confd);
		/*
		while(p==NULL)
		{
				temp1.type=-2;
				write(confd,&temp1,sizeof(struct user_online1));
				break;
		}
while(1)
{
		struct user_online *p=head1;
		temp1.type=0;
		while(p!=NULL)
		{
				temp1.id=p->id;
				temp1.confd=p->confd;
				strcpy(temp1.name,p->name);
				temp1.phone=p->phone;
				temp1.type=p->type;
				temp1.longitude=p->longitude;
				temp1.latitude=p->latitude;
				if(p->next==NULL)
						temp1.type=-1;
				int ret=write(confd,&temp1,sizeof(struct user_online1));
				if(ret<=0)
				{
						printf("write failed  %d\n",__LINE__);
						return ;
				}
				p=p->next;
		}
		p=head1;
		read(confd,&temp,sizeof(struct driver));
		if(temp.flag==0)
				break;
		if(temp.flag==-2)
				return ;
}

		int ret=read(confd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read failed  %d\n",__LINE__);
				return ;
		}
		if(temp.flag==-3)
			return ;
	*/
		read(confd,&temp,sizeof(struct driver));    //test
		_stop=1;
//		if(temp.user_id==0)
//				return ;
		if(delete_user_online(temp.user_confd)==0)
{
			temp.flag=-7;
}
		else
			temp.flag=6;
		write(confd,&temp,sizeof(struct driver));  //test
		if(temp.flag==-7)
				return ;
		int ret=read(confd,&buf,1024);
//		printf("buf=%s\n",buf);
		if(ret<=0)
		{
				printf("read failed  %d\n",__LINE__);
				return ;
		}
		sprintf(buf1,"wget \"%s\" -O driver.jpg",buf);
		system(buf1);
//		printf("temp.user_confd=%d\n",temp.user_confd);
//		printf("id=%d\n",temp.id);
		send_picture(temp.user_confd,temp.id,confd);
}

void hander_user_register(int confd)
{
		struct order temp;
		int ret=read(confd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("read register failed  %d\n",__LINE__);
				return ;
		}
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		sprintf(sql,"insert into user(name,passwd,phone) values(\'%s\',\'%s\',%ld)",temp.name,temp.passwd,temp.phone);
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		strcpy(sql,"select max(id) as value from user");
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		MYSQL_RES *res;
		MYSQL_ROW row;
		res=mysql_use_result(conn);
		row=mysql_fetch_row(res);
		if(row==NULL)
		{
				printf("id or pass error\n");
				temp.flag=0;
		}
		else
		{
				temp.id=atoi(row[0]);
				temp.flag=1;
		}
		write(confd,&temp,sizeof(struct order));
		mysql_free_result(res);
		mysql_close(conn);
}

void hander_user_login(int confd)
{
		struct order temp;
		int ret=read(confd,&temp,sizeof(struct driver));
		if(ret<=0)
		{
				printf("read login failed  %d\n",__LINE__);
				return ;
		}
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		if(temp.flag==11)
		{
				sprintf(sql,"select * from user where phone=%ld and passwd=\'%s\'",temp.phone,temp.passwd);
		}
		if(temp.flag==12)
		{
				sprintf(sql,"select * from user where id=%d and passwd=\'%s\'",temp.id,temp.passwd);
		}
				if(mysql_query(conn,sql))
				{
						fprintf(stderr,"%s\n",mysql_error(conn));
						return ;
				}
				MYSQL_RES *res;
				MYSQL_ROW row;
				res=mysql_use_result(conn);
				row=mysql_fetch_row(res);
				if(row==NULL)
				{
						if(temp.flag==11)
								printf("phone or passwd error\n");
						if(temp.flag==12)
								printf("id or passwd error\n");
						temp.flag=0;
				}
				else
				{
						temp.id=atoi(row[0]);
						strcpy(temp.name,row[1]);
						temp.phone=atol(row[3]);
						temp.flag=1;
				}
		printf("temp.flag=%d\n",temp.flag);
		/*
				if(temp.flag==11)
				{
						if(search_user_all_id(temp.phone)==-1)
						{
								temp.flag=1;
								add_user_online_all(confd,temp.id,temp.name,temp.phone);
						}
						else
						{
								temp.flag=-1;
						}
				}
				if(temp.flag==12)
				{
						if(search_user_all_confd(temp.id)==-1)
						{
								temp.flag=1;
								add_user_online_all(confd,temp.id,temp.name,temp.phone);
						}
						else
						{
								temp.flag=-1;
						}
				}
				*/
				write(confd,&temp,sizeof(struct order));
				mysql_free_result(res);
				mysql_close(conn);
}

void send_driver_info(int user_confd,int id,int confd)
{
		struct online *p=head;
		struct driver temp;
		while(p!=NULL)
		{
				printf("%d\n",p->id);
				if(p->id==id)
				{
						temp.id=p->id;
						strcpy(temp.name,p->name);
						strcpy(temp.model,p->model);
						strcpy(temp.number,p->number);
						temp.phone=p->phone;
						break;
				}
				p=p->next;
		}
		printf("user_confd=%d\n",user_confd);
		printf("name:%s\nphone:%d\ncarNumber:%s\ncarModel:%s\n",temp.name,temp.phone,temp.number,temp.model);
		int ret=write(user_confd,&temp,sizeof(struct driver));
//		printf("ret=%d\n",ret);
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				return ;
		}
		read(confd,&temp,sizeof(struct driver));
		printf("temp.user_confd=%d\n",temp.user_confd);
		write(temp.user_confd,&temp,sizeof(struct driver));
}


void send_picture(int user_confd,int id,int confd)
{
		int len;
		char buffer[1024];
		FILE *fp=fopen("driver.jpg","rb");
		while(!feof(fp))
		{
				len=fread(buffer,1,1024,fp);
				if(len!=write(user_confd,buffer,len))
				{
						perror("write failed\n");
						break;
				}
		}
		fclose(fp);
		send_driver_info(user_confd,id,confd);
		return ;
}



void order(int confd)
{
		struct online *p=head;
		struct order temp;
//		struct online1 buf;
		int ret=read(confd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("read failed %d\n",__LINE__);
				return ;
		}
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		if(temp.flag==11)
		{
				sprintf(sql,"select * from user where phone=%ld",temp.phone);
				printf("phone=%ld\n",temp.phone);
		}
		if(temp.flag==12)
		{
		printf("id=%d\n",temp.id);
				sprintf(sql,"select * from user where id=%d",temp.id);
		}
				if(mysql_query(conn,sql))
				{
						fprintf(stderr,"%s\n",mysql_error(conn));
						return ;
				}
				MYSQL_RES *res;
				MYSQL_ROW row;
				res=mysql_use_result(conn);
				row=mysql_fetch_row(res);
				if(row==NULL)
				{
						if(temp.flag==11)
								printf("phone error\n");
						if(temp.flag==12)
								printf("id error\n");
				}
				else
				{
						temp.id=atoi(row[0]);
						strcpy(temp.name,row[1]);
						temp.phone=atol(row[3]);
				}
				add_user_online(confd,temp.id,temp.name,temp.phone,temp.longitude,temp.latitude);
				system("clear");
				show_user_online();
				mysql_free_result(res);
				mysql_close(conn);
/*		while(p!=NULL)
		{
				buf.id=p->id;
				buf.confd=p->confd;
				buf.type=p->type;
				strcpy(buf.number,p->number);
				ret=write(confd,&buf,sizeof(struct online1));
				if(ret<=0)
				{
						printf("write failed  %d\n",__LINE__);
						return;
				}
				p=p->next;
		}
		*/
}
				
void hander_user_info(int confd)
{
		struct order temp;
		int ret=read(confd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("read failed  %d\n",__LINE__);
				return ;
		}	
		MYSQL *conn;
		conn=mysql_init(NULL);
		if(!mysql_real_connect(conn,server,user,passwd,database,0,NULL,0))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		char sql[100];
		if(temp.flag==11)
		{
				sprintf(sql,"select * from user where phone=%ld",temp.phone);
		}
		if(temp.flag==12)
		{
				sprintf(sql,"select * from user where id=%d",temp.id);
		}
		if(mysql_query(conn,sql))
		{
				fprintf(stderr,"%s\n",mysql_error(conn));
				return ;
		}
		MYSQL_RES *res;
		MYSQL_ROW row;
		res=mysql_use_result(conn);
		row=mysql_fetch_row(res);
		if(row==NULL)
		{
				temp.flag=0;
		}
		else
		{
				temp.id=atoi(row[0]);
				strcpy(temp.name,row[1]);
				temp.phone=atol(row[3]);
				temp.flag=1;
		}
		mysql_free_result(res);
		mysql_close(conn);
		ret=write(confd,&temp,sizeof(struct order));
		if(ret<=0)
		{
				printf("write failed  %d\n",__LINE__);
				exit(0);
		}
}

void * unheart_pack(void *arg)
{
		int confd=*((int *)arg);
		int count=1;
		struct pack temp;
		int ret;
		sleep(10);
		while(1)
		{
				if(_flag==1)
				{
						_flag=0;
				}
				else if(_flag==0)
				{
						printf("客户端无响应,第%d次尝试...\n",count);
						count++;
				}
				sleep(10);
				if(count==6)
						break;
		}
		printf("client %d heartbeat stop!\n",confd);
		delete_online(confd);
		close(confd);
}

void * show_all()
{
		printf("input 0 show driver list,1 show user list");
		int show_key;
		while(1)
		{
				struct online *head;
				struct user_online *q=head1;
				scanf("%d",&show_key);
				if(show_key==0)
				{
						show_online();
				}
				if(show_key==1)
				{
						show_user_online();
				}
		}
		return NULL;
}

void * unpack(void *arg)
{
		int confd=*((int *)arg);
		struct pack temp;
	//	pthread_t thread1;
	//	pthread_create(&thread1,NULL,unheart_pack,&confd);
		while(read(confd,&temp,sizeof(struct pack))==sizeof(struct pack))
		{
			//	printf("server ret=%d\n",ret);
//				printf("temp.type=%d\n",temp.type);
//				if(temp.type==0)
//				{
				printf("flag=%d\n",temp.flag);
					switch(temp.flag)
					{
							case 1001:hander_info(confd);
								  break;
							case 1002:hander_register(confd);
								  break;
							case 1003:hander_login(confd);
								  break;
							case 1004:change_online(confd);
								  system("clear");
								  show_online();
								  break;
							case 1005:download_picture(confd);
								  break;
							case 2001:hander_user_info(confd);
								  break;
							case 2002:hander_user_register(confd);
								  break;
							case 2003:hander_user_login(confd);
								  break;
							case 2004:order(confd);
								  break;
							case 2005://delete_user_online_one(confd);
								//	  system("clear");
								//	  show_user_online_all();
									  break;
							case 3001:_flag=1;
									  break;
							default: 
							//	  change_online(confd);
							//	  system("clear");
							//	  show_online();
								  break;
					}
//				}
		}
	//	printf("client close\n");
	//	delete_online(confd);
		change_online(confd);
		delete_user_online(confd);
		system("clear");
		show_online();
  	//	close(confd);
		return NULL;
}
#if 0
void * heartbeat(void *arg)
{
		printf("test heartbeat\n");
		int confd=*((int *)arg);
		int alive,error,idle,cnt,intv;
		alive=1;
		int ret=setsockopt(confd,SOL_SOCKET,SO_KEEPALIVE,&alive,sizeof(alive));
		if(ret<0)
		{
				printf("set socket failed\n");
				return ;
		}
		idle=5;
		ret=setsockopt(confd,SOL_TCP,TCP_KEEPIDLE,&idle,sizeof(idle));
		if(ret<0)
		{
				printf("set keepalive idle error\n");
				return ;
		}
		intv=15;
		ret=setsockopt(confd,SOL_TCP,TCP_KEEPINTVL,&intv,sizeof(intv));
		if(ret<0)
		{
				printf("set keppalive intv error\n");
				return ;
		}
		cnt=3;
		ret=setsockept(confd,SOL_TCP,TCP_KEEPCNT,&cnt,sizeof(cnt));
		if(ret<0)
		{
				printf("set keepalive cnt error\n");
				return ;
		}
}
#endif

int main()
{
		head=NULL;
		int listenfd=socket(AF_INET,SOCK_STREAM,0);
		struct sockaddr_in myaddr;
		myaddr.sin_family=AF_INET;
		myaddr.sin_port=htons(PORT);
		inet_pton(AF_INET,IP,&myaddr.sin_addr.s_addr);
		int ret=bind(listenfd,(struct sockaddr *)&myaddr,sizeof(myaddr));
		if(ret<0)
		{
				perror("bind failed\n");
				ERR_EXIT("bind");
				return -1;
		}
		ret=listen(listenfd,20);
		if(ret<0)
		{
				perror("listen failed\n");
				return -1;
		}
		struct sockaddr_in caddr;
		int len=sizeof(caddr);
		char buf[20];
		int confd;
		while(1)
		{
				confd=accept(listenfd,(struct sockaddr *)&caddr,&len);
				pthread_t thread,thread2;
				pthread_create(&thread,NULL,unpack,&confd);
			//	pthread_create(&thread1,NULL,unheart_pack,&confd);
				pthread_create(&thread2,NULL,show_all,NULL);
		}
		close(listenfd);
}

void add_online(int id,char name[32],char model[32],char number[32],int phone,int confd,int type)
{
		struct online *temp=(struct online *)malloc(sizeof(struct online));
		temp->id=id;
		strcpy(temp->number,number);
		strcpy(temp->name,name);
		strcpy(temp->model,model);
		temp->phone=phone;
		temp->confd=confd;
		temp->type=type;
		temp->next=head;
		head=temp;
		_people++;
}

void add_user_online(int confd,int id,char name[32],long phone,float longitude,float latitude)
{
		struct user_online *temp=(struct user_online *)malloc(sizeof(struct user_online));
		temp->confd=confd;
		strcpy(temp->name,name);
		temp->phone=phone;
		temp->id=id;
		temp->longitude=longitude;
		temp->latitude=latitude;
		temp->next=head1;
		head1=temp;
		_send=1;
		_order++;
}

void add_user_online_all(int confd,int id,char name[32],long phone)
{
		struct user_online_all *temp=(struct user_online_all *)malloc(sizeof(struct user_online_all));
		temp->confd=confd;
		strcpy(temp->name,name);
		temp->phone=phone;
		temp->id=id;
		head2=temp;
		_user++;
}

int  change_online(int confd)
{
		struct online *p=head;
		while(p!=NULL)
		{
				if(p->confd==confd)
				{
						if(p->type==1)
						{
							p->type=0;
							_people--;
						}
					//	else if(p->type==0)
					//		p->type=1;
				}
				p=p->next;
		}
	return -1;
}

void show_online()
{
		int flag=1;
		struct online *p=head;
		char type1[10];
		printf("id\tcarNumber\ttype\n");
		while(p!=NULL)
		{
				if(p->type==1)
						strcpy(type1,"on line");
				if(p->type==0)
						strcpy(type1,"off line");
				printf("%d\t%s\t\t%s\n",p->id,p->number,type1);		
				if(p->next==NULL)
						printf("online number:%d\n",_people);
				p=p->next;
		}
}

void show_user_online()
{
		struct user_online *p=head1;
		printf("id\tname\tphone\t\tlongitude\ttitude\n");
		while(p!=NULL)
		{
				printf("%d\t%s\t%ld\t%f\t%f\n",p->id,p->name,p->phone,p->longitude,p->latitude);
				if(p->next==NULL)
						printf("order number:%d\n",_order);
				p=p->next;
		}
}

void show_user_online_all()
{
		struct user_online_all *p=head2;
		printf("confd\tid\tname\tphone\n");
		while(p!=NULL)
		{
				printf("%d\t%d\t%s\t%ld\n",p->confd,p->id,p->name,p->phone);
				if(p->next==NULL)
						printf("user number:%d\n",_user);
				p=p->next;
		}
}


int search_confd(int id)
{
		struct online *p=head;
		while(p!=NULL)
		{
				if(p->id==id)
						return p->confd;
				p=p->next;
		}
		return -1;
}

int search_user_all_confd(int id)
{
		struct user_online_all *p=head2;
		while(p!=NULL)
		{
				if(p->id==id)
						return p->confd;
				p=p->next;
		}
		return -1;
}

int search_user_all_id(long phone)
{
		struct user_online_all *p=head2;
		while(p!=NULL)
		{
				if(p->phone==phone)
						return p->id;
				p=p->next;
		}
		return -1;
}
		
int search_id(int confd)
{
		struct online *p=head;
		while(p!=NULL)
		{
				if(p->confd==confd)
						return p->id;
				p=p->next;
		}
		return -1;
}

void delete_online(int confd)
{
		struct online *p,*q;
		if(head==NULL)
				return ;
		if(head->confd==confd)
		{
				p=head->next;
				free(head);
				head=p;
				_people--;
				return ;
		}
		p=head,q=head->next;
		while(q!=NULL)
		{
				if(q->confd==confd)
				{
						p->next=q->next;
						free(q);
						_people--;
						return ;
				}
				p=p->next;
				q=q->next;
		}
}
void delete_user_online_one(int confd)
{
		struct user_online_all *p,*q;
		printf("confd=%d\n",confd);
		if(head2==NULL)
				return ;
		else if(head2->confd==confd)
		{
		printf("test  %d\n",__LINE__);
				p=head2->next;
				free(head2);
				head2=p;
				_user--;
				return ;
		}

		p=head2,q=head2->next;
		while(q!=NULL)
		{
		printf("test  %d\n",__LINE__);
				if(q->confd==confd)
				{
						p->next=q->next;
						free(q);
						_user--;
						return ;
				}
				p=p->next;
				q=q->next;
		}
}

int delete_user_online(int confd)
{
		struct user_online *p,*q;
		if(head1==NULL)
				return 0;
		if(head1->confd==confd)
		{
				p=head1->next;
				free(head1);
				head1=p;
				_order--;
				_send=1;
				return 1;
		}
		p=head1,q=head1->next;
		while(q!=NULL)
		{
				if(q->confd==confd)
				{
						p->next=q->next;
						free(q);
						_send=1;
						_order--;
						return 1;
				}
				p=p->next;
				q=q->next;
		}
		return 0;
}


	

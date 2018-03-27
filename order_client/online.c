#include "./online.h"

static int _order=0;

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
}

void add_user_online(int confd,int id,char name[32],long phone,float longitude,float latitude)
{
		struct user_online *temp=(struct user_online *)malloc(sizeof(struct user_online));
		temp->confd=confd;
		temp->id=id;
		strcpy(temp->name,name);
		temp->phone=phone;
		temp->longitude=longitude;
		temp->latitude=latitude;
		temp->next=head1;
		head1=temp;
		_order++;
}


int  change_online(int confd)
{
		struct online *p=head;
		while(p!=NULL)
		{
				if(p->confd==confd)
				{
						if(p->type==1)
							p->type=0;
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
		system("clear");
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
				p=p->next;
		}
}

void show_user_online()
{
		struct user_online *p=head1;
		system("clear");
		printf("id\tname\tphone\t\tlongitude\tlatitude\n");
		while(p!=NULL)
		{
				printf("%d\t%s\t%ld\t%f\t%f\n",p->id,p->name,p->phone,p->longitude,p->latitude);
				if(p->next==NULL)
						printf("order number:%d\n",_order);
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

int search_user_confd(int id)
{
		struct user_online *p=head1;
		while(p!=NULL)
		{
				if(p->id==id)
						return p->confd;
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
				return ;
		}

		p=head,q=head->next;
		while(q!=NULL)
		{
				if(q->confd==confd)
				{
						p->next=q->next;
						free(q);
						return ;
				}
				p=p->next;
				q=q->next;
		}
}

void delete_user_online(int confd)
{
		struct user_online *p,*q;
		if(head1==NULL)
				return ;
		if(head1->confd==confd)
		{
				p=head1->next;
				free(head1);
				head1=p;
				_order--;
				return ;
		}
		p=head1,q=head1->next;
		while(q!=NULL)
		{
				if(q->confd==confd)
				{
						p->next=q->next;
						free(q);
						_order--;
						return ;
				}
				p=p->next;
				q=q->next;
		}
}

void delete_user_online_all()
{
		struct user_online *p,*q;
		if(head1==NULL)
				return ;
		p=head1;
		while(q!=NULL)
		{
				q=p->next;
				free(q);
				p=q;
		}
		head1=NULL;
		_order=0;
}
			

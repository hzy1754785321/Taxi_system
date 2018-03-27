#ifndef __FUN_H_
#define __FUN_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <pthread.h>

struct online
{
		int id;
		char name[32];
		int confd;
		int type;
		int phone;
		char model[32];
		char number[32];
		struct online *next;
};
/*
struct online1
{
		int id;
		int confd;
		int type;
		int phone;
		char model[32];
		char number[32];
};
*/
struct user_online
{
		int id;
		int confd;
		char name[32];
		long phone;	
		int type;
		float longitude;
		float latitude;
		struct user_online *next;
};

struct user_online1
{
		int id;
		int confd;
		char name[32];
		long phone;
		int type;
		float longitude;
		float latitude;
};


struct online *head;
struct user_online *head1;
void add_user_online(int confd,int id,char name[32],long phone,float longitude,float latitude);
void delete_online(int confd);
void show_user_online();
void delete_user_online(int confd);
int search_id(int confd);
int search_confd(int id);
int change_online(int confd);
void show_online();
void add_online(int id,char name[32],char model[32],char number[32],int phone,int confd,int type);
int search_user_confd(int id);
void delete_user_online_all();

#endif

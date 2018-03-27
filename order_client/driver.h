#ifndef __DRIVER_H_
#define __DRIVER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <mysql/mysql.h>
#include <sys/wait.h>


struct driver
{
		int flag;
		int id;
		int user_id;
		int user_confd;
		char name[32];
		char passwd[32];
		int  phone;
		char model[32];
		char number[32];
};

#endif

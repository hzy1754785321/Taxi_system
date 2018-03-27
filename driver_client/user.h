#ifndef __USER_H_
#define __USER_H_

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <sys/wait.h>

struct order
{
		int flag;
		int id;
		char name[32];
		char passwd[32];
		long phone;
		float longitude;  //经度
		float latitude;  //纬度
};

#endif

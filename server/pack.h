#ifndef __PACK_H_
#define __PACK_H_

#include <stdio.h>
#include <fcntl.h>

struct pack
{
		int flag;
		int type;
		int len;
		char data[0];
};

struct heart_pack
{
//		int flag;
		int type;
		int len;
		char data[0];
};

struct head
{
		size_t size;
		int len;
		char data[0];
};

#endif

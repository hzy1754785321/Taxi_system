#include <stdio.h>
#include <unistd.h>
#include "online.h"
#include "user.h"

int main()
{
		struct user_online1 temp;
		struct user_online *p=head1;
		while(1)
		{
			FILE *fp=fopen("link.txt","r+");
	//		while(!feof(fp))
			while(fread(&temp,sizeof(struct user_online),1,fp))
			{
					add_user_online(temp.confd,temp.id,temp.name,temp.phone,temp.longitude,temp.latitude);
			}
			system("clear");
			show_user_online();
			delete_user_online_all();
			sleep(2);
			fclose(fp);
		}
}

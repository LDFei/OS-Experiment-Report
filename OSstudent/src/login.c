//
// Created by Heng on 25-6-20.
//

#include "login.h"
#include <string.h>
#include <stdio.h>
#include "macros.h"
#include <conio.h>
#include <stdlib.h>
#include <time.h>
#include "utils.h"
#include "command_handler.h"

//char username[MAXNAME];  /*record login user's name22*/

void LoginF()  /*LOGIN FileSystem*/
{
	char loginame[MAXNAME],loginpw[9],logincpw[9],str[50];
	int i,j,flag=1;
	char a[25];
	int findout; /*login user not exist*/

	while(1)
	{
		findout=0;
		printf("\n\nLogin Name:");
		fgets(loginame,sizeof(loginame),stdin);
		loginame[strcspn(loginame, "\n")] = '\0';

		ltrim(rtrim(loginame));
		fp_mfd=fopen("c:\\osfile\\mfd.txt","rb");
		for(i=0;fread(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd)!=0;i++)
			if (strcmp(strupr(ufd_lp.ufdname),strupr(loginame))==0)
			{
				findout=1;
				strcpy(logincpw,ufd_lp.ufdpword);
			}
		fclose(fp_mfd);
		if (findout==1)  /*user exist*/
		{
			printf("Login Password:");
			InputPW(loginpw); /*input password,use '*' replace*/
			if (strcmp(loginpw,logincpw)==0)
			{
				strcpy(username,strupr(loginame));
				strcpy(dirname,username);
				fp_mfd=fopen("c:\\osfile\\mfd.txt","rb");
				for(j=0;fread(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd)!=0;j++)
				{
					strcpy(str,"c:\\osfile\\");
					strcat(str,ufd_lp.ufdname);
					strcat(str,".txt");
					ufd[j]=(OSUFD*)malloc(sizeof(OSUFD));
					strcpy(ufd[j]->ufdname,strupr(ufd_lp.ufdname));
					fp_ufd=fopen(str,"rb");
					fcount[j]=0;
					for(i=0;fread(&ufd[j]->ufdfile[i],sizeof(OSFILE),1,fp_ufd)!=0;i++,fcount[j]++)
					{
						ifopen[j][i].ifopen=0;
						ifopen[j][i].openmode=4;
					}
					fclose(fp_ufd);
				}

				fclose(fp_mfd);
				ucount=j;

				SetPANo(0);
				printf("\n\nLogin successful! Welcome to this FileSystem\n\n");
				loginsuc=1;

				LoadShares(); // 加载共享记录
				strcpy(dirname, username); // 设置初始目录为用户名

				return;
			}
			else
			{
				printf("\n\n");
				flag=1;
				while(flag)
				{
					printf("Login Failed!  Password Error.  Try Again(Y/N):");
					fgets(a,sizeof(a),stdin);
					a[strcspn(a, "\n")] = '\0';
					ltrim(rtrim(a));

					if (strcmp(strupr(a),"Y")==0)
					{
						loginsuc=0;
						flag=0;
					}
					else if(strcmp(strupr(a),"N")==0)
					{
						loginsuc=0;
						flag=0;
						return;
					}
				}
			}
		}
		else
		{
			printf("New Password(<=8):");
			InputPW(loginpw); /*input new password,use '*' replace*/
			printf("\nConfirm Password(<=8):"); /*input new password,use '*' replace*/
			InputPW(logincpw);
			if (strcmp(loginpw,logincpw)==0)
			{
				strcpy(ufd_lp.ufdname,strupr(loginame));
				strcpy(ufd_lp.ufdpword,loginpw);
				fp_mfd=fopen("c:\\osfile\\mfd.txt","ab");
				fwrite(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd);
				fclose(fp_mfd);
				strcpy(username,strupr(loginame));
				strcpy(dirname,loginame);

				////////////////////////////////////////////////////////
				strcpy(str,"c:\\osfile\\");
				strcat(str,username);
				strcat(str,".txt");
				if((fp_ufd=fopen(str,"rb"))==NULL)
				{
					fp_ufd=fopen(str,"wb");
					fclose(fp_ufd);
				}
				fp_mfd=fopen("c:\\osfile\\mfd.txt","rb");
				for(j=0;fread(&ufd_lp,sizeof(OSUFD_LOGIN),1,fp_mfd)!=0;j++)
				{
					/////////////////////////////////////
					strcpy(str,"c:\\osfile\\");
					strcat(str,ufd_lp.ufdname);
					strcat(str,".txt");
					ufd[j]=(OSUFD*)malloc(sizeof(OSUFD));
					strcpy(ufd[j]->ufdname,strupr(ufd_lp.ufdname));
					fp_ufd=fopen(str,"rb");
					for(i=0;fread(&ufd[j]->ufdfile[i],sizeof(OSFILE),1,fp_ufd)!=0;i++,fcount[j]++)
					{
						ifopen[j][i].ifopen=0;
					    ifopen[j][i].openmode=4;
					}
					fclose(fp_ufd);
				}
				fclose(fp_mfd);
				ucount=j;
				SetPANo(0);
				printf("\n\nLogin Successful! Welcome to this System\n\n");
				loginsuc=1;

				LoadShares(); // 加载共享记录
				strcpy(dirname, username); // 设置初始目录为用户名

				return;
			}
		    else
			{
				printf("\n\n");
				flag=1;
				while(flag)
				{
					printf("Login Failed! Password Error. Try Again(Y/N):");

					fgets(a,sizeof(a),stdin);
					a[strcspn(a, "\n")] = '\0';
					ltrim(rtrim(a));

					if (strcmp(strupr(a),"Y")==0)
					{
						loginsuc=0;
						flag=0;
					}
					else if(strcmp(strupr(a),"N")==0)
					{
						loginsuc=0;
						flag=0;
						return;
					}
				}
			}
		}
	}
}

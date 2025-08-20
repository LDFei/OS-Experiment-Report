#ifdef _WIN32
#include <windows.h>
#endif
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>
#include "macros.h"
#include "command_handler.h"
#include "utils.h"


 

int main()
{
#ifdef _WIN32
	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);
#endif

	int i,choice1;
	char choice[50];  /*choice operation:dir,create,delete,open,delete,modify,read,write*/
	int choiceend=1;  /*whether choice end*/
	char *rtrim(char *str);  /*remove the trailing blanks.*/
	char *ltrim(char *str);  /*remove the heading blanks.*/

	if((fp_mfd=fopen("c:\\osfile\\mfd.txt","rb"))==NULL)
	{
		fp_mfd=fopen("c:\\osfile\\mfd.txt","wb");
		fclose(fp_mfd);
	}

	for(i=0;i<MAX;i++) fpaddrno[i]=0;
	//textattr(BLACK*16|WHITE);
	clrscr();   /*clear screen*/
	LoginF();   /*user login*/
	clrscr();

	if(loginsuc==1)  /*Login Successfully*/
	{
		while (1)
		{
			wgetchar=0;
			if (choiceend==1)
				printf("\n\nC:%s>",strupr(dirname));
			else 
				printf("Bad command or file name.\nC:\\%s>",strupr(username));

		    fgets(choice,sizeof(choice),stdin);
			choice[strcspn(choice, "\n")] = '\0';

		    strcpy(choice,ltrim(rtrim(strlwr(choice))));
		    if (strcmp(choice,"dir")==0) choice1=1;
		    else if(strcmp(choice,"create")==0) choice1=2;
		    else if(strcmp(choice,"delete")==0) choice1=3;
		    else if(strcmp(choice,"attrib")==0) choice1=4;
		    else if(strcmp(choice,"open")==0) choice1=5;
		    else if(strcmp(choice,"close")==0) choice1=6;
		    else if(strcmp(choice,"read")==0) choice1=7;
		    else if(strcmp(choice,"write")==0) choice1=8;
		    else if(strcmp(choice,"exit")==0) choice1=9;
		    else if(strcmp(choice,"cls")==0) choice1=10;
		    else if(strcmp(choice,"cd")==0) choice1=11;
		    else if(strcmp(choice,"help")==0) choice1=20;
			//文件共享相关的逻辑判断
		    else if(strcmp(choice,"share")==0) choice1=13;
		    else if(strcmp(choice,"listshares")==0) choice1=14;
		    else if(strcmp(choice,"access")==0) choice1=15;
			else if (strcmp(choice,"unshare")==0)choice1 = 16;
		    else choice1=12;

			switch(choice1)
			{
				case 1:DirF();choiceend=1;break;
				case 2:CreateF();choiceend=1;if(!wgetchar) getchar();break;
				case 3:DeleteF();choiceend=1;if(!wgetchar)getchar();break;
				case 4:ModifyFM();choiceend=1;if(!wgetchar) getchar();break;
				case 5:OpenF();choiceend=1;if (!wgetchar) getchar();break;
				case 6:CloseF();choiceend=1;if (!wgetchar) getchar();break;
				case 7:ReadF();choiceend=1;if (!wgetchar) getchar();break;
				case 8:WriteF();choiceend=1;if (!wgetchar) getchar();break;
				case 9:printf("\nYou have exited this system.");
					   QuitF();exit(0);break;

				case 10:clrscr();choiceend=1;break;
				case 11:CdF();choiceend=1;break;

				case 13:ShareF();choiceend = 1;break;
				case 14:ListShares();choiceend = 1;break;
				case 15:AccessSharedFile();choiceend = 1;break;
				case 16:UnshareF();choiceend = 1;break;
				case 20:help();choiceend=1;break;
				default:choiceend=0;
			}
		}	
	}
	else 
		printf("\n登录失败,请检查密码或用户名.");
}

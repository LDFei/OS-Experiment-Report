//
// Created by Heng on 25-6-20.
//

#include "command_handler.h"

#include <stdio.h>
#include <string.h>
#include "utils.h"
#include <conio.h>
#include <stdlib.h>
OSUFD *ufd[MAXCHILD];   /*ufd and ufd own files*/
OSUFD_LOGIN ufd_lp;


OSSHARE shares[MAX];   // 分享记录数组
int share_count = 0;   // 当前分享记录数



int ucount=0;  /*the count of mfd's ufds*/
int fcount[MAXCHILD];  /*the count of ufd's files*/
int loginsuc=0; /*whether login successfully*/
char username[MAXNAME];  /*record login user's name22*/
char dirname[MAXNAME];/*record current directory*/
int fpaddrno[MAX];  /*record file physical address num*/
OSUFD_OPENMODE ifopen[MAXCHILD][MAXCHILD]; /*record file open/close*/
int wgetchar; /*whether getchar()*/


void clrscr()
{
	system("cls");
}

FILE *fp_mfd;//主文件目录（Master File Directory）的文件指针,对应 c:\osfile\mfd.txt，
FILE *fp_ufd;//用户文件目录（User File Directory）的文件指针，c:\osfile\用户名.txt。
FILE *fp_file_p;//文件物理地址记录的文件指针，c:\osfile\file\file_p.txt
FILE *fp_file;//正在操作的“具体文件”的文件指针，c:\osfile\file\fileX.txt

void help(void)
{
    printf("\nThe Command List\n");
    printf("\nCd(更换目录)  "
		   "Attrib(更改文件的权限)  "
			"Create(创建文件)  "
			"Write(书写文件)  "
			"Read(读取文件)  "
			"Open(打开文件)  "
			"\n"
			"Cls(清除控制台信息)  "
			"Delete(删除文件)  "
			"Exit(退出系统)  "
			"Close(关闭文件)  "
			"Dir(查询用户名下的所有文件信息)"
			"\n"
		   "Share(分享文件)  "
		   "ListShares(查看分享文件)  "
		   "Access(访问分享文件)"
		   "Unshare(删除分享过的文件)"
		   "\n"
			"/*以上操作命令不区分大小写*/"
			"\n");
}

void SetPANo(int RorW)  /*Set physical address num,0-read,1-write*/
{
	int i,j;
	if (RorW==0)
	{
		if((fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","rb"))==NULL)
	    {
			fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","wb");
			fclose(fp_file_p);
	    }
		fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","rb");

		///////////////////////////////////////////////////////////////////
		for(i=0;fread(&j,sizeof(int),1,fp_file_p)!=0;i++)
			fpaddrno[j]=1;

		/*for(i=1;i<MAX;i++)
			if ((i%13)==0)
				fpaddrno[i]=1;*/
	}
	else
	{
		fp_file_p=fopen("c:\\osfile\\file\\file_p.txt","wb");
		/*for(i=1;i<MAX;i++)
			if((i%13)==0)
				fpaddrno[i]=0;*/
		for(i=0;i<MAX;i++)
			if (fpaddrno[i]==1)
				fwrite(&i,sizeof(int),1,fp_file_p);
	}
	fclose(fp_file_p);
}

void InputPW(char *password)  /*input password,use '*' replace*/
{
	int j;
	for(j=0;j<=7;j++)
	{
		password[j]=getch();
		if ((int)(password[j])!=13)
		{
			if((int)(password[j])!=8)
			putchar('*');
			else
			{
				if (j>0)
				{
					j--;
					j--;
					putchar('\b');putchar(' ');putchar('\b');
				}
				else j--;
			}
		}
		else
		{	password[j]='\0';
			break;
		}
	}
	password[j]='\0';
}

void DirF() //你这个Dir设计的比较巧妙，如果目录文件存在则一输出目录文件，如果一开始不存在就直接输出有几个文件
{
	int i,j,count=0;

	//sfmode存的文件的读写类型，sfpaddr存的物理地址，str存的是第几个文件，用于对sfpaddr的拼接
	char sfmode[25],sfpaddr[25],str[25];

	if (strcmp(strupr(ltrim(rtrim(dirname))),"")!=0)
	{
		printf("\n\nC:\\%s>dir\n",dirname);
		printf("\n%14s%16s%14s%10s%18s\n","FileName","FileAddress","FileLength","Type","FileMode");
		j=ExistD(dirname);
		for(i=0;i<fcount[j];i++)
		{
			if ((i%16==0)&&(i!=0))
			{
				printf("\nPress any key to continue..");
				getch();
				clrscr();
				printf("\n%14s%16s%14s%10s%18s\n","FileName","FileAddress","FileLength","Type","FileMode");
			}
			/////////////////////////////////////////////////////////
			itoa(ufd[j]->ufdfile[i].fpaddr,str,10);
			strcpy(sfpaddr,"file");
			strcat(sfpaddr,str);
			if (ufd[j]->ufdfile[i].fmode==0) strcpy(sfmode,"Read Only");
			else if(ufd[j]->ufdfile[i].fmode==1) strcpy(sfmode,"Write Only");
			else if(ufd[j]->ufdfile[i].fmode==2)strcpy(sfmode,"Read And Write");
			else strcpy(sfmode,"Protect");
			printf("%14s%16s%14d%10s%18s\n",ufd[j]->ufdfile[i].fname,sfpaddr,ufd[j]->ufdfile[i].flength,"<FILE>",sfmode);
		}
		printf("\n %3d file(s)\n",fcount[j]);
	}
	else
	{
		printf("\n\nC:\\>dir\n");
		printf("\n%14s%18s%8s\n","DirName","OwnFileCount","Type");
		for(i=0;i<ucount;i++)
		{
			if ((i%16==0)&&(i!=0))
			{
				printf("\nPress any key to continue...");
				getch();
				clrscr();
				printf("\n%14s%18s%8s\n","DirName","OwnFileCount","Type");
			}
			printf("%14s%18d%8s\n",ufd[i]->ufdname,fcount[i],"<UFD>");
			count=count+fcount[i];
		}
		printf("\n %3d dir(s),%5d file(s)\n",ucount,count);
	}
}

int ExistD(char *dirname)  /*Whether DirName Exist,Exist-i,Not Exist-0*/
{
	int i;
	int exist=0;
	for(i=0;i<ucount;i++)
		if (strcmp(strupr(ufd[i]->ufdname),strupr(dirname))==0)
		{
			exist=1;
			break;
		}
	if (exist) return(i);
	else return(-1);
}

void CdF() /*Exchange Dir*/
{
	char dname[MAXNAME];

	printf("\nPlease input DirName (cd..-Previous dir; DirNAME-cd [DirNAME]):");

	fgets(dname,sizeof(dname),stdin);
	dname[strcspn(dname, "\n")] = '\0';
	ltrim(rtrim(dname));

	if (ExistD(dname)>=0)  strcpy(dirname,strupr(dname));
	else
		if(strcmp(strupr(dname),"CD..")==0)  strcpy(ltrim(rtrim(dirname)),"");
		else printf("\nError.\'%s\' does not exist.\n",dname);
}

void CreateF()  /*Create File*/
{
	int fpaddrno,flag=1,i;
	char fname[MAXNAME],str[50],str1[50],a[25];
	char fmode[25];


	if (strcmp(strupr(dirname),strupr(username))!=0)
	{
		printf("\nError. You must create file in your own dir.\n");
		wgetchar=1;
	}
	else
	{
		printf("\nPlease input FileName:");

		fgets(fname,sizeof(fname),stdin);
		fname[strcspn(fname, "\n")] = '\0';

		ltrim(rtrim(fname));
		if (ExistF(fname)>=0)
		{
			printf("\nError. Name \'%s\' has already existed.\n",fname);
			wgetchar=1;
		}
		else
		{
			printf("Please input FileMode(0-Read Only, 1-Write Only, 2-Read and Write, 3-Protect):");

			fgets(fmode,sizeof(fmode),stdin);
			fmode[strcspn(fmode, "\n")] = '\0';

			ltrim(rtrim(fmode));
			if((strcmp(fmode,"0")==0)||(strcmp(fmode,"1")==0)||(strcmp(fmode,"2")==0)||(strcmp(fmode,"3")==0))
			{
				fpaddrno=FindPANo();
				if (fpaddrno>=0)
				{
					i=ExistD(username);
					strcpy(ufd[i]->ufdfile[fcount[i]].fname,fname);
					ufd[i]->ufdfile[fcount[i]].fpaddr=fpaddrno;
					ufd[i]->ufdfile[fcount[i]].fmode=atoi(fmode);
					ifopen[i][fcount[i]].ifopen=0;
					ifopen[i][fcount[i]].openmode=4;
					strcpy(str,"c:\\osfile\\file\\file");
					itoa(fpaddrno,str1,10);
					strcat(str,str1);
					strcat(str,".txt");
					fp_file=fopen(str,"wb");
					fclose(fp_file);
					fcount[i]++;
					while(flag)
					{
						printf("Input text now(Y/N):");

						fgets(a,sizeof(a),stdin);
						a[strcspn(a, "\n")] = '\0';
						ltrim(rtrim(a));

						ufd[i]->ufdfile[fcount[i]-1].flength=0;
						if(strcmp(strupr(a),"Y")==0)
						{
							fp_file=fopen(str,"wb+");
							ufd[i]->ufdfile[fcount[i]-1].flength=WriteF1();
							flag=0;
						}
						else if(strcmp(strupr(a),"N")==0)
						{
							flag=0;
							wgetchar=1;
						}
					}
					printf("\n\'%s\' has been created successfully!\n",fname);
				}
				else
				{
					printf("\nFail!No Disk Space. Please format your disk.\n");
					wgetchar=1;
				}
			}
			else
			{
				printf("\nError. FileMode\'s Range is 0-3\n");
				wgetchar=1;
			}
		}
	}
}

int ExistF(char *filename)  //确认文件是否存在的函数
{
	int i,j;
	int exist=0;

	j=ExistD(dirname);
	for(i=0;i<fcount[j];i++)
	if (strcmp(strupr(ufd[j]->ufdfile[i].fname),strupr(filename))==0)
	{
		exist=1;
		break;
	}
	if (exist) return(i);
	else return(-1);
}

int FindPANo()  //寻找物理地址的函数
{
	int i;
	for(i=0;i<MAX;i++)
	if (fpaddrno[i]==0)
	{
		fpaddrno[i]=1;
		break;
	}
	if (i<MAX) return(i);
	else return(-1);
}

int WriteF1()  //这是一个书写函数，
{
	int length=0;
	char c;
	printf("Please input text(\'#\' stands for end):\n");
	while((c=getchar())!='#')
	{
		fprintf(fp_file,"%c",c);
		if (c!='\n') length++;
	}
	fprintf(fp_file,"\n");
	fclose(fp_file);
	return(length);
}

void ModifyFM() /*Modify FileMode*/
{
	char fname[MAXNAME],str[50];
	int i,k;
	char fmode[25]; /*whether delete*/

	if (strcmp(strupr(dirname),strupr(username))!=0)
	{
		printf("\nError.You can only modify filemode in yourself dir.\n");
		wgetchar=1;
	}
	else
	{
		printf("\nPlease input FileName:");

		fgets(fname,sizeof(fname),stdin);
		fname[strcspn(fname, "\n")] = '\0';

		ltrim(rtrim(fname));
		i=ExistF(fname);
		if (i>=0)
		{
			k=ExistD(username);
			if(ifopen[k][i].ifopen==1)
			{
				printf("\nError.\'%s\' is in open status. Close it before modify.\n",fname);
				wgetchar=1;
			}
			else
			{
				if(ufd[k]->ufdfile[i].fmode==0) strcpy(str,"read only");    /*FileMode*/
				else if(ufd[k]->ufdfile[i].fmode==1) strcpy(str,"write only");
				else if(ufd[k]->ufdfile[i].fmode==2) strcpy(str,"read and write");
				else strcpy(str,"Protect");

				printf("\'%s\' filemode is %s.\n",fname,strupr(str));
				printf("Modify to(0-read only,1-write only,2-read and write,3-Protect):");
				fgets(fmode,sizeof(fmode),stdin);
				fmode[strcspn(fmode, "\n")] = '\0';


				ltrim(rtrim(fmode));
				if(strcmp(fmode,"0")==0)
				{
					ufd[k]->ufdfile[i].fmode=0;
					printf("\n\'%s\' has been modified to READ ONLY mode successfully.\n",fname);
					wgetchar=1;
				}
				else if(strcmp(fmode,"1")==0)
				{
					ufd[k]->ufdfile[i].fmode=1;
					printf("\n\'%s\' has been modified to WRITE ONLY mode successfully.\n",fname);
					wgetchar=1;
				}
				else if(strcmp(fmode,"2")==0)
				{
					ufd[k]->ufdfile[i].fmode=2;
					printf("\n\'%s\' has been modified to READ AND WRITE mode successfully.\n",fname);
					wgetchar=1;
				}
				else if(strcmp(fmode,"3")==0)
				{
					ufd[k]->ufdfile[i].fmode=3;
					printf("\n\'%s\' has been modified to FORBID mode successfully.\n",fname);
					wgetchar=1;
				}
				else
				{
					printf("\nError.\'%s\' is not modified.\n",fname);
					wgetchar=1;
				}
			}
		}
		else
		{
			printf("\nError. \'%s\' dose not exist.\n",fname);
			wgetchar=1;
		}
	}
}



//----------------------------------------------------------------------------------------------
//这个函数老师文件里面包含了。。
// 读取文件函数：读取已打开且具有读取权限的文件内容，并显示
void ReadF() {
    int i, k, n = 0;
    char fname[MAXNAME]; // 用户输入的文件名
    char str[255], str1[255], c; // 临时字符串和字符变量

    // 检查当前是否在某个用户目录中
    if (strcmp(strupr(ltrim(rtrim(dirname))), "") == 0) {
        printf("\n错误：请先切换到用户目录后再读取文件。\n");
        wgetchar = 1;
        return;
    }

    printf("\n注意：请先打开文件\n");
    printf("已打开的文件列表：\n");
    k = ExistD(dirname); // 获取当前目录索引

    // 遍历目录中所有文件，输出已打开且允许读取的文件名
    for (i = 0; i < fcount[k]; i++) {
        if (ifopen[k][i].ifopen == 1)
            if ((ifopen[k][i].openmode == 0) || (ifopen[k][i].openmode == 2)) {
                printf("%15s", ufd[k]->ufdfile[i].fname);
                n++;
            }
        if ((n % 4 == 0) && (n != 0)) printf("\n");
    }
    printf("\n共打开了 %d 个文件。\n", n);

    if (n == 0) wgetchar = 1; // 没有文件可读

    if (n != 0) {
        printf("\n请输入要读取的文件名：");
        fgets(fname, sizeof(fname), stdin); // 输入要读取的文件名
        fname[strcspn(fname, "\n")] = '\0';
        ltrim(rtrim(fname));

        i = ExistF(fname); // 查找文件索引
        if (i >= 0) {
            if (ifopen[k][i].ifopen == 1) {
                if ((ifopen[k][i].openmode == 0) || (ifopen[k][i].openmode == 2)) {
                    // 构造完整文件路径
                    itoa(ufd[k]->ufdfile[i].fpaddr, str, 10);
                    strcpy(str1, "file");
                    strcat(str1, str);
                    strcpy(str, "c:\\osfile\\file\\");
                    strcat(str, str1);
                    strcat(str, ".txt");

                    // 打开文件并读取内容
                    fp_file = fopen(str, "rb");
                    fseek(fp_file, 0, 0); // 设置文件指针到文件开头
                    printf("\n文件内容如下：\n\n");
                    printf("   ");
                    while (fscanf(fp_file, "%c", &c) != EOF)
                        if (c == '\n')
                            printf("\n   ");
                        else
                            printf("%c", c);

                    printf("\n\n文件长度：%d 字符。\n", ufd[k]->ufdfile[i].flength);
                    fclose(fp_file);
                    wgetchar = 1;
                } else {
                    // 文件以只写方式打开，无法读取
                    printf("\n错误：文件 '%s' 以只写方式打开，无法读取。\n", fname);
                    wgetchar = 1;
                }
            } else {
                // 文件未打开
                printf("\n错误：文件 '%s' 尚未打开，请先打开该文件。\n", fname);
                wgetchar = 1;
            }
        } else {
            // 文件不存在
            printf("\n错误：文件 '%s' 不存在。\n", fname);
            wgetchar = 1;
        }
    }
}
//---------------------------------------------------------------------------------------------
//以下四个函数需自己实现：
// 删除文件的函数：关闭并删除指定的用户文件，并更新目录和物理地址映射
void DeleteF() {
    int i, j;
    char fname[MAXNAME]; // 文件名

    printf("请输入你要删除的文件名称 :");
    fgets(fname, sizeof(fname), stdin); // 获取文件名
    fname[strcspn(fname, "\n")] = '\0'; // 去掉末尾换行符
    ltrim(rtrim(fname)); // 去除首尾空格

    j = ExistD(dirname); // 获取当前用户目录索引
    i = ExistF(fname);   // 检查文件是否存在于当前目录

    if (i >= 0) {
        if (ifopen[j][i].ifopen == 1) {
            // 如果文件已打开，提示先关闭
            printf("\nError: 文件 '%s' 已经被打开,请先关闭\n", fname);
            wgetchar = 1;
            return;
        }

        // 构造物理文件路径
        char filepath[50];
        strcpy(filepath, "c:\\osfile\\file\\file");
        char str[10];
        itoa(ufd[j]->ufdfile[i].fpaddr, str, 10); // 获取物理地址编号
        strcat(filepath, str);
        strcat(filepath, ".txt");

        // 删除物理文件
        if (remove(filepath) != 0) {
            printf("\nError: 无法删除该文件的物理地址\n");
            wgetchar = 1;
            return;
        }

        // 释放物理地址
        fpaddrno[ufd[j]->ufdfile[i].fpaddr] = 0;

        // 从目录中删除该文件记录
        for (int k = i; k < fcount[j] - 1; k++) {
            ufd[j]->ufdfile[k] = ufd[j]->ufdfile[k + 1];
            ifopen[j][k] = ifopen[j][k + 1];
        }
        fcount[j]--; // 文件数减少

        printf("\n文件 '%s' 删除成功！\n", fname);
    } else {
        printf("\nError: 文件 '%s' 不存在 \n", fname);
        wgetchar = 1;
    }
}

// 打开文件函数：按指定的权限模式打开文件
void OpenF() {
    char fname[MAXNAME]; // 文件名
    char mode[10]; // 模式输入
    int i, j;

    printf("Please input the filename to open: ");


    fgets(fname, sizeof(fname), stdin); // 获取文件名
    fname[strcspn(fname, "\n")] = '\0';
    ltrim(rtrim(fname));

    j = ExistD(dirname); // 当前用户目录索引
    i = ExistF(fname);   // 查找文件

    if (i >= 0) {
        if (ifopen[j][i].ifopen == 1) {
            // 文件已打开
            printf("\nError: File '%s' is already open.\n", fname);
            wgetchar = 1;
            return;
        }

        printf("Please input open mode (0-Read, 1-Write, 2-Read/Write): ");
        fgets(mode, sizeof(mode), stdin); // 获取模式
        mode[strcspn(mode, "\n")] = '\0';
        ltrim(rtrim(mode));

        int openMode = atoi(mode); // 转换为整型
        if (openMode < 0 || openMode > 2) {
            printf("\nError: Invalid open mode.\n");
            wgetchar = 1;
            return;
        }

        // 检查权限匹配
        if ((ufd[j]->ufdfile[i].fmode == 0 && openMode != 0) ||
            (ufd[j]->ufdfile[i].fmode == 1 && openMode == 0)) {
            printf("\nError: File access mode does not match.\n");
            wgetchar = 1;
            return;
        }

        // 构造文件路径并打开文件
        char filepath[50];
        strcpy(filepath, "c:\\osfile\\file\\file");
        char str[10];
        itoa(ufd[j]->ufdfile[i].fpaddr, str, 10);
        strcat(filepath, str);
        strcat(filepath, ".txt");

        if (openMode == 0) {
            fp_file = fopen(filepath, "rb");
        } else if (openMode == 1) {
            fp_file = fopen(filepath, "wb");
        } else {
            fp_file = fopen(filepath, "rb+");
        }

        if (fp_file == NULL) {
            printf("\nError: Failed to open file.\n");
            wgetchar = 1;
            return;
        }

        // 标记打开状态
        ifopen[j][i].ifopen = 1;
        ifopen[j][i].openmode = openMode;

        printf("\nFile '%s' opened successfully in mode %d.\n", fname, openMode);
        loginsuc == 1; // 没有实际作用的语句（逻辑无效）
    } else {
        printf("\nError: File '%s' does not exist.\n", fname);
        wgetchar = 1;
    }
}

// 关闭文件函数：关闭已打开的文件
void CloseF() {
    char fname[MAXNAME];
    int i, j;

    printf("Please input the filename to close: ");
    fgets(fname, sizeof(fname), stdin); // 获取文件名
    fname[strcspn(fname, "\n")] = '\0';
    ltrim(rtrim(fname));

    j = ExistD(dirname); // 当前目录索引
    i = ExistF(fname);   // 文件索引

    if (i >= 0) {
        if (ifopen[j][i].ifopen == 0) {
            printf("\nError: File '%s' is not open.\n", fname);
            wgetchar = 1;
            return;
        }

        fclose(fp_file); // 关闭文件指针
        ifopen[j][i].ifopen = 0; // 清除打开标记
        ifopen[j][i].openmode = 4; // 重置模式

        printf("\nFile '%s' closed successfully.\n", fname);
    } else {
        printf("\nError: File '%s' does not exist.\n", fname);
        wgetchar = 1;
    }
}

// 写文件函数：向已打开的文件中追加内容
void WriteF() {
    char fname[MAXNAME];
    int i, j;

    printf("Please input the filename to write: ");
    fgets(fname, sizeof(fname), stdin); // 获取文件名
    fname[strcspn(fname, "\n")] = '\0';
    ltrim(rtrim(fname));

    j = ExistD(dirname); // 当前用户目录
    i = ExistF(fname);   // 查找文件

    if (i >= 0) {
        if (ifopen[j][i].ifopen != 1 || ifopen[j][i].openmode == 0) {
            printf("\nError: File '%s' is not open for writing.\n", fname);
            wgetchar = 1;
            return;
        }

        // 构造文件路径并以追加方式打开
        char filepath[50];
        strcpy(filepath, "c:\\osfile\\file\\file");
        char str[10];
        itoa(ufd[j]->ufdfile[i].fpaddr, str, 10);
        strcat(filepath, str);
        strcat(filepath, ".txt");

        fp_file = fopen(filepath, "a"); // 以追加方式打开
        if (fp_file == NULL) {
            printf("\nError: Failed to open file for writing.\n");
            wgetchar = 1;
            return;
        }

        printf("Please input text to append (end with '#'):\n");
        char c;
        int added_length = 0;
        while ((c = getchar()) != '#') {
            fputc(c, fp_file); // 写入字符
            if (c != '\n') added_length++; // 统计字符数
        }

        ufd[j]->ufdfile[i].flength += added_length; // 更新文件长度
        fclose(fp_file); // 关闭文件

        printf("\n%d characters appended to file '%s'.\n", added_length, fname);
    } else {
        printf("\nError: File '%s' does not exist.\n", fname);
        wgetchar = 1;
    }
}

//文件共享功能：
void ShareF() {
    char fname[MAXNAME];
    char receiver[MAXNAME];
    char perm_str[10];
    int permission;
    int i, j, k;

    // 检查当前目录是否是用户自己的目录
	if (strcmp(strupr(dirname), strupr(username)) != 0) {
		// printf("\nError:你只能分享你自己拥有的文件.\n");
		wgetchar = 1;
		return;
	}

	printf("\n请输入你想共享的文件: ");
	fgets(fname, sizeof(fname), stdin);
	fname[strcspn(fname, "\n")] = '\0';
	ltrim(rtrim(fname));

	j = ExistD(dirname);
	i = ExistF(fname);

	if (i < 0) {
		printf("\n错误: 文件 '%s' 不存在.\n", fname);
		wgetchar = 1;
		return;
	}

	printf("请输入接收者的用户ID: ");
	fgets(receiver, sizeof(receiver), stdin);
	receiver[strcspn(receiver, "\n")] = '\0';
	ltrim(rtrim(receiver));

	// 检查接收者是否存在
	if (ExistD(receiver) < 0) {
		printf("\n错误: 用户 '%s' 不存在.\n", receiver);
		wgetchar = 1;
		return;
	}

	// 检查是否已经分享给该用户
	for (k = 0; k < share_count; k++) {
		if (strcmp(shares[k].owner, username) == 0 &&
			strcmp(shares[k].filename, fname) == 0 &&
			strcmp(shares[k].receiver, receiver) == 0) {
			printf("\n错误: 文件 '%s' 已经分享给用户 '%s'。\n", fname, receiver);
			wgetchar = 1;
			return;
			}
	}

	printf("请输入权限 (0-只读, 1-可写, 2-读写): ");
	fgets(perm_str, sizeof(perm_str), stdin);
	perm_str[strcspn(perm_str, "\n")] = '\0';
	ltrim(rtrim(perm_str));

	permission = atoi(perm_str);
	if (permission < 0 || permission > 2) {
		printf("\n错误: 权限值无效。\n");
		wgetchar = 1;
		return;
	}

	// 添加分享记录（第一段冗余逻辑建议删掉，保留带时间的）
	if (share_count < MAX) {
		strcpy(shares[share_count].owner, username);
		strcpy(shares[share_count].filename, fname);
		strcpy(shares[share_count].receiver, receiver);
		shares[share_count].permission = permission;
		shares[share_count].share_time = time(NULL); // 记录当前时间
		share_count++;

		printf("\n文件 '%s' 已成功共享给用户 '%s'，权限为：%s，时间：%s\n",
			   fname, receiver,
			   PermissionToString(permission),
			   FormatTime(shares[share_count-1].share_time));
	} else {
		printf("\n错误: 分享列表已满，无法继续添加。\n");
		wgetchar = 1;
	}
}

// 检查文件是否存在于指定用户的UFD中
int ExistFInUFD(int ufd_idx, char *filename) {
	int i;
	for (i = 0; i < fcount[ufd_idx]; i++) {
		if (strcmp(ufd[ufd_idx]->ufdfile[i].fname, filename) == 0) {
			return i;
		}
	}
	return -1;
}

//列出分享的文件的状态
void ListShares() {
	int i, count = 0;

	printf("\n共享文件信息:\n");
	printf("========================================================================================\n");
	printf("%-20s %-20s %-20s %-15s %-20s %s\n",
		   "拥有者", "文件名", "接收者", "权限", "共享时间", "状态");
	printf("========================================================================================\n");

	// 显示我分享给别人的文件
	printf("\n我（%s）分享的文件:\n", username);
	for (i = 0; i < share_count; i++) {
		if (strcmp(shares[i].owner, username) == 0) {
			int receiver_idx = ExistD(shares[i].receiver);
			int file_exists = (ExistFInUFD(ExistD(username), shares[i].filename) >= 0);

			printf("%-15s %-15s %-15s %-12s %-20s %s\n",
				   shares[i].owner,
				   shares[i].filename,
				   shares[i].receiver,
				   PermissionToString(shares[i].permission),
				   FormatTime(shares[i].share_time),
				   file_exists ? "有效" : "已删除");
			count++;
		}
	}

	// 显示别人分享给我的文件
	printf("\n别人分享给我（%s）的文件:\n", username);
	for (i = 0; i < share_count; i++) {
		if (strcmp(shares[i].receiver, username) == 0) {
			int owner_idx = ExistD(shares[i].owner);
			int file_exists = (owner_idx >= 0) &&
							(ExistFInUFD(owner_idx, shares[i].filename) >= 0);

			printf("%-15s %-15s %-15s %-12s %-20s %s\n",
				   shares[i].owner,
				   shares[i].filename,
				   shares[i].receiver,
				   PermissionToString(shares[i].permission),
				   FormatTime(shares[i].share_time),
				   file_exists ? "有效" : "已删除");
			count++;
		}
	}

	printf("\n共享文件总数: %d\n", count);
	printf("========================================================================================\n");
}

// 读取共享文件内容的函数，根据拥有者和文件名读取文件内容并输出到控制台
void ReadSharedFile(char *owner, char *fname) {
    int j = ExistD(owner); // 获取文件拥有者对应的目录索引
    int i = ExistFInUFD(j, fname); // 在该目录中查找文件索引

    if (i < 0) {
        printf("\n错误：文件 '%s' 不存在于用户 %s 的目录中。\n", fname, owner);
        wgetchar = 1;
        return;
    }

    char filepath[50]; // 定义文件路径字符串
    strcpy(filepath, "c:\\osfile\\file\\file"); // 拼接基础路径
    char str[10];
    itoa(ufd[j]->ufdfile[i].fpaddr, str, 10); // 将物理地址转为字符串
    strcat(filepath, str); // 添加地址到路径
    strcat(filepath, ".txt"); // 添加文件后缀

    FILE *read_fp = fopen(filepath, "r"); // 以只读方式打开文件
    if (read_fp == NULL) {
        printf("\n错误：无法打开文件进行读取。\n");
        wgetchar = 1;
        return;
    }

    printf("\n读取共享文件 '%s'（来自用户 '%s'）的内容：\n", fname, owner);
    printf("--------------------------------\n");
    char ch;
    while ((ch = fgetc(read_fp)) != EOF) { // 逐字符读取并输出
        putchar(ch);
    }
    printf("\n--------------------------------\n");

    fclose(read_fp); // 关闭文件
}

// 写入共享文件内容的函数，在文件末尾追加用户输入的内容
void WriteSharedFile(char *owner, char *fname) {
    int j = ExistD(owner); // 获取文件拥有者对应的目录索引
    int i = ExistFInUFD(j, fname); // 在该目录中查找文件索引

    if (i < 0) {
        printf("\n错误：文件 '%s' 不存在于用户 %s 的目录中。\n", fname, owner);
        wgetchar = 1;
        return;
    }

    char filepath[50];
    strcpy(filepath, "c:\\osfile\\file\\file");
    char str[10];
    itoa(ufd[j]->ufdfile[i].fpaddr, str, 10);
    strcat(filepath, str);
    strcat(filepath, ".txt");

    FILE *write_fp = fopen(filepath, "a"); // 以追加方式打开文件
    if (write_fp == NULL) {
        printf("\n错误：无法打开文件进行写入。\n");
        wgetchar = 1;
        return;
    }

    printf("\n请输入要追加的内容（以 '#' 结束）：\n");
    char c;
    int added_length = 0;
    while ((c = getchar()) != '#') { // 读取直到'#'结束
        fputc(c, write_fp);
        if (c != '\n') added_length++;
    }

    ufd[j]->ufdfile[i].flength += added_length; // 更新文件长度
    fclose(write_fp); // 关闭文件

    printf("\n已向共享文件 '%s' 追加了 %d 个字符。\n", fname, added_length);
}

// 打开共享文件的函数，根据权限以不同模式打开文件
void OpenSharedFile(char *owner, char *fname, int permission) {
    int j = ExistD(owner); // 获取目录索引
    int i = ExistFInUFD(j, fname); // 获取文件索引

    if (i < 0) {
        printf("\n错误：文件 '%s' 不存在于用户 %s 的目录中。\n", fname, owner);
        wgetchar = 1;
        return;
    }

    if (ifopen[j][i].ifopen == 1) { // 检查是否已打开
        printf("\n错误：文件 '%s' 已被其拥有者打开。\n", fname);
        wgetchar = 1;
        return;
    }

    char filepath[50];
    strcpy(filepath, "c:\\osfile\\file\\file");
    char str[10];
    itoa(ufd[j]->ufdfile[i].fpaddr, str, 10);
    strcat(filepath, str);
    strcat(filepath, ".txt");

    // 根据权限选择打开模式
    if (permission == SHARE_READ) {
        fp_file = fopen(filepath, "rb");
    } else if (permission == SHARE_WRITE) {
        fp_file = fopen(filepath, "wb");
    } else {
        fp_file = fopen(filepath, "rb+");
    }

    if (fp_file == NULL) {
        printf("\n错误：打开共享文件失败。\n");
        wgetchar = 1;
        return;
    }

    printf("\n共享文件 '%s' 打开成功，当前模式：%s。\n",
           fname, permission == SHARE_READ ? "只读" :
           (permission == SHARE_WRITE ? "写入" : "读写"));
}

// 加载共享记录的函数，从文件中读取所有共享信息到内存数组
void LoadShares() {
	FILE *fp_share = fopen("c:\\osfile\\shares.txt", "rb"); // 以二进制读取方式打开共享记录文件
	if (fp_share != NULL) {
		share_count = 0;
		while (fread(&shares[share_count], sizeof(OSSHARE), 1, fp_share) == 1) {
			share_count++;
			if (share_count >= MAX) break; // 防止溢出
		}
		fclose(fp_share);
		printf("\n已加载 %d 条共享记录。\n", share_count);
	}
}

// 保存共享记录的函数，将共享数组写入文件以持久化
void SaveShares() {
	FILE *fp_share = fopen("c:\\osfile\\shares.txt", "wb"); // 以二进制写方式保存共享记录
	if (fp_share != NULL) {
		for (int i = 0; i < share_count; i++) {
			fwrite(&shares[i], sizeof(OSSHARE), 1, fp_share);
		}
		fclose(fp_share);
		printf("\n已保存 %d 条共享记录。\n", share_count);
	}
}

// 检查某用户目录下是否存在指定文件，返回其索引
int ExistFInOtherUser(char *username, char *filename) {
	int user_idx = ExistD(username); // 获取用户目录索引
	if (user_idx < 0) return -1;

	for (int i = 0; i < fcount[user_idx]; i++) {
		if (strcmp(ufd[user_idx]->ufdfile[i].fname, filename) == 0) {
			return i; // 找到文件返回索引
		}
	}
	return -1; // 未找到返回 -1
}
// 访问共享文件的函数：根据权限允许用户对共享的文件进行读取、写入或打开操作
void AccessSharedFile() {
    char owner[MAXNAME]; // 拥有者用户名
    char fname[MAXNAME]; // 文件名
    int i;
    char action[10]; // 用户操作选择

    printf("\n请输入文件拥有者的用户名：");
    fgets(owner, sizeof(owner), stdin); // 读取用户名
    owner[strcspn(owner, "\n")] = '\0'; // 去除换行符
    ltrim(rtrim(owner)); // 去除前后空格
	strupr(owner); // 转为大写

    printf("请输入文件名：");
    fgets(fname, sizeof(fname), stdin); // 读取文件名
    fname[strcspn(fname, "\n")] = '\0';
    ltrim(rtrim(fname));
	strupr(fname);

    int share_idx = -1; // 初始化共享记录索引
    for (i = 0; i < share_count; i++) {
        if (strcmp(shares[i].owner, owner) == 0 &&
            strcmp(shares[i].filename, fname) == 0 &&
            strcmp(shares[i].receiver, username) == 0) {
            share_idx = i; // 找到匹配的共享记录
            break;
        }
    }

    if (share_idx == -1) {
        printf("\n错误：没有找到来自用户 '%s' 的文件 '%s' 的共享记录。\n", owner, fname);
        wgetchar = 1;
        return;
    }

    int file_idx = ExistFInOtherUser(owner, fname); // 检查文件是否仍存在
    if (file_idx < 0) {
        printf("\n错误：文件 '%s' 已不再存在于用户 %s 的目录中。\n", fname, owner);
        wgetchar = 1;
        return;
    }

    printf("\n你希望对该共享文件进行什么操作？\n");
    printf("1. 读取\n");
    printf("2. 写入\n");
    printf("3. 打开\n");
    printf("请选择操作（1-3）：");

    fgets(action, sizeof(action), stdin); // 获取用户选择
    action[strcspn(action, "\n")] = '\0';
    ltrim(rtrim(action));

    int choice = atoi(action); // 转换为整数
    if (choice < 1 || choice > 3) {
        printf("\n错误：选择无效。\n");
        wgetchar = 1;
        return;
    }

    // 检查是否有权限执行操作
    if ((choice == 1 && shares[share_idx].permission == SHARE_WRITE) ||
        (choice == 2 && shares[share_idx].permission == SHARE_READ)) {
        printf("\n错误：你没有足够的权限执行此操作。\n");
        wgetchar = 1;
        return;
    }

    // 执行对应操作
    switch (choice) {
        case 1:
            ReadSharedFile(owner, fname);
            break;
        case 2:
            WriteSharedFile(owner, fname);
            break;
        case 3:
            OpenSharedFile(owner, fname, shares[share_idx].permission);
            break;
    }
}

// 取消共享文件的函数：从共享记录中删除指定的共享关系
void UnshareF() {
	char fname[MAXNAME]; // 要取消共享的文件名
	char receiver[MAXNAME]; // 接收者用户名

	printf("\n请输入要取消共享的文件名：");
	fgets(fname, sizeof(fname), stdin);
	fname[strcspn(fname, "\n")] = '\0'; // 去除换行符
	ltrim(rtrim(fname)); // 去除空格
	strupr(fname);

	printf("请输入接收者用户名：");
	fgets(receiver, sizeof(receiver), stdin);
	receiver[strcspn(receiver, "\n")] = '\0';
	ltrim(rtrim(receiver));
	strupr(receiver);
	// 遍历共享记录，查找匹配项
	for (int i = 0; i < share_count; i++) {
		if (strcmp(shares[i].owner, username) == 0 &&
			strcmp(shares[i].filename, fname) == 0 &&
			strcmp(shares[i].receiver, receiver) == 0) {
			// 找到后将其后面的记录向前移动覆盖
			for (int j = i; j < share_count - 1; j++) {
				shares[j] = shares[j + 1];
			}
			share_count--; // 共享数量减少
			printf("\n共享记录取消成功。\n");
			return;
		}
	}

	printf("\n错误：未找到匹配的共享记录。\n");
	wgetchar = 1;
}
//修复分享的时间
void FixAllInvalidTimes() {
	time_t now = time(NULL);
	int fixed = 0;

	for (int i = 0; i < share_count; i++) {
		if (shares[i].share_time < 315532800) { // 早于1980年视为无效
			shares[i].share_time = now;
			fixed++;

			printf("已修复: %s 分享给 %s 的文件 %s (原时间: %s)\n",
				  shares[i].owner,
				  shares[i].receiver,
				  shares[i].filename,
				  FormatTime(shares[i].share_time));
		}
	}

	if (fixed > 0) {
		SaveShares();
		printf("\n共修复了 %d 条无效时间记录\n", fixed);
	} else {
		printf("\n没有发现无效的时间记录\n");
	}
}

void QuitF() //退出函数
{
	int i,j;
	char str[50];
	SetPANo(1);
	if (fp_mfd!=NULL) fclose(fp_mfd);
	if (fp_ufd!=NULL) fclose(fp_ufd);
	if (fp_file!=NULL) fclose(fp_file);
	for(j=0;j<ucount;j++)
	{
		strcpy(str,"c:\\osfile\\");
		strcat(str,ufd[j]->ufdname);
		ltrim(rtrim(str));
		strcat(str,".txt");
		fp_ufd=fopen(str,"wb");
		fclose(fp_ufd);
		fp_ufd=fopen(str,"ab");
		for(i=0;i<fcount[j];i++)
			fwrite(&ufd[j]->ufdfile[i],sizeof(OSFILE),1,fp_ufd);
		fclose(fp_ufd);
	}
	SaveShares();
}
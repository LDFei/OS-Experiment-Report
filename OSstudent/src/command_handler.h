//
// Created by Heng on 25-6-20.
//

#ifndef COMMAND_HANDELER_H

#define COMMAND_HANDELER_H
#include <macros.h>
#include <stdio.h>


// 文件分享记录结构
typedef struct {
    char owner[MAXNAME];      // 文件所有者
    char filename[MAXNAME];   // 被分享的文件名
    char receiver[MAXNAME];   // 接收分享的用户名
    int permission;          // 分享权限
    time_t share_time;       // 分享时间
} OSSHARE;



typedef struct  /* OS文件结构体 */
{
    int  fpaddr;                /* 文件物理地址 */
    int  flength;               /* 文件长度 */
    int  fmode;   /* 文件模式：0-只读；1-只写；2-读写；3-保护；*/
    char fname[MAXNAME];        /* 文件名 */
} OSFILE;

typedef struct     /* OS用户文件目录结构体 */
{
    char ufdname[MAXNAME];   /* 用户文件目录名 */
    OSFILE ufdfile[MAXCHILD];   /* 用户文件目录拥有的文件 */
} OSUFD;

typedef struct  /* OS用户文件目录登录结构体 */
{
    char ufdname[MAXNAME];       /* 用户文件目录名 */
    char ufdpword[8];            /* 用户文件目录密码 */
} OSUFD_LOGIN;

typedef struct     /* 文件打开模式结构体 */
{
    int ifopen; /* 是否打开：0-关闭，1-打开 */
    int openmode; /* 打开模式：0-只读，1-只写，2-读写，3-初始化 */
} OSUFD_OPENMODE;


void clrscr();
// 全局变量
extern OSSHARE shares[MAX];   // 分享记录数组
extern int share_count;       // 当前分享记录数


extern OSUFD *ufd[MAXCHILD];   /*ufd and ufd own files*/
extern OSUFD_LOGIN ufd_lp;
extern int ucount;  /*the count of mfd's ufds*/
extern int fcount[MAXCHILD];  /*the count of ufd's files*/
extern int loginsuc; /*whether login successfully*/
extern char username[MAXNAME];  /*record login user's name22*/
extern char dirname[MAXNAME];/*record current directory*/
extern int fpaddrno[MAX];  /*record file physical address num*/
extern OSUFD_OPENMODE ifopen[MAXCHILD][MAXCHILD]; /*record file open/close*/
extern int wgetchar; /*whether getchar()*/
extern FILE *fp_mfd;//主文件目录（Master File Directory）的文件指针,对应 c:\osfile\mfd.txt，
extern FILE *fp_ufd;//用户文件目录（User File Directory）的文件指针，c:\osfile\用户名.txt。
extern FILE *fp_file_p;//文件物理地址记录的文件指针，c:\osfile\file\file_p.txt
extern FILE *fp_file;//正在操作的“具体文件”的文件指针，c:\osfile\file\fileX.txt
void LoginF();  /*LOGIN FileSystem*/
void DirF();  /*Dir FileSystem*/
void CdF();  /*Change Dir*/
void CreateF();  /*Create File*/
void DeleteF(); /*Delete File*/
void ModifyFM(); /*Modify FileMode*/
void OpenF();  /*Open File*/
void CloseF();  /*Close File*/
void ReadF(); /*Read File*/
void WriteF(); /*Write File*/
void QuitF(); /*Quit FileSystem*/
void help();
void InputPW(char *password);  /*input password,use '*' replace*/
void SetPANo(int RorW);  /*Set physical address num*/
int ExistD(char *dirname);  /*Whether DirName Exist,Exist-i,Not Exist-0*/
int WriteF1(); /*write file*/
int ExistF(char *filename);  /*Whether FileName Exist,Exist-i,Not Exist-0*/
int FindPANo();  /*find out physical address num*/


//共享文件函数声明
void ShareF();
void ListShares();
void AccessSharedFile();
void LoadShares(); // 加载共享记录
void  ReadSharedFile(char *owner, char *fname);
void OpenSharedFile(char *owner, char *fname, int permission);
void LoadShares();
void SaveShares();
int ExistFInOtherUser(char *username, char *filename);
void UnshareF();
#endif //COMMAND_HANDELER_H

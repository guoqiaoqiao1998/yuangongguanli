#ifndef __HEAD_H__
#define __HEAD_H__
#include <sqlite3.h>
#include <sys/types.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sqlite3.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h> 

#define ERR_MSG(msg)  do{\
	fprintf(stderr,"__%d__:",__LINE__);\
	perror(msg);\
	return (-1);\
}while(0)

#define PORT 8888             //
#define IP "192.168.250.100 "    //IP      


typedef struct  //登陆信息
{
    int  type;//判断是否为管理员
    char name[20];  
    char passwd[20];
}USER;
typedef struct   //员工信息
{
	int id;
	int type;
    char name[20];//姓名
    int  age;//年龄
    double  salary;//工资
    char phone[20];//手机号
	char addr[20];
}INFO;
typedef struct
{
	USER user;
	INFO info;
	int n; 
	int no;
	int flags;
	sqlite3 *db;
	int type;
}MSG;

//服务器功能函数

//登录函数
int land(int nfd,MSG msg,sqlite3 *db);

//子进程处理函数
int rcv_cli_msg(int nfd,struct sockaddr_in cin,sqlite3 *db);

//管理员查询函数
int quily(int nfd,MSG msg,sqlite3 *db);

//添加用户函数
int add(int nfd,MSG msg,sqlite3 *db);

//删除用户函数
int deletes(int nfd,MSG msg,sqlite3 *db);

//用户查询函数
int user_quily(int nfd,MSG msg,sqlite3 *db);

//更改函数
int update(int nfd,MSG msg,sqlite3 *db);
//

int serch_ls(int nfd,MSG msg,sqlite3 *db);

#endif

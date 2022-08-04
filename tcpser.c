#include "head.h"



typedef void (*sighandler_t)(int);

void handler(int sig)
{
	while(waitpid(-1,NULL,WNOHANG)>0);
	return;
}

int main(int argc, const char *argv[])
{


	sqlite3* db = NULL;
	if(sqlite3_open("./my.db",&db) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_open:%s\n",__LINE__,sqlite3_errmsg(db));
		fprintf(stderr,"__%d__ sqlite3_open:%d\n",__LINE__,sqlite3_errcode(db));
		return -1;
	}
	printf("open ***.db Ok\n");

	char sql[128] = "create table if not exists inf(id int primary key,type int,name char,int age,menoy float,phone char,addr char)";
	char *errmsg = NULL;
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
	}
	printf("create inf  tabie OK\n");
	char sql1[128] = "create table if not exists pass(name char,pass char,type int)";
	if(sqlite3_exec(db,sql1,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
	}
	printf("create pass  tabie OK\n");

	char sql2[128] = "create table if not exists ls(time char,user char,mod char)";
	if(sqlite3_exec(db,sql1,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		fprintf(stderr,"__%d__ sqlite3_exec:%s\n",__LINE__,errmsg);
		return -1;
	}

	printf("create ls  tabie OK\n");

	sighandler_t s = signal(17,handler);

	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd < 0)
		ERR_MSG("socket");

	//允许端口快速重用
	int reuse = 1;
	if(setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
		ERR_MSG("setsockopt");

	//绑定地址信息结构体
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);

	if(bind(fd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
		ERR_MSG("bind");

	if(listen(fd,10) < 0)
		ERR_MSG("listen");
	printf("server init  OK\n");
	struct sockaddr_in cin;
	socklen_t addrlen = sizeof(cin);

	int pid;
	while(1)
	{
		int nfd = accept(fd,(struct sockaddr*)&cin,&addrlen);
		if(nfd < 0)
			ERR_MSG("accept");
		printf("[%s--%d]=%d 客户端链接\n",inet_ntoa(cin.sin_addr),ntohs(cin.sin_port),nfd);
		pid = fork();
		if(pid == 0)
		{
			close(fd);
			rcv_cli_msg(nfd,cin,db);
			exit(0);
		}

	}
	close(fd);
	return 0;
}

int rcv_cli_msg(int nfd,struct sockaddr_in cin,sqlite3 * db)//子进程处理函数
{
	printf("accept ok\n");
	MSG msg;
	int k[10] = {0};
	ssize_t rec = 0;
	while(1)
	{
		rec = recv(nfd,&msg,sizeof(msg),0);
		if(0 == rec)
		{
			printf("%d:客户端退出\n",ntohs(cin.sin_port));
			return 0;
		}
		else if(rec < 0)
			ERR_MSG("recv");
		else
			printf("接收  ssucese\n");

		printf("%d\n",msg.n);

		switch(msg.n)
		{

		case 0:
			printf("**\n");
			k[0] = land(nfd,msg,db );
			printf("K=%d\n",k[0]);
			if(send(nfd,k,sizeof(k),0) < 0)
				ERR_MSG("send");
			printf("发送完成\n");
			break;
		case 1:
			quily(nfd,msg,db);
			break;
		case 2:
			update(nfd,msg,db);
			break;
		case 3:
			add(nfd,msg,db);
			break;
		case 4:deletes(nfd,msg,db);
			   break;
		case 5:
			   serch_ls(nfd,msg,db);
			   break;
		case 6:
			   user_quily(nfd,msg,db);
			   break;

		default:printf("无效\n");
		}

	}
	close(nfd);
	return 0;
	char sql[128];
}
int land(int nfd,MSG msg,sqlite3 *db)//登录函数
{

	printf(" %d\n",__LINE__);
	char sql[128] ;
	char** pres = NULL; 	//存储查询到的结果
	int row, column;
	char* errmsg = NULL;
	int flag = 1;
	//	sprintf(sql,"select *from pass ");
	printf("%s\n",msg.user.name);
	/*	 if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg)!=0)
		 {
		 printf("sqlite3_get_table   -1");
		 return 0;
		 }
		 for(int i =0;i<(row+1)*column;i+=column)
		 {
		 printf("%10s",pres[i]);
		 }

*/	

	sprintf(sql,"select *from pass where name = '%s'and pass= '%s' and type = '%d';",msg.user.name,msg.user.passwd,msg.user.type);
	if(sqlite3_get_table(db, sql, &pres, &row, &column, &errmsg) != SQLITE_OK)
	{
		fprintf(stderr, "__%d__ sqlite3_get_table:%s\n", __LINE__, errmsg);
		return -1;
	}
	printf("%d\n",row);
	if(row == 1)
	{
		flag = 0;
	}
	return flag;
}

int user_quily(int nfd,MSG msg,sqlite3 *db)//用户查询函数
{
	printf(" %d\n",__LINE__);
	char sql[128];
	static char **pres = NULL;
	int column,row;
	int rec;
	msg.flags = 0;
	char*errmsg = NULL;
	sprintf(sql,"select * from inf where name='%s';",msg.user.name);
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg) != 0)
		ERR_MSG("errmsg");
			msg.info.id= *pres[column]-48;
				msg.info.type= *pres[column+1] -48;
				strcpy(msg.info.name,pres[column+2]);
				msg.info.age= atoi(pres[column+3]);
				msg.info.salary= atol(pres[column+4]);
				strcpy(msg.info.phone,pres[column+5]);
				strcpy(msg.info.addr,pres[column+6]);

				msg.flags=1;
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");
		printf("发送完成\n");

		return 0;

	
}

int serch_ls(int nfd,MSG msg,sqlite3 *db)//查询历史记录
{
	typedef struct {
		char buf1[128];
		char buf2[128];
		char buf3[128];
	}cm;
	printf(" %d\n",__LINE__);
	int j=0;
	 cm str[128] ={0};
	char sql[128] = {"select *from ls"};
	static char **pres = NULL;
	int column,row;
	char*errmsg = NULL;
	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg) != 0)
		ERR_MSG("errmsg");
	for(int i=column;i<(row+1)*column;i+=column,j++)
	{
		strcpy(str[j].buf1,pres[i]);
		strcpy(str[j].buf2,pres[i+1]);
		strcpy(str[j].buf3,pres[i+2]);
		printf("i= %d\n",i);
		printf(" %d\n",(row+1)*column);
	}
	printf(" %d\n",__LINE__);
		if(send(nfd,str,sizeof(str),0) < 0)
			ERR_MSG("send");

		return 0;
}


int quily(int nfd,MSG msg,sqlite3 *db)//管理员查询函数
{
	printf(" %d\n",__LINE__);
	char buf[128];
	char sql[128]={"select *from inf"};
	static char **pres = NULL;
	int column,row;
	int rec;
	int i,j;
	msg.flags = 0;
	char*errmsg = NULL;

	if(sqlite3_get_table(db,sql,&pres,&row,&column,&errmsg) != 0)
		ERR_MSG("errmsg");
	rec = recv(nfd,&msg,sizeof(msg),0);
	if(0 == rec)
	{
		printf("%d:客户端退出\n",nfd);
		return -1;
	}
	else if(rec < 0)
		ERR_MSG("recv");
	printf("%d\n",msg.no);
	if(msg.no == 1)
	{
		printf(" %d\n",__LINE__);
		for(i=2;i<(row+1)*column;i+=column)
		{
			if(strcmp(msg.info.name,pres[i]) == 0)
			{

				msg.info.id= atoi(pres[i-2]);
				msg.info.type= atoi(pres[i-1]) ;
				strcpy(msg.info.name,pres[i]);
				msg.info.age= atoi(pres[i+1]);
				msg.info.salary= atol(pres[i+2]);
				strcpy(msg.info.phone,pres[i+3]);
				strcpy(msg.info.addr,pres[i+4]);
				msg.flags = 1;
			}
		}
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");
		printf("发送完成\n");
	}
	if(msg.no == 2)
	{
		printf(" %d\n",__LINE__);
		for(i=column;i<(row+1)*column;i+=column)
		{
			printf("%d\n",i);

			printf(" %d\n",__LINE__);
			if( msg.info.id == atoi(pres[i]))
			{


				msg.info.id= atoi(pres[i]);
				msg.info.type= atoi(pres[i+1]);
				strcpy(msg.info.name,pres[i+2]);
				msg.info.age= atoi(pres[i+3]);
				msg.info.salary= atol(pres[i+4]);
				strcpy(msg.info.phone,pres[i+5]);
				strcpy(msg.info.addr,pres[i+6]);

				msg.flags =1;

			}
		} 



		for(int j=0; j<(row+1)*column; j++)
		{
			printf("%8s\t",pres[j]);
			if(j%column == (column-1))
				putchar(10);
		}
		printf(" %d\n",__LINE__);
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");
		printf("发送完成\n");
	}
	if(msg.no == 3)
	{
		printf(" %d\n",__LINE__);
		 MSG pmsg[20] = {0};
		printf(" %d\n",__LINE__);
		for(i=column,j=0;i<(row+1)*column;i+=column,j++)
		{
			
				pmsg[j].info.id= atoi(pres[i]);
				pmsg[j].info.type= atoi(pres[i+1]);
				strcpy(pmsg[j].info.name,pres[i+2]);
				pmsg[j].info.age= atoi(pres[i+3]);
				pmsg[j].info.salary= atol(pres[i+4]);
				strcpy(pmsg[j].info.phone,pres[i+5]);
				strcpy(pmsg[j].info.addr,pres[i+6]);
				printf("i=%d\n",i);
		}
		printf(" %d\n",__LINE__);
		if(send(nfd,pmsg,sizeof(pmsg),0) < 0)
			ERR_MSG("send");
		printf("发送完成\n");
	}

	return 0;
}
int addls(MSG msg,sqlite3 *db,char buf[128])//添加历史记录
{
	time_t ti;
	struct tm* t_info;
	char time_c[128];
	char*errmsg = NULL;
	char sql[128];
	
	ti = time(NULL);
           t_info = localtime(&ti);
            sprintf(time_c,"%d-%02d-%02d %02d-%02d-%02d", \
                    t_info->tm_year+1900, t_info->tm_mon+1, t_info->tm_mday,\
                    t_info->tm_hour, t_info->tm_min, t_info->tm_sec);
	bzero(sql,sizeof(sql));
	sprintf(sql,"INSERT INTO ls VALUES('%s','%s','%s');",time_c,msg.user.name,buf);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		printf("add inf error\n");
		return -1;
	}

}

int add(int nfd,MSG msg,sqlite3 *db)//添加用户
{
	printf(" %d\n",__LINE__);
	char sql[128];
	char buf[128];
	msg.flags = 1;
	char*errmsg = NULL;
	sprintf(sql,"insert into pass values('%s','%s','%d');",msg.info.name,msg.user.passwd,msg.info.type);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		printf("add pass error\n");
		return -1;
	}
	sprintf(sql,"insert into inf values('%d','%d','%s','%d','%f','%s','%s');",msg.info.id,msg.info.type,msg.info.name,msg.info.age,msg.info.salary,msg.info.phone,msg.info.addr);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		printf("add inf error\n");
		return -1;
	}
	sprintf(buf,"增加了%s 成员",msg.info.name);
	addls(msg,db,buf);
	msg.flags=0;
	if(send(nfd,&msg,sizeof(msg),0) < 0)
		ERR_MSG("send");
	return 0;

}

int deletes(int nfd,MSG msg,sqlite3 *db)//删除用户
{
	printf(" %d\n",__LINE__);
	char sql[128];
	char buf[128];
	msg.flags = 1;
	char*errmsg = NULL;
	sprintf(sql,"delete from inf where name = \"%s\";",msg.info.name);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");
		printf("delete inf error\n");
		return -1;
	}
	bzero(sql,sizeof(sql));
	sprintf(sql,"delete from pass where name= \'%s\';",msg.info.name);
	printf("%s\n",msg.user.name);
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");
		printf("delete pass error\n");
		return -1;
	}
	msg.flags=0;
	if(send(nfd,&msg,sizeof(msg),0) < 0)
		ERR_MSG("send");
	bzero(sql,sizeof(sql));
	sprintf(buf,"删除了%s 成员",msg.info.name);
	addls(msg,db,buf);
	return 0;
}
int update(int nfd,MSG msg,sqlite3 *db)//修改用户信息
{
	printf(" %d\n",__LINE__);
	char sql[128];
	msg.flags = 1;
	char buf[128]="";
	char*errmsg = NULL;
	if(msg.type == 1)
	{
		switch(msg.no)
		{
			case 1:
				sprintf(sql,"update inf set int ='%d' where name = '%s';",msg.info.age,msg.info.name);
				sprintf(buf,"修改 %s 年龄为%d",msg.info.name,msg.info.age);
				break;
			case 2:
				sprintf(sql,"update inf set menoy ='%lf' where name = '%s';",msg.info.salary,msg.info.name);
				sprintf(buf,"修改 %s 工资为%f",msg.info.name,msg.info.salary);
				break;
			case 3:
				sprintf(sql,"update inf set phone ='%s' where name = '%s';",msg.info.phone,msg.info.name);
				sprintf(buf,"修改 %s 手机号为%s",msg.info.name,msg.info.phone);
				break;
	char sql[128];
			case 4:
				sprintf(sql,"update inf set addr ='%s' where name = '%s';",msg.info.addr,msg.info.name);
				sprintf(buf,"修改 %s 地址为%s",msg.info.name,msg.info.addr);
				break;
			case 5:
				sprintf(sql,"update pass set passwd ='%s' where name = '%s';",msg.user.passwd,msg.info.name);
				sprintf(buf,"修改 %s 密码为%s",msg.info.name,msg.user.passwd);
				break;
		}
	}
	else
	{
			printf("%d\n",__LINE__);
		switch(msg.no)
		{
			case 1:
				sprintf(sql,"update inf set int ='%d' where name = '%s';",msg.info.age,msg.user.name);
				sprintf(buf,"修改 %s 年龄为%d",msg.user.name,msg.info.age);
				break;
			case 2:
				sprintf(sql,"update inf set phone ='%s' where name = '%s';",msg.info.phone,msg.user.name);
				sprintf(buf,"修改 %s 手机号为%s",msg.user.name,msg.info.phone);
				break;
			case 3:
				sprintf(sql,"update inf set addr ='%s' where name = '%s';",msg.info.addr,msg.user.name);
				sprintf(buf,"修改 %s 地址为%s",msg.user.name,msg.info.addr);
				break;
			case 4:
				sprintf(sql,"update pass set passwd ='%s' where name = '%s';",msg.user.passwd,msg.user.name);
				sprintf(buf,"修改 %s 密码为%s",msg.user.name,msg.user.passwd);
				break;
		}
	}
	
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg) != SQLITE_OK)
	{
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");
		printf("updata pass error\n");
		return -1;
	}
	bzero(sql,sizeof(sql));
	msg.flags = 0;
		if(send(nfd,&msg,sizeof(msg),0) < 0)
			ERR_MSG("send");

	addls(msg,db,buf);

		return 0;
}

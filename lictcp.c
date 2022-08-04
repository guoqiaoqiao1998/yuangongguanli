#include "head.h"


int luser_find(int fd,MSG msg)
{
	int rec;
	if(send(fd,&msg,sizeof(msg),0) < 0)
	{
		ERR_MSG("send");
	}
	rec = recv(fd,&msg,sizeof(msg),0);
	if(rec < 0)
	{
		ERR_MSG("recv");
	}
	else if(0==rec)
	{
		printf("服务器退出\n");
		return -1;
	}
	if(msg.flags != 1)
	{
		printf("查询失败\n");
		return -1;
	}


	printf("工号:%d\t类型:%d\t姓名:%s\t年龄:%d\t工资:%f\t手机号:%s\t地址:%s\n",
			msg.info.id,msg.info.type,msg.info.name,(msg.info.age),msg.info.salary,msg.info.phone,msg.info.addr);
	return 0;
}
int lfind(int fd,MSG msg)
{
	sqlite3*db;
	char buf[128] = "select * from inf";
	MSG pmsg[20] = {0};
	int i;
	int rec;
	int row,column;
	char **pres;
	scanf("%d",&msg.no);
	if(send(fd,&msg,sizeof(msg),0) < 0)
	{
		ERR_MSG("send");
	}
	switch(msg.no)
	{
	case 1:
		printf("请输入要查询的名字>>");
		//	fgets(buf,sizeof(buf),stdin);
		scanf("%s",msg.info.name);
		getchar();
		if(send(fd,&msg,sizeof(msg),0) < 0)
		{
			ERR_MSG("send");
		}

		rec = recv(fd,&msg,sizeof(msg),0);
		if(rec < 0)
		{
			ERR_MSG("recv");
		}
		else if(0==rec)
		{
			printf("服务器退出\n");
			return -1;
		}
		if(msg.flags ==0)
		{
			printf("失败\n");
			break;
		}
		printf("工号:%d\t类型:%d\t姓名:%s\t\t年龄:%d\t工资:%f\t手机号:%s\t\t\t地址:%s\n",
				msg.info.id,msg.info.type,msg.info.name,(msg.info.age),msg.info.salary,msg.info.phone,msg.info.addr);
		break;
	case 2:
		printf("请输入要查询的id>>>");
		scanf("%d",&msg.info.id);
		getchar();
		printf("输入完成\n");
		if(send(fd,&msg,sizeof(msg),0) < 0)
		{
			ERR_MSG("send");
		}
		printf("发送完成\n");
		rec = recv(fd,&msg,sizeof(msg),0);
		if(rec < 0)
		{
			ERR_MSG("recv");
		}
		else if(0==rec)
		{
			printf("服务器退出\n");
			return -1;
		}
		if(msg.flags ==0)
		{
			printf("失败\n");
			break;
		}
		printf("工号:%d\t类型:%d\t姓名:%s\t年龄:%d\t工资:%f\t手机号:%s\t地址:%s\n",msg.info.id,msg.info.type,msg.info.name,(msg.info.age),msg.info.salary,msg.info.phone,msg.info.addr);

		break;
	case 3:
		if(send(fd,&msg,sizeof(msg),0) < 0)
		{
			ERR_MSG("send");
		}
		rec = recv(fd,pmsg,sizeof(pmsg),0);
		if(rec < 0)
		{
			ERR_MSG("recv");
		}
		else if(0==rec)
		{
			printf("服务器退出\n");
			return -1;
		}
		for(i=0; pmsg[i].info.id != 0; i++)
		{
			printf("工号:%d\t类型:%d\t姓名:%s\t年龄:%d\t工资:%f\t手机号:%s\t地址:%s\n",
					pmsg[i].info.id,pmsg[i].info.type,pmsg[i].info.name,(pmsg[i].info.age),pmsg[i].info.salary,pmsg[i].info.phone,pmsg[i].info.addr);				
			if(i%7 == 6)
				putchar(10);
		}
		break;
	case 4:
		break;
	}

	return 0;
}
int ladd(int fd,MSG msg)
{
	int rec;
	printf("请输入要添加的id号>>");
	scanf("%d",&msg.info.id);
	printf("请输入要添加类型(root 1  普通用户　2)>>");
	scanf("%d",&msg.info.type);
	getchar();
	printf("请输入要添加的name>>");
	scanf("%s",msg.info.name);
	printf("请输入要添加的年龄>>");
	scanf("%d",&msg.info.age);
	printf("请输入要添加的工资>>");
	scanf("%lf",&msg.info.salary);
	getchar();
	printf("请输入要添加的手机号>>");
	scanf("%s",msg.info.phone);
	printf("请输入要添加的地址>>");
	scanf("%s",msg.info.addr);
	printf("请输入要添加的密码>>");
	scanf("%s",msg.user.passwd);
	if(send(fd,&msg,sizeof(msg),0) < 0)
	{
		ERR_MSG("send");
	}
	rec = recv(fd,&msg,sizeof(msg),0);
	if(rec < 0)
	{
		ERR_MSG("recv");
	}
	else if(0==rec)
	{
		printf("服务器退出\n");
		return -1;
	}



	return msg.flags;
}
int ldelete(int fd,MSG msg)
{

	int rec;
	printf("请输入要删除用户的姓名>>");
	scanf("%s",msg.info.name);
	if(send(fd,&msg,sizeof(msg),0) < 0)
	{
		ERR_MSG("send");
	}
	rec = recv(fd,&msg,sizeof(msg),0);
	if(rec < 0)
	{
		ERR_MSG("recv");
	}
	else if(0==rec)
	{
		printf("服务器退出\n");
		return -1;
	}

	return msg.flags;

}
int lupdata(int fd,MSG msg)
{

	int rec;
	if(msg.type == 1)
	{
		printf("请输入要修改用户的姓名>>");
		scanf("%s",msg.info.name);
		printf("请输你要修改的选项：\n");
		printf("１ 年龄　　２工资　３手机号　　４地址　　５密码\n");
		scanf("%d",&msg.no);
		getchar();
		switch(msg.no)
		{
		case 1:
			printf("请输入你要修改后的年龄>>");
			scanf("%d",&msg.info.age);
			getchar();
			break;
		case 2:
			printf("请输入你要修改后的工资>>");
			scanf("%lf",&msg.info.salary);
			getchar();
			break;
		case 3:
			printf("请输入你要修改后的手机号>>");
			scanf("%s",msg.info.phone);
			break;
		case 4:
			printf("请输入你要修改后的地址>>");
			scanf("%s",msg.info.addr);
			break;
		case 5:
			printf("请输入你要修改后的密码>>");
			scanf("%s",msg.user.passwd);
			break;
		default:printf("输入有误\n");
		}
	}
	else
	{
		printf("请输你要修改的选项：\n");
		printf("１ 年龄　　2 手机号　　3 地址　　4 密码\n");
		scanf("%d",&msg.no);
		getchar();
		switch(msg.no)
		{
		case 1:
			printf("请输入你要修改后的年龄>>");
			scanf("%d",&msg.info.age);
			getchar();
			break;
		case 2:
			printf("请输入你要修改后的手机号>>");
			scanf("%s",msg.info.phone);
			break;
		case 3:
			printf("请输入你要修改后的地址>>");
			scanf("%s",msg.info.addr);
			break;
		case 4:
			printf("请输入你要修改后的密码>>");
			scanf("%s",msg.user.passwd);
			break;
		default:printf("输入有误\n");
		}
	}
	if(send(fd,&msg,sizeof(msg),0) < 0)
	{
		ERR_MSG("send");
	}
	rec = recv(fd,&msg,sizeof(msg),0);
	if(rec < 0)
	{
		ERR_MSG("recv");
	}
	else if(0==rec)
	{
		printf("服务器退出\n");
		return -1;
	}
	return msg.flags;

}	

int lserch_ls(int fd,MSG msg)
{
	typedef struct{
		char buf1[128];
		char buf2[128];
		char buf3[128];
	}buf;
	int rec;
	buf str[128] ={0};
	if(send(fd,&msg,sizeof(msg),0) < 0)
	{
		ERR_MSG("send");
	}
	rec = recv(fd,str,sizeof(str),0);
	if(rec < 0)
	{
		ERR_MSG("recv");
	}
	else if(0==rec)
	{
		printf("服务器退出\n");
		return -1;
	}
	for(int i=0;*str[i].buf1!='\0';i++)
	{
		printf("%s %s %s\n",str[i].buf1,str[i].buf2,str[i].buf3);
	}
	return 0;

}

int rootuser_func(int fd,MSG msg)
{
	int rec;
	while(1)
	{
		printf("******************************************************************\n");
		printf("1：查询  2：修改 3：添加用户  4：删除用户   5：查询历史记录　６：退出\n");
		printf("******************************************************************\n");
		scanf("%d",&msg.n);
		switch(msg.n)
		{
		case 1:
			printf("******************************************************************\n");
			printf("1：名字查询　　　２：ID查询　　　　３：查询所有 ４：退出\n");
			printf("******************************************************************\n");
			lfind(fd,msg);
			break;
		case 2:
			rec = lupdata(fd,msg);
			if(rec != 0)
			{
				printf("修改失败\n");
				return -1;
			}
			printf("成功\n");
			break;
		case 3:
			rec=ladd(fd,msg);
			if(rec != 0)
			{
				printf("添加失败\n");
				return -1;
			}
			printf("成功\n");
			break;
		case 4:
			rec=ldelete(fd,msg);
			if(rec == 1)
			{
				printf("删除失败\n");
				return -1;
			}
			printf("删除成功\n");
			break;
		case 5:
			lserch_ls(fd,msg);
			break;
		case 6:
			goto loop;
		default:printf("输入选择有误\n");
		}
	}
loop:
	return 0;
}
int  luser_func(int fd,MSG msg)
{
	int rec;
	int chose;
	while(1)
	{
		printf("******************************************************************\n");
		printf("1：查询  		2：修改 		3：退出\n");
		printf("******************************************************************\n");
		scanf("%d",&chose);
		switch(chose)
		{
		case 1:
			msg.n=6;
			luser_find(fd,msg);
			break;
		case 2:
			msg.n=2;
			rec = lupdata(fd,msg);
			if(rec != 0)
			{
				printf("修改失败\n");
				return -1;
			}
			break;
		case 3:
			goto loop1;
		}
	}
loop1:
	return 0;
}
int main(int argc, const char *argv[])
{
	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd < 0)
		ERR_MSG("socket");

	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port   = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);
	if(connect(fd,(struct sockaddr*)&sin,sizeof(sin)) < 0)
		ERR_MSG("connect");
	printf("connect ok\n");

	MSG msg;
	msg.flags=0;
	int k[10] = {0};
	char buf[128] = "";
	ssize_t rec = 0;
	while(1)
	{
		printf("*************************************************************\n");
		printf("********  1：管理员登录　２：员工登录　3:退出　　　　********\n");
		printf("*************************************************************\n");
		scanf("%d%*c",&msg.user.type);	
		msg.type = msg.user.type;
		if(msg.user.type > 2)
			break;

		printf("输入姓名>>");
		scanf("%s",msg.user.name);
		//fgets(msg.user.name,sizeof(msg.user.name),stdin);
		printf("输入密码>>");
		scanf("%s",msg.user.passwd);
		//fgets(msg.user.passwd,sizeof(msg.user.passwd),stdin);
		msg.n = 0;
		if(send(fd,&msg,sizeof(msg),0) < 0)
		{
			ERR_MSG("send");
		}
		rec = recv(fd,k,sizeof(k),0);
		if(rec < 0)
			ERR_MSG("recv");
		else if(0==rec)
		{
			printf("服务器退出\n");
			return -1;
		}
		if(k[0])
		{
			printf("登录信息有误,重新登录\n");

		}
		else{
			printf("登录成功\n");
			if(msg.user.type == 1)
				rootuser_func(fd,msg);
			else if(msg.user.type == 2)
				luser_func(fd,msg);
		}
	}


	return 0;
}


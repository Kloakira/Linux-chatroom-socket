#include <sys/types.h> 
#include <sys/socket.h> 
#include <stdio.h> 
#include <unistd.h> 
#include <string.h> 
#include <arpa/inet.h> 
#include <stdlib.h>
#include <time.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include "clientLogin.h"

struct Msg msg;

void Interface() {  
	printf("====#     MENU    #===\n");
	printf ("\t1. 注册\n"); 
	printf ("\t2. 登录\n"); 
	printf("====#             #===\n");
} 

/**用户注册
	消息内容设定为：#service
	用户名：输入
	密码：输入
	操作类型cmd：1.注册
	
	返回类型：改动过的msg.cmd 1002（成功） -1（失败）*/
void Reg(int sockfd)
{
	char name[20];
	char password[20];
	
	strcpy(msg.msg,"#service");

	printf("==UserName==:");
	scanf("%s",name);
	while(getchar()!='\n');
	strcpy(msg.name,name);
	
	printf("==PassWord==:");
	scanf("%s",password);
	while(getchar()!='\n');
	strcpy(msg.password,password);
	
	msg.cmd=1;
	write(sockfd,&msg,sizeof(msg));
	read(sockfd,&msg,sizeof(msg));

	printf("msg.cmd = %d\n", msg.cmd);

	if(msg.cmd==1001){
		printf("Login success!waiting...\n");
	}
	else if(msg.cmd==-1){
		printf("the UserName has been used.\n");
		printf("returning...\n");
		sleep(1); 
	}
}

/**用户登录
	消息内容设定为：#service
	用户名：输入
	密码：输入
	操作类型cmd：2.登录
	
	返回类型：改动过的msg.cmd 1001（成功） -1（失败）*/
void Entry(int sockfd)
{
	char name[20];
	char password[20];
	
	strcpy(msg.msg,"#service");

	printf("==UserName==:");
	scanf("%s",name);
	while(getchar()!='\n');
	strcpy(msg.name,name);
	
	printf("==PassWord==:");
	scanf("%s",password);
	strcpy(msg.password,password);
	
	msg.cmd=2;

	write(sockfd,&msg,sizeof(msg));
	read(sockfd,&msg,sizeof(msg));

	printf("msg.cmd = %d\n", msg.cmd);
	if(msg.cmd==-1){
		printf("Fail To Login ,Please Resume Load...\n");
	}
	else if(msg.cmd==1002){
		printf("Login success ! waiting...\n");
	}

}

//客户端 注册/登录的选择
struct Msg ask_server(int sockfd)  
{ 
	char ch[2]; 
	while (1) { 
		Interface(); 
		scanf("%c",ch); 
		while(getchar()!= '\n'); 
		switch(ch[0]) { 
			case '1': // 注册 
			Reg(sockfd); 
			break; 
			case '2': // 登录 
			Entry(sockfd); 
			break;  
		}
		sleep(2);
		system("clear");
		return msg;
	}
}

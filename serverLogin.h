#include <stdio.h> 
#include <stdlib.h>
#include <mysql/mysql.h>
#define MAXLINE 1024

struct Msg { 
    char online[303][20];//打印 在线用户用
    int len;//打印 在线用户用 
    char msg[1024]; // 消息内容
    char file[MAXLINE]; // wenjian内容
    int flen;
    char name[20]; // 用户账号 
    char password[20];// 用户密码 
    int cmd; // 消息类型
 }Msg;
 
//用户注册
void Reg(int client_socket, struct Msg msg,MYSQL *con); 
//登录账号 
void Entry(int client_socket, struct Msg msg,MYSQL *con);


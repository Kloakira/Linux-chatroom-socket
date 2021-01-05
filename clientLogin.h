#include <stdio.h> 
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

void Interface();
//用户注册
void Reg(int sockfd);
//用户登陆
void Entry(int sockfd);
//客户端向服务器发送数据
struct Msg ask_server(int sockfd);

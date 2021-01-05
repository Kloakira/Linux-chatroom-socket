#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h> 
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>  
#include <pthread.h> 
#include <stdlib.h>
#include <sys/stat.h> 
#include <fcntl.h>
#include <mysql/mysql.h>
#include "dbtest.h"
#include "serverLogin.h"
//用户注册
void Reg(int client_socket, struct Msg msg,MYSQL *con) { 
    printf ("%s 进行注册\n", msg.name); 
    // 将用户进行保存
    if(data_insert(con,msg.name,msg.password)){
        msg.cmd = 1001;//注册成功 
        data_insert_online(con,msg.name,"online");
    }else{
        msg.cmd = -1; //数据库中已有该账号
    }
    write (client_socket, &msg, sizeof(msg));  
} 
//登录账号 
void Entry(int client_socket, struct Msg msg,MYSQL *con) 
{
    //检测数据库中name correct?pwd correct? 
    if(!data_judge(con,msg.name,msg.password)){
        //login error
        msg.cmd = -1; 
    }else{
        data_delete_online(con,msg.name);
        data_insert_online(con,msg.name,"online");
        msg.cmd = 1002;
        //enter the chatroom
    } 
    write (client_socket, &msg, sizeof(msg));
}


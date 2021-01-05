#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h> 
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "dbtest.h"
#include "serverLogin.h"
/**服务器端
   目前实现：多线程*/
int sockfd;//服务器socket
int fds[100];//客户端的socketfd,100个元素，fds[0]~fds[99]
int size =100 ;//用来控制进入聊天室的人数为100以内
int online_count = 0; //the num of starting client
char* IP = "127.0.0.1";
short PORT = 10222;
typedef struct sockaddr SA;

MYSQL *mysql_handle = NULL;//database connection

struct Msg msg;
void SendMsgToAll(struct Msg msg,int fd);
void connect_db();
struct sockaddr_in addr;
/**服务器建立*/
void init()
{
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    if (sockfd == -1){
        perror("创建socket失败");
        exit(-1);
    }
    
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    if (bind(sockfd,(SA*)&addr,sizeof(addr)) == -1){
        perror("绑定失败");
        exit(-1);
    }
    if (listen(sockfd,100) == -1){
        perror("设置监听失败");
        exit(-1);
    }
    connect_db();
}

void connect_db()
{
    //connect the database
    mysql_handle = db_initial(mysql_handle);
    if(NULL == mysql_handle){
        fprintf(stderr, "%s\n", "initialize failed.");
        exit(1);
    }
    mysql_handle = db_connect(mysql_handle, "localhost","root","9529346*","chatroom");
    //创建表：ID为主键自增，从1001开始，用户名，密码
    table_create(mysql_handle,"chatroom","create table usrinfo(userID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, userName TEXT,userPWD TEXT)engine=INNODB auto_increment=1001 default charset=gbk");
    //创建表：ID为主键自增，从1001开始，用户名，在线状态
    table_create(mysql_handle,"chatroom","create table onlineinfo(userID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, userName TEXT,userState TEXT)engine=INNODB auto_increment=1001 default charset=gbk");
}



/**多线程*/
void* service_thread(void* p)
{
    int fd = *(int*)p;
    
    printf("pthread = %d\n",fd);
    while(1){
        //receive message
        int ret = read(fd, &msg, sizeof(msg));
        //printf("receive :%s\n", msg.msg);
        int len ;
        if ( ret == -1 ) { 
            perror("read error."); 
            break; 
        }else if ( ret == 0 ){ // 代表客户端退出 
            //printf ("客户端退出\n"); 
            data_delete_online(mysql_handle,msg.name);
            data_insert_online(mysql_handle,msg.name,"offline");
            pthread_exit(0);
            online_count--;//the num of starting client
            break; 
        }else if ( strcmp(msg.msg,"#service")==0 && (msg.cmd > 0 && msg.cmd < 7)){
            switch(msg.cmd)
            {
                case 1 :// 客户端进行注册 参数：线程号、（用户名、密码）数据、数据库连接
                Reg(fd, msg, mysql_handle); 
                break; 

                case 2 : // 客户端进行登录 参数：线程号、（用户名、密码）数据、数据库连接
                Entry(fd, msg, mysql_handle); 
                break;

                case 3 :// 打印在线人员
                len = table_display(mysql_handle,"select * from onlineinfo",msg.online);
                msg.len = len;
                write(fd,&msg,sizeof(msg));
                break;

                case 4 :// 传输文件
                while(1){
                    if(msg.flen == -1){
                        SendMsgToAll(msg,fd);              
                        break;
                    }else{
                        SendMsgToAll(msg,fd);
                        read(fd, &msg, sizeof(msg));
                    }
                }
                printf("server transmits the file successful.\n");
                break;

                case 5 :// 设置在线
                data_delete_online(mysql_handle,msg.name);
                data_insert_online(mysql_handle,msg.name,"online");
                break;

                case 6 :// 设置隐身
                data_delete_online(mysql_handle,msg.name);
                data_insert_online(mysql_handle,msg.name,"offline");
                break;

            }
        }else if( strcmp(msg.msg,"#service") && strcmp(msg.msg,"") && strcmp(msg.msg,"#clear") && msg.cmd == 0){
            //把服务器接受到的信息发给所有的客户端
            SendMsgToAll(msg,fd);
        }
    }
}

/**群发*/
void SendMsgToAll(struct Msg msg,int fd)
{
    int i;
    for (i = 0;i < online_count;i++)
    {        
        if (fds[i] != fd)
        {
            write(fds[i],&msg,sizeof(msg));
        }
    }
}

/**服务*/
void service()
{
    printf("服务器启动\n");
    while(1){
        struct sockaddr_in fromaddr;
        socklen_t len = sizeof(fromaddr);
        int fd = accept(sockfd,(SA*)&fromaddr,&len);
        if (fd == -1){
            printf("客户端连接出错...\n");
            continue;
        }
        int i = 0;
        for (i = 0;i < size;i++){
            if (fds[i] == 0){
                //记录客户端的socket
                fds[i] = fd;
                printf("fd = %d\n",fd);
                //有客户端连接之后，启动线程给此客户服务
                pthread_t tid;
                pthread_create(&tid,0,service_thread,&fd);
                pthread_detach(tid); // 线程分离
                online_count++; //the num of starting client
                break;
            }
            if (size == i){
            //发送给客户端说聊天室满了
            char* str = "对不起，聊天室已经满了!";
            send(fd,str,strlen(str),0); 
            close(fd);
            }
        }
    }
}



int main()
{
    init();
    service();
}

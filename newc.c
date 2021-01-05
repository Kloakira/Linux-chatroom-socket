#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "clientLogin.h"
int sockfd;//客户端socket
char* IP = "127.0.0.1";//服务器的IP
short PORT = 10222;//服务器服务端口
typedef struct sockaddr SA;
char username[20]; // 用户账号 

void service_menu();
void online_display(struct Msg msg);
void filesend(int sockfd, struct Msg msg);

/**客户端初始化（连接服务器）*/
void init(){
    struct Msg login_msg;
    sockfd = socket(PF_INET,SOCK_STREAM,0);
    struct sockaddr_in addr;
    addr.sin_family = PF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(IP);
    if (connect(sockfd,(SA*)&addr,sizeof(addr)) == -1){
        perror("无法连接到服务器");
        exit(-1);
    }
    printf("客户端启动成功\n");

    login_msg = ask_server(sockfd);
    // save the client name
    strcpy(username,login_msg.name);
    //printf("username%s\n",username);
    if(login_msg.cmd == 1001 || login_msg.cmd == 1002){
        sprintf(login_msg.msg,"%s进入了聊天室",login_msg.name);
        printf("%s进入了聊天室\n",login_msg.name);
        login_msg.cmd = 0;
        write(sockfd,&login_msg,sizeof(login_msg));
        //发送给服务器：“xx进入聊天室”
    }
}
void start(){

    int choose;
    pthread_t id;
    int create;
    void* recv_thread(void*);
    create = pthread_create(&id,0,recv_thread,0);
    if(0 != create){
       printf("read thread create failed.error:%d\n",create);
    }
        
    struct Msg c_msg;//contact write use   
    pthread_detach(id);
    
    while(1){
        //聊天内容输入
        scanf("%s",c_msg.msg);
        c_msg.cmd = 0;
        strcpy(c_msg.name,username);
        
        //客户端退出
        if (strcmp(c_msg.msg,"#exit") == 0){
            sprintf(c_msg.msg,"%s退出了聊天室\n",username);
            strcpy(c_msg.name,username);
            write(sockfd,&c_msg,sizeof(c_msg));
            close(sockfd);
            break;
        }else if(strcmp(c_msg.msg,"#clear") == 0){
            system("clear");
        }else if(strcmp(c_msg.msg,"#service") == 0){//客户端service
            service_menu();
            scanf("%d",&choose);
            switch(choose)
            {
                case 1://查看当前在线人员
                    c_msg.cmd = 3;
                    write(sockfd,&c_msg,sizeof(c_msg));
                    read(sockfd,&c_msg,sizeof(c_msg));
                    c_msg.cmd = 0;//reset msg type
                    online_display(c_msg);
                break;
                case 2://传输文件
                    c_msg.cmd = 4;
                    filesend(sockfd,c_msg);
                    c_msg.cmd = 0;
                break;
                case 3://设置在线
                    c_msg.cmd = 5;
                    strcpy(c_msg.name,username);
                    write(sockfd,&c_msg,sizeof(c_msg));
                    c_msg.cmd = 0;//reset msg type
                    printf("ok.\n");
                break;
                case 4://设置隐身
                    c_msg.cmd = 6;
                    strcpy(c_msg.name,username);
                    write(sockfd,&c_msg,sizeof(c_msg));
                    c_msg.cmd = 0;//reset msg type
                    printf("ok.\n");
                break;
            }
        }else{
            write(sockfd,&c_msg,sizeof(c_msg));
        }
    }
}

void filesend(int sockfd, struct Msg msg)
{    
    FILE *fq;
    char filename[50];
    printf("请输入你要打开的文件名及路径，如/home/wenlu/liming/jpgc1/1.jpg\n");
    scanf("%s",filename); //这句要用户输入文件名 
    if ((fq=fopen(filename,"rb"))==NULL){//打开文件，并判断是否有打开错误
        printf("打开文件%s出现错误\n",filename);    
    }
    while(!feof(fq)){   
        msg.flen = fread(msg.file, 1, sizeof(msg.file), fq);
        write(sockfd, &msg, sizeof(msg));
    }
    msg.flen = -1;//send to server & others:the file has been sent over
    write(sockfd, &msg, sizeof(msg));
    printf("file send successful.\n");
    strcpy(msg.file,"");

    fclose(fq);
}
/**接受来自服务器的消息*/
void* recv_thread(void* p){
    struct Msg c_msg;//contact read use
    FILE *fp;
    int read_rec;
    while(1){
        if( c_msg.cmd != 3){
            read(sockfd,&c_msg,sizeof(c_msg));
        }
        if( c_msg.cmd == 0){//msg
            printf("%s:%s\n",c_msg.name,c_msg.msg);
        }else if( c_msg.cmd == 4 ){
            if((fp = fopen("/home/lmhuang/rec-test/new.jpg","a+")) == NULL ){
                printf("File open failed.\n");
                break;
            }
            printf("receive a file from : %s \n",c_msg.name);
            while(1){
                if(c_msg.flen == -1){
                    break;
                }else{
                    fwrite(c_msg.file, 1, c_msg.flen, fp);
                    //printf("fsize:%ld,flen:%d\n", strlen(c_msg.file),c_msg.flen); 
                    read(sockfd, &c_msg, sizeof(c_msg));
                }
            }
            printf("receive a file-> /home/lmhuang/rec-test\n");
            fclose(fp);
        }
    }
}
void service_menu()
{
    printf("====#     MENU    #====\n");
    printf("\t1.查看user state\n");
    printf("\t2.传输文件\n");
    printf("\t3.设置在线\n"); 
    printf("\t4.设置隐身\n");
    printf("====#             #====\n");
}

void online_display(struct Msg disp_msg)
{
    int m = 0;
    printf("============IN THIS CHATROOM============\n");
    for(m = 0; m < 3; m++)
    {
        printf(" %-10s ",disp_msg.online[m]);
    }
    for(m = 3; m < disp_msg.len+3 ; m++)
    {
        if(m%3==0)
            printf("\n");
        printf("  %-10s ",disp_msg.online[m]);
    }
    printf("\n");
    printf("========================================\n");
}



/**主函数*/
int main()
{
    init();
    start();
    return 0;
}

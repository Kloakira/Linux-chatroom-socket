#include <mysql/mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "dbtest.h" 

/**功能：初始化数据库
   参数：mysql句柄con
   返回值：con*/
MYSQL * db_initial(MYSQL *con)
{
	if(mysql_library_init(0,NULL,NULL)){
		fprintf(stderr, "Could't initialize MYSQL library.\n");
		return con;
	}
	con = mysql_init(NULL);
	return con;
}

/**功能：连接数据库
   参数：con，host地址，用户名，密码，数据库名称
   返回值：con*/
MYSQL * db_connect(MYSQL *con ,char *hostip, char *username , char * pwd, char *db)
{
	if(!(mysql_real_connect(con, hostip, username, pwd, db,3306,NULL,0))){
		fprintf(stderr, "\nError:%s [%d]\n", mysql_error(con),mysql_errno(con));
		exit(1);
	}
	printf("Connection Successful!\n");
	return con;
}

/**功能：创建数据库
   参数：con，以空字符为结尾的SQL查询字符串cmd
   返回值：空*/
void db_create(MYSQL *con,char *cmd)
{
	if(mysql_query(con,cmd)){
		printf("Create database failed.\n");
	}else{
		printf("Create database successful.\n");
	}
}

/**功能：创建表
   参数：con，数据库名，SQL指令
   返回值：空*/
void table_create(MYSQL *con,char *db,char *cmd)
{
	if( mysql_select_db(con,db)){
		printf("select DB failed.\n");
	}
	if(mysql_query(con,cmd)){
		printf("Create table failed.\n");
	}else{
		printf("Create table successful.\n");
	}
}

/**功能：显示表
   参数：con，SQL查询字符串
   返回值：3*n个用户 -> 表项个数*/
int table_display(MYSQL *con,char *query,char display[100][20])
{
	MYSQL_RES *mysql_res;
	MYSQL_ROW mysql_row;
	MYSQL_FIELD *field;
	int i,j;
	int num_row,num_col;
	int count = 0;
	if((mysql_real_query(con,query,(unsigned int)strlen(query))) != 0){
		printf("Query failed.\n");
	}
	//save the qurey result
	if((mysql_res = mysql_store_result(con)) == NULL){
		printf("Store result failed.\n");
	}

	num_row = mysql_num_rows(mysql_res);
	num_col = mysql_num_fields(mysql_res);
	//printf("The table :row number = %1u  ,  col number = %1u \n", num_row,num_col);
	printf("===========================================\n");
	int k = 0; 
	for(i = 0; i <num_row ; i++){
		if((mysql_row = mysql_fetch_row(mysql_res)) == NULL){
			break;
		}
		if (i == 0) {
			
            while(field = mysql_fetch_field(mysql_res)) {
                printf(" %-10s ", field->name);
                strcpy(display[k],field->name);
                k++;
            }
            printf("\n");
        }
		mysql_fetch_lengths(mysql_res);
		for(j = 0; j <num_col ; j++){
			printf("  %-10s ", mysql_row[j] ? mysql_row[j] : "NULL" );
			strcpy(display[3*i+j+3],mysql_row[j] ? mysql_row[j] : "NULL");
		}
		printf("\n");
	}
	count = num_row * num_col;
	printf("===========================================\n\n");
	mysql_free_result(mysql_res);
	return count;
}

/**功能：判断当前表中是否已有用户名为 name 的项
   参数：con，用户名
   返回值：1存在，0不存在*/
int data_exist(MYSQL *con,char *name)
{
	struct user user1;
	char query[1024];
	strcpy(user1.username,name);

	MYSQL_RES *result;
 	MYSQL_ROW row;
  	MYSQL_FIELD *field;
 
	int num_fields;
	sprintf(query,"SELECT * FROM usrinfo WHERE username = '%s'",
		user1.username);
	mysql_query(con,query);
	result = mysql_store_result(con);
	num_fields = mysql_num_fields(result);
	while ((row = mysql_fetch_row(result)))
	{
	    if(field = mysql_fetch_field(result)) {
	    	printf("%s ,", row[1]);
	    	//if exist ,!strcmp() return 1,if == ture
	        if(!strcmp(user1.username,row[1])){
	            return 1;
	        }    
	    }
	    
	}
	mysql_free_result(result);
	
	mysql_commit(con);
	return 0;
}

/**功能：判断用户名和密码正确
   参数：con，用户名,密码
   返回值：1 =正确 ,0 =错误*/
int data_judge(MYSQL *con,char *name,char *pwd)
{
	struct user user1;
	char query[1024];
	strcpy(user1.username,name);
	strcpy(user1.userpwd,pwd);

	MYSQL_RES *result;
 	MYSQL_ROW row;
  	MYSQL_FIELD *field;
 
	int num_fields;
	int i;
	sprintf(query,"SELECT * FROM usrinfo WHERE username = '%s'",
		user1.username);
	mysql_query(con,query);
	result = mysql_store_result(con);
	num_fields = mysql_num_fields(result);
	while ((row = mysql_fetch_row(result)))
	{
	    if(field = mysql_fetch_field(result)) {
	    	printf("username:%s,", row[1]);
	    	printf("pwd:%s,", row[2]);
	    	//if exist ,!strcmp() return 1,if == ture
	        if(!strcmp(user1.username,row[1]) && !strcmp(user1.userpwd,row[2])){
	            	printf("login successful!\n");
	            	return 1;
	        }    
	    }
	    
	}

	mysql_free_result(result);
	
	mysql_commit(con);
	return 0;
}

/**功能：所有用户数据插入
   参数：con，用户名，用户密码
   返回值：1成功/0失败*/
int data_insert(MYSQL *con,char *name,char *pwd)
{
	struct user user1;
	char query[1024];
	MYSQL_RES *result;
	strcpy(user1.username,name);
	strcpy(user1.userpwd,pwd);
	//判断用户名是否存在
	if(data_exist(con,name)){
		printf("this name has existed.\n");
		return 0;
	}
	if(data_judge(con,name,pwd)){
		printf("name/pwd wrong.\n");
		return 0;
	}
	sprintf(query,"insert into usrinfo(userName,userPWD) values('%s','%s')",
		user1.username ,user1.userpwd);
	if((mysql_real_query(con,query,(unsigned int)strlen(query))) != 0){
		mysql_rollback(con);
		printf("insert failed.\n");
		return 0;
	}
	mysql_commit(con);
	//table_display(con,"select * from usrinfo");
	return 1;
}

/**功能：在线用户数据插入
   参数：con，用户名，用户在线状态
   返回值：1成功/0失败*/
int data_insert_online(MYSQL *con,char *name,char *state)
{
	struct onlineuser user1;
	char query[1024];
	strcpy(user1.username,name);
	strcpy(user1.userstate,state);

	sprintf(query,"insert into onlineinfo(userName,userState) values('%s','%s')",
		user1.username ,user1.userstate);
	if((mysql_real_query(con,query,(unsigned int)strlen(query))) != 0){
		mysql_rollback(con);
		printf("insert failed.\n");
		return 0;
	}
	mysql_commit(con);
	//table_display(con,"select * from onlineinfo");
	return 1;
}

/**功能：数据删除
   参数：con，待删除用户的用户名*/
int data_delete(MYSQL *con,char *name)
{
	char query[1024];
	sprintf(query,"delete from usrinfo where userName = '%s'", name );
	if((mysql_real_query(con,query,(unsigned int)strlen(query))) != 0){
		mysql_rollback(con);
		printf("delete failed.\n");
		return 0;
	}
	mysql_commit(con);
	if( mysql_affected_rows(con)>0){
		printf("delete successful.\n");	
	}
	return 1;
	//table_display(con,"select * from usrinfo");
}
/**功能：数据删除
   参数：con，待删除用户的用户名*/
int data_delete_online(MYSQL *con,char *name)
{
	char query[1024];
	sprintf(query,"delete from onlineinfo where userName = '%s'", name );
	if((mysql_real_query(con,query,(unsigned int)strlen(query))) != 0){
		mysql_rollback(con);
		printf("delete failed.\n");
		return 0;
	}
	mysql_commit(con);
	if( mysql_affected_rows(con)>0){
		printf("delete successful.\n");	
	}
	return 1;
	//table_display(con,"select * from onlineinfo");
}

/**主函数*/
/*
int main(void)
{

	MYSQL *mysql_handle = NULL;
	char display[303][20] = {};
	int m,n;
	int len = 0;
	mysql_handle = db_initial(mysql_handle);
	if(NULL == mysql_handle){
		fprintf(stderr, "%s\n", "initialize failed.");
		exit(1);
	}
	mysql_handle = db_connect(mysql_handle, "localhost","root","9529346*","chatroom");
	//db_create(mysql_handle,"create database chatroom");
	//创建表：ID为主键自增，从1001开始，用户名，密码
	//table_create(mysql_handle,"chatroom","create table usrinfo(userID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, userName TEXT,userPWD TEXT)engine=INNODB auto_increment=1001 default charset=gbk");
	//创建表：ID为主键自增，从1001开始，用户名，在线状态
	//table_create(mysql_handle,"chatroom","create table onlineinfo(userID INT UNSIGNED NOT NULL PRIMARY KEY AUTO_INCREMENT, userName TEXT,userState TEXT)engine=INNODB auto_increment=1001 default charset=gbk");
	
	data_insert(mysql_handle,"kira","123456");
	data_insert(mysql_handle,"lele","123456");
	data_insert(mysql_handle,"yuer","123456");
	if(data_judge(mysql_handle,"kira","1234")){
		printf("name & pwd correct.\n");
	}else{
		printf("wrong.\n");
	}
	data_delete(mysql_handle,"kira");
	data_delete(mysql_handle,"lele");
	table_display(mysql_handle,"select * from usrinfo",display);

	data_insert_online(mysql_handle,"yuer","online");
	data_insert_online(mysql_handle,"kira","offline");
	data_insert_online(mysql_handle,"lele","online");
	table_display(mysql_handle,"select * from onlineinfo",display);

	data_delete_online(mysql_handle,"kira");
	data_delete_online(mysql_handle,"yuer");
	data_delete_online(mysql_handle,"lele");
	table_display(mysql_handle,"select * from onlineinfo",display);

	//客户端需要的在线人员打印输出
	//需要的关键：全长len，存放数据的数组display
	len = table_display(mysql_handle,"select * from usrinfo",display);
	
	for(m = 0; m < 3; m++)
	{
		printf(" %-10s ",display[m]);
	}
	for(m = 3; m < len+3 ; m++)
	{
		if(m%3==0)
			printf("\n");
		printf("  %-10s ",display[m]);
	}
	printf("\n");
	

	mysql_close(mysql_handle);
	mysql_library_end();
	return 0;
}
*/

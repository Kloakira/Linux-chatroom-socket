#include <mysql/mysql.h>
#include <stdio.h>
struct user
{
	char username[20];
	char userpwd[20];
};
struct onlineuser
{
	char username[20];
	char userstate[20];
};

/**功能：初始化数据库
   参数：mysql指针con
   返回值：con*/
MYSQL * db_initial(MYSQL *con);

/**功能：连接数据库
   参数：con，host地址，用户名，密码，数据库名称
   返回值：con*/
MYSQL * db_connect(MYSQL *con ,char *hostip, char *username , char * pwd, char *db);

/**功能：创建数据库
   参数：con，以空字符为结尾的SQL查询字符串cmd
   返回值：空*/
void db_create(MYSQL *con,char *cmd);

/**功能：创建表
   参数：con，数据库名，SQL指令
   返回值：空*/
void table_create(MYSQL *con,char *db,char *cmd);

/**功能：显示表
   参数：con，SQL查询字符串
   返回值：空*/
int table_display(MYSQL *con,char *query,char display[100][20]);

/**功能：判断当前表中是否已有用户名为 name 的项
   参数：con，用户名
   返回值：空*/
int data_exist(MYSQL *con,char *name);
/**功能：判断name correct & pwd correct
   参数：con，用户名,password
   返回值：1 =correct ,0 =wrong*/
int data_judge(MYSQL *con,char *name,char *pwd);

/**功能：所有用户数据插入
   参数：con，用户名，用户密码，用户是否存在
   返回值：空*/
int data_insert(MYSQL *con,char *name,char *pwd);

/**功能：在线用户数据插入
   参数：con，用户名，用户在线状态
   返回值：空*/
int data_insert_online(MYSQL *con,char *name,char *state);
/**功能：数据删除
   参数：con，所需删除用户的用户名*/
int data_delete(MYSQL *con,char *name);
/**功能：数据删除
   参数：con，所需删除用户的用户名*/
int data_delete_online(MYSQL *con,char *name);


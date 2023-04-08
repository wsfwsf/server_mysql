#include "../SqlConnectionPool.h"
#include "../SqlConnectionPool.cc"
#include <string.h>

int main(){
	sqlconnectionpool pool(10);
	pool.sqlconnectionpool_init();
	MYSQL* mysql = pool.sqlconnection_get();
	std::string str="select * from online_user";
	std::string ins="insert into online_user values(3,3)";
	mysql_real_query(mysql,ins.c_str(),strlen(ins.c_str()));
	int rows= mysql_real_query(mysql,str.c_str(),strlen(str.c_str()));
	MYSQL_RES* res = mysql_store_result(mysql);
	if(res==NULL){
		printf("res==NULL\n");
		return 0;
	}
	int fields = mysql_num_rows(res);
	if(fields==0){
		printf("fields==0\n");
		return 0;
	}
	MYSQL_ROW row;
	while((row=mysql_fetch_row(res))){
		for(int i=0;i<fields;i++){
			printf("%s\t",row[i]);
		}
		printf("\n");
	}
	return 0;
}

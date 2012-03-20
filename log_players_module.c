#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <errno.h>
#include <mysql/mysql.h>
#include "log_players_module.h"
#include "crcon.h"

int log_players(player_list *plist){

	player_node *cur_pnode;
	cur_pnode = plist->first;
	while(cur_pnode != NULL){
		printf("%s\n",cur_pnode->player_name);
		cur_pnode = cur_pnode->next;
	}
	
/*
	MYSQL *mysql;
	MYSQL_RES *res;
	MYSQL_ROW row;
	int fields,i;
	mysql = mysql_init(mysql);
	if( mysql == NULL ){
		printf("Error: Couldn't init mysql\n");
		return -1;
	}
	mysql = mysql_real_connect(mysql, "172.16.2.103", "logparser", "l0gp@r34",
			"statsDB",0,NULL,0 );
	if ( mysql == NULL ){
		printf("Error: Couldn't connect to mysql server\n");
		return -1;
	}

	if( mysql_query(mysql, "select * from Users") != 0 ){
		printf("Error: %s\n",mysql_error(mysql));
		return -1;
	}
	
	res = mysql_store_result(mysql);
	if ( res == NULL ){
		printf("Error: %s\n",mysql_error(mysql));
		return -1;
	}
	row = mysql_fetch_row(res);
	fields = mysql_num_fields(res);
	while ( row != NULL ){
		for(i=0;i<fields;i++){
			printf("%s - ",row[i]);
		}
		printf("\n");
		row = mysql_fetch_row(res);

	}
	mysql_free_result(res);
	mysql_close(mysql);

	*/
	return 0;
		
} 


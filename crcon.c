/*
    crcon - C rcon cli app Version 1.0
	Copyright 2003, Steve Johnson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This application is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Steve Johnson 
    steve@webninja.com
*/
/***********************************************************
* File: crcon.c
* Desc: This is the crcon command line client to make
	crcon command calls to quake game servers
	pass -h or -? for options
* *********************************************************/

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <netdb.h>
#include <errno.h>
#include "crcon.h"
#define _GNU_SOURCE
#include <getopt.h>

void usage (char *prog){
	printf("usage: \n");
	printf("%s [-p password] [-P port] [-t type] [-b buffersize] server command\n",prog);
	printf("%s -l [-P port] [-t type] [-b buffersize] server\n",prog);
	printf("%s -v variable -p password [-P port] [-t type] [-b buffersize] server player_name\n",prog);
	printf("     -h | --help  			diplay this message\n");
	printf("     -l | --list 	 		get player list\n");
	printf("     -p password | --password=password 	set password\n");
	printf("     -P port | --port=port  	  	default is 27960\n");
	printf("     -t gametype | --type=gametype    	default is q3a, type -h gametype \n");
	printf("			              	or --help=gametype for list\n");
	printf("     -v variable | --var=variable 	get player variable\n");
	printf("     -b buffersize-kbytes | --buffersize=buffersize-kbytes  defaults to 4k\n");
}

void gametypes (void){
	printf("Tested game types:\n");
	printf("	q3a - quake3arena (default)\n");
	printf("	sof2 - soldier of fortune 2(also works with default)\n");
	printf("	jk2 - jeki outcast 2(also works with default)\n");
	printf("	moh - medal of honor (this must be specified to work with mohaa\n");
	printf("	bf - Battlefield 1942 (status port only)\n");

}
int main ( int argc, char **argv )
{
	player_list plist;
	char oName0[] = "help";
	char oName1[] = "list";
	char oName2[] = "password";
	char oName3[] = "port";
	char oName4[] = "type";
	char oName5[] = "variable";
	char oName6[] = "buffersize";
	struct hostent *he;
	struct option oOpt[7];
	const char getstatus[] = "getstatus";
	char gtype[8] = "q3a";
	//char result_buf[SIZE_RESULT_BUF];
	char *result_buf;
	char c_flag;
	char *password = NULL;
	char *server,*temp,*msg;
	char ipaddress[16];
	char *player,*var;
	char getvar[SIZE_GETVAR] = "dumpuser";
	unsigned short int port = 0;
	unsigned char getplist = 0;
	unsigned char getpvar = 0;
	unsigned char printpvar = 0;
	unsigned char printplist = 0;
	char rcon[SIZE_RCON];
	unsigned short int size_result_buf = 4096;
    extern int h_errno;
	extern char *optarg;
	extern int optind;
	
	/***********************************************
	* build long option structure array
	* *********************************************/
	oOpt[0].name = oName0;
	oOpt[0].has_arg = 0;
	oOpt[0].flag = NULL;
	oOpt[0].val = 'h';
	oOpt[1].name = oName1;
	oOpt[1].has_arg = 0;
	oOpt[1].flag = NULL;
	oOpt[1].val = 'l';
	oOpt[2].name = oName2;
	oOpt[2].has_arg = 1;
	oOpt[2].flag = NULL;
	oOpt[2].val = 'p';
	oOpt[3].name = oName3;
	oOpt[3].has_arg = 1;
	oOpt[3].flag = NULL;
	oOpt[3].val = 'P';
	oOpt[4].name = oName4;
	oOpt[4].has_arg = 1;
	oOpt[4].flag = NULL;
	oOpt[4].val = 't';
	oOpt[5].name = oName5;
	oOpt[5].has_arg = 1;
	oOpt[5].flag = NULL;
	oOpt[5].val = 'v';
	oOpt[6].name = oName6;
	oOpt[6].has_arg = 1;
	oOpt[6].flag = NULL;
	oOpt[6].val = 'b';
	memset(&oOpt[6],'\0',sizeof(struct option));

	/*******************************************************
	 * parse options 
	 * ****************************************************/
	while((c_flag = getopt_long(argc,argv,"b:t:?h:v:lp:P:",&oOpt[0],NULL))!= EOF){
		switch(c_flag){
		/*******************************************************
		 * help 
		 * ****************************************************/
		case '?' :
			//printf("unknown option '-%c':\n",c_flag);
			usage(argv[0]);
			exit(1);
		/*******************************************************
		 * help 
		 *****************************************************/
		case 'h' :
			//printf("unknown option '-%c':\n",c_flag);
			if(optarg != NULL){
				if(strcmp(optarg,"gametype") == 0){
					gametypes();	
					exit(1);
				}
			}
			usage(argv[0]);
			exit(1);
		/*******************************************************
		 * buffersize 
		 *****************************************************/
		case 'b':
			if(optarg == NULL){
				printf("need buffer size\n");
				usage(argv[0]);
				exit(1);
			}
			size_result_buf = atoi(optarg);
			size_result_buf = size_result_buf * 1024;
			break;
		/*******************************************************
		 * password 
		 *****************************************************/
		case 'p':
			if(optarg == NULL){
				printf("bad password\n");
				usage(argv[0]);
				exit(1);
			}
			password = optarg;
			break;
		/*******************************************************
		 * game type 
		 *****************************************************/
		case 't':
			if(optarg == NULL){
				printf("bad game type\n");
				usage(argv[0]);
				exit(1);
			}
			if(strcmp(optarg,"moh") == 0){
				if(port == 0){
					port = 12203;
				}
				strcpy(gtype,"moh");
			} else if(strcmp(optarg,"sof2") == 0){
				if(port == 0){
					port = 20100;
				}
				strcpy(gtype,"sof2");
			} else if(strcmp(optarg,"jk2") == 0){
				if(port == 0){
					port = 28070;
				}
				strcpy(gtype,"jk2");
			} else if(strcmp(optarg,"bf") == 0){
				if(port == 0){
					port = 23000;
				}
				strcpy(gtype,"bf");
			} else {
				if(port == 0){
					port = 27960;
				}
				strcpy(gtype,"q3a");
			}
			break;
		/*******************************************************
		 * port 
		 *****************************************************/
		case 'P':
			if(optarg == NULL){
				printf("bad port number\n");
				usage(argv[0]);
				exit(1);
			}
			port = atoi(optarg);
			break;
		/*******************************************************
		 * get player list 
		 *****************************************************/
		case 'l':
			getplist++;	
			printplist++;
			msg = (char *)getstatus;
			break;
		/*******************************************************
		 * get player variable 
		 *****************************************************/
		case 'v':
			var = optarg;
			getpvar++;
			printpvar++;
			break;
		}
		
	}
	/*******************************************************
	 * if -v (player variable) build msg and set options 
	 *****************************************************/
	if(printpvar == 1){
		/*******************************************************
		 * need three arguments for -v 
		 *****************************************************/
		if((optind+2) != argc){
			printf("wrong number of arguments\n");
			usage(argv[0]);
			exit(1);
		}
		/*******************************************************
		 * must set password with -p for -v 
		 *****************************************************/
		if(password == NULL){
			printf("you must provide a password\n");
			usage(argv[0]);
			exit(1);
		}
		server = argv[argc-2];
		player = argv[argc-1];
		strcat(getvar, " \"");
		strcat(getvar,player);
		strcat(getvar, "\"");
	/*******************************************************
	 * if -l (player list) 
	 *****************************************************/
	} else if(printplist == 1){
		/*******************************************************
		 * need one arg (server) 
		 *****************************************************/
		if((optind+1) != argc){
			printf("wrong number of arguments\n");
			usage(argv[0]);
			exit(1);
		}
		server = argv[argc-1];
	/*******************************************************
	 * else if regular rcon command 
	 *****************************************************/
	} else {
		/*******************************************************
		 * need server and command 
		 *****************************************************/
		if((optind+2) != argc){
			printf("wrong number of arguments\n");
			usage(argv[0]);
			exit(1);
		}
		msg = argv[argc-1];
		server = argv[argc-2];
	}
	/*******************************************************
	 * if !port set to default (quake) 
	 *****************************************************/
	if(port == 0){
		port = DEFAULT_PORT;
	}
	memset(ipaddress,'\0',sizeof(ipaddress));
	result_buf = (char*)malloc(size_result_buf);
	he = gethostbyname(server);

	/*******************************************************
	* parse address
	*******************************************************/	
	sprintf(ipaddress,"%d.%d.%d.%d",(he->h_addr_list[0][0])&0xff,
			(he->h_addr_list[0][1])&0xff,
			(he->h_addr_list[0][2])&0xff,
			(he->h_addr_list[0][3])&0xff);
	/*******************************************************
	 * if password set, append rcon <password> to msg, then send message and fill buf with server return string 
	 *****************************************************/
	if(password != NULL){
		memset(rcon,'\0',SIZE_RCON);
		strcpy(rcon,"rcon ");	
		strcat(rcon,password);
		strcat(rcon," ");
		strcat(rcon,msg);
		/*******************************************************
	 * send msg 
	 *****************************************************/
		if( crcon( rcon, result_buf, ipaddress,  port, gtype, size_result_buf ) ){
			perror("crcon - rconPassword");
			free((void*)result_buf);
			return -1;
		}		
	/*******************************************************
	 * else just send msg and fill result_buf with server return string 
	 *****************************************************/
	} else {	
		/*******************************************************
		 * send msg *
		 ****************************************************/	
		if( crcon( msg, result_buf, ipaddress,  port, gtype , size_result_buf) ){
			perror("crcon");
			free((void*)result_buf);
			return -1;	
		}
	}
	/*******************************************************
	 * if -v make call to get_player_var and pass result_buf 
	 *****************************************************/
	if (getpvar > 0){
		/*******************************************************
		 * parse var, print on success 
		 *****************************************************/
		if((temp = get_player_var(result_buf,var)) == NULL){
			free((void*)result_buf);
			perror("could not find player\n");
			return -1;
		} else {
			print_player_var(temp);
		}

	/*******************************************************
	 * if -l make call to get_player_list passing result_buf and address of player list structure 
	 *****************************************************/
	} else if(getplist > 0){
		/*******************************************************
		 * build list 
		 *****************************************************/
		if(get_player_list(result_buf,&plist)){
			free((void*)result_buf);
			perror("get_plist");
			return -1;
		}
		/*******************************************************
		 * print list 
		 *****************************************************/
		if(printplist > 0){
			print_player_list(&plist);
		}
		/*******************************************************
		 * free list 
		 *****************************************************/
		free_player_list(&plist);

	/*******************************************************
	 * else print return string 
	 *****************************************************/
	} else {
		printf("%s\n",result_buf);
		free((void*)result_buf);
	}
	return 0;
	
}


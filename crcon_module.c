/*
    crcon - C rcon cli app Version 1.0
	Copyright 2003, Steve Johnson

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public
    License along with this library; if not, write to the Free
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Steve Johnson 
    steve@webninja.com
*/
/* ******************************************************************
 * File: crcon_module.c
 * Desc: This function makes rcon calls to quake based games.
 * 	 function will buffer 4 ascii 255 chars in front of
 *	 commands.  It will send the *cmd and return *p_result_buf to
 *	 *server (ip or domain name) on port.
 * ******************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "crcon.h"

//void test(void){
//	int i;
//	i=0;
//}
int crcon  ( char *cmd, char *p_result_buf, char *server, int port, char *gtype, unsigned short int size_result_buf )
{
	unsigned char i;
	int sock;
	//FILE *fp;
	//int alloccount;
	short int bytesleft;
	short int buflen;
	struct sockaddr_in addr;	
	const unsigned int addr_len = sizeof(addr);
	char *sock_buffer;
	char send_buffer[SIZE_SEND_BUFFER];
	char *tmp;
	struct timespec stimespec;

	/*********************************************************
	*  set to 1 tenth of a second
	*********************************************************/
	stimespec.tv_sec = 0;
	stimespec.tv_nsec = 1000000; 
	/**********************************************************
	*  prepend the cmd with for char 255's (don't know why but quake wants them!) 
	************************************************************/
	if(strcmp(gtype,"moh") == 0){
		send_buffer[0] = 0xff;
		send_buffer[1] = 0xff;
		send_buffer[2] = 0xff;
		send_buffer[3] = 0xff;
		send_buffer[4] = 0x02;
		strcpy(&send_buffer[5],cmd);
	} else if(strcmp(gtype,"bf") == 0){
		send_buffer[0] = 0xff;
		strcpy(&send_buffer[1],cmd);
	} else {
		send_buffer[0] = 0xff;
		send_buffer[1] = 0xff;
		send_buffer[2] = 0xff;
		send_buffer[3] = 0xff;
		strcpy(&send_buffer[4],cmd);
	}
	/**********************************************************
	*  get socket pointer 
	************************************************************/
	sock = socket (PF_INET, SOCK_DGRAM, 0);

	if(sock <=  0){
		perror("socket");
		return -1;
	}

	bzero(&addr, addr_len);

	/**********************************************************
	*  fill sockaddr struct with port,server,flags 
	************************************************************/
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if( inet_aton ( server,  (struct in_addr*)&addr.sin_addr ) == 0 ){
		perror("inet_aton");
		return -1;
	}
	
	/**********************************************************
	*  connect to socket 
	************************************************************/
	if( connect ( sock, (struct sockaddr*)&addr, addr_len) ){
		perror("connect");
		return -1;
	}
	fcntl(sock,F_SETFL,O_NONBLOCK);
	//fp = fdopen(sock,"r");

	/**********************************************************
	*  send buffer (command) *
	***********************************************************/	
	if ( send(sock, send_buffer, strlen(send_buffer), 0) < 1 ) {
		perror("send");
		return -1;
	}
	bzero(p_result_buf,size_result_buf);
	i = 1;
	//alloccount = 2;
	bytesleft = size_result_buf;
	sock_buffer = (char*)malloc(size_result_buf);
	bzero(sock_buffer, size_result_buf);

	/*************************************************************
	 This segment needs explaining:
	 Need to loop continuously to poll for return packets .
	 If a packet is there, append it to result_buf(make sure result_buf is big enough for your needs (ie. cvarlist)
	 Sleep for 1 tenths of a second and check again.
	 If no packet is there, it will check 10 more times (1 sec.)
	 If a packet is received it resets the counter to 1.
         This is kind of sloppy, but it seems to be stable.
	 This should be upgraded to use the proper listen functions later.
	 TODO: replace below with proper poll functions
	 TODO: make result_buf dynamic instead of a char array
	*************************************************************/
	while(i < 10){
		while( read (sock,sock_buffer,( bytesleft - 1 )) > 0 ){
			buflen = strlen(sock_buffer);
			strcat(p_result_buf,sock_buffer);
			bzero(sock_buffer, size_result_buf);
			bytesleft = bytesleft - buflen;
			i=1;
		}
		if(nanosleep(&stimespec,NULL) != 0){
			perror("nanosleep");
			break;
       		}
		i++;
	}
	free(sock_buffer);
	/******************************************************************************************
	* if it found data clean data (some games return lists in 
	* multiple packets instead of one, for instance when requesting cvarlist
	* quake returns a nice printable single packet, but sof2 and mohaa return a packet for each
	* name and value.  yes, that's two packets for one variable, it also prepends each packet 
	* with a "print\n", so this code should (and does as of writing this) replace those 
	*******************************************************************************************/
	if(i > 1){
		while((tmp = strstr(p_result_buf,"print\n")) != NULL){
			for(i=0;i<6;i++){
			    *tmp = ' ';
			    tmp++;
			}
		}
	}
	/**********************************************************
	*  close socket 
	**********************************************************/
	if( close(sock) ){
		perror("close");
		return -1;
	}
	
	return 0;

}

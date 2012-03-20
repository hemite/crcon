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
/**************************************************************
* File: get_player_var.c
* Description: function get_player_var takes a info_list which
 		should be a list of name = value pairs of info
		then should return the value. this was written
		for grabbing player variables from /dumpuser
		commands.  but should work find for others like
		cvarlist, however, this should be done after
		the TODO item in crcon_module.c. Right now
		there's not enough memory in the buffer
		to accept all the cvarlist packets.  This should
		and most likely will be changed soon. After
		which, get_player_var should be renamed to 
		get_var or something. :)
**************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <errno.h>
#include "crcon.h"

#define REGEXP1 "\n[ ]*"
#define REGEXP2 "[ ]*"

/**********************************
* really lame function to print var :)
***********************************/

void print_player_var(char *val){
	printf("%s\n",val);
}
char *get_player_var (char *info_list, char *var){
	
	regex_t *r;
	char p[24] = REGEXP1;
	regmatch_t *result;
	size_t no_sub;
	int err_no = 0;
	int start = 0;
	char *endline;
	
	strcat(p,var);
	strcat(p,REGEXP2);
	r = (regex_t *)malloc(sizeof(regex_t));
	memset(r, 0, sizeof(regex_t));

	if((err_no = regcomp(r, p, REG_EXTENDED))!=0){
		
		size_t length;
		char *buffer;
		length = regerror(err_no, r, NULL, 0);
		buffer = malloc(length);
		regerror(err_no, r, buffer, length);
		perror("player_info");
		free(buffer);
		regfree(r);
		free(r);
		return NULL;
	}
	no_sub = r->re_nsub+1;	
	
	if((result = (regmatch_t *) malloc(sizeof(regmatch_t) * no_sub))==0){
		regfree(r);
		free(r);
		perror("No more memory - aaaagh! (Die kicking and screaming.)");
 		return NULL;
	}	
	
	if(regexec(r, info_list+start, no_sub, result, 0)==0) /* Found a match */
	{
	  info_list = &info_list[result->rm_eo];
          endline = strchr(info_list,'\n'); 
	  endline[0] = '\0';
	//printf("%s\n\n\n\n\n",info_list);exit(1);

	  while((info_list[0] == ':')||(info_list[0] == ' ')){
		info_list++;	
	  }
	  while(info_list[strlen(info_list)-1] == ' '){
		info_list[strlen(info_list)-1] = '\0';
	  }
	  return info_list;
	}

        regfree(r);
	free(r);


	return NULL;
		
} 


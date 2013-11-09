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
/*************************************************************
* File: get_player_list.c
* Description: function get_player_list takes info_list
	       and parses out players from that and builds
	       linked list of players (player_node) and 
	       sets to list master struct player_list returns
	       0 on success or -1
**************************************************************/
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <regex.h>
#include <errno.h>
#include "crcon.h"

#define REGEXP1 "\n([0-9]+[ ]+)*\".[^\"]*\""
#define REGEXP2 "^[^0-9 \"]$"
//void test(){
//	printf("\n");
//}
/******************************************
* couldn't think of better place for this
* free's plist
******************************************/
void free_player_list(player_list *plist){
	player_node *pnode,*cur_pnode;
	cur_pnode = plist->first;
	while(cur_pnode != NULL){
		pnode = cur_pnode->next;
		free(cur_pnode);	
		cur_pnode = pnode;
	}
}
/*****************************************
* function simply prints all players 
*****************************************/
void print_player_list(player_list *plist){
	player_node *cur_pnode;
	cur_pnode = plist->first;
	while(cur_pnode != NULL){ 
		printf("\"%s\"\n",cur_pnode->player_name);
		cur_pnode = cur_pnode->next;
	}
}
/*************************** 
* function get_player_list
****************************/
int get_player_list (char *info_list, player_list *plist){
	
	regex_t *r;
	regmatch_t *result;
	player_node *pnode;
	player_node *cur_pnode = NULL; 
	char p[] = REGEXP1;
	char name[SIZE_NAME];
	size_t no_sub;
	int err_no = 0;
	unsigned short int start = 0;
	unsigned char pcount = 0;
	unsigned short int i;
	unsigned char j;
	char *strindex;
	//char *np;
	
	plist->first = NULL;
	plist->count = 0;

	r = (regex_t *)malloc(sizeof(regex_t));
	memset(r, 0, sizeof(regex_t));

	/************************************
	* build regex struct
	************************************/
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
		return -1;
	}
	/***********************************************************************
	* not sure yet how this works and where this comes from but I snagged
	* code from somwhere else :) I believe it may
	* be the number of sub expressions in my regexp? whatever that means, 
	* apparently there's only one for me.
	************************************************************************/
	no_sub = r->re_nsub+1;	
	if((result = (regmatch_t *) malloc(sizeof(regmatch_t) * no_sub))==0){
		regfree(r);
		free(r);
		perror("No more memory - aaaagh! (Die kicking and screaming.)");
 		return -1;
	}	
	/***********************************
	* loops through each match and 
	***********************************/
	while(regexec(r, info_list+start, no_sub, result, 0)==0) /* Found a match */
	{
		/**********************************************
		* we've found a match (new player)
		* allocate space
		***********************************************/
		if((pnode = (player_node *) malloc(sizeof(player_node)))==0){
			perror("No more memory - aaaagh! (Die kicking and screaming.)");
			regfree(r);
			free(r);
              		return -1;
		}
		memset(pnode->player_name,'\0',SIZE_PLAYER_NAME);
		pnode->next = NULL;
		plist->count++;
		/**********************
		*  if first player 
		**********************/
		if(cur_pnode == NULL){
			plist->first = pnode;
			cur_pnode = pnode;
		/**********************
		* else another player 
		**********************/	
		} else {
			cur_pnode->next = pnode;
			cur_pnode = pnode;
		}
		//np = info_list+start;
		j = 0;
		/******************************************************
		* loop through chars to find name and set to name[]
		******************************************************/
		for(i=result->rm_so;i<result->rm_eo;i++){
			name[j] = info_list[(start+i)];
			j++;
		}
		/*******************************
		* now copy name into list
		*******************************/
		strcpy(cur_pnode->player_name,name);
		name[j] = '\0';
	  	start +=result->rm_eo;
		pcount++;
	}

        regfree(r);
	free(r);

	
	
	/* *****************************************
	 * clean quotes	and bad chars for nice name
	 * nice_name is a player_node member that
	 * holds the "nice name", which is the name
	 * that you enter in rcon commands to get
	 * player info, kick, etc. 
	 * ie: if you have colors your name may be
	 * ( ^7priv^2_^5joker's ), well that's the player
	 * name, as will show up in logs, but
  	 * rcon doen't allow you to reference that
	 * player with that, instead it's ( priv_joker's )
	 * *****************************************/
	
	cur_pnode = plist->first;
	while(cur_pnode != NULL){
		if((strindex = strstr(cur_pnode->player_name,"\"")) != NULL){
			strcpy(cur_pnode->player_name,(strindex+1));	
			if((strindex = strstr(cur_pnode->player_name,"\"")) != NULL){
				*strindex  = '\0';
			}
		}
		memset(name,'\0',SIZE_NAME);
		for(i=0;i<strlen(cur_pnode->player_name);i++){
			if(cur_pnode->player_name[i] == '^'){
				i++;
			} else if((cur_pnode->player_name[i] < 32)||(cur_pnode->player_name[i] > 135)){
				if((cur_pnode->player_name[i] > 140)&&(cur_pnode->player_name[i] < 177)){
					strncat(name,&cur_pnode->player_name[i],1);
				} else {
					continue;
				}

			} else {
				strncat(name,&cur_pnode->player_name[i],1);	
			}
		}
		strcpy(cur_pnode->nice_name,name);
		cur_pnode = cur_pnode->next;
	} 
	return 0;
		
} 


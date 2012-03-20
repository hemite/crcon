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
#include <stdlib.h>

#ifndef _PLAYER_LIST
#define _PLAYER_LIST
typedef struct _player_node {
	unsigned char player_name[SIZE_PLAYER_NAME];
	char nice_name[SIZE_NICE_NAME];
	unsigned long int kioskid;
	unsigned long int userid;
	struct _player_node *next;
} player_node;

typedef struct _player_list {
	unsigned char count;
	player_node *first;
} player_list;


int get_player_list (char *info_list, player_list *plist);
void free_player_list(player_list *plist);
void print_player_list (player_list *plist);
#endif

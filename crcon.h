/*
    crcon - C rcon cli app Version 1.0
	Copyright 2003, Steve Johnson

    This application is free software; you can redistribute it and/or
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
#ifndef CRCON_H
#define CRCON_H

#define DEFAULT_PORT 27960
#define MAX_CMD_SIZE 512
#define MAX_PLAYERS 64
#define MAX_NAME_SIZE 96


//#define SIZE_RESULT_BUF 1024
//#define SIZE_SOCK_BUFFER 1024 
#define SIZE_NAME MAX_NAME_SIZE
#define SIZE_PLAYER_NAME MAX_NAME_SIZE
#define SIZE_NICE_NAME MAX_NAME_SIZE
#define SIZE_SEND_BUFFER 512
#define SIZE_GETVAR 32
#define SIZE_RCON MAX_NAME_SIZE
#define DEFAULT_PORT 27960
#define MAX_CMD_SIZE 512
#define MAX_PLAYERS 64
#define MAX_NAME_SIZE 96
#include "crcon_module.h"
#include "get_player_list.h"
#include "get_player_var.h"
#endif

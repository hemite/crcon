#    crcon - C rcon cli app Version 1.0
#	 Copyright 2003, Steve Johnson


#    This library is free software; you can redistribute it and/or
#    modify it under the terms of the GNU Library General Public
#    License as published by the Free Software Foundation; either
#    version 2 of the License, or (at your option) any later version.
#
#    This application is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
#    Library General Public License for more details.
#
#    You should have received a copy of the GNU Library General Public
#    License along with this library; if not, write to the Free
#    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
#
#    Steve Johnson 
#    steve@webninja.com

CC=gcc 
#OPT=-ggdb -Wall 
OPT=-Wall 

crcon: crcon_module.o get_player_list.o get_player_var.o crcon.c crcon.h
	$(CC) $(OPT) -o crcon crcon.c crcon_module.o get_player_list.o get_player_var.o

get_player_list.o: get_player_list.c get_player_list.h
	$(CC) $(OPT) -c get_player_list.c

get_player_var.o: get_player_var.c get_player_var.h
	$(CC) $(OPT) -c get_player_var.c

db.o: db.c db.h
	$(CC) $(OPT) -c db.c

crcon_module.o: crcon_module.c crcon_module.h
	$(CC) $(OPT) -c crcon_module.c

install: crcon
	cp crcon /usr/local/bin	
clean: 
	rm -f crcon log_players *.o
uninstall: clean
	rm -f /usr/local/bin/crcon

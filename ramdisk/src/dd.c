/**
  * GreenPois0n Anthrax - launchd.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Justin Williams
  *
  * This program is free software: you can redistribute it and/or modify
  * it under the terms of the GNU General Public License as published by
  * the Free Software Foundation, either version 3 of the License, or
  * (at your option) any later version.
  *
  * This program is distributed in the hope that it will be useful,
  * but WITHOUT ANY WARRANTY; without even the implied warranty of
  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  * GNU General Public License for more details.
  *
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 **/

#include "syscalls.h"
#include "utils.h"
#include <string.h>

int main(int argc, char **args) {
	int bs = 512;
	int seek = -1,skip = -1,count = -1;
	char inf[80],of[80];
	int i,j,checksize;
	char *token[80];
	_puts("dd-lite by Chronic-Dev Team\n");
	
	if (argc < 3) {
		_puts("Error! Not enough arguments. Required: if= of=\n");
		exit(1);
	}
	
	//Parse Args
	for (i = 0; i < argc; i++) {
		token[0] = strtok(args[i], "=");
		token[1] = strtok(NULL, "=");
		if (!strcmp(token[0],"if")) {
			strcpy(inf,token[1]);
		} else if (!strcmp(token[0],"of")) {
			strcpy(of,token[1]);
		} else if (!strcmp(token[0],"bs")) {
			bs = atoi(token[1]);
		} else if (!strcmp(token[0],"seek")) {
			seek = atoi(token[1]);
		} else if (!strcmp(token[0],"skip")) {
			skip = atoi(token[1]);
		} else if (!strcmp(token[0],"count")) {
			count = atoi(token[1]);
		}
	}
	char buf[bs];

	int ifd = open(inf, O_RDONLY, S_IRWXU | S_IRGRP | S_IROTH); //Mode is ignored here... but it's fun to have anyway lol
	if (ifd < 0) {
		_puts("Error: Unable to open if\n");
		exit(1);
	}
	int ofd = open(of, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IROTH); //Mode /should/ be ignored here unless we create the file
	if (ofd < 0) {
		_puts("Error: Unable to open of\n");
		exit(1);
	}

	//First Skip...
	if (skip != -1) {
		checksize = lseek(ifd, (bs * skip), SEEK_SET);
		if (checksize != (bs * skip)) {
			_puts("Error: File ran out of space to skip\n");
			close(ifd);
			close(ofd);
			exit(1);
		}
	}
	
	//Now Seek...
	if (seek != -1) {
		checksize = lseek(ofd, (bs * seek), SEEK_SET);
		if (checksize != (bs * seek)) {
			_puts("Error: File ran out of space to seek\n");
			close(ifd);
			close(ofd);
			exit(1);
		}
	}
	
	if (count != -1) {
		for (i = 0; i < count; i++) {
			//Now do our copying
			checksize = read(ifd, buf, bs);
			if (checksize <= -1) {
				puts_("Error: File ran out of space to copy on copy-read\n",checksize,bs);
				close(ifd);
				close(ofd);
				exit(1);
			} else if (checksize != bs) { //Write our last few bytes and break. This is not the size of bs though
				checksize = write(ofd, buf, checksize);
			} else {
				checksize = write(ofd, buf, bs);
			}
			
			if (checksize == -1) {
				_puts("Error: File ran out of space to copy on copy-write\n");
				close(ifd);
				close(ofd);
				exit(1);
			}
			
			if (i != count && checksize != bs) { //See if we are done copying even though we were told to keep going
				puts_("Warning: Not enough space in if to complete number of block copies specified in count\n");
				puts_("Wrote remaining bytes to file anyway.\n");
				break;
			}
		}
	} else {
		//Whole file
		do {
			checksize = read(ifd, buf, bs);
			if (checksize <= -1) {
				_puts("Error: File ran out of space to copy on copy-read\n",checksize,bs);
				close(ifd);
				close(ofd);
				exit(1);
			} else if (checksize != bs) { //Write our last few bytes and break. This is not the size of bs though
				checksize = write(ofd, buf, checksize);	
			} else {
				checksize = write(ofd, buf, bs);
			}
			if (checksize == -1) {
				_puts("Error: File ran out of space to copy on copy-write (%d/%d)\n",checksize,bs);
				close(ifd);
				close(ofd);
				exit(1);
			}
		} while (checksize == bs);
	}
	_puts("Successful!\n");
	close(ifd);
	close(ofd);
	exit(0);
}

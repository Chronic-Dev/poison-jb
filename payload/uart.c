/**
  * GreenPois0n Cynanide - uart.c
  * Copyright (C) 2010 Chronic-Dev Team
  * Copyright (C) 2010 Joshua Hill
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

#include "uart.h"
#include "common.h"
#include "commands.h"
#include "functions.h"

void* find_uart_read() {
	return find_function(TARGET_BASEADDR, TARGET_BASEADDR, "uart_read");
}

void* find_uart_write() {
	return find_function(TARGET_BASEADDR, TARGET_BASEADDR, "uart_write");
}

int uart_init() {
	return 0;
}

int uart_cmd(int argc, CmdArg* argv) {
	return 0;
}

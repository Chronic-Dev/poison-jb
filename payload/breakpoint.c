/*
 *
 *  greenpois0n - payload/src/breakpoint.c
 *  (c) 2010 Chronic-Dev Team
 *
 */

#include <stdio.h>
#include <string.h>

#include "lock.h"
#include "common.h"
#include "breakpoint.h"
#include "coprocessor.h"

/*
 * Public Functions
 */

BreakpointEntry* breakpoint_root = NULL;

BreakpointLog* breakpoint_logs = NULL;

void breakpoint_log_list() {
	BreakpointLog *bl = breakpoint_logs;
	enter_critical_section();
	if (bl==NULL) {
		printf("The log is empty.\n");
	} else {
		printf("Breakpoint log :\n");
		while (bl!=NULL) {
			printf("#%04d id:%04d pc=0x%08x\n", bl->id, bl->bp_id, bl->pc);
			bl = bl->next;
		}
	}
	exit_critical_section();
}

void breakpoint_log_show(unsigned int id) {
	BreakpointLog *bl = breakpoint_log_find(id);
	enter_critical_section();
	if (bl==NULL) {
		printf("[!] Can't find the log entry of id %d\n", id);
	} else {
		printf("Breakpoint log entry #%d:\n", bl->id);
		printf("\nBreakpoint %d hit at 0x%08x\n", bl->bp_id, bl->pc);
		printf(" r0:  0x%08x  r1:  0x%08x  r2:  0x%08x  r3:   0x%08x\n", bl->r0, bl->r1, bl->r2, bl->r3);
		printf(" r4:  0x%08x  r5:  0x%08x  r6:  0x%08x  r7:   0x%08x\n", bl->r4, bl->r5, bl->r6, bl->r7);
		printf(" r8:  0x%08x  r9:  0x%08x  r10: 0x%08x  r11:  0x%08x\n", bl->r8, bl->r9, bl->r10, bl->r11);
		printf(" r12: 0x%08x  sp:  0x%08x  lr:  0x%08x  spsr: 0x%08x\n", bl->r12, bl->sp, bl->lr, bl->spsr);

		printf("\nStack Dump:\n");
		int sp = bl->sp;
		int i;
		for(i = 0; i < 0x10; i += 4) {
			printf("0x%08x:  0x%08x  0x%08x  0x%08x  0x%08x\n", sp, bl->stack[i], bl->stack[i+1], bl->stack[i+2], bl->stack[i+3]);
			sp += 0x10;
		}
	
		if (bl->hexdump_result!=NULL) {	
			printf("\nMemory Dump of 0x%08x:\n", bl->hexdump_address);
			hexdump((unsigned char*) bl->hexdump_result, bl->hexdump_len);
		}

		printf("\n");
	}
	exit_critical_section();
}

void breakpoint_log_clear() {
	BreakpointLog *bl = breakpoint_logs;
	while (bl!=NULL) {
		BreakpointLog *tmp = bl;
		bl = bl->next;
		if (tmp->hexdump_result!=NULL) free(tmp->hexdump_result);
		free(tmp);
	}
	breakpoint_logs = NULL;
}

BreakpointLog* breakpoint_log_find(unsigned int id) {
	BreakpointLog *bl = breakpoint_logs;
	while (bl!=NULL) {
		if (bl->id==id) {
			return bl;
		}
		bl = bl->next;
	}
	return NULL;
}

void breakpoint_log_add(BreakpointLog *log) {
	if (breakpoint_logs==NULL) {
		log->id = 0;
		log->next = NULL;
		breakpoint_logs = log;
	} else {
		// seek to last element
		BreakpointLog *bl = breakpoint_logs;
		int id = 0;
		while (bl->next!=NULL) {
			id = bl->id + 1; 
			bl = bl->next;
		}
		if (bl!=NULL) id = bl->id +1;
		log->id = id;
		log->next = NULL;
		bl->next = log;
	}
}

BreakpointLog* breakpoint_log_remove(unsigned int id) {
	BreakpointLog *prev = breakpoint_logs;
	BreakpointLog *bl = breakpoint_logs;
	while (bl!=NULL) {
		if (bl->id==id) {
			prev->next = bl->next;
			return bl;
		}
		prev = bl;
		bl = bl->next;
	}
	return NULL;
}

void breakpoint_list() {
	enter_critical_section();

	BreakpointEntry* bp = breakpoint_root;
	if(bp == NULL) {
		printf("No active breakpoints\n");
	} else {
		printf("Breakpoints:\n");
		while(bp != NULL) {
			if(*((unsigned int*)bp->address) == bp->value) {
				*((volatile unsigned short*)bp->address) = BKPT_THUMB;
			}
			printf("  [%d] 0x%08x reset:%s hexdump:0x%08x-0x%08x\n", bp->id, bp->address,
					bp->reset ? "true" : "false", bp->hexdump_address, bp->hexdump_len);

			bp = bp->next;
		}
		printf("\n");
	}

	exit_critical_section();
}

BreakpointEntry* breakpoint_add(void* address, Bool reset) {
	BreakpointEntry* bp = breakpoint_root;
	if(bp != NULL) {
		while(bp != NULL) {
			if(*((unsigned int*)bp->address) == bp->value) {
				*((volatile unsigned short*)bp->address) = BKPT_THUMB;
			}

			if(bp->next == NULL) {
				BreakpointEntry* new_bp = malloc(sizeof(BreakpointEntry));
				new_bp->id = bp->id + 1;
				new_bp->reset = reset;
				new_bp->value = *((unsigned int*)address);
				new_bp->address = (void*)address;
				new_bp->next = NULL;
				new_bp->hexdump_address = NULL;
				new_bp->hexdump_len = 0;
				bp->next = new_bp;

				*((volatile unsigned short*)address) = BKPT_THUMB;
				clear_icache();
				//clear_dcache();

				return new_bp;
			}
			bp = bp->next;
		}

	} else {
		// no breakpoints exist yet, create breakpoint_root
		breakpoint_root = malloc(sizeof(BreakpointEntry));
		breakpoint_root->id = 0;
		breakpoint_root->reset = reset;
		breakpoint_root->value = *((unsigned int*)address);
		breakpoint_root->address = (void*)address;
		breakpoint_root->next = NULL;
		breakpoint_root->hexdump_address = NULL;
		breakpoint_root->hexdump_len = 0;

		*((volatile unsigned short*)address) = BKPT_THUMB;
		clear_icache();
		//clear_dcache();

		return breakpoint_root;
	}

	return NULL;
}

Bool breakpoint_remove(unsigned int id) {
	BreakpointEntry* prev = NULL;
	BreakpointEntry* bp = breakpoint_root;
	while(bp != NULL) {
		if(bp->id == id) {
			if(prev == NULL) {
				breakpoint_root = bp->next;
			} else {
				prev->next = bp->next;
			}
			*((volatile unsigned int*)bp->address) = bp->value;
			clear_icache();
			//clear_dcache();
			
			//free(bp);
			return TRUE;
		}
		prev = bp;
		bp = bp->next;
	}

	return FALSE;
}

/*
 * Private Functions
 */
void breakpoint(unsigned int* stack) {
	BreakpointEntry* bp = breakpoint_root;

	unsigned int r[13];
	unsigned int *sp = (unsigned int*) stack[0];
	unsigned int lr = stack[1];
	unsigned int spsr = stack[2];
	unsigned int pc = stack[16];
	unsigned int i = 0;
	for(i = 0; i <= 12; i++) {
		r[i] = stack[i+3];
	}
	
	unsigned int id = 0;
	void* hexdump_address = NULL;
	unsigned int hexdump_len = 0;
	while(bp != NULL) {
		if(bp->address == (void*) pc) {
			if(bp->reset) {
				// reset our original breakpoint and remove our reset breakpoint
				hexdump_address = bp->hexdump_address;
				hexdump_len = bp->hexdump_len;
				if (breakpoint_remove(bp->id)) {
					BreakpointEntry *bp_copy = breakpoint_add(bp->address - 2, FALSE);
					bp_copy->hexdump_address = hexdump_address;
					bp_copy->hexdump_len = hexdump_len;
				}
				return;
			}

			// add the reset breakpoint and remove the original
			id = bp->id;
			hexdump_address = bp->hexdump_address;
			hexdump_len = bp->hexdump_len;
			if (breakpoint_remove(bp->id)) {
				BreakpointEntry *bp_reset = breakpoint_add(bp->address + 2, TRUE);
				bp_reset->hexdump_address = hexdump_address;
				bp_reset->hexdump_len = hexdump_len;
			}
			
			break;
		}
		bp = bp->next;
	}

	BreakpointLog *bl = malloc(sizeof(BreakpointLog));
	bl->r0 = r[0];
	bl->r1 = r[1];
	bl->r2 = r[2];
	bl->r3 = r[3];
	bl->r4 = r[4];
	bl->r5 = r[5];
	bl->r6 = r[6];
	bl->r7 = r[7];
	bl->r8 = r[8];
	bl->r9 = r[9];
	bl->r10 = r[10];
	bl->r11 = r[11];
	bl->r12 = r[12];
	bl->sp = (unsigned int) sp;
	bl->lr = lr;
	bl->spsr = spsr;
	bl->pc = pc;
	bl->bp_id = id;
	for(i = 0; i < 0x10; i++) {
		bl->stack[i] = sp[i];
	}
	if (hexdump_len > 0) {
		unsigned char* buf = (unsigned char*) malloc(hexdump_len);
		memcpy(buf, hexdump_address, hexdump_len);
		bl->hexdump_address = hexdump_address;
		bl->hexdump_len = hexdump_len;
		bl->hexdump_result = (void*) buf;
	} else {
		bl->hexdump_address = NULL;
		bl->hexdump_len = 0;
		bl->hexdump_result = NULL;
	}
	breakpoint_log_add(bl);
	breakpoint_log_show(bl->id);
}

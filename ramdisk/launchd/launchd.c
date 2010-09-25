#include "utils.h"

const char* greeting = "GreenPois0n Ramdisk - by Chronic-Dev\n";

int main() {
	//LDR     R0, =aDevConsole ; "dev/console"
	//MOV     R1, #0          ; int
	//MOV     R2, #0
	//BL      _open
	int stdout = open("/dev/console", 0, 0);
	//MOV     R3, R0
	//ADD     R1, SP, #0x10C8+var_C8
	//STR     R3, [R1,#0x94]
	//LDR     R0, =aDevConsole_0 ; "/dev/console"
	//MOV     R1, #2          ; int
	//MOV     R2, #0
	//BL      _open
	int stderr = open("/dev/console", 2, 0);
	//MOV     R2, R0
	//LDR     R3, =_stdout
	//STR     R2, [R3]
	//LDR     R0, =aWelcomeToPurpl ; "welcome to purpled4sk\n"
	//BL      _puts
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	puts(greeting);
	//LDR     R0, =aThisRamdiskIsA ; "this ramdisk is a piece of shit\n"
	//BL      _puts
	//LDR     R3, =aDevDisk0s1 ; "/dev/disk0s1"
	//ADD     R2, SP, #0x10C8+var_C8
	//STR     R3, [R2,#0x98]
	//LDR     R0, =aLookingForTheH ; "looking for the hard drive...\n"
	//BL      _puts
	close(stdout);
	close(stderr);
	//reboot(1);
	return 0;
}

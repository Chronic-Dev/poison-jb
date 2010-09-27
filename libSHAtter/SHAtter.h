/********************************
 * SHAtter.h - For use by GUI's *
 ********************************/
int initInjector(); //Returns 0 on success.
int isInDFU(); //Returns 0 on success, -1 if failed to connect, -2 if not in DFU
int isCompatibleDevice(); //Returns 0 on success, -1 if failed to discover type, -2 if incompatible device
int SHAtter(); //Returns 0 on success.


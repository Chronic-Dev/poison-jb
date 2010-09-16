/*
 *
 *  greenpois0n - payload/include/coprocessor.h
 *  (c) 2010 Chronic-Dev Team
 *
 */

#ifndef COPROCESSOR_H
#define COPROCESSOR_H

// c0
int read_processor_id();
int read_processor_features();
int read_silicon_id();

// c1
int read_control_register();
void write_control_register(int value);
int read_auxcontrol_register();
void write_auxcontrol_register(int value);

// c2
int read_TBB0();
void write_TBB0(int value);

// c3
int read_DAC();
void write_DAC(int value);

// c7
void clear_icache();
void clear_dcache();

// multiple
void clear_cpu_caches();

#endif

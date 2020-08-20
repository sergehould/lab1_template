/* 
 * File:   isr_driven.h
 * Author: sh
 *
 * Created on November 22, 2018, 8:43 AM
 */

#ifndef GENERATOR_H
#define	GENERATOR_H

#include "xc.h"
#define _ISR_NO_PSV  __attribute__((__interrupt__, no_auto_psv))
void initT3_50Hz( void);
void initT3_3kHz( void);
#endif	/* GENERATOR_H */


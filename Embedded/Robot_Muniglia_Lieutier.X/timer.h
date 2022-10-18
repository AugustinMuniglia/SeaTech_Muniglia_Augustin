/* 
 * File:   timer.h
 * Author: TP-EO-1
 *
 * Created on 26 septembre 2022, 16:28
 */

#ifndef TIMER_H
#define	TIMER_H

void InitTimer23(void);
void InitTimer1(void);
void PWMUpdateSpeed();
void InitTimer4(void);
void SetFreqTimer1(float freq);
void SetFreqTimer4(float freq);
extern unsigned long timestamp;

#endif	/* TIMER_H */


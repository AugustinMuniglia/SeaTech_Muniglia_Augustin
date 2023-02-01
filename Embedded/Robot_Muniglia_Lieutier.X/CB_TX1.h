/* 
 * File:   CB_TX1.h
 * Author: TP-EO-1
 *
 * Created on 1 février 2023, 15:03
 */

#ifndef CB_TX1_H
#define	CB_TX1_H

void SendMessage(unsigned char* message, int length);
void CB_TX1_Add(unsigned char value);
unsigned char CB_TX1_Get(void);
unsigned char CB_TX1_IsTransmitting(void);
int CB_TX1_GetDataSize(void);
int CB_TX1_GetRemainingSize(void);
void SendOne();

#endif	/* CB_TX1_H */


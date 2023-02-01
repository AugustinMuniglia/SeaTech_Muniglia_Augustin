/* 
 * File:   UART.h
 * Author: TP-EO-1
 *
 * Created on 27 janvier 2023, 12:05
 */

#ifndef UART_H
#define UART_H

void InitUART(void);
void SendMessageDirect(unsigned char* message, int length);

#endif /* UART_H */
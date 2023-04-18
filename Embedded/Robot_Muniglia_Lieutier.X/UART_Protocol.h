/* 
 * File:   UART_Protocol.h
 * Author: TP-EO-1
 *
 * Created on 18 avril 2023, 09:58
 */

#ifndef UART_PROTOCOL_H
#define	UART_PROTOCOL_H

unsigned char UartCalculateChecksum(int, int, unsigned char*);
void UartEncodeAndSendMessage(int, int, unsigned char*);
int msgDecodedFunction;
int msgDecodedPayloadLength;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex;
//void UartDecodeMessage(unsigned char);
//void UartProcessDecodedMessage(unsigned char, unsigned char, unsigned char*);

#endif	/* UART_PROTOCOL_H */
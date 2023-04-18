#include <xc.h>
#include "UART_Protocol.h"
#include "UART.h"

unsigned char UartCalculateChecksum(int msgFunction, int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction prenant entrée la trame et sa longueur pour calculer le checksum
    unsigned char checksum = 0;
    checksum ^= 0xFE;
    checksum ^= (unsigned char)(msgFunction >> 8);
    checksum ^= (unsigned char)(msgFunction);
    checksum ^= (unsigned char)(msgPayloadLength >> 8);
    checksum ^= (unsigned char)(msgPayloadLength);
    for (int i = 0; i < msgPayloadLength; i++)
    {
        checksum ^= msgPayload[i];
    }
    return checksum;
}

void UartEncodeAndSendMessage(int msgFunction, int msgPayloadLength, unsigned char* msgPayload)
{
//Fonction d?encodage et d?envoi d?un message
    unsigned char Frame[msgPayloadLength + 6];
    int pos = 0;

    Frame[pos++] = (unsigned char)0xFE;
    Frame[pos++] = (unsigned char)(msgFunction >> 8);
    Frame[pos++] = (unsigned char)(msgFunction);
    Frame[pos++] = (unsigned char)(msgPayloadLength >> 8);
    Frame[pos++] = (unsigned char)(msgPayloadLength);

    for (int i = 0; i < msgPayloadLength; i++)
    {
        Frame[pos++] = msgPayload[i];
    }
    Frame[pos++] = UartCalculateChecksum(msgFunction, msgPayloadLength, msgPayload);
    
    SendMessageDirect(Frame, msgPayloadLength);
}

int msgDecodedFunction = 0;
int msgDecodedPayloadLength = 0;
unsigned char msgDecodedPayload[128];
int msgDecodedPayloadIndex = 0;

//void UartDecodeMessage(unsigned char c)
//{
////Fonction prenant en entrée un octet et servant à reconstituer les trames
//?
//}
//
//void UartProcessDecodedMessage(unsigned char function, unsigned char payloadLength, unsigned char* payload)
//{
////Fonction appelée après le décodage pour exécuter l?action
////correspondant au message reçu
//?
//}
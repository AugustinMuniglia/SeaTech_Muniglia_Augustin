/* 
 * File:   main.c
 * Author: TP-EO-1
 *
 * Created on 26 septembre 2022, 14:56
 */


#include <stdio.h>
#include <stdlib.h>
#include <xc.h>
#include "ChipConfig.h"
#include "IO.h"
#include "timer.h"
#include "PWM.h"
#include "ADC.h"
#include "Robot.h"
#include "main.h"

unsigned char sensorState;

int main(void) {

    //Initialisation de l'oscillateur
    InitOscillator();

    // Configuration des entrées sorties
    InitIO();

    //Initialisation des timers
    InitTimer1();
    InitTimer23();
    InitTimer4();

    //Initialisation Moteur
    InitPWM();
    
    //Initiation ADC
    InitADC1();
            
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;

    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        if (ADCIsConversionFinished() == 1 ){
            ADCClearConversionFinishedFlag();
            unsigned int * result = ADCGetResult();
            
            float volts = ((float)result[3]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtrGauche = 34 / volts - 5;
            
            volts = ((float)result[4])* 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreGauche = 34 / volts - 5;
            
            volts = ((float)result[2]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreCentre = 34 / volts - 5;
            
            volts = ((float)result[1]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreDroit = 34 / volts - 5;
            
            volts = ((float)result[0]) * 3.3 / 4096 * 3.2;
            robotState.distanceTelemetreExtrDroit = 34 / volts - 5; 
            
            sensorState = 0;
            if(robotState.distanceTelemetreExtrGauche<=30)
                sensorState = sensorState | 0b10000;
            if(robotState.distanceTelemetreGauche<=30)
                sensorState = sensorState | 0b01000;
            if(robotState.distanceTelemetreCentre<=30)
                sensorState = sensorState | 0b00100;
            if(robotState.distanceTelemetreDroit<=30)
                sensorState = sensorState | 0b00010;
            if(robotState.distanceTelemetreExtrDroit<=30)
                sensorState = sensorState | 0b00001;
            
            if (robotState.distanceTelemetreExtrGauche <= 30){
                LED_BLANCHE = 1;
            }
            else{
                LED_BLANCHE = 0;
            }
            
            if (robotState.distanceTelemetreCentre <= 30){
                LED_BLEUE = 1;
            }
            else{
                LED_BLEUE = 0;
            }
            
            if (robotState.distanceTelemetreExtrDroit <= 30){
                LED_ORANGE = 1;
            }
            else{
                LED_ORANGE = 0;
            }
        }
    } // fin main
}

unsigned char stateRobot;

void OperatingSystemLoop(void) {
    switch (stateRobot) {
        case STATE_ATTENTE:
            timestamp = 0;
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_ATTENTE_EN_COURS;
        case STATE_ATTENTE_EN_COURS:
            if (timestamp > 1000)
                stateRobot = STATE_AVANCE;
            break;

        case STATE_AVANCE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_AVANCE_EN_COURS;
            break;
        case STATE_AVANCE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_GAUCHE:
            PWMSetSpeedConsigne(25, MOTEUR_DROIT);
            PWMSetSpeedConsigne(0, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_DROITE:
            PWMSetSpeedConsigne(0, MOTEUR_DROIT);
            PWMSetSpeedConsigne(25, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_GAUCHE:
            PWMSetSpeedConsigne(15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(-15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_GAUCHE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;

        case STATE_TOURNE_SUR_PLACE_DROITE:
            PWMSetSpeedConsigne(-15, MOTEUR_DROIT);
            PWMSetSpeedConsigne(15, MOTEUR_GAUCHE);
            stateRobot = STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS;
            break;
        case STATE_TOURNE_SUR_PLACE_DROITE_EN_COURS:
            SetNextRobotStateInAutomaticMode();
            break;
            
        default:
            stateRobot = STATE_ATTENTE;
            break;
    }
}

unsigned char nextStateRobot = 0;

void SetNextRobotStateInAutomaticMode() {    
    switch(sensorState)
    {
        case 0b00000:
           nextStateRobot = STATE_AVANCE;
           break;
        case 0b00001:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b00010:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b00011:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b00100:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b00101:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b00110:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b00111:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b01000:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b01001:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b01010:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b01011:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b01100:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b01101:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b01110:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b01111:
           nextStateRobot = STATE_TOURNE_GAUCHE;
           break;
        case 0b10000:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b10001:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b10010:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b10011:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b10100:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b10101:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b10110:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b10111:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
           break;
        case 0b11000:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b11001:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
           break;
        case 0b11010:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b11011:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
           break;
        case 0b11100:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b11101:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
           break;
        case 0b11110:
           nextStateRobot = STATE_TOURNE_DROITE;
           break;
        case 0b11111:
           nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;
           break;
    }

    //Si l'on n'est pas dans la transition de l'étape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
    }
}

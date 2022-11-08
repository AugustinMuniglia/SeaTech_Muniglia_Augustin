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
    unsigned char positionObstacle = PAS_D_OBSTACLE;
    float d = 35;

    //Détermination de la position des obstacles en fonction des télémètres
    if (robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //D
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //C/D
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ||//ED
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ||//ED/C
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ||//D/ED
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ||//C/D/ED
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ) //G/C/D/ED
        positionObstacle = OBSTACLE_A_DROITE;
    
    else if (robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d || //EG
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //G
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d || //G/EG
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d || //C/EG
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //C/G
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d || //C/G/EG
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d ) //D/C/EG/G
        positionObstacle = OBSTACLE_A_GAUCHE;
    
    else if (robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //centre
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //G&D
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d || //GDC
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d || //tous
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d ||//D/G/ED/EG
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d || //EG/ED
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d )//C/EG/ED 
        if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreGauche)
            positionObstacle = OBSTACLE_EN_FACE_DROIT;
        else if (robotState.distanceTelemetreDroit < robotState.distanceTelemetreGauche)
            positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
    
    else if (robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche > d) //pas d'obstacle
        positionObstacle = PAS_D_OBSTACLE;
        
    else if (robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d ||
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d ||
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit > d &&
            robotState.distanceTelemetreExtrGauche < d)
        if (robotState.distanceTelemetreDroit > robotState.distanceTelemetreExtrGauche)
            positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
        else if (robotState.distanceTelemetreDroit < robotState.distanceTelemetreExtrGauche)
            positionObstacle = OBSTACLE_EN_FACE_DROIT;
     
    else if (robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ||
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d ||
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche > d)
        if (robotState.distanceTelemetreExtrDroit > robotState.distanceTelemetreGauche)
            positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
        else if (robotState.distanceTelemetreExtrDroit < robotState.distanceTelemetreGauche)
            positionObstacle = OBSTACLE_EN_FACE_DROIT;
        
    else if (robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d ||
            robotState.distanceTelemetreDroit > d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche < d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d ||
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre > d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d||
            robotState.distanceTelemetreDroit < d &&
            robotState.distanceTelemetreCentre < d &&
            robotState.distanceTelemetreGauche > d &&
            robotState.distanceTelemetreExtrDroit < d &&
            robotState.distanceTelemetreExtrGauche < d)
        if (robotState.distanceTelemetreExtrDroit > robotState.distanceTelemetreExtrGauche)
            positionObstacle = OBSTACLE_EN_FACE_GAUCHE;
        else if (robotState.distanceTelemetreExtrDroit < robotState.distanceTelemetreExtrGauche)
            positionObstacle = OBSTACLE_EN_FACE_DROIT; 
    
    //Détermination de l'état à venir du robot
    if (positionObstacle == PAS_D_OBSTACLE)
        nextStateRobot = STATE_AVANCE;
    
    else if (positionObstacle == OBSTACLE_A_DROITE)
        nextStateRobot = STATE_TOURNE_GAUCHE;
    
    else if (positionObstacle == OBSTACLE_A_GAUCHE)
        nextStateRobot = STATE_TOURNE_DROITE;
    
    else if (positionObstacle == OBSTACLE_EN_FACE_GAUCHE)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_DROITE;
    
    else if (positionObstacle == OBSTACLE_EN_FACE_DROIT)
        nextStateRobot = STATE_TOURNE_SUR_PLACE_GAUCHE;

    //Si l'on n'est pas dans la transition de l'étape en cours
    if (nextStateRobot != stateRobot - 1) {
        stateRobot = nextStateRobot;
    }
}

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

int main(void) {

    //Initialisation de l'oscillateur
    InitOscillator();

    // Configuration des entrées sorties
    InitIO();

    //Initialisation des timers
    InitTimer1();
    InitTimer23();

    //Initialisation Moteur
    InitPWM();
//    PWMSetSpeed(-20, MOTEUR_DROIT);
//    PWMSetSpeed(-20, MOTEUR_GAUCHE);
    PWMSpeedConsigne(15, MOTEUR_DROIT);
    PWMSpeedConsigne(15, MOTEUR_GAUCHE);
            
    LED_BLANCHE = 1;
    LED_BLEUE = 1;
    LED_ORANGE = 1;

    /****************************************************************************************************/
    // Boucle Principale
    /****************************************************************************************************/
    while (1) {
        //    LED_BLANCHE = !LED_BLANCHE;
        //    LED_ORANGE = !LED_ORANGE;
        //    LED_BLEUE = !LED_BLEUE;
    } // fin main
}

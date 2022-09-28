/* 
 * File:   PWM.h
 * Author: TP-EO-1
 *
 * Created on 28 septembre 2022, 08:43
 */

#ifndef PWM_H
#define	PWM_H
#define MOTEUR_DROIT 0
#define MOTEUR_GAUCHE 1

void InitPWM(void);
void PWMSetSpeed(float vitesseEnPourcents, int numMoteur);
void PWMUpdateSpeed();
void PWMSpeedConsigne(float vitesse, int numMoteur);

#endif	/* PWM_H */


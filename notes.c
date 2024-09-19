/*
 * notes.c
 *
 *  Created on: Apr 3, 2024
 *      Author: mrcloutier
 */

#include "notes.h"
#include "peripherals.h"
#include <msp430.h>

int score = 0;

void playNote(int frequency){

    float freq = 32768 / frequency;

    // Initialize PWM output on P3.5, which corresponds to TB0.5
    P3SEL |= BIT5; // Select peripheral output mode for P3.5
    P3DIR |= BIT5;

    TB0CTL  = (TBSSEL__ACLK|ID__1|MC__UP);  // Configure Timer B0 to use ACLK, divide by 1, up mode
    TB0CTL  &= ~TBIE;                       // Explicitly Disable timer interrupts for safety

    // Now configure the timer period, which controls the PWM period
    // Doing this with a hard coded values is NOT the best method
    // We do it here only as an example. You will fix this in Lab 2.
    TB0CCR0   = freq;                    // Set the PWM period in ACLK ticks
    TB0CCTL0 &= ~CCIE;                  // Disable timer interrupts

    // Configure CC register 5, which is connected to our PWM pin TB0.5
    TB0CCTL5  = OUTMOD_7;                   // Set/reset mode for PWM
    TB0CCTL5 &= ~CCIE;                      // Disable capture/compare interrupts
    TB0CCR5   = TB0CCR0/2;
}

void playA1(){
    playNote(440);
}

void playBb1(){
    playNote(466);
}

void playB1(){
    playNote(494);
}

void playC1(){
    playNote(523);
}

void playCS1(){
    playNote(554);
}

void playD1(){
    playNote(587);
    configUserLED(BIT3);
    char button = buttonPress();
    if(button == BIT3){
        Graphics_drawStringCentered(&g_sContext, "TO BEGIN", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
        Graphics_flushBuffer(&g_sContext);
    }
}

void playEb1(){
    playNote(622);
}

void playE1(){
    playNote(659);
}

void playF1(){
    playNote(698);
}

void playFS1(){
    playNote(740);
}

void playG1(){
    playNote(784);
}

void playAb1(){
    playNote(831);
}

void playA2(){
    playNote(880);
}

void playAS2(){
    playNote(932);
}



/************** ECE2049 DEMO CODE ******************/
/**************  25 August 2021   ******************/
/***************************************************/

#include <msp430.h>

/* Peripherals.c and .h are where the functions that implement
 * the LEDs and keypad, etc are. It is often useful to organize
 * your code by putting like functions together in files.
 * You include the header associated with that file(s)
 * into the main file of your project. */
#include "peripherals.h"
#include "notes.h"

long unsigned int timer_cnt = 0;
char tdir = 1;

void setButtons(){
     P7SEL = P7SEL & ~(BIT4 | BIT0);
     P7DIR = P7DIR & ~(BIT4 | BIT0);
     P3SEL = P3SEL & ~(BIT6);
     P3DIR = P3DIR & ~(BIT6);
     P2SEL = P2SEL & ~(BIT2);
     P2DIR = P2DIR & ~(BIT2);
     P7REN = P7REN | (BIT4 | BIT0);
     P7OUT = P7OUT | (BIT4 | BIT0);
     P3REN = P3REN | BIT6;
     P3OUT = P3OUT | BIT6;
     P2REN = P2REN | BIT2;
     P2OUT = P2OUT | BIT2;
 }

 char buttonPress(){
     char button1 = P7IN & BIT0;
     char button2 = (P3IN & BIT6) >> 6;
     char button3 = (P2IN & BIT2) >> 2;
     char button4 = (P7IN & BIT4) >> 4;
     char button = button1 + (button2 >> 1) + (button3 >> 2) + (button4 >> 3);
     return button;
 }

 void configUserLED(char inbits){
     P6SEL = P6SEL & ~(BIT4 | BIT3 | BIT2 | BIT1);
     P6DIR = P6DIR | (BIT4 | BIT3 | BIT2 | BIT1);
     P6OUT = P6OUT & ~(BIT4 | BIT3 | BIT2 | BIT1);
     char led = 0;
     if(inbits & BIT0){
         led = led | BIT4;
     }
     if(inbits & BIT1){
         led = led | BIT3;
     }
     if(inbits & BIT2){
         led = led | BIT1;
     }
     if(inbits & BIT3){
         led = led | BIT2;
     }
     P6OUT = P6OUT | led;
 }

 void runtimerA2(void)
 {
 // This function configures and starts Timer A2
 // Timer is counting ~0.01 seconds
 //
 // Input: none, Output: none
 //
 // smj, ECE2049, 17 Sep 2013
 //
 // Use ACLK, 16 Bit, up mode, 1 divider
 TA2CTL = TASSEL_1 + MC_1 + ID_0;
 TA2CCR0 = 655; // 327+1 = 328 ACLK tics = ~1/100 seconds
 TA2CCTL0 = CCIE; // TA2CCR0 interrupt enabled
 }
 void stoptimerA2(int reset)
 {
 // This function stops Timer A2 andresets the global time variable
 // if input reset = 1
 //
 // Input: reset, Output: none
 //
 // smj, ECE2049, 17 Sep 2013
 //
 TA2CTL = MC_0; // stop timer
 TA2CCTL0 &= ~CCIE; // TA2CCR0 interrupt disabled
 if(reset)
 timer_cnt = 0;
 }
 void displayTime(unsigned int inTme)
 {
 // This converts the global counter variable timer_cnt to a display
 // of minutes and seconds -- MM:SS.S
 //
 // Input: none, Output: none
 //
 // smj, ECE2049, 17 Sep 2013
 char asc_arr[6];
 unsigned int min;
 unsigned int sec;
 unsigned int msec;

 /* Calling the draw command with OPAQUE_TEXT instead of TRANSPARENT_TEXT
 removes the need to clear whole screen with every update */
 Graphics_drawStringCentered(&g_sContext, asc_arr,6,51,32,OPAQUE_TEXT);
 Graphics_flushBuffer(&g_sContext);
 }
 // Timer A2 interrupt service routine
 #pragma vector=TIMER2_A0_VECTOR
 __interrupt void TimerA2_ISR (void)
 {
 if(tdir)
 {
 timer_cnt++;
 if (timer_cnt == 60000)
 timer_cnt = 0;
 if (timer_cnt%100==0) // blink LEDs once a second
 {
 P1OUT = P1OUT ^ BIT0;
 P4OUT ^= BIT7;
 }
 }
 else
 timer_cnt--;
 }

// Declare globals here

// Main
void main(void)

{
    unsigned char currKey=0, dispSz = 3;
    unsigned char dispThree[3];
    unsigned int tempo = 0;

    WDTCTL = WDTPW | WDTHOLD;    // Stop watchdog timer. Always need to stop this!!
                                 // You can then configure it properly, if desired
    // Useful code starts here
    initLeds();

    configDisplay();
    configKeypad();


    char state = 0;
    int count = 0;
    while(1){
    switch(state){
        case 0: //Welcome Screen
            Graphics_drawStringCentered(&g_sContext, "PRESS", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "*", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
            Graphics_drawStringCentered(&g_sContext, "TO BEGIN", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
            Graphics_flushBuffer(&g_sContext);
            currKey = getKey();
            if(currKey == '*'){
                state = 1;
                count = 0;
                runtimerA2();
            }
            break;
        case 1: //Countdown
            if(timer_cnt % 100000 == 0){
                count++;
            }

            if(count == 1000){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "3", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                configUserLED(BIT3);
            }
            if(count == 2000){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "2", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                configUserLED(BIT2);
                        }
            if(count == 3000){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "1", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                configUserLED(BIT1);
                                    }
            if(count == 4000){
                Graphics_clearDisplay(&g_sContext);
                Graphics_drawStringCentered(&g_sContext, "GO", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                Graphics_flushBuffer(&g_sContext);
                configUserLED(BIT3 | BIT2 | BIT1 | BIT0);
                              }
            if(count == 5000){
                Graphics_clearDisplay(&g_sContext);
                state = 2;
                stoptimerA2(1);
                runtimerA2();
                count = 0;
                tempo = 0;
            }
            break;
        case 2: //play the song
                if(timer_cnt % 50 == 0){ //31 is what it sound be to be the right BPM, but skill issue
                    configUserLED(BIT3);
                    configUserLED(0);
                    tempo++;
                }

                //measure 1
                if(tempo == 1){
                    //Graphics_drawStringCentered(&g_sContext, "FREEBIRD", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);
                    //Graphics_flushBuffer(&g_sContext);
                    playG1(); //eight
                }
                if(tempo == 2){
                    playFS1(); //QUICK
                }
                if (tempo == 3){
                    playD1();//eight
                }

                //measure 2
                if (tempo == 4){
                    playEb1();
                }
                if (tempo == 5){
                    playF1();//eighth
                }
                if(tempo == 7){
                   playG1();//eight
                }
                if(tempo == 8){
                   BuzzerOff(); //buzzer off to provide articulation
                }
                if(tempo == 9){
                  playG1(); //quarter
                }
                if(tempo == 12){
                    BuzzerOff(); //buzzer off to provide articulation
                }
                if(tempo == 13){
                  playG1();//quarter
                }
                if(tempo == 17){
                    playF1();//eight
                }
                if(tempo == 19){
                    playD1();//eight
                }

                //measure 3
                if(tempo == 21){
                    playF1();//eight
                }
                if(tempo == 23){
                    playG1();//eight
                }
                if(tempo == 25){
                    playF1();//sixteenth
                }
                if(tempo == 26){
                    playE1();//sixteenth
                }
                if(tempo == 27){
                   playD1();//eighth
                }
                if(tempo == 29){
                    playG1();//quarter
                }
                if(tempo == 33){
                    playF1();//eigth
                }
                if(tempo == 35){
                    playD1();//eigth
                }

                //measure 4
                if(tempo == 36){
                    playF1(); //eigth
                }
                if(tempo == 37){
                    playG1();//eigth
                }
                if(tempo ==38){
                    playF1(); //eigth
                }
                if(tempo == 39){
                    playG1();//quarter
                }
                if(tempo == 43){
                    playAS2();//eigth
                }
                if(tempo == 44){
                    playF1(); //eigth
                }
                if(tempo == 45){
                    playG1();//quarter
                }
                if(tempo == 49){
                    playA2();//eigth
                }
                if(tempo == 51){
                    playF1(); //eigth
                }
                if(tempo == 53){
                    playG1();//eigth
                }

                //measure 5
                if(tempo == 55){
                    playG1(); //eigth
                }
                if(tempo == 57){
                    playAS2();//eigth
                }
                if(tempo == 59){
                    playG1();
                }
                if(tempo == 62){
                    BuzzerOff(); //quarter
                }
                if(tempo == 63){
                    playG1(); //quarter
                }
                if(tempo == 66){
                    BuzzerOff();
                }
                if(tempo == 67){
                    playG1();//eigth
                }
                if(tempo == 69){
                    playD1();//eigth
                }

                //measure 6
                if(tempo == 70){
                    playF1();
                }
                if(tempo == 71){
                    playG1();
                }
                if(tempo == 72){
                    BuzzerOff();
                }
                if(tempo == 73){
                    playG1();
                }
                if(tempo == 74){
                    BuzzerOff();
                }
                if(tempo == 75){
                    playG1();
                }
                if(tempo == 78){
                    BuzzerOff();
                }
                if(tempo == 79){
                    playG1();
                }
                if(tempo == 83){
                    playF1();
                }
                if(tempo == 85){
                    playD1();
                }

                //measure 7
                if(tempo == 87){
                    playF1();//eight
                }
                if(tempo == 89){
                    playG1();//eight
                }
                if(tempo == 91){
                    playF1();//sixteenth
                }
                if(tempo == 92){
                    playE1();//sixteenth
                }
                if(tempo == 93){
                   playD1();//eighth
                }
                if(tempo == 95){
                    playG1();//quarter
                }
                if(tempo == 99){
                    playF1();//eigth
                }
                if(tempo == 101){
                    playD1();//eigth
                }

                //measure 8
                if(tempo == 103){
                    playG1();
                }
                if(tempo == 104){
                    BuzzerOff();
                }
                if(tempo == 105){
                    playG1();
                }
                if(tempo == 108){
                    BuzzerOff();
                }
                if(tempo == 109){
                    playAS2();
                }
                if(tempo == 110){
                    playF1();
                }
                if(tempo == 111){
                    playG1();
                }
                if(tempo == 115){
                    playA2();
                }
                if(tempo == 116){
                    playF1();
                }
                if(tempo == 117){
                    playG1();
                }


                if(tempo == 150){
                    Graphics_clearDisplay(&g_sContext);
                    BuzzerOff();
                    stoptimerA2(1);
                    state = 0;
                    tempo = 0;
                }
            break;
    }
    }

    Graphics_clearDisplay(&g_sContext); // Clear the display
    // Write some text to the display
    Graphics_drawStringCentered(&g_sContext, "Welcome", AUTO_STRING_LENGTH, 48, 15, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "to", AUTO_STRING_LENGTH, 48, 25, TRANSPARENT_TEXT);
    Graphics_drawStringCentered(&g_sContext, "ECE2049-A21!", AUTO_STRING_LENGTH, 48, 35, TRANSPARENT_TEXT);

    // Draw a box around everything because it looks nice
    Graphics_Rectangle box = {.xMin = 5, .xMax = 91, .yMin = 5, .yMax = 91 };
    Graphics_drawRectangle(&g_sContext, &box);
    Graphics_flushBuffer(&g_sContext);
}

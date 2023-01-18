#include "engr2350_msp432.h"

// Add function prototypes here, as needed.
void GPIOInit();
void TimerInit();
void CCR_ISR();
// Add global variables here, as needed.

Timer_A_UpModeConfig timer2;
Timer_A_ContinuousModeConfig timer;
Timer_A_CompareModeConfig compareleft;
Timer_A_CompareModeConfig compareright;
Timer_A_CaptureModeConfig captureleft;
Timer_A_CaptureModeConfig captureright;;
int32_t pwm1 = 0;
int32_t pwm2 = 0;
int32_t enc_total3 = 0;
int32_t enc_counts_track3 = 0;
int32_t enc_counts3 = 0;
int32_t enc_flag3 = 0;
uint32_t interruptstatus3 = 0;
uint32_t capturestatus3 = 0;
int32_t capturevalue3 = 0;
int32_t enc_total4 = 0;
int32_t enc_counts_track4 = 0;
int32_t enc_counts4 = 0;
int32_t enc_flag4 = 0;
uint32_t interruptstatus4 = 0;
uint32_t capturestatus4 = 0;
int32_t capturevalue4 = 0;
int32_t encoderedge3 = 0;
int32_t encoderedge4 = 0;
uint32_t setpoint = 65000;
//uint16_t pathA[9] = {1830,117,860,117,860,117,860,296,1300};
uint16_t pathA[9] = {1660,150,620,150,620,150,670,490,1100};
uint16_t pathB[9] = {855,360,435,160,1655,560,435,380,855};
uint8_t BMP0 = 0;
uint8_t BMP1 = 0;

uint16_t i = 0;
uint16_t current_d = 0;
void Drive(uint16_t * distances);
uint16_t distance_track3 =0 ;
uint16_t distance_track4 =0 ;



//uint16_t pathB[5] = {5,5,5,5,5};


int main(void) /* Main Function */
{
    // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    GPIOInit();
    TimerInit();



    // Place initialization code (or run-once) code here
        while(1){
////            printf("Left: %d         Right: %d\r\n",pwm1,pwm2);
//            if(enc_flag3){ // Check to see if capture occurred
//                enc_flag3 = 0; // reset capture flag
//                //printf("Left: %u\r\n",pwm1);
//                Drive(pathA);
//
//            }
//            if(enc_flag4){
//                enc_flag4 = 0;
//                //printf("%Right u\r\n",pwm2);
//                Drive(pathA);
            BMP0 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0);
            BMP1 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);

            if (!BMP0){
                Drive(pathA);
            }

            if (!BMP1) {
                Drive(pathB);
            }



        }



}


// Add function declarations here as needed
void GPIOInit(){
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN6);
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN7);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN4);
    GPIO_setAsOutputPin(GPIO_PORT_P5, GPIO_PIN5);

    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN4);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN7);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN6);

    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN6,GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,GPIO_PIN4,GPIO_PRIMARY_MODULE_FUNCTION);
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);

    //setup BUMPERS
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN2);


}
void TimerInit(){
    timer2.clockSource = TIMER_A_CLOCKSOURCE_SMCLK ;
    timer2.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer2.timerPeriod = 800;
    Timer_A_configureUpMode (TIMER_A0_BASE, &timer2);

    timer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK ;
    timer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    timer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    Timer_A_configureContinuousMode (TIMER_A3_BASE, &timer);

    captureleft.captureRegister =  TIMER_A_CAPTURECOMPARE_REGISTER_0;
    captureleft.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    captureleft.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    captureleft.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    captureleft.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;

    captureright.captureRegister =  TIMER_A_CAPTURECOMPARE_REGISTER_1;
    captureright.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    captureright.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    captureright.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    captureright.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;

    compareleft.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    compareright.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    compareleft.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    compareright.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
//    compareleft.compareValue = 200;
//    compareright.compareValue = 200;

    compareleft.compareValue = 0;
    compareright.compareValue = 0;



    Timer_A_initCapture (TIMER_A3_BASE, &captureleft);
    Timer_A_initCapture (TIMER_A3_BASE, &captureright);

    Timer_A_initCompare(TIMER_A0_BASE, &compareleft);
    Timer_A_initCompare(TIMER_A0_BASE, &compareright);

    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT,CCR_ISR);
    Timer_A_registerInterrupt(TIMER_A3_BASE, TIMER_A_CCR0_INTERRUPT ,CCR_ISR);
    Timer_A_startCounter(TIMER_A3_BASE, TIMER_A_CONTINUOUS_MODE);
    Timer_A_startCounter(TIMER_A0_BASE, TIMER_A_UP_MODE);
}
void CCR_ISR(){
    if(Timer_A_getEnabledInterruptStatus (TIMER_A3_BASE) == TIMER_A_INTERRUPT_PENDING){
        Timer_A_clearInterruptFlag(TIMER_A3_BASE);
        enc_counts_track3 += 65536;
        enc_counts_track4 += 65536;
    }
    if(Timer_A_getCaptureCompareEnabledInterruptStatus(
            TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0)
                    & TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_0);
        capturevalue3 = Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_counts3 = enc_counts_track3 + capturevalue3;
        enc_total3 += enc_counts3;
        encoderedge3++;
        enc_counts_track3 = -capturevalue3;
        enc_flag3 = 1;
        distance_track3++;
        //printf("Left: %d\r\n",enc_total3);
        if(encoderedge3 == 6){
            if(enc_total3 / 6 > setpoint){
                pwm1++;
            }
            if(enc_total3 / 6 < setpoint){
                pwm1--;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3,pwm1);
            encoderedge3 = 0;
            enc_total3 = 0;
        }
    }
    if(Timer_A_getCaptureCompareEnabledInterruptStatus(
            TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1)
                    & TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE, TIMER_A_CAPTURECOMPARE_REGISTER_1);
        capturevalue4 = Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_counts4 = enc_counts_track4 + capturevalue4;
        enc_total4 += enc_counts4;
        encoderedge4++;
        enc_counts_track4 = -capturevalue4;
        enc_flag4 = 1;
        distance_track4++;

        //printf("Right: %d\r\n",enc_total4);
        if(encoderedge4 == 6){
            if(enc_total4 / 6 > setpoint){
                pwm2++;
            }
            if(enc_total4 / 6 < setpoint){
                pwm2--;
            }
            Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_4,pwm2);
            encoderedge4 = 0;
            enc_total4 = 0;
        }
    }
}




void Drive(uint16_t * distances){
    // Loop through elements of array "distances"
    uint16_t current_d = 0;
    for (i = 0 ; i < 9 ; i++){
        printf ("this is i -> %u \r\n", i );
        distance_track3 = 0;
        //go straight
        pwm1 = 200;
        pwm2 = 200;
        Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3,200);
        Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_4,200);

        if (i % 2 == 0){
            printf("%u\r\n",1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN5);
            while (current_d <= distances[i]){ //________) //90 degrees * radius){
                current_d = ((distance_track3/360.0)*(2.00)*(3.14)*(31.75)); // Calculate distance travelled in mm
//                printf("%5u mm\tr\n",current_d);
            }
            current_d = 0;

        }
        else{
            printf("%u\r\n",2);
            GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6);
            GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN6);
            //one thing on
//            distance_track3 = 0;
//            pwm1 = 200;
//            pwm2 = 200;
//            Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3,200);
//            Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_4,200);
            while (current_d <= (distances[i]/2)){ //________) //90 degrees * radius){
                current_d = ((distance_track3/360.0)*(2.00)*(3.14)*(31.75)); // Calculate distance travelled in mm

//                printf("%5u mm\tr\n",current_d);

            }
            current_d = 0;

        }
    }
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN7);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6);
}


// Add interrupt functions last so they are easy to find


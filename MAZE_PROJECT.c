

// We'll always add this include statement. This basically takes the
// code contained within the "engr_2350_msp432.h" file and adds it here.
#include "engr2350_msp432.h"

// Add function prototypes here, as needed.
void GPIOInit();
void TimerInit();
void CCR_ISR();
void GetInputs();
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

uint16_t i = 0;
uint16_t current_d = 0;
void Drive();
uint16_t distance_track3 =0 ;
uint16_t distance_track4 =0 ;

uint8_t BMP0,BMP1,BMP2,BMP3,BMP4,BMP5;




int main(void) /* Main Function */
{
    // Add local variables here, as needed.

    // We always call the "SysInit()" first to set up the microcontroller
    // for how we are going to use it.
    SysInit();
    GPIOInit();
    TimerInit();
    Drive();

    // Place initialization code (or run-once) code here
        while(1){
            GetInputs();
            Drive();
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
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN7);


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




void Drive(){
    uint16_t current_d = 0;
    distance_track3 = 0;
    //curve left
    pwm1 = 200;
    pwm2 = 90;
    Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3,200);
    Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_4,90);
    GetInputs();
    //NO BUMP
    __delay_cycles(240e3);
    GetInputs();
    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN5);
    //curving left
    while (current_d <= 150){
        current_d = ((distance_track3/360.0)*(2.00)*(3.14)*(31.75)); // Calculate distance travelled in mm
        GetInputs();
        printf("Right motor faster than left, curving left\r\n");
        if(!BMP0||!BMP1 || !BMP2 || !BMP3 || !BMP4 || !BMP5 ){
            printf("Bumper has been pressed\r\n");
            printf("Right motor goes opposite direction\r\n");
            __delay_cycles(240e3);
            GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6);
            GPIO_setOutputHighOnPin(GPIO_PORT_P5,GPIO_PIN5);
            GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN6);
            //Adjusts right wheel to drive away from the wall but still "hugs" the wall
            while (current_d <= 20){
                current_d = ((distance_track3/360.0)*(2.00)*(3.14)*(31.75)); // Calculate distance travelled in mm
            }
            current_d = 0;
            GetInputs();
        }
    }
    current_d = 0;
}

void GetInputs(){
    BMP0 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0);
    BMP1 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);
    BMP2 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3);
    BMP3 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN5);
    BMP4 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6);
    BMP5 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN7);
}

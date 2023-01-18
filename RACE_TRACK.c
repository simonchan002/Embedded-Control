
#include "engr2350_msp432.h"

void GPIOInit();
void TimerInit();
void ADCInit();
void Encoder_ISR();
void T1_100ms_ISR();

Timer_A_UpModeConfig TA0cfg;
Timer_A_UpModeConfig TA1cfg;
Timer_A_ContinuousModeConfig TA3cfg;
Timer_A_CompareModeConfig TA0_ccr3;
Timer_A_CompareModeConfig TA0_ccr4;
Timer_A_CaptureModeConfig TA3_ccr0;
Timer_A_CaptureModeConfig TA3_ccr1;

// Encoder total events
uint32_t enc_total_L,enc_total_R;
// Speed measurement variables
int32_t Tach_L_count,Tach_L,Tach_L_sum,Tach_L_sum_count,Tach_L_avg; // Left wheel
int32_t Tach_R_count,Tach_R,Tach_R_sum,Tach_R_sum_count,Tach_R_avg; // Right wheel
uint8_t run_control = 0; // Flag to denote that 100ms has passed and control should be run.

float pot_val,pot_val2;
float des_speed, des_turn;
float straight_length , turn_radius;
uint8_t turn, turning_flag;
float diff_speed;
float straight_length_mm, turn_radius_mm;
float des_left, des_right;
float mes_speed_r, mes_speed_l;
float compare_left, compare_right;
float right_c_error, left_c_error;
float error_sum_left, error_sum_right;
float corrected_left, corrected_right;
float ki = 1;
float turn_distance;

int main(void)

{
    SysInit();
    GPIOInit();
    ADCInit();
    TimerInit();
    __delay_cycles(24e6);
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7); // Turn motors ON

    while(1){
        if(run_control){    // If 100 ms has passed
            run_control = 0;    // Reset the 100 ms flag
            printf("\r\n100ms passed");
//            uint16_t distance_l = (3.1415926535897932384626433*70.0*enc_total_L)/360.0 ; // Calculate distance traveled in mm
            uint16_t distance_r = (3.1415926535897932384626433*70.0*enc_total_R)/360.0 ; // Calculate distance traveled in mm
//            printf("distance_l = %u\r\n", distance_l);
            //Read potentiometers!
            ADC14_toggleConversionTrigger();
            while( ADC14_isBusy() ){}
            pot_val = ADC14_getResult( ADC_MEM0 ); // value from speed potentiometer
            pot_val2 =  ADC14_getResult( ADC_MEM1 ); // Value from turn potentiometer

            //DUTY CYCLE PERCENTAGE
            des_speed = 100.0 * pot_val / 16384.0 ;
            //DECIMAL OF DUTY CYCLE NOT PERCENTAGE
            des_turn = pot_val2 / 16384.0 ;


            printf ("desired pot2 -> %f\r\n", des_turn);

            if (distance_r < straight_length_mm){
                turning_flag = 0;
            }
            else if (distance_r < turn_distance){
                turning_flag = 1;
            }
            else{
                enc_total_R = 0;
                enc_total_L = 0;
                turn++;
            }


            //Calculate Straight Length AND TURN radius
            straight_length = (75.0 * des_turn) + 20.0;
            turn_radius = ( 102.0 * des_turn) + 15.0;

            straight_length_mm = straight_length * 25.4;
            turn_radius_mm = turn_radius * 25.4;
            turn_distance = 3.86969 * turn_radius_mm  ; // arc length of semi-circle



            // ----START TURNING CONTROL----
            if (turning_flag == 1){
                diff_speed = (des_speed * 0.5 * 149) / turn_radius_mm;
                printf("\r\nturning_flag = 1\r\n");
            }

            //NO DIFF SPEED
            else if (turning_flag == 0){
                diff_speed = 0;
                printf("\r\nturning_flag = 0\r\n");
            }
            // ----END TURNING CONTROL----


            //WHEEL SPEED CONTROL START---


            //LEFT WHEEL

            //Calculate desired wheel speed as desired + / - diff speed

            des_left = des_speed - diff_speed;
            des_right = des_speed + diff_speed;

            if (des_left < 10 ){
                des_speed = 0;
                compare_left = 0;
            }

            else{
                mes_speed_l = 1500000.0/Tach_L_avg;
                left_c_error = des_left - mes_speed_l;
                error_sum_left += left_c_error;
                corrected_left = des_left + ki * error_sum_left;
                compare_left = corrected_left * 8;

                if (compare_left < 80){
                    compare_left = 80;
                }

                else if (compare_left > 720){
                    compare_left = 720;
                }
            }


            if (des_right < 10){
                des_speed = 0;
                compare_right = 0;
            }

            else{
                mes_speed_r = 1500000.0/Tach_R_avg;
                right_c_error = des_right - mes_speed_r;
                error_sum_right += right_c_error;
                corrected_right = des_right + ki * error_sum_right;
                compare_right = corrected_right * 8;

                if (compare_right < 80){
                    compare_right = 80;
                }

                else if (compare_right > 720){
                    compare_right = 720;
                }
            }

            //APPLY COMPARE_VALUE
            Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_3,compare_right);
            Timer_A_setCompareValue(TIMER_A0_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_4,compare_left);

            //----END SPEED CONTROL----

            if (turn == 4){
                GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7); // Turn motors OFF
            }
            printf("desired %f, measured %f, error_sum -> %.3f compare_left -> %f \r\n" , des_left, mes_speed_l, error_sum_left, compare_left);
            printf("desired %f, measured %f, error_sum -> %.3f compare_right -> %f  \r\n" , des_right, mes_speed_r, error_sum_right, compare_right);
        }
    }
}


void ADCInit(){
    // Add your ADC initialization code here.
    //  Don't forget the GPIO, either here or in GPIOInit()!!
    ADC14_enableModule();
    ADC14_initModule( ADC_CLOCKSOURCE_SMCLK  , ADC_PREDIVIDER_1 , ADC_DIVIDER_4  , ADC_NOROUTE   );
    ADC14_setResolution( ADC_14BIT );

    GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4 , GPIO_PIN1 , GPIO_TERTIARY_MODULE_FUNCTION ); //A12
    GPIO_setAsPeripheralModuleFunctionInputPin( GPIO_PORT_P4 , GPIO_PIN4 , GPIO_TERTIARY_MODULE_FUNCTION ); // A9

    ADC14_configureConversionMemory(ADC_MEM0,ADC_VREFPOS_AVCC_VREFNEG_VSS ,ADC_INPUT_A9,false);
    ADC14_configureConversionMemory(ADC_MEM1,ADC_VREFPOS_AVCC_VREFNEG_VSS ,ADC_INPUT_A12,false);
    ADC14_configureMultiSequenceMode(ADC_MEM0,ADC_MEM1,false);  //???

    ADC14_setSampleHoldTrigger( ADC_TRIGGER_ADCSC , false );
    ADC14_enableSampleTimer (ADC_MANUAL_ITERATION);
    ADC14_enableConversion();

}

void GPIOInit(){
    GPIO_setAsOutputPin(GPIO_PORT_P5,GPIO_PIN4|GPIO_PIN5);   // Motor direction pins
    GPIO_setAsOutputPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7);   // Motor enable pins
        // Motor PWM pins
    GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_P2,GPIO_PIN6|GPIO_PIN7,GPIO_PRIMARY_MODULE_FUNCTION);
        // Motor Encoder pins
    GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P10,GPIO_PIN4|GPIO_PIN5,GPIO_PRIMARY_MODULE_FUNCTION);

    GPIO_setOutputLowOnPin(GPIO_PORT_P5,GPIO_PIN4|GPIO_PIN5);   // Motors set to forward
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN6|GPIO_PIN7);   // Motors are OFF
}

void TimerInit(){
    // Configure PWM timer for 30 kHz
    TA0cfg.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    TA0cfg.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    TA0cfg.timerPeriod = 800;
    Timer_A_configureUpMode(TIMER_A0_BASE,&TA0cfg);
    // Configure TA0.CCR3 for PWM output
    TA0_ccr3.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_3;
    TA0_ccr3.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    TA0_ccr3.compareValue = 0;
    Timer_A_initCompare(TIMER_A0_BASE,&TA0_ccr3);
    // Configure TA0.CCR4 for PWM output
    TA0_ccr4.compareRegister = TIMER_A_CAPTURECOMPARE_REGISTER_4;
    TA0_ccr4.compareOutputMode = TIMER_A_OUTPUTMODE_RESET_SET;
    TA0_ccr4.compareValue = 0;
    Timer_A_initCompare(TIMER_A0_BASE,&TA0_ccr4);
    // Configure Encoder timer in continuous mode
    TA3cfg.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    TA3cfg.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_1;
    TA3cfg.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    Timer_A_configureContinuousMode(TIMER_A3_BASE,&TA3cfg);
    // Configure TA3.CCR0 for Encoder measurement
    TA3_ccr0.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_0;
    TA3_ccr0.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    TA3_ccr0.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    TA3_ccr0.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    TA3_ccr0.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE,&TA3_ccr0);
    // Configure TA3.CCR1 for Encoder measurement
    TA3_ccr1.captureRegister = TIMER_A_CAPTURECOMPARE_REGISTER_1;
    TA3_ccr1.captureMode = TIMER_A_CAPTUREMODE_RISING_EDGE;
    TA3_ccr1.captureInputSelect = TIMER_A_CAPTURE_INPUTSELECT_CCIxA;
    TA3_ccr1.synchronizeCaptureSource = TIMER_A_CAPTURE_SYNCHRONOUS;
    TA3_ccr1.captureInterruptEnable = TIMER_A_CAPTURECOMPARE_INTERRUPT_ENABLE;
    Timer_A_initCapture(TIMER_A3_BASE,&TA3_ccr1);
    // Register the Encoder interrupt
    Timer_A_registerInterrupt(TIMER_A3_BASE,TIMER_A_CCR0_INTERRUPT,Encoder_ISR);
    Timer_A_registerInterrupt(TIMER_A3_BASE,TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT,Encoder_ISR);
    // Configure 10 Hz timer
    TA1cfg.clockSource = TIMER_A_CLOCKSOURCE_SMCLK;
    TA1cfg.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64;
    TA1cfg.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    TA1cfg.timerPeriod = 37500;
    Timer_A_configureUpMode(TIMER_A1_BASE,&TA1cfg);
    Timer_A_registerInterrupt(TIMER_A1_BASE,TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT,T1_100ms_ISR);
    // Start all the timers
    Timer_A_startCounter(TIMER_A0_BASE,TIMER_A_UP_MODE);
    Timer_A_startCounter(TIMER_A1_BASE,TIMER_A_UP_MODE);
    Timer_A_startCounter(TIMER_A3_BASE,TIMER_A_CONTINUOUS_MODE);
}


void Encoder_ISR(){
    // If encoder timer has overflowed...
    if(Timer_A_getEnabledInterruptStatus(TIMER_A3_BASE) == TIMER_A_INTERRUPT_PENDING){
        Timer_A_clearInterruptFlag(TIMER_A3_BASE);
        Tach_R_count += 65536;
        Tach_L_count += 65536;
    // Otherwise if the Left Encoder triggered...
    }else if(Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0)&TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        enc_total_R++;   // Increment the total number of encoder events for the left encoder
        // Calculate and track the encoder count values
        Tach_R = Tach_R_count + Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        Tach_R_count = -Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_0);
        // Sum values for averaging
        Tach_R_sum_count++;
        Tach_R_sum += Tach_R;
        // If 6 values have been received, average them.
        if(Tach_R_sum_count == 6){
            Tach_R_avg = Tach_R_sum/6;
            Tach_R_sum_count = 0;
            Tach_R_sum = 0;
        }
    // Otherwise if the Right Encoder triggered...
    }else if(Timer_A_getCaptureCompareEnabledInterruptStatus(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1)&TIMER_A_CAPTURECOMPARE_INTERRUPT_FLAG){
        Timer_A_clearCaptureCompareInterrupt(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
        enc_total_L++;
        Tach_L = Tach_L_count + Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
        Tach_L_count = -Timer_A_getCaptureCompareCount(TIMER_A3_BASE,TIMER_A_CAPTURECOMPARE_REGISTER_1);
        Tach_L_sum_count++;
        Tach_L_sum += Tach_L;
        if(Tach_L_sum_count == 6){
            Tach_L_avg = Tach_L_sum/6;
            Tach_L_sum_count = 0;
            Tach_L_sum = 0;
        }
    }
}

void T1_100ms_ISR(){
    Timer_A_clearInterruptFlag(TIMER_A1_BASE);
    run_control = 1;
}
void GPIOInit();
void GetInputs();
void Timer_Init();
void Lighting();
void displaysequence(uint8_t counter);
void makesequence();
void lost();
void win();
void timerISR();
void checkseq();



uint8_t BMP0,BMP1,BMP2,BMP3,BMP4,BMP5;
uint8_t PB1,BILED;
uint8_t sequence [10];
uint8_t bumpers [6] = {0,1,2,3,4,5};
uint8_t randbmp;
Timer_A_UpModeConfig timer;
uint8_t lose = 0;
uint8_t time = 0;
uint8_t won = 0;
uint8_t score = 0;
uint8_t length = 0;


// Main Function
 int main(void){
    SysInit();
    GPIOInit();
    GetInputs();
    Timer_Init();


    while(1){
        GetInputs();
        lose = 0;
        score=0;
        won = 0;
        printf("Click the matching corresponding colors to the displayed sequence\r\n");
        printf("Waiting for Push button to be pressed\r\n");
        while (!PB1){
            Lighting();
        }
        time = 0;
        while(time < 20){
        }

        makesequence();
        //Check if right
        //within 3s click right color if wrong lose if right set time to 3 again
        while(1){
            checkseq();
            break;
        }

    }
}


void makesequence(){
    uint8_t i = 0;
    for(i = 0; i<10; i++){
        uint8_t random = rand() % 6;
        sequence [i] = random;
    }
}

void checkseq(){
    uint8_t disp;
    uint8_t pbp = 0;
    score = 0;
    for (length = 1; length < 4; length++){
        // how many things displayed
       displaysequence(length);
        for (disp = 0; disp < length;){
           time = 0;
           if(lose == 1){
               break;
           }
           //wait 3 seconds.
           while(time < 60){
               GetInputs();
               if(PB1){
                   while(PB1){
                       Lighting();
                       GetInputs();
                       if(time > 60){
                           lost();
                           lose = 1;
                           break;
                       }
                   }
                   if(lose == 1){
                      break;
                   }
               }
               //IF ANY BUMPER PRESSED
               //reset time, turn on LED
               if(!BMP0){
                  pbp = 0;
                  time = 0;
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                  __delay_cycles(240e3);
                  while(!BMP0){
                      GetInputs();
                      if(time > 60){
                          lost();
                          lose = 1;
                          break;
                      }
                      if(PB1){
                          pbp = 1;
                      }
                  }
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                  //wrong bumper AND didn't lose yet
                  if(sequence[disp] != 0 && lose != 1 && pbp == 0){
                      lost();
                      lose = 1;
                      break;
                  }
                  if(pbp == 0){
                      disp++;
                      break;
                  }
               }
               if(!BMP1){
                  pbp = 0;
                  time = 0;
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                  __delay_cycles(240e3);
                  while(!BMP1){
                  GetInputs();
                      if(time > 60){
                          lost();
                          lose = 1;
                          break;
                      }
                      if(PB1){
                          pbp = 1;
                      }
                  }
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                  if(sequence[disp] != 1&& lose != 1 && pbp == 0){
                      lost();
                      lose = 1;
                      break;
                  }
                  if(pbp == 0){
                      disp++;
                      break;
                  }
               }

               if(!BMP2){
                   pbp = 0;
                   time = 0;
                   GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                   __delay_cycles(240e3);
                   while(!BMP2){
                       GetInputs();
                       if(time > 60){
                           lost();
                           lose = 1;
                           break;
                       }
                       if(PB1){
                           pbp = 1;
                       }
                   }
                   GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                   if(sequence[disp] != 2&& lose != 1 && pbp == 0){
                       lost();
                       lose = 1;
                       break;
                   }
                   if(pbp == 0){
                       disp++;
                       break;
                   }
               }
               if(!BMP3){
                  pbp = 0;
                  time = 0;
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                  __delay_cycles(240e3);
                  while(!BMP3){
                      GetInputs();
                      if(time > 60){
                          lost();
                          lose = 1;
                          break;
                      }
                      if(PB1){
                          pbp = 1;
                      }
                  }
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
                  if(sequence[disp] != 3 && lose != 1 && pbp == 0){
                      lost();
                      lose = 1;
                      break;
                  }
                  if(pbp == 0){
                      disp++;
                      break;
                  }
               }
               if(!BMP4){
                  pbp = 0;
                  time = 0;
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
                  __delay_cycles(240e3);
                  while(!BMP4){
                      GetInputs();
                      if(time > 60){
                          lost();
                          lose = 1;
                          break;
                      }
                      if(PB1){
                          pbp = 1;
                      }
                  }
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);

                  if(sequence[disp] != 4 && lose != 1 && pbp == 0){
                      lost();
                      lose = 1;
                      break;
                  }
                  if(pbp == 0){
                      disp++;
                      break;
                  }
               }
               if(!BMP5){
                  pbp = 0;
                  time = 0;
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
                  GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
                  __delay_cycles(240e3);
                  while(!BMP5){
                      GetInputs();
                      if(time > 60){
                          lost();
                          lose = 1;
                          break;
                      }
                      if(PB1){
                          pbp = 1;
                      }
                  }
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
                  GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
                  //wrong number
                  if(sequence[disp] != 5 && lose != 1 && pbp == 0){
                      lost();
                      lose = 1;
                      break;
                  }
                  if(pbp == 0){
                      disp++;
                      break;
                  }
              }
           }
           if(time >= 60 && lose != 1){
               lost();
               lose = 1;
               break;
           }

           if (lose == 1){
               break;
           }
        }
        if(lose == 1){
           break;
       }
    }
    if(lose != 1){
        win();
    }
}

void win(){
    uint8_t breakout = 0;
    printf("WIN\r\n");
    printf("You got all the 10 colors good job!\r\n");
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);
    while(1){
        time = 0;
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN5);
        while(time < 5){
            GetInputs();
            if(PB1){
                breakout = 1;
                break;
            }
        }
        if(breakout == 1){
            break;
        }
        time = 0;
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);
        while(time < 5){
            GetInputs();
            if(PB1){
                breakout = 1;
                break;
            }
        }
        if(breakout == 1){
            break;
        }
    }
}

void lost(){
    score = length-1;
    uint8_t breakout = 0;
    printf("LOSE\r\n");
    printf("SCORE IS: %u\r\n",score);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN0);
    while(1){
        time = 0;
        GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN0);
        while(time < 5){
            GetInputs();
            if(PB1){
                breakout = 1;
                break;
            }
        }
        if(breakout == 1){
            break;
        }
        time = 0;
        GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN0);
        while(time < 5){
            GetInputs();
            if(PB1){
                breakout = 1;
                break;
            }
        }
        if(breakout == 1){
            break;
        }
    }
}



void displaysequence(uint8_t count){
    time = 0;
    while(time < 10){}
    uint8_t len;
    GPIO_setOutputHighOnPin(GPIO_PORT_P3,GPIO_PIN0);
    GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN5);
    for(len=0;len<count;len++){
        if(sequence[len] == 0){
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            time = 0;
            while(time < 10){}
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
            time = 0;
            while(time < 10){}
        }
        else if(sequence[len] == 1){
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            time = 0;
            while(time < 10){}
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            time = 0;
            while(time < 10){}

        }
        else if(sequence[len] == 2){
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
            time = 0;
            while(time < 10){}
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
            time = 0;
            while(time < 10){}

        }
        else if(sequence[len] == 3){
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
            time = 0;
            while(time < 10){}
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
            time = 0;
            while(time < 10){}
        }
        else if(sequence[len] == 4){
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
            time = 0;
            while(time < 10){}
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
            time = 0;
            while(time < 10){}
        }
        else if(sequence[len] == 5){
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
            time = 0;
            while(time < 10){}
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
            GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
            time = 0;
            while(time < 10){}
        }
    }
    GPIO_setOutputLowOnPin(GPIO_PORT_P3,GPIO_PIN0);
    GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN5);
}
void GPIOInit(){
    //BUMPER INIT
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN0);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN2);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN3);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN5);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN6);
    GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P4, GPIO_PIN7);

    //PB1
    GPIO_setAsInputPin(GPIO_PORT_P1, GPIO_PIN6);
    //BILED
    GPIO_setAsOutputPin(GPIO_PORT_P3, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN5);

    //RGB
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN0);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN1);
    GPIO_setAsOutputPin(GPIO_PORT_P2, GPIO_PIN2);


}


void GetInputs(){
    PB1 =  GPIO_getInputPinValue(GPIO_PORT_P1, GPIO_PIN6);
    BMP0 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN0);
    BMP1 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN2);
    BMP2 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN3);
    BMP3 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN5);
    BMP4 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN6);
    BMP5 = GPIO_getInputPinValue(GPIO_PORT_P4, GPIO_PIN7);
}

void Lighting(){
    GetInputs();
    if (!BMP0){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
        __delay_cycles(240e3);
        while(!BMP0){GetInputs();}
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        __delay_cycles(240e3);

    }
    if (!BMP1){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
        __delay_cycles(240e3);
        while(!BMP1){GetInputs();}
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        __delay_cycles(240e3);

    }
    if (!BMP2){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
        __delay_cycles(240e3);
        while(!BMP2){GetInputs();}
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        __delay_cycles(240e3);

    }
    if(!BMP3){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
        __delay_cycles(240e3);
        while(!BMP3){GetInputs();}
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        __delay_cycles(240e3);

    }
    if(!BMP4){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN2);
        __delay_cycles(240e3);
        while(!BMP4){GetInputs();}
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN2);
        __delay_cycles(240e3);

    }
    if(!BMP5){
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputHighOnPin(GPIO_PORT_P2,GPIO_PIN1);
        __delay_cycles(240e3);
        while(!BMP5){GetInputs();}
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN0);
        GPIO_setOutputLowOnPin(GPIO_PORT_P2,GPIO_PIN1);
        __delay_cycles(240e3);


    }
}

void Timer_Init(){
    timer.clockSource = TIMER_A_CLOCKSOURCE_SMCLK ;
    timer.clockSourceDivider = TIMER_A_CLOCKSOURCE_DIVIDER_64;
    timer.timerPeriod = 18750;
    timer.timerInterruptEnable_TAIE = TIMER_A_TAIE_INTERRUPT_ENABLE;
    Timer_A_configureUpMode (TIMER_A1_BASE, &timer);
    Timer_A_startCounter(TIMER_A1_BASE , TIMER_A_UP_MODE);
    Timer_A_registerInterrupt(TIMER_A1_BASE, TIMER_A_CCRX_AND_OVERFLOW_INTERRUPT, timerISR );

}

void timerISR(){
    time++;
    Timer_A_clearInterruptFlag(TIMER_A1_BASE);
}

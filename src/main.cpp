#include <math.h>

#include <mbed.h>
#include "../lib/DigitalEncoderAS5601/DigitalEncoderAS5601.h"
#include "../lib/PID/PID.h"

#define TIM_USR TIM7
#define TIM_USR_IRQ TIM7_IRQn
Timer tim;

volatile char flag = 0;

TIM_HandleTypeDef mTimUserHandle;

extern "C"
void M_TIM_USR_Handler(void){
    if(__HAL_TIM_GET_FLAG(&mTimUserHandle, TIM_FLAG_UPDATE)==SET){
        __HAL_TIM_CLEAR_FLAG(&mTimUserHandle, TIM_FLAG_UPDATE);
        flag= 1;
    }
}

AnalogIn  pot(PB_1);
DigitalOut enable1(PC_10);
DigitalOut enable2(PC_11);
DigitalOut enable3(PC_12);
DigitalOut en_chip(PA_6);
PwmOut uh_1(PA_8);
PwmOut vh_2(PA_9);
PwmOut wh_3(PA_10);
Serial pc(USBTX, USBRX, 9600); // tx, rx

DigitalEncoderAS5601 encoder(I2C_SDA,I2C_SCL);

float pos_ref = 180.0f;
float pos_curr;
//float err;
//float K_p=0.3f;
//float K_d = 1.1f;;

float position;
int step_number=0;
float duty_r = 0.0f;
float duty_l = 0.0f;

//controllo pd (primo test con solo p)
void setDutyCycle(float angle){

    if(angle >= 0){
        duty_l=0;
        duty_r = (float) (angle/360);// + ierr*1.5f)/32000;
    }else{
        duty_r=0;
        duty_l = (float) (-angle/360);
    }
}

// void pd(){
//     err = pos_ref - pos_curr;

//     err = err*K_p ;//   + err*K_d;


//     // PI function to obtain duty_cycle
//     if(err >= 0){
//         duty_l=0;
//         duty_c = (float) (err/360);// + ierr*1.5f)/32000;
//     }else{
//         duty_c=0;
//         duty_l = (float) (-err/360);
//     }
        
// }

void cycle(){
    //duty_c = fmod(duty_c+0.1f, 1.0f);
}

void stepRead()
{
  // Check in which of 6 position the motor is
  if(position<=8.97f || position>50.5f)
  {
    step_number=5;
  }
  if(position>8.35f && position<=17.49f)
  {
    step_number=4;
  }
  if(position>16.87f && position<=26.11f)
  {
    step_number=3;
  }
  if(position>25.49f && position<=34.81f)
  {
    step_number=2;
  }
  if(position>33.84f && position<=43.2f)
  {
    step_number=1;
  }
  if((position>42.47f && position<53.0f) || position<=0.87f)
  {
    step_number=0;
  }
}

//Modificare i valori in base al count
// void stepRead(){
//     // Check in which of 6 position the motor is
//   // Check in which of 6 position the motor is
//   if(position>18.89f && position<=27.29f)
//   {
//     step_number=5;
//   }
//   if(position>9.69f && position<=18.99f)
//   {
//     step_number=4;
//   }
//   if(position>1.46f && position<=9.69f)
//   {
//     step_number=3;
//   }
//   if((position>44.19f && position<=53.0f) || position<=1.46f )
//   {
//     step_number=2;
//   }
//   if(position>35.19f && position<=44.19f)
//   {
//     step_number=1;
//   }
//   if(position>27.29f && position<=35.19f)
//   {
//     step_number=0;
//   }


// }

int main() {

    // put your setup code here, to run once:
    /*uh_1.period(0.0001f);
    vh_2.period(0.0001f);
    wh_3.period(0.0001f);*/
    //float val = 0.0f;
    uh_1.period(0.0001f);
    vh_2.period(0.0001f);
    wh_3.period(0.0001f);
    en_chip=1;
    float max = 0.0f;
    float min = 1024.0f;
    float err;

    //Controllore PD (per ora solo P)
    PID::PID pd(0.2f, 0, 0, 0.11f, 360.0f, 0.0f);  //k_p, k_i, k_d, dt, max, min

    //TIMER INIT
    __HAL_RCC_TIM7_CLK_ENABLE();

    //Freq = 90 Hz
    mTimUserHandle.Instance = TIM_USR;
    mTimUserHandle.Init.Prescaler = 10000-1;
    mTimUserHandle.Init.Period = 100-1;
    mTimUserHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    mTimUserHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_Base_Init(&mTimUserHandle);
    HAL_TIM_Base_Start_IT(&mTimUserHandle);
    NVIC_SetVector(TIM_USR_IRQ, (uint32_t)M_TIM_USR_Handler);
    NVIC_EnableIRQ(TIM_USR_IRQ);
    tim.start();

    float angle;

    //encoder.setAngleFormat(angleContinuous);
    while(1) {
        if(flag==1){
             // put your main code here, to run repeatedly:
            //val = pot.read();
            // Read hall sensor's angle
            
      
            // Read position from hall sensor's angle divided by electrical poles
            angle = encoder.getAngleDeg();
            pos_curr = angle;

            //Get the error output from the pd controller
            err = pd.getOutput(pos_ref, pos_curr);
         
            setDutyCycle(err);
            //duty_r = 0.5f;
            //duty_l= 0.0f;
            //position = fmod(angle,51.43f); //360/7, where 7 is number of magnets divided by 2, 
            position = fmod(angle, 51.43f);
           
            
            stepRead();
            //pc.printf("%.2f\n", angle);
            switch(step_number){
                case 0:
                    uh_1.write(duty_r);
                    vh_2.write(0.0f);
                    wh_3.write(duty_l);
                    enable1 = 1;
                    enable2 = 0;
                    enable3 = 1;
                    break;

                case 1:
                uh_1.write(0.0f);
                vh_2.write(duty_r);
                wh_3.write(duty_l);
                enable1 = 0;
                enable2 = 1;
                enable3 = 1;
                break;

                case 2:
                uh_1.write(duty_l);
                vh_2.write(duty_r);
                wh_3.write(0.0f);
                enable1 = 1;
                enable2 = 1;
                enable3 = 0;
                break;

                case 3:
                uh_1.write(duty_l);
                vh_2.write(0.0f);
                wh_3.write(duty_r);
                enable1 = 1;
                enable2 = 0;
                enable3 = 1;
                break;

                case 4:
                uh_1.write(0.0f);
                vh_2.write(duty_l);
                wh_3.write(duty_r);
                enable1 = 0;
                enable2 = 1;
                enable3 = 1;
                break;

                case 5:
                uh_1.write(duty_r);
                vh_2.write(duty_l);
                wh_3.write(0.0f);
                enable1 = 1;
                enable2 = 1;
                enable3 = 0;
                //step_number=-1;
                break;
            }
            //cycle();
            //step_number= (step_number+1) % 6;
            flag=0;
        }
        //wait_ms(500);
       
    
        



    }
       

    
}


/*
FREQUENZA 2Hz
MaxFreq (90 * 10^6 Hz)
Prescaler (100-1)
Period (200-1)
Freq = MaxFreq/((Prescaler+1)*(Period+1)) = 1
*/
#include <math.h>

#include <mbed.h>
#include "PID.h"    //../lib/PID/
#include "Motor.h"

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

//AnalogIn  pot(PB_1);
/*DigitalOut enable1(PC_10);
DigitalOut enable2(PC_11);
DigitalOut enable3(PC_12);
DigitalOut en_chip(PA_6);
PwmOut uh_1(PA_8);
PwmOut vh_2(PA_9);
PwmOut wh_3(PA_10);

DigitalEncoderAS5601 encoder(I2C_SDA,I2C_SCL);

//float err;
//float K_p=0.3f;
//float K_d = 1.1f;
*/


Serial pc(USBTX, USBRX, 9600); // tx, rx

float pos_ref = 180.0f;
float pos_curr;


float position;
int step_number=0;
float duty_r = 0.0f;
float duty_l = 0.0f;


//controllo pd (primo test con solo p)
void setDutyCycle(float angle){

    if(angle >= 0){
        duty_r=0;
        duty_l = (float) (angle/360);// + ierr*1.5f)/32000;
    }else{
        duty_l=0;
        duty_r = (float) (-angle/360);
    }

    if(duty_l > 1.0f){
        duty_l = 1.0f;
    }else if(duty_l < 0.0f){
        duty_l = 0.0f;
    }

    if(duty_r > 1.0f){
        duty_r = 1.0f;
    }else if(duty_r < 0.0f){
        duty_r = 0.0f;
    }

    // pc.printf("%.2f %.2f %.2f\n", pos_curr, duty_r, duty_l);
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
    /**
     * Params del costruttore: enable_chip, enable1, enable2, enable3, Array con PinName di u,v,w
     */
    PinName coilPins[3] = {PA_8, PA_9, PA_10};
    Motor motor(PA_6, PC_10, PC_11, PC_12, coilPins);

    //try{
        //motor = new Motor(PA_6, PC_10, PC_11, PC_12, coilPins);
    //}catch(std::exception& e){
      //  pc.printf("Error: %s\n", e.what());
   // }
    
    // put your setup code here, to run once:
    /*uh_1.period(0.0001f);
    vh_2.period(0.0001f);
    wh_3.period(0.0001f);*/
    //float val = 0.0f;
    motor.setPeriodU(0.00001f);
    motor.setPeriodV(0.00001f);
    motor.setPeriodW(0.00001f);
    /*
    uh_1.period(1/90.0f);// f=90 Hz -> T= 1/f
    vh_2.period(1/90.0f);
    wh_3.period(1/90.0f);
    */
   
    motor.setEnableChip(1);
    //en_chip=1;
    //float max = 0.0f;
    //float min = 1024.0f;
    float err;

    //Controllore PD (per ora solo P)
    PID pd(10.0f, 0.0f, 4.0f, 0.001f);  //k_p, k_i, k_d, dt, max, min

    //TIMER INIT
    __HAL_RCC_TIM7_CLK_ENABLE();

    //Freq = 1KHz
    mTimUserHandle.Instance = TIM_USR;          
    mTimUserHandle.Init.Prescaler = 9000-1; 
    mTimUserHandle.Init.Period = 10-1;
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
            angle = motor.encoder.getAngleDeg();
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

            motor.setStep(step_number, duty_r, duty_l);

            
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
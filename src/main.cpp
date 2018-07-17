#include <math.h>

#include <mbed.h>
#include "PID.h"    //../lib/PID/
#include "Motor.h"  //../lib/Motor/

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

//utilizzato in fase di test
//Serial pc(USBTX, USBRX, 115200); // tx, rx

float pos_ref = 310.0f;     // posizione di riferimento in input al controllore
float pos_curr;             // posizione rilevata dal sensore
float position;
int step_number=0;
float duty_r = 0.0f;
float duty_l = 0.0f;


void setDutyCycle(float angle){

    if(angle >= 0.0f){//in base al segno si determina la direzione del motore
        duty_r=0;
        duty_l = (float) (angle/360);// + ierr*1.5f)/32000;
    }else{
        duty_l=0;
        duty_r = (float) (-angle/360);
    }

    if(duty_l > 1.0f){ //limiti superiori e inferiori
        duty_l = 1.0f;
    }else if(duty_l < 0.0f){
        duty_l = 0.0f;
    }

    if(duty_r > 1.0f){
        duty_r = 1.0f;
    }else if(duty_r < 0.0f){
        duty_r = 0.0f;
    }
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


int main() {
    /**
     * Params del costruttore: enable_chip, enable1, enable2, enable3, Array con PinName di u,v,w
     */
    PinName coilPins[3] = {PA_8, PA_9, PA_10};
    Motor motor(PC_10, PC_11, PC_12, PA_6, coilPins);
    
   
    motor.setPeriodU(0.00001f);
    motor.setPeriodV(0.00001f);
    motor.setPeriodW(0.00001f);
   
    motor.setEnableChip(1);
    
    float err;
    float curr; //variabile secondaria per la posizione corrente

    //Controllore PD 
    PID pd(10.0f, 0.0f, 0.000004f, 0.001f);  //k_p, k_i, k_d, dt, max, min

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

    while(1) {
        if(flag==1){
             // put your main code here, to run repeatedly:
            
            // Read position from hall sensor's angle divided by electrical poles
            angle = motor.encoder.getAngleDeg();
            pos_curr = angle;

            
            curr=pos_curr;
            if(pos_curr>180.0f+pos_ref/*&& pos_ref<180*/)//inverte il segno per determinare la direzione da prendere
                curr=curr-360;
            /*else if(pos_ref=>180 && pos_curr>(180.0f+pos_ref-360))//da testare nel caso ref >180
                curr=curr-360;
            */
            // if(pos_curr>(180.0f))
            //     curr=360-curr;
            // if(pos_curr>ref+180)
            //      curr=360-curr;
            
            
            
            //Get the error output from the pd controller
            err = pd.getOutput(pos_ref, curr); 

            setDutyCycle(err);
            //360/7, where 7 is number of magnets divided by 2, 
            position = fmod(angle, 51.43f);
           
            
            stepRead();
            //pc.printf("%.2f ,%.2f ,%.2f\n", err,pos_curr,ref-curr);

            motor.setStep(step_number, duty_r, duty_l);

            
            flag=0;
        }
        //wait_ms(500);
  
    }
  
}


/*
FREQUENZA 1KHz
MaxFreq (90 * 10^6 Hz)
Prescaler (100-1)
Period (200-1)
Freq = MaxFreq/((Prescaler+1)*(Period+1)) = 1
*/

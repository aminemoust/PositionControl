#include <mbed.h>


#ifndef _Motor_H
#define _Motor_H

class Motor{
    public:
        /*
            Costruttore con i PinName come parametri, per settare i pin 
            @param coilPins: array di dim. 3 che contiene i PinName di u,v,w (rispettivamente in questo ordine)
        */
        Motor() = default;
        Motor(PinName en1, PinName en2, PinName en3, PinName en_chip, PinName* coilPins=NULL);

        void setPeriodU(float period);
        void setPeriodV(float period);
        void setPeriodW(float period);

        void setEnableChip(uint8_t value);

        void setStep(uint8_t step_number, float duty_right, float duty_left);
    private:
        DigitalOut enable1;
        DigitalOut enable2;
        DigitalOut enable3;

        DigitalOut en_chip;

        PwmOut uh;
        PwmOut vh;
        PwmOut wh;

};


#endif
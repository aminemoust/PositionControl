#include <mbed.h>
#include "../DigitalEncoderAS5601/DigitalEncoderAS5601.h"

#ifndef _Motor_H
#define _Motor_H


namespace Motor{

    class Motor{
        public:
            /*
                Costruttore con i PinName come parametri, per settare i pin 
                @param coilPins: array di dim. 3 che contiene i PinName di u,v,w (rispettivamente in questo ordine)
            */
            //Motor() = default;
            Motor(PinName en1, PinName en2, PinName en3, PinName en_chip, PinName* coilPins = nullptr);

            void setPeriodU(float period);
            void setPeriodV(float period);
            void setPeriodW(float period);

            void setEnableChip(uint8_t value);

            void setStep(uint8_t step_number, float duty_right, float duty_left);

            DigitalEncoderAS5601 encoder;

        private:
            DigitalOut _enable1;
            DigitalOut _enable2;
            DigitalOut _enable3;

            DigitalOut _en_chip;

            PwmOut _uh;
            PwmOut _vh;
            PwmOut _wh;

    };


}



#endif
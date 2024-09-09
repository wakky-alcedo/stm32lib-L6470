#ifndef L6470_HPP_
#define L6470_HPP_
#include <spi.h>
#include <l6470_definitions.hpp>

namespace l6470{

struct GPIO_pin {
	GPIO_TypeDef* GPIOx;
	uint16_t GPIO_Pin;
};

class L6470
{
    public:
        L6470(SPI_HandleTypeDef& hspi, GPIO_pin CS, GPIO_pin FLAG, GPIO_pin BUSY, GPIO_pin STCK, GPIO_pin RST);
        L6470(SPI_HandleTypeDef& hspi, GPIO_pin CS, GPIO_pin FLAG, GPIO_pin BUSY, GPIO_pin STCK);
        L6470(SPI_HandleTypeDef& hspi, GPIO_pin CS);

        //--- basic functions ---//
        void begin();

        //--- Set or Get comunicate functions ---//
        uint32_t GetParam(Addres addr,uint8_t size);
        uint32_t SetParam(Addres addr,uint8_t size,uint8_t val);
        void SetHoldKVAL(uint8_t val);
        StepMode GetStepMode();
        void SetStepMode(StepMode mode);

        //--- Motor action functions --- //
        void run(uint32_t speed,Direction dir);
        void move(uint32_t step,Direction dir,bool is_wait = true);
        void GoTo(uint32_t pos,bool is_wait = true);
        void GoToDir(uint32_t pos,Direction dir,bool is_wait = true);
        void GoUntil(uint32_t speed,Direction dir,uint8_t act,bool is_wait = true);
        void ReleaseSW(Direction dir,uint8_t act,bool is_wait = true);
        void GoHome();
        void GoMark();
        void ResetPos();
        void HardStop();
        void SoftStop();
        void HardHiz();
        void SoftHiz();

        long GetABSpos();
        void StckPulse();
        
        void SetInterrupt(SwMode mode);
        void SetStepClock(Direction dir);

        void HardReset();
        void SoftReset();

    private:
        uint8_t xfer(uint8_t send);
        void send24bit(uint32_t val);
        bool is_busy();
        void wait_busy();

        StepMode _stepmode;
        uint16_t _config;
        SPI_HandleTypeDef* hspi;
        GPIO_pin cs_pin;
		GPIO_pin flag_pin;
		GPIO_pin busy_pin;
		GPIO_pin stck_pin;
		GPIO_pin rst_pin;
        uint8_t _cmd;
};

};

#endif

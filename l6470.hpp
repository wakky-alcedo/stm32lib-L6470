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
        uint32_t GetParam(uint8_t addr,uint8_t size);
        uint32_t SetParam(uint8_t addr,uint8_t size,uint8_t val);
        void SetHoldKVAL(uint8_t val);
        int16_t GetStepMode();
        void SetStepMode(uint8_t mode);

        //--- Motor action functions --- //
        void run(uint32_t speed,uint8_t dir);
        void move(uint32_t step,uint8_t dir,bool is_wait = true);
        void GoTo(uint32_t pos,bool is_wait = true);
        void GoToDir(uint32_t pos,uint8_t dir,bool is_wait = true);
        void GoUntil(uint32_t speed,uint8_t dir,uint8_t act,bool is_wait = true);
        void ReleaseSW(uint8_t dir,uint8_t act,bool is_wait = true);
        void GoHome();
        void GoMark();
        void ResetPos();
        void HardStop();
        void SoftStop();
        void HardHiz();
        void SoftHiz();

        long GetABSpos();
        void StckPulse();
        
        void SetInterrupt(uint8_t mode);
        void SetStepClock(uint8_t dir);

        void HardReset();
        void SoftReset();

    private:
        void setSPImode();
        uint8_t xfer(uint8_t send);
        void send24bit(uint32_t val);
        bool is_busy();
        void wait_busy();

        uint8_t _stepmode;
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

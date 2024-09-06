#ifndef L6470_H_
#define L6470_H_
#include <Arduino.h>

//L6470 Action commands
#define CMD_NOP 0x00
#define CMD_SET_PARAM 0x00
#define CMD_GET_PARAM 0x20
#define CMD_RUN 0x50
#define CMD_STEP_CLOCK 0x58
#define CMD_MOVE 0x40
#define CMD_GOTO 0x60
#define CMD_GOTO_DIR 0x68
#define CMD_GO_UNTIL 0x82
#define CMD_RELEASE_SW 0x92
#define CMD_GO_HOME 0x70
#define CMD_GO_MARK 0x78
#define CMD_RESET_POS 0xD8
#define CMD_RESET_DEVICE 0xC0
#define CMD_SOFT_STOP 0xB0
#define CMD_HARD_STOP 0xB8
#define CMD_SOFT_HIZ 0xA0
#define CMD_HARD_HIZ 0xA8
#define CMD_GET_STATUS 0xD0

#define CW 0x00
#define CCW 0x01
#define ON 0x01
#define OFF 0x00

#define CMD_ACT_ON 0x08
#define CMD_ACT_OFF 0x00

//L6470 Addres
#define ADR_ABS_POS 0x01
#define ADR_EL_POS 0x02
#define ADR_MARK 0x03
#define ADR_SPEED 0x04
#define ADR_ACC 0x05
#define ADR_DEC 0x06
#define ADR_MAX_SPEED 0x07
#define ADR_MIN_SPEED 0x08
#define ADR_FS_SPD 0x15
#define ADR_KVAL_HOLD 0x09
#define ADR_KVAL_RUN 0x0A
#define ADR_KVAL_ACC 0x0B
#define ADR_KVAL_DEC 0x0C
#define ADR_INT_SPEED 0x0D
#define ADR_ST_SLP 0x0E
#define ADR_FN_SLP_ACC 0x0F
#define ADR_FN_SLP_DEC 0x10
#define ADR_K_THERM 0x11
#define ADR_ADC_OUT 0x12
#define ADR_OCD_TH 0x13
#define ADR_STALL_TH 0x14
#define ADR_STEP_MODE 0x16
#define ADR_ALARME_EN 0x17
#define ADR_CONFIG 0x18
#define ADR_STATUS 0x19

#define MASK_STEP_SEL 0x07;

#define FULL_STEP 1
#define MICRO_STEP1 2
#define MICRO_STEP2 4
#define MICRO_STEP3 8
#define MICRO_STEP4 16
#define MICRO_STEP5 32
#define MICRO_STEP6 64
#define MICRO_STEP7 128

#define BUSY_ON 0
#define BUSY_OFF 1

#define SOFT_STOP 0
#define HARD_STOP 1

class L6470
{
    public:
        L6470(uint8_t CS, uint8_t FLAG, uint8_t BUSY, uint8_t STCK, uint8_t RST);
        L6470(uint8_t CS, uint8_t FLAG, uint8_t BUSY, uint8_t STCK);

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
        void move(uint32_t step,uint8_t dir,uint8_t wait = BUSY_ON);
        void GoTo(uint32_t pos,uint8_t wait = BUSY_ON);
        void GoToDir(uint32_t pos,uint8_t dir,uint8_t wait = BUSY_ON);
        void GoUntil(uint32_t speed,uint8_t dir,uint8_t act,uint8_t wait = BUSY_ON);
        void ReleaseSW(uint8_t dir,uint8_t act,uint8_t wait = BUSY_ON);
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
        uint8_t available();
        void wait_available();

        uint8_t _stepmode;
        uint16_t _config;
        uint8_t _cs;
        uint8_t _flag;
        uint8_t _busy;
        uint8_t _stck;
        uint8_t _rst;
        uint8_t _cmd;
};

#endif
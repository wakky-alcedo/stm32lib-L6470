#include <arduino.h>
#include <SPI.h>
#include "L6470.h"

L6470::L6470(uint8_t CS, uint8_t FLAG, uint8_t BUSY, uint8_t STCK, uint8_t RST)
{
    _cs = CS;
    _flag = FLAG;
    _busy = BUSY;
    _stck = STCK;
    _rst = RST;
    _config = 0x0000;

    pinMode(_cs,OUTPUT);
    pinMode(_flag,INPUT);
    pinMode(_busy,INPUT);
    pinMode(_stck,OUTPUT);
    pinMode(_rst,OUTPUT);
    
    digitalWrite(_rst, HIGH);
    digitalWrite(_cs,HIGH);
}

//--- basic functions ---//
void L6470::begin(){

    setSPImode();
    HardReset();
    SoftReset();

    SetParam(ADR_ACC,2,0x40);
    SetParam(ADR_DEC,2,0x40);
    SetParam(ADR_MAX_SPEED,2,0x03f);
    SetParam(ADR_MIN_SPEED,2,0xfff);
    SetParam(ADR_FS_SPD,2,0x3ff);
    SetHoldKVAL(125);
    SetParam(ADR_KVAL_RUN,1,0xff);
    SetParam(ADR_KVAL_ACC,1,0xff);
    SetParam(ADR_KVAL_DEC,1,0xff);
    SetParam(ADR_STEP_MODE,1,0x07);
    _config = GetParam(ADR_CONFIG,2);

    _stepmode = GetStepMode();
}

inline void L6470::setSPImode(){
  SPI.setDataMode(SPI_MODE3);
  SPI.setBitOrder(MSBFIRST);
}

void L6470::SetHoldKVAL(uint8_t val){
    SetParam(ADR_KVAL_HOLD,1,val);
}

inline uint8_t L6470::xfer(uint8_t send){
    uint8_t buf = 0;

    setSPImode();

    digitalWrite(_cs, LOW);
    buf = SPI.transfer(send);
    digitalWrite(_cs, HIGH);
    delayMicroseconds(1);
    return buf;
}

inline void L6470::send24bit(uint32_t val){
    for (int i = 0; i < 3; i++){
        xfer(val >> (8*(2-i)));
    }
}

inline uint8_t L6470::available(){
    return !digitalRead(_busy);
}

inline void L6470::wait_available(){
    while(available());
    delay(1);
}

//--- Set or Get comunicate functions ---//
uint32_t L6470::GetParam(uint8_t addr,uint8_t size){
    uint32_t buf;

    _cmd = CMD_GET_PARAM | addr;

    xfer(_cmd);
    for (int i = 0; i < size; i++){
        buf |= xfer(CMD_NOP) << (8*(size-1-i));
    }

    return buf;
}

uint32_t L6470::SetParam(uint8_t addr,uint8_t size,uint8_t val){
    uint32_t buf;

    _cmd = CMD_SET_PARAM | addr;

    xfer(_cmd);
    for (int i = 0; i < size; i++){
        xfer(val >> (8*(size-1-i)));
    }
}

void L6470::SetStepMode(uint8_t mode){
    uint8_t buf;
    buf = GetParam(ADR_STEP_MODE,1);
    buf = buf & MASK_STEP_SEL;

    switch(mode){
        case FULL_STEP:
            SetParam(ADR_STEP_MODE,1,0x00);
            break;
            
        case MICRO_STEP1:
            SetParam(ADR_STEP_MODE,1,0x01);
            break;
            
        case MICRO_STEP2:
            SetParam(ADR_STEP_MODE,1,0x02);
            break;
            
        case MICRO_STEP3:
            SetParam(ADR_STEP_MODE,1,0x03);
            break;
            
        case MICRO_STEP4:
            SetParam(ADR_STEP_MODE,1,0x04);
            break;
            
        case MICRO_STEP5:
            SetParam(ADR_STEP_MODE,1,0x05);
            break;
            
        case MICRO_STEP6:
            SetParam(ADR_STEP_MODE,1,0x06);
            break;
            
        case MICRO_STEP7:
            SetParam(ADR_STEP_MODE,1,0x07);
            break;
    }
    _stepmode = GetStepMode();
}

int16_t L6470::GetStepMode(){
    uint8_t buf;
    buf = GetParam(ADR_STEP_MODE,1);
    buf = buf & MASK_STEP_SEL;

    switch(buf){
        case 0x00:
            return FULL_STEP;
            
        case 0x01:
            return MICRO_STEP1;
            
        case 0x02:
            return MICRO_STEP2;
            
        case 0x03:
            return MICRO_STEP3;
            
        case 0x04:
            return MICRO_STEP4;
            
        case 0x05:
            return MICRO_STEP5;
            
        case 0x06:
            return MICRO_STEP6;
            
        case 0x07:
            return MICRO_STEP7;
            
        default:
            return -1;
    }
}

//--- Motor action functions --- //
void L6470::run(uint32_t speed,uint8_t dir){
    xfer(CMD_RUN | dir);
    send24bit(speed);
}

void L6470::move(uint32_t step,uint8_t dir,uint8_t wait){
    GetStepMode();

    step = step*_stepmode;

    xfer(CMD_MOVE | dir);
    send24bit(step);

    if(wait == BUSY_ON)wait_available();
}

void L6470::GoTo(uint32_t pos,uint8_t wait){
    xfer(CMD_GOTO); 
    send24bit(pos);

    if(wait == BUSY_ON)wait_available();
}

void L6470::GoToDir(uint32_t pos,uint8_t dir,uint8_t wait){
    xfer(CMD_GOTO_DIR | dir); 
    send24bit(pos);
    
    if(wait == BUSY_ON)wait_available();
}

void L6470::GoUntil(uint32_t speed,uint8_t dir,uint8_t act,uint8_t wait){
    xfer(CMD_GO_UNTIL | dir | act);
    send24bit(speed);
    
    if(wait == BUSY_ON)wait_available();
}

void L6470::ReleaseSW(uint8_t dir,uint8_t act,uint8_t wait){
    xfer(CMD_RELEASE_SW | dir | act);
    
    if(wait == BUSY_ON)wait_available();
}

void L6470::GoHome(){ xfer(CMD_GO_HOME); }
void L6470::GoMark(){ xfer(CMD_GO_MARK); }
void L6470::ResetPos(){ xfer(CMD_RESET_POS); }
void L6470::HardStop(){ xfer(CMD_HARD_STOP); }
void L6470::SoftStop(){ xfer(CMD_SOFT_STOP); }
void L6470::HardHiz(){ xfer(CMD_HARD_HIZ); }
void L6470::SoftHiz(){ xfer(CMD_SOFT_HIZ); }

long L6470::GetABSpos(){
    return (long)((GetParam(ADR_ABS_POS,3)) << 10) / 1024;
}

void L6470::StckPulse(){
    digitalWrite(_stck,HIGH);
    delayMicroseconds(1);
    digitalWrite(_stck,LOW);
    delayMicroseconds(1);
}

void L6470::SetInterrupt(uint8_t mode){
    _config = GetParam(ADR_CONFIG,2);

    if(mode == ON) _config = _config & 0xffef;
    if(mode == OFF) _config = _config | 0x0010;

    SetParam(ADR_CONFIG,2,_config);
}

void L6470::SetStepClock(uint8_t dir){
    xfer(CMD_STEP_CLOCK | dir);
}

void L6470::HardReset(){
    digitalWrite(_rst,LOW);
    delay(500);
    digitalWrite(_rst,HIGH);
}

void L6470::SoftReset(){
    xfer(CMD_NOP);
    xfer(CMD_NOP);
    xfer(CMD_NOP);
    xfer(CMD_NOP);
    xfer(CMD_RESET_DEVICE);
    delay(100);
}

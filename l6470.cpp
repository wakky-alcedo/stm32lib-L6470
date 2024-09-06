#include <l6470.hpp>
#include <SPI.h>

using namespace l6470;

L6470::L6470(SPI_HandleTypeDef& hspi, GPIO_pin cs_pin, GPIO_pin flag_pin, GPIO_pin busy_pin, GPIO_pin stck_pin, GPIO_pin rst_pin):
	hspi(&hspi),
	cs_pin(cs_pin),
	flag_pin(flag_pin),
	busy_pin(busy_pin),
	stck_pin(stck_pin),
	rst_pin(rst_pin)
{
    _config = 0x0000;

//    pinMode(_cs,OUTPUT);
//    pinMode(_flag,INPUT);
//    pinMode(_busy,INPUT);
//    pinMode(_stck,OUTPUT);
//    pinMode(_rst,OUTPUT);

}
L6470::L6470(SPI_HandleTypeDef& hspi, GPIO_pin cs_pin, GPIO_pin flag_pin, GPIO_pin busy_pin, GPIO_pin stck_pin):
	hspi(&hspi),
	cs_pin(cs_pin),
	flag_pin(flag_pin),
	busy_pin(busy_pin),
	stck_pin(stck_pin)
{
    _config = 0x0000;
}

L6470::L6470(SPI_HandleTypeDef& hspi, GPIO_pin cs_pin):
	hspi(&hspi),
	cs_pin(cs_pin)
{
    _config = 0x0000;
}


//--- basic functions ---//
void L6470::begin(){
	if (rst_pin.GPIOx != 0x00) {
		HAL_GPIO_WritePin(rst_pin.GPIOx, rst_pin.GPIO_Pin, GPIO_PIN_SET);
	}
    HAL_GPIO_WritePin(cs_pin.GPIOx, cs_pin.GPIO_Pin, GPIO_PIN_SET);

    setSPImode();
    HardReset();
    SoftReset();

    SetParam(Addres::ADR_ACC,2,0x40);
    SetParam(Addres::ADR_DEC,2,0x40);
    SetParam(Addres::ADR_MAX_SPEED,2,0x03f);
    SetParam(Addres::ADR_MIN_SPEED,2,0xfff);
    SetParam(Addres::ADR_FS_SPD,2,0x3ff);
    SetHoldKVAL(125);
    SetParam(Addres::ADR_KVAL_RUN,1,0xff);
    SetParam(Addres::ADR_KVAL_ACC,1,0xff);
    SetParam(Addres::ADR_KVAL_DEC,1,0xff);
    SetParam(Addres::ADR_STEP_MODE,1,0x07);
    _config = GetParam(Addres::ADR_CONFIG,2);

    _stepmode = GetStepMode();
}

inline void L6470::setSPImode(){
//  SPI.setDataMode(SPI_MODE3);
//  SPI.setBitOrder(MSBFIRST);
}

void L6470::SetHoldKVAL(uint8_t val){
    SetParam(Addres::ADR_KVAL_HOLD,1,val);
}

inline uint8_t L6470::xfer(uint8_t send){
    uint8_t buf = 0;

    setSPImode();
//    while(!HAL_GPIO_ReadPin(busy_pin.GPIOx, busy_pin.GPIO_Pin)){} //BESYが解除されるまで待機
    HAL_GPIO_WritePin(cs_pin.GPIOx, cs_pin.GPIO_Pin, GPIO_PIN_RESET);
//	HAL_SPI_TransmitReceive(hspi,(uint8_t*)&send, (uint8_t*)&buf, sizeof(send), 1000);
	HAL_SPI_Transmit(hspi,(uint8_t*)&send, sizeof(send), 1000);
    HAL_GPIO_WritePin(cs_pin.GPIOx, cs_pin.GPIO_Pin, GPIO_PIN_SET);
//    delayMicroseconds(1);
    return buf;
}

inline void L6470::send24bit(uint32_t val){
    for (int i = 0; i < 3; i++){
        xfer(val >> (8*(2-i)));
    }
}

inline bool L6470::is_busy(){
    return (bool)!HAL_GPIO_ReadPin(busy_pin.GPIOx, busy_pin.GPIO_Pin);
}

inline void L6470::wait_busy(){
    while(is_busy());
    HAL_Delay(1);
}

//--- Set or Get comunicate functions ---//
uint32_t L6470::GetParam(uint8_t addr,uint8_t size){
    uint32_t buf;

    _cmd = Command::CMD_GET_PARAM | addr;

    xfer(_cmd);
    for (int i = 0; i < size; i++){
        buf |= xfer(Command::CMD_NOP) << (8*(size-1-i));
    }

    return buf;
}

uint32_t L6470::SetParam(uint8_t addr,uint8_t size,uint8_t val){
    uint32_t buf;

    _cmd = Command::CMD_SET_PARAM | addr;

    xfer(_cmd);
    for (int i = 0; i < size; i++){
        xfer(val >> (8*(size-1-i)));
    }
}

void L6470::SetStepMode(uint8_t mode){
    uint8_t buf;
    buf = GetParam(Addres::ADR_STEP_MODE,1);
    buf = buf & MASK_STEP_SEL;

    switch(mode){
        case FULL_STEP:
            SetParam(Addres::ADR_STEP_MODE,1,0x00);
            break;

        case MICRO_STEP1:
            SetParam(Addres::ADR_STEP_MODE,1,0x01);
            break;

        case MICRO_STEP2:
            SetParam(Addres::ADR_STEP_MODE,1,0x02);
            break;

        case MICRO_STEP3:
            SetParam(Addres::ADR_STEP_MODE,1,0x03);
            break;

        case MICRO_STEP4:
            SetParam(Addres::ADR_STEP_MODE,1,0x04);
            break;

        case MICRO_STEP5:
            SetParam(Addres::ADR_STEP_MODE,1,0x05);
            break;

        case MICRO_STEP6:
            SetParam(Addres::ADR_STEP_MODE,1,0x06);
            break;

        case MICRO_STEP7:
            SetParam(Addres::ADR_STEP_MODE,1,0x07);
            break;
    }
    _stepmode = GetStepMode();
}

int16_t L6470::GetStepMode(){
    uint8_t buf;
    buf = GetParam(Addres::ADR_STEP_MODE,1);
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
void L6470::run(uint32_t speed,Direction dir){
    xfer(Command::CMD_RUN | (uint8_t)dir);
    send24bit(speed);
}

void L6470::move(uint32_t step,Direction dir,bool is_wait){
    GetStepMode();

    step = step*_stepmode;

    xfer(Command::CMD_MOVE | (uint8_t)dir);
    send24bit(step);

    if(is_wait)wait_busy();
}

void L6470::GoTo(uint32_t pos,bool is_wait){
    xfer(Command::CMD_GOTO);
    send24bit(pos);

    if(is_wait)wait_busy();
}

void L6470::GoToDir(uint32_t pos,Direction dir,bool is_wait){
    xfer(Command::CMD_GOTO_DIR | (uint8_t)dir);
    send24bit(pos);

    if(is_wait)wait_busy();
}

void L6470::GoUntil(uint32_t speed,Direction dir,uint8_t act,bool is_wait){
    xfer(Command::CMD_GO_UNTIL | (uint8_t)dir | act);
    send24bit(speed);

    if(is_wait)wait_busy();
}

void L6470::ReleaseSW(Direction dir,uint8_t act,bool is_wait){
    xfer(Command::CMD_RELEASE_SW | (uint8_t)dir | act);

    if(is_wait)wait_busy();
}

void L6470::GoHome(){ xfer(Command::CMD_GO_HOME); }
void L6470::GoMark(){ xfer(Command::CMD_GO_MARK); }
void L6470::ResetPos(){ xfer(Command::CMD_RESET_POS); }
void L6470::HardStop(){ xfer(Command::CMD_HARD_STOP); }
void L6470::SoftStop(){ xfer(Command::CMD_SOFT_STOP); }
void L6470::HardHiz(){ xfer(Command::CMD_HARD_HIZ); }
void L6470::SoftHiz(){ xfer(Command::CMD_SOFT_HIZ); }

long L6470::GetABSpos(){
    return (long)((GetParam(Addres::ADR_ABS_POS,3)) << 10) / 1024;
}

void L6470::StckPulse(){
    HAL_GPIO_WritePin(stck_pin.GPIOx, stck_pin.GPIO_Pin, GPIO_PIN_SET);
//    delayMicroseconds(1);
    HAL_GPIO_WritePin(stck_pin.GPIOx, stck_pin.GPIO_Pin, GPIO_PIN_RESET);
//    delayMicroseconds(1);
}

void L6470::SetInterrupt(uint8_t mode){
    _config = GetParam(Addres::ADR_CONFIG,2);

    if(mode == ON) _config = _config & 0xffef;
    if(mode == OFF) _config = _config | 0x0010;

    SetParam(Addres::ADR_CONFIG,2,_config);
}

void L6470::SetStepClock(Direction dir){
    xfer(Command::CMD_STEP_CLOCK | (uint8_t)dir);
}

void L6470::HardReset(){
	if (rst_pin.GPIOx != 0x00) {
	    HAL_GPIO_WritePin(rst_pin.GPIOx, rst_pin.GPIO_Pin, GPIO_PIN_RESET);
	    HAL_Delay(500);
	    HAL_GPIO_WritePin(rst_pin.GPIOx, rst_pin.GPIO_Pin, GPIO_PIN_SET);
	}
}

void L6470::SoftReset(){
    xfer(Command::CMD_NOP);
    xfer(Command::CMD_NOP);
    xfer(Command::CMD_NOP);
    xfer(Command::CMD_NOP);
    xfer(Command::CMD_RESET_DEVICE);
    HAL_Delay(100);
}


#include <l6470.hpp>
#include <SPI.h>

using namespace l6470;

L6470::L6470(SPI_HandleTypeDef& hspi, GPIO_Pin cs_pin, GPIO_Pin flag_pin, GPIO_Pin busy_pin, GPIO_Pin stck_pin, GPIO_Pin rst_pin, uint8_t timeout):
    hspi(&hspi),
    cs_pin(cs_pin),
    flag_pin(flag_pin),
    busy_pin(busy_pin),
    stck_pin(stck_pin),
    rst_pin(rst_pin),
	timeout(timeout)
{
}

L6470::L6470(SPI_HandleTypeDef& hspi, GPIO_Pin cs_pin, GPIO_Pin flag_pin, GPIO_Pin busy_pin, GPIO_Pin stck_pin, uint8_t timeout):
    hspi(&hspi),
    cs_pin(cs_pin),
    flag_pin(flag_pin),
    busy_pin(busy_pin),
    stck_pin(stck_pin),
	timeout(timeout)
{
}

L6470::L6470(SPI_HandleTypeDef& hspi, GPIO_Pin cs_pin, uint8_t timeout):
    hspi(&hspi),
    cs_pin(cs_pin),
	timeout(timeout)
{
}


//--- basic functions ---//
void L6470::init(){
    if (rst_pin.gpio_x != 0x00) {
        HAL_GPIO_WritePin(rst_pin.gpio_x, rst_pin.gpio_pin, GPIO_PIN_SET);
    }
    HAL_GPIO_WritePin(cs_pin.gpio_x, cs_pin.gpio_pin, GPIO_PIN_SET);

    hard_reset();
    soft_reset();

    set_param(Address::ADR_ACC,2,0x40);
    set_param(Address::ADR_DEC,2,0x40);
    set_param(Address::ADR_MAX_SPEED,2,0x03f);
    set_param(Address::ADR_MIN_SPEED,2,0xfff);
    set_param(Address::ADR_FS_SPD,2,0x3ff);
    set_kval_hold(125);
    set_param(Address::ADR_KVAL_RUN,1,0xff);
    set_param(Address::ADR_KVAL_ACC,1,0xff);
    set_param(Address::ADR_KVAL_DEC,1,0xff);
    set_step_mode(StepMode::MICRO_STEP128);
}

void L6470::set_kval_hold(uint8_t val){
    set_param(Address::ADR_KVAL_HOLD,1,val);
}

inline uint8_t L6470::transmit_receve(uint8_t transmit_data){
	wait_busy();

	uint8_t receve_data = 0;
    HAL_GPIO_WritePin(cs_pin.gpio_x, cs_pin.gpio_pin, GPIO_PIN_RESET);
    HAL_SPI_TransmitReceive(hspi,(uint8_t*)&transmit_data, (uint8_t*)&receve_data, sizeof(transmit_data), timeout);
    HAL_GPIO_WritePin(cs_pin.gpio_x, cs_pin.gpio_pin, GPIO_PIN_SET);
//    delayMicroseconds(1);
    return receve_data;
}

inline void L6470::transmit_24bit(uint32_t value){
    for (int i = 0; i < 3; i++){
        transmit_receve(value >> (8*(2-i)));
    }
}

inline bool L6470::is_busy(){
	if (busy_pin.gpio_x != 0x00) {
		return !(bool)HAL_GPIO_ReadPin(busy_pin.gpio_x, busy_pin.gpio_pin);
	}else{
		return false;
	}
}

inline void L6470::wait_busy(){
    while(is_busy());
//    HAL_Delay(1);
}

//--- Set or Get comunicate functions ---//
uint32_t L6470::get_param(Address addr, uint8_t size){
    transmit_receve(Command::CMD_GET_PARAM | (uint8_t)addr);

    uint32_t buf = 0;
    for (int i = 0; i < size; i++){
        buf |= transmit_receve(Command::CMD_NOP) << (8*(size-1-i));
    }
    return buf;
}

void L6470::set_param(Address addr, uint8_t size, uint8_t val){
    transmit_receve(Command::CMD_SET_PARAM | (uint8_t)addr);

    for (int i = 0; i < size; i++){
        transmit_receve(val >> (8*(size-1-i)));
    }
}

void L6470::set_step_mode(StepMode mode){
    set_param(Address::ADR_STEP_MODE,1,(uint8_t)mode);
    step_mode = get_step_mode();
}

StepMode L6470::get_step_mode(){
    return (StepMode)(get_param(Address::ADR_STEP_MODE,1) & MASK_STEP_SEL);
}

//--- Motor action functions --- //
void L6470::run(uint32_t speed, Direction dir){
    transmit_receve(Command::CMD_RUN | (uint8_t)dir);
    transmit_24bit(speed);
}

void L6470::move(uint32_t step, Direction dir, bool is_wait){
    step = step*num_step[(uint8_t)step_mode]; // step_mode変数が常に最新であることが前提なので，非安全な処理である

    transmit_receve(Command::CMD_MOVE | (uint8_t)dir);
    transmit_24bit(step);

    if(is_wait)wait_busy();
}

void L6470::go_to(uint32_t pos, bool is_wait){
    transmit_receve(Command::CMD_GOTO);
    transmit_24bit(pos);

    if(is_wait)wait_busy();
}

void L6470::go_to_dir(uint32_t pos, Direction dir, bool is_wait){
    transmit_receve(Command::CMD_GOTO_DIR | (uint8_t)dir);
    transmit_24bit(pos);

    if(is_wait)wait_busy();
}

void L6470::go_until(uint32_t speed, Direction dir, uint8_t act, bool is_wait){
    transmit_receve(Command::CMD_GO_UNTIL | (uint8_t)dir | act);
    transmit_24bit(speed);

    if(is_wait)wait_busy();
}

void L6470::release_sw(Direction dir, uint8_t act, bool is_wait){
    transmit_receve(Command::CMD_RELEASE_SW | (uint8_t)dir | act);

    if(is_wait)wait_busy();
}

void L6470::go_home(){ transmit_receve(Command::CMD_GO_HOME); }
void L6470::go_mark(){ transmit_receve(Command::CMD_GO_MARK); }
void L6470::reset_pos(){ transmit_receve(Command::CMD_RESET_POS); }
void L6470::hard_stop(){ transmit_receve(Command::CMD_HARD_STOP); }
void L6470::soft_stop(){ transmit_receve(Command::CMD_SOFT_STOP); }
void L6470::hard_hiz(){ transmit_receve(Command::CMD_HARD_HIZ); }
void L6470::soft_hiz(){ transmit_receve(Command::CMD_SOFT_HIZ); }

long L6470::get_abs_pos(){
    return (long)((get_param(Address::ADR_ABS_POS,3)) << 10) / 1024;
}

void L6470::stck_pulse(){
    HAL_GPIO_WritePin(stck_pin.gpio_x, stck_pin.gpio_pin, GPIO_PIN_SET);
//    delayMicroseconds(1);
    HAL_GPIO_WritePin(stck_pin.gpio_x, stck_pin.gpio_pin, GPIO_PIN_RESET);
//    delayMicroseconds(1);
}

void L6470::set_interrupt(SwMode mode){
	uint16_t config = get_param(Address::ADR_CONFIG,2);

    if(mode == SwMode::HardStopInterrupt){
        config = config & 0xffef;
    }else if(mode == SwMode::UserDisposal){
        config = config | 0x0010;
    }

    set_param(Address::ADR_CONFIG,2,config);
}

void L6470::set_step_clock(Direction dir){
    transmit_receve(Command::CMD_STEP_CLOCK | (uint8_t)dir);
}

void L6470::hard_reset(){
    if (rst_pin.gpio_x != 0x00) {
        HAL_GPIO_WritePin(rst_pin.gpio_x, rst_pin.gpio_pin, GPIO_PIN_RESET);
        HAL_Delay(500);
        HAL_GPIO_WritePin(rst_pin.gpio_x, rst_pin.gpio_pin, GPIO_PIN_SET);
    }
}

void L6470::soft_reset(){
    transmit_receve(Command::CMD_NOP);
    transmit_receve(Command::CMD_NOP);
    transmit_receve(Command::CMD_NOP);
    transmit_receve(Command::CMD_NOP);
    transmit_receve(Command::CMD_RESET_DEVICE);
    HAL_Delay(100);
}

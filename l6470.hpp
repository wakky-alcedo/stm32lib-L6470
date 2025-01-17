#ifndef L6470_HPP_
#define L6470_HPP_

#include <spi.h>
#include <l6470_definitions.hpp>

namespace l6470 {

struct GPIO_Pin {
    GPIO_TypeDef* gpio_x;
    uint16_t gpio_pin;
};

class L6470 {
public:
	L6470(SPI_HandleTypeDef& hspi, GPIO_Pin cs_pin, GPIO_Pin flag_pin, GPIO_Pin busy_pin, GPIO_Pin stck_pin, GPIO_Pin rst_pin, uint8_t timeout);
	L6470(SPI_HandleTypeDef& hspi, GPIO_Pin cs_pin, GPIO_Pin flag_pin, GPIO_Pin busy_pin, GPIO_Pin stck_pin, uint8_t timeout);
	L6470(SPI_HandleTypeDef& hspi, GPIO_Pin cs_pin, uint8_t timeout);

    //--- basic functions ---//
    void init();

    //--- Set or Get communicate functions ---//
    uint32_t get_param(Address addr, uint8_t size);
    void set_param(Address addr, uint8_t size, uint8_t val);

    void set_kval_hold(uint8_t val);
    void set_step_mode(StepMode mode);
    StepMode get_step_mode();

    //--- Motor action functions --- //
    void run(uint32_t speed, Direction dir);
    void move(uint32_t step, Direction dir, bool is_wait = false);
    void go_to(uint32_t pos, bool is_wait = false);
    void go_to_dir(uint32_t pos, Direction dir, bool is_wait = false);
    void go_until(uint32_t speed, Direction dir, uint8_t act, bool is_wait = false);
    void release_sw(Direction dir, uint8_t act, bool is_wait = false);
    void go_home();
    void go_mark();
    void reset_pos();
    void hard_stop();
    void soft_stop();
    void hard_hiz();
    void soft_hiz();

    long get_abs_pos();
    void stck_pulse();
        
    void set_interrupt(SwMode mode);
    void set_step_clock(Direction dir);

    void hard_reset();
    void soft_reset();

private:
    uint8_t transmit_receve(uint8_t transmit_data);
    void transmit_24bit(uint32_t value);
    bool is_busy();
    void wait_busy();

    StepMode step_mode;
    SPI_HandleTypeDef* hspi;
    GPIO_Pin cs_pin;
    GPIO_Pin flag_pin;
    GPIO_Pin busy_pin;
    GPIO_Pin stck_pin;
    GPIO_Pin rst_pin;
    uint8_t timeout;
};

};

#endif

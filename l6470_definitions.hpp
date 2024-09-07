#ifndef _L6470_DEF
#define _L6470_DEF

#include <stdint.h>
#include <array>

namespace l6470{

//L6470 Action commands
namespace Command {
	constexpr uint8_t CMD_NOP 		    = 0x00;
	constexpr uint8_t CMD_SET_PARAM 	= 0x00;
	constexpr uint8_t CMD_GET_PARAM 	= 0x20;
	constexpr uint8_t CMD_RUN 		    = 0x50;
	constexpr uint8_t CMD_STEP_CLOCK 	= 0x58;
	constexpr uint8_t CMD_MOVE 		    = 0x40;
	constexpr uint8_t CMD_GOTO 		    = 0x60;
	constexpr uint8_t CMD_GOTO_DIR 	    = 0x68;
	constexpr uint8_t CMD_GO_UNTIL 	    = 0x82;
	constexpr uint8_t CMD_RELEASE_SW 	= 0x92;
	constexpr uint8_t CMD_GO_HOME 		= 0x70;
	constexpr uint8_t CMD_GO_MARK 		= 0x78;
	constexpr uint8_t CMD_RESET_POS 	= 0xD8;
	constexpr uint8_t CMD_RESET_DEVICE 	= 0xC0;
	constexpr uint8_t CMD_SOFT_STOP 	= 0xB0;
	constexpr uint8_t CMD_HARD_STOP 	= 0xB8;
	constexpr uint8_t CMD_SOFT_HIZ 		= 0xA0;
	constexpr uint8_t CMD_HARD_HIZ 		= 0xA8;
	constexpr uint8_t CMD_GET_STATUS 	= 0xD0;
}

enum class Direction :uint8_t {
	CW  = 0x00,
	CCW = 0x01
};

enum class SwMode : uint8_t {
	HardStopInterrupt 	= 0x00,
	UserDisposal 		= 0x01,
};

enum class Act : uint8_t {
	OFF = 0x00,
	ON  = 0x08,
};

//L6470 Addres
namespace Addres {
	constexpr uint8_t ADR_ABS_POS 		= 0x01;
	constexpr uint8_t ADR_EL_POS 		= 0x02;
	constexpr uint8_t ADR_MARK 			= 0x03;
	constexpr uint8_t ADR_SPEED 		= 0x04;
	constexpr uint8_t ADR_ACC 			= 0x05;
	constexpr uint8_t ADR_DEC 			= 0x06;
	constexpr uint8_t ADR_MAX_SPEED 	= 0x07;
	constexpr uint8_t ADR_MIN_SPEED 	= 0x08;
	constexpr uint8_t ADR_FS_SPD 		= 0x15;
	constexpr uint8_t ADR_KVAL_HOLD 	= 0x09;
	constexpr uint8_t ADR_KVAL_RUN 		= 0x0A;
	constexpr uint8_t ADR_KVAL_ACC 		= 0x0B;
	constexpr uint8_t ADR_KVAL_DEC 		= 0x0C;
	constexpr uint8_t ADR_INT_SPEED 	= 0x0D;
	constexpr uint8_t ADR_ST_SLP 		= 0x0E;
	constexpr uint8_t ADR_FN_SLP_ACC 	= 0x0F;
	constexpr uint8_t ADR_FN_SLP_DEC 	= 0x10;
	constexpr uint8_t ADR_K_THERM 		= 0x11;
	constexpr uint8_t ADR_ADC_OUT 		= 0x12;
	constexpr uint8_t ADR_OCD_TH 		= 0x13;
	constexpr uint8_t ADR_STALL_TH 		= 0x14;
	constexpr uint8_t ADR_STEP_MODE 	= 0x16;
	constexpr uint8_t ADR_ALARME_EN 	= 0x17;
	constexpr uint8_t ADR_CONFIG 		= 0x18;
	constexpr uint8_t ADR_STATUS 		= 0x19;
}

constexpr uint8_t MASK_STEP_SEL = 0x07; //0b0111

enum class StepMode : uint8_t {
	FULL_STEP 		= 0b000,
	MICRO_STEP002 	= 0b001, // Half step
	MICRO_STEP004 	= 0b010,
	MICRO_STEP008 	= 0b011,
	MICRO_STEP016 	= 0b100,
	MICRO_STEP032 	= 0b101,
	MICRO_STEP064 	= 0b110,
	MICRO_STEP128 	= 0b111
};

constexpr std::array<uint8_t, 8> num_step = {1, 2, 4, 8, 16, 32, 64, 128};

}

#endif

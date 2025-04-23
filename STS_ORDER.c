#include <stdio.h>
#include "STS_ORDER.h"

static uint8_t Level = 1; // Servo ACK Level
static uint8_t END = 0; // 0 = LITTLE, 1 = BIG
static uint8_t servo_status;
static uint8_t servo_error; 


void ORDER_GET_END(uint8_t _END){
    // GET END SETTINGS
    END = _END;
    return END;
}

bool ORDER_IS_BIG_END(uint8_t _END){
    // RETURN 1 IF BIG END, 0 IF LITTLE END
    END = _END;
    if(END == 0){
        return false; 
    }
    return true;
}

bool ORDER_IS_LITTLE_END(uint8_t _END){
    // RETURN 1 IF LITTLE END, 0 IF BIG END
    END = _END;
    if(END == 1){
        return false;
    }
    return true;
}

void ORDER_GET_LEVEL(void){
    // GET SERVO ACK LEVEL
    return Level;
}

void ORDER_GET_SERVO_STATUS(void){
    // GET SERVO STATUS
    return servo_status; 
}

void ORDER_GET_SERVO_ERROR(void){
    // GET SERVO ERROR
    return servo_error;
}

void ORDER_SET_LEVEL(uint8_t _Level){
    // SET SERVO ACK LEVEL
    Level = _Level;
    return Level; 
}

void 
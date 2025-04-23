#include <stdio.h>
#include "STS_ORDER.h"

static uint8_t ORDER = 0; // 0 = LITTLE, 1 = BIG
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

bool
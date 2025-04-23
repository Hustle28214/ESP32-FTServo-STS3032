#include <stdio.h>
#include "STS_ORDER.h"
#include "STS_INST.h"

static uint8_t Level = 1; // Servo ACK Level
static uint8_t End = 0; // 0 = LITTLE, 1 = BIG
static uint8_t servo_status;
static uint8_t servo_error; 


void ORDER_GET_END(uint8_t _End){
    // GET END SETTINGS
    END = _End;
    return END;
}

bool ORDER_IS_BIG_END(uint8_t _End){
    // RETURN 1 IF BIG END, 0 IF LITTLE END
    END = _End;
    if(END == 0){
        return false; 
    }
    return true;
}

bool ORDER_IS_LITTLE_END(uint8_t _End){
    // RETURN 1 IF LITTLE END, 0 IF BIG END
    END = _End;
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
// 16 digits devided to 2 * 8 digits
void DIGITS16_TO_DIGITS8(uint8_t *_dataL,uint8_t *_dataH, uint16_t _digits16){
    
    if (_dataL == NULL || _dataH == NULL) {
        return;
    }// Check if pointers are valid

    if(End) // BIG END, data addr : high to low
    {
        *_dataL = (uint8_t)(_digits16 >> 8); // dataL
        *_dataH = (uint8_t)(_digits16 & 0xFF); // dataH
    }
    else{
        // LITTLE END, data addr : low to high
        *_dataL = (uint8_t)(_digits16 & 0xFF); // dataL
        *_dataH = (uint8_t)(_digits16>>8); // dataH
    }
}

// 2 * 8 digits combined to 16 digits

void DIGITS8_TO_DIGITS16(uint8_t _dataL,uint8_t _dataH, uint8_t _digits8){
   
    uint16_t digits16;
    if(End) // BIG END, data addr : high to low
    {
        *digits16 = (uint16_t)((dataH << 8) | dataL);
    }
    else{
        // LITTLE END, data addr : low to high
        *digits16 = (uint16_t)((dataL << 8) | dataH);
    }
    return digits16;
}

void MSGBUF_WRITE(uint8_t _ID, uint8_t _Addr, uint8_t *_Data, uint8_t _Len,uint8_t _Inst){
    // WRITE DATA TO SERVO
    uint8_t delta = 2; // Len = n+2
    uint8_t _MsgBuffer[6]; // MAX ORDER LENGTH IS 10 BYTES
    _MsgBuffer[0] = 0xFF; // START BYTE
    _MsgBuffer[1] =0xFF;
    _MsgBuffer[2] = _ID; // ID, 0~253, 0X00~0XFD, 254 is Broadcast ID
    _MsgBuffer[4] = _Inst; // Inst 
    uint8_t FinalLen;
    // Len = 
    if(_Data){
        FinalLen = delta + _Len + 1; // Q: WHY plus 1?
        _MsgBuffer[3] = FinalLen;
        _MsgBuffer[5] = _Addr; // MemoryAddr, Double Byte Parameter,
        writeSCS(_MsgBuffer,6);
    }else{
        // _Data is NULL
        _MsgBuffer[3] = _Len;
        writeSCS(_MsgBuffer,5);
    }
    uint8_t _CheckSum = ~(_ID + _Inst + FinalLen + _Addr); // CHECKSUM
    
    // TODO: Add writeSCS() to another file

    if(_Data){
        for (int i = 0; i < _Len; i++){
           _CheckSum += _Data[i]; // Plus all Para data to checksums
        }
        writeSCS(_Data,_Len); // Write Data to Servo
    }
    _CheckSum = ~_CheckSum; // CheckSum = ~CheckSum
    _MsgBuffer[6] = _CheckSum;
    writeSCS(&_CheckSum,1);
}


// Inst: Ping 0x01, Read 0x02, Write 0x03, Reg Write 0x04, Action 0x05, Reset 0x0A, Sync Write 0x83, Sync Read 0x82, Recovery 0x06
// 3 kinds of Write Order: Ordinary Write, Reg Write, Sync Write
void OrdinaryWrite(uint8_t _ID, uint8_t _Addr, uint8_t *_Data, uint8_t _Len){
    // Write Data to Servo, ID, Addr, Data, Len
    // TODO: Add SerialBusDelay() to another file(Serial) and pick Non-Blocking Mode or Blocking Mode
    SerialBusDelay();
    MSGBUF_WRITE(_ID,_Addr,_Data,_Len,STS_INST_WRITE);
    SerialBusDelay();
    // TODO: Add Ack() to this file(STS_ORDER)
    return Ack(_ID);
}


void RegWrite(uint8_t _ID, uint8_t _Addr, uint8_t *_Data, uint8_t _Len){
    // Write Data to Servo, ID, Addr, Data, Len
    SerialBusDelay();
    MSGBUF_WRITE(_ID,_Addr,_Data,_Len,STS_INST_REG_WRITE);  // Inst = 0x04
    SerialBusDelay();
    return Ack(_ID);
}

void SyncWrite(uint8_t _ID[], uint8_t ID_Num, uint8_t _Addr, uint8_t *_Data, uint8_t _Len){
    // Sync Write Mode, Broadcast Mode, ID = 0xFE
    // Write Data to Servo, ID, Addr, Data, Len
    uint8_t _MsgLenSync = ;
    uint8_t _MsgBufferSync[7]; 
    uint8_t _MsgLenSync = 4;

    

    

    uint8_t _CheckSumSync = 0;

    
}
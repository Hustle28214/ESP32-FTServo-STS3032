#include <stdio.h>
#include "STS_ORDER.h"
#include "STS_INST.h"

static uint8_t Level = 1; // Servo ACK Level
static uint8_t End = 0; // 0 = LITTLE, 1 = BIG
static uint8_t servo_status;
static uint8_t servo_error; 


void ORDER_GET_END(uint8_t _End){
    // GET End SETTINGS
    End = _End;
    return End;
}

void ORDER_IS_BIG_END(uint8_t _End){
    // RETURN 1 IF BIG End, 0 IF LITTLE End
    End = _End;
    if(End == 0){
        return 0; 
    }
    return 1;
}

void ORDER_IS_LITTLE_END(uint8_t _End){
    // RETURN 1 IF LITTLE End, 0 IF BIG End
    End = _End;
    if(End == 1){
        return 0;
    }
    return 1;
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

    if(End) // BIG End, data addr : high to low
    {
        *_dataL = (uint8_t)(_digits16 >> 8); // dataL
        *_dataH = (uint8_t)(_digits16 & 0xFF); // dataH
    }
    else{
        // LITTLE End, data addr : low to high
        *_dataL = (uint8_t)(_digits16 & 0xFF); // dataL
        *_dataH = (uint8_t)(_digits16>>8); // dataH
    }
}

// 2 * 8 digits combined to 16 digits

int DIGITS8_TO_DIGITS16(uint8_t _dataL,uint8_t _dataH){
   
    uint16_t digits16;
    if(End) // BIG End, data addr : high to low
    {
        digits16 = (uint16_t)((_dataH << 8) | _dataL);
    }
    else{
        // LITTLE End, data addr : low to high
        digits16 = (uint16_t)((_dataL << 8) | _dataH);
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
    uint8_t _MsgLenSync = ((_Len+1)*ID_Num + 4);
    uint8_t _MsgBufferSync[7]; 
    uint8_t _MsgLenSync = 4;

    _MsgBufferSync[0] = 0xff;
    _MsgBufferSync[1] = 0xff;
    _MsgBufferSync[2] = 0xfe;
    _MsgBufferSync[3] = _MsgLenSync;
    _MsgBufferSync[4] = STS_INST_SYNC_WRITE;
    _MsgBufferSync[5] = _Addr;
    _MsgBufferSync[6] = _Len;

    // TODO: add SerialBusDelay()
    // TODO: add SerialWrite();
    SerialBusWrite(_MsgBufferSync, 7);
    // TODO: add SerialBusDelay()

    

    uint8_t _CheckSumSync = 0;

    _CheckSumSync = 0xf + _MsgLenSync + STS_INST_SYNC_WRITE + _Addr + _Len;

    for (int i = 0; i< ID_Num; ++i){
        SerialBusWrite(&_ID[i],1);
        SerialBusWrite(_Data+i*_Len, _Len);
        _CheckSumSync += _ID[i];
        for (int j = 0; j < _Len; j++)
        {
            _CheckSumSync += _Data[i*_Len + j];
        }
        
    }
    _CheckSumSync = ~_CheckSumSync;

    SerialBusWrite(&_CheckSumSync,1);

    SerialBusWrite();
}


void WRITE_BYTE(uint8_t ID, uint8_t _Addr,uint8_t ByteDat){
    // SerialBusDelay();
    MSGBUF_WRITE(ID,_Addr,&ByteDat,1,STS_INST_WRITE);
    // SerialBusDelay();
    return Ack(ID);

}

void WRITE_WORD(uint8_t ID,uint8_t _Addr,uint16_t WordDat){
    uint8_t _MsgBuffer[2];
    DIGITS16_TO_DIGITS8(_MsgBuffer,_MsgBuffer + 1, WordDat );
    // SerialBusDelay();
    MSGBUF_WRITE(ID,_Addr,_MsgBuffer,2,STS_INST_WRITE);
    // SerialBusDelay();
    return Ack(ID);
}

int OrdinaryRead(uint8_t ID,uint8_t _Addr, uint8_t *Dat, uint8_t _Len){
    //
    
    int ReadSize = 0;
    uint8_t _MsgBuffer[4];
    uint8_t _CheckSum = 0;
     // SerialBusDelay();
    MSGBUF_WRITE(ID, _Addr,&_Len,1,STS_INST_READ);
     // SerialBusDelay();
    servo_status = 0;
    if(!HeadCheck()){
        servo_status = STS_ERR_NO_REPLY;
        return 0;
    }
    uint8_t servo_error = 0;
    // TODO: Add SerialBusRead()
    if(SerialBusRead(_MsgBuffer,3)!=3){
        servo_error = STS_ERR_NO_REPLY;
        return 0;
    }
    if (_MsgBuffer[0]!=ID && ID!=0xfe)
    {
        // it means slave servo id is incorrect
        servo_error = STS_ERR_SLAVE_ID;
        return 0;
    }
    if (_MsgBuffer[1]!=_Len + 2)
    {
        /* code */
        servo_error = STS_ERR_BUFFER_LEN;
        return 0;
    }
    ReadSize = SerialBusRead(Dat,_Len);
    if(ReadSize!=_Len){
        servo_error = STS_ERR_NO_REPLY;
        return 0;
    }
    if(SerialBusRead(_MsgBuffer+3,1)!=1){
        servo_error = STS_ERR_NO_REPLY;
        return 0;
    }
    _CheckSum = _MsgBuffer[0]+_MsgBuffer[1]+_MsgBuffer[2];
    for(int i = 0;i<ReadSize;++i){
        _CheckSum += Dat[i];
    }

    _CheckSum =~_CheckSum;
    if (_CheckSum != _MsgBuffer[3]){
        servo_error = STS_ERR_CRC_CMP;
        return 0;
    }
    servo_status = _MsgBuffer[2];
    return ReadSize;

}

int ByteRead(uint8_t ID,uint8_t _Addr){
    uint8_t _ByteDat;
    int ByteDatSize = OrdinaryRead(ID,_Addr,&_ByteDat,1);
    if(ByteDatSize!=1){
        return -1;
    }// servo_error Check
    else{
        return _ByteDat;
    }
}

int WordRead(uint8_t ID, uint8_t _Addr){
    uint8_t Dat[2]; // 2 Byte = 1 Word
    int WordSize;
    uint16_t WordDat;
    WordSize = OrdinaryRead(ID, _Addr, Dat,2);// Size = Read(ID, MemAddr, nDat, 2);
    if(WordSize!=2){
        return -1;
    }
    WordDat = DIGITS8_TO_DIGITS16(Dat[0],Dat[1]);
    return WordDat;
}
// PING for STS3032
int PING(uint8_t ID){
    uint8_t _PingMsgBuffer[4];
    uint8_t _CheckSum;
    SerialBusDelay();
    servo_status = 0;
    if(!HeadCheck()){
        servo_error = STS_ERR_NO_REPLY;
        return -1;
    }
    servo_error = 0;
    if(SerialRead(_PingMsgBuffer, 4)!= 4){
        servo_error = STS_ERR_NO_REPLY;
        return -1;
    }
    if(_PingMsgBuffer[0]!=ID && ID != 0xfe){
        servo_error = STS_ERR_SLAVE_ID;
        return -1;
    }
    if(_PingMsgBuffer[1]!=2){

    }
}
int HeadCheck(void){
    uint8_t ByteDat;
    uint8_t ByteBuffer[2] = {0,0};
    uint8_t cnt = 0;

    while(1){
        if(!SerialRead(&ByteDat,1)){
            return 0;
        }
        ByteBuffer[1] = ByteBuffer[0];
        ByteBuffer[0] = ByteDat;
        if(ByteBuffer[0] == 0xff && ByteBuffer[1] == 0xff){
            break;
        }
        cnt++;
        if(cnt>10){
            return 0;
        }
    }
    return 1;
}

int Ack(uint8_t ID){
    uint8_t ByteBuffer[4];
    uint8_t _CheckSum = 0;
    servo_error = 0;
    if(ID!=0xfe && Level){
        // if current level is
    }

}
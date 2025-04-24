#ifndef _STS_INST_H
#define _STS_INST_H

#include <stdint.h>

#ifndef NULL
#define NULL ((void *)0)
#endif

#define STS_INST_PING 0x01
#define STS_INST_READ 0x02
#define STS_INST_WRITE 0x03
#define STS_INST_REG_WRITE 0x04
#define STS_INST_REG_ACTION 0x05
#define STS_INST_RECOVERY 0x06
#define STS_INST_SYNC_READ 0x82
#define STS_INST_SYNC_WRITE 0x83
#define STS_INST_RESET 0x0A


#endif

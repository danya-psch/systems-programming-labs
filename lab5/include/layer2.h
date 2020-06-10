#ifndef LAYER2_H_
#define LAYER2_H_

#include "layer3.h"
#include "error.h"
#include <inttypes.h>


#define STX 0x02
#define ETX 0x03

typedef struct Layer2_trans_buf_block {
    uint8_t stx;
    uint8_t count[3];
    uint8_t ack;
    int8_t seq;
    uint8_t lframe;
    uint8_t resvd[10];
    L3_tb_block l3_block;
    uint16_t chksum;
    uint8_t etx;
} L2_tb_block;


int layer2_transmit(L3_tb_block* l3_tb, size_t buf_size, size_t last_block_size, bool last_package);
int layer2_receive(L3_tb_block segments[], size_t* last_data_size, bool* last_in_file);

#endif
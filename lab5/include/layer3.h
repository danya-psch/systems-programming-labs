#ifndef LAYER3_H_
#define LAYER3_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "error.h"


#define L3_TB_SIZE 40
#define L3_MSS 60
#define L3_TB_HEADER_SIZE 40

typedef struct Layer3_trans_buf_block {
    uint8_t header[L3_TB_HEADER_SIZE];
    uint8_t data[L3_MSS];
} L3_tb_block;

int layer3_transmit(uint8_t buf[], size_t buf_size, bool last_package);
int layer3_receive(uint8_t buf[], bool* last_package);


#endif
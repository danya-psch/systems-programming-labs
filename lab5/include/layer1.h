#ifndef LAYER1_H_
#define LAYER1_H_

#include "layer2.h"
#include "error.h"


#define ACK_SUCCESS 0x06
#define ACK_ERROR 0x15

#define TIMEOUT_MS 10
#define TTR 16

int layer1_transmit(L2_tb_block* l2_tbb);
int layer1_receive(L2_tb_block* pkg);

#endif
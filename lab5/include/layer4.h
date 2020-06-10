#ifndef LAYER4_H_
#define LAYER4_H_

#include "error.h"

#define L4_TB_LEN 1024



int layer4_transmit(const char* filename);
int layer4_receive(const char* file_name);


#endif
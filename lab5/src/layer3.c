#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/layer2.h"
#include "../include/layer3.h"

int layer3_transmit(uint8_t buf[], size_t buf_size, bool last_package) {
    puts("layer3_transmit");
    L3_tb_block* l3_tb = (L3_tb_block*)calloc(L3_TB_SIZE, sizeof(L2_tb_block));
    int num_blocks = buf_size / L3_MSS;

    for (int i = 0; i < num_blocks; ++i) {
        memcpy(&l3_tb[i].data, &buf[i * L3_MSS], L3_MSS);
    }

    size_t last_block_size = buf_size - num_blocks * L3_MSS;
    if (last_block_size != 0) {
        memcpy(&l3_tb[num_blocks].data, &buf[num_blocks*L3_MSS], last_block_size);
        num_blocks++;
    }

    int res = layer2_transmit(l3_tb, num_blocks, last_block_size, last_package);
    free(l3_tb);
    return res;
}


int layer3_receive(uint8_t buf[], bool* last_package) {
    puts("layer3_receive");
    L3_tb_block* buffer = calloc(L3_TB_SIZE, sizeof(L2_tb_block));
    if (buffer == NULL) {
        return -1;
    }

    size_t last_block_size;
    size_t len = layer2_receive(buffer, &last_block_size, last_package);

    for (size_t i = 0; i < len-1; i++) {
        memcpy(&buf[i * L3_MSS], &buffer[i].data, L3_MSS);
    }
    memcpy(&buf[(len - 1) * L3_MSS], &buffer[len - 1].data, last_block_size);

    free(buffer);
    return L3_MSS * (len - 1) + last_block_size;
}
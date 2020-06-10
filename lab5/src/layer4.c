#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "../include/layer3.h"
#include "../include/layer4.h"

static bool is_eof(FILE* file) {
    char c = fgetc(file);
    if (feof(file)) {
        return true;
    } else {
        ungetc(c, file);
        return false;
    }
}

int layer4_transmit(const char* file_name) {
    puts("layer4_transmit");
    FILE* fin = fopen(file_name, "r");
    if (fin == NULL) {
        perror("fopen: ");
    }
    size_t buf_len = L3_TB_SIZE * L3_MSS;
    uint8_t buf[buf_len];
    size_t size;
    size_t sent = 0;
    while ((size = fread(buf, 1, buf_len, fin)) != 0) {
        layer3_transmit(buf, size, is_eof(fin));
        sent += 1;
    }
    return 0;
}

int layer4_receive(const char* file_name) {
   
    puts("layer4_receive");
    FILE* out = fopen(file_name, "w");

    size_t buf_len = L3_TB_SIZE * L3_MSS;
    uint8_t buf[buf_len];
    bool last_package = false;
    while (!last_package) {
        size_t len = layer3_receive(buf, &last_package);
        size_t wrote_bytes = fwrite(buf, 1, len, out);
        if (wrote_bytes == 0) {
            return -1;
        }
    }

    return 0;
}
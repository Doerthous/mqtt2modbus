#include <stdio.h>
#include <modbus.h>
#include <errno.h>

static int dev_id = 0x10;

#ifndef MQTT2MODBUS_SERIAL
    #define MQTT2MODBUS_SERIAL "/dev/serial0"
#endif

int main(void) {
    modbus_t *mb;
    uint16_t tab_reg[32];
    int rc;

    mb = modbus_new_rtu(MQTT2MODBUS_SERIAL, 115200, 'N', 8, 1);
    if (mb == NULL) {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        return -1;
    }

    modbus_set_slave(mb, dev_id);

    if (modbus_connect(mb) == -1) {
        fprintf(stderr, "Connection failed: %s\n", modbus_strerror(errno));
        modbus_free(mb);
        return -1;
    }

    rc = modbus_read_registers(mb, 0, 7, tab_reg);
    if (rc == -1) {
        fprintf(stderr, "%s\n", modbus_strerror(errno));
        return -1;
    }
     
    modbus_close(mb);
    modbus_free(mb);  
}

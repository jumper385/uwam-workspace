#ifndef _CAN_PROBE_H_
#define _CAN_PROBE_H_

#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>

// Hardcode Codes First
#define MAX_IDS 200

// ERROR STATE DEFINITIONS
#define CANPROBE_MAX_ID_REACHED 30

enum CAN_DTypes
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_UINT,
    TYPE_CHAR,
};

union CAN_Data_Union
{
    uint64_t u64;
    int i[2];
    double d;
    float f[2];
};

struct CAN_DType_struct
{
    enum CAN_DTypes type;
};

struct CANTask_rx_probe
{
    int ids[MAX_IDS];
    uint64_t data[MAX_IDS];
    struct CAN_DType_struct dtypes[MAX_IDS];
    int idx;
};

int CANTask_probe_init(struct CANTask_rx_probe *probe);
int CANTask_probe_register_id(struct CANTask_rx_probe *probe, uint32_t id, enum CAN_DTypes dtype);
int CANTask_probe_update_state(struct CANTask_rx_probe *probe, struct can_frame *frame);
float CANTask_probe_id_get_float(struct CANTask_rx_probe *probe, uint32_t id);

#endif
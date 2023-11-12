#ifndef _CAN_PROBE_H_
#define _CAN_PROBE_H_

#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>

// Hardcode Codes First
#define MAX_IDS 200

// ERROR STATE DEFINITIONS
#define CANPROBE_MAX_ID_REACHED 30

enum ValueType
{
    TYPE_INT,
    TYPE_FLOAT,
    TYPE_UINT,
    TYPE_CHAR,
};

struct RegisteredId
{
    enum ValueType type;
};

struct CANTask_rx_probe
{
    int ids[MAX_IDS];
    uint64_t data[MAX_IDS];
    struct RegisteredId dtypes[MAX_IDS];
    int idx;
};

int CANTask_probe_init(struct CANTask_rx_probe *probe);
int CANTask_probe_register_id(struct CANTask_rx_probe *probe, uint32_t id, enum ValueType dtype);
int CANTask_probe_update_state(struct CANTask_rx_probe *probe, struct can_frame *frame);

#endif
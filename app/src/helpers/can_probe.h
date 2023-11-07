#ifndef _CAN_PROBE_H_
#define _CAN_PROBE_H_

#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>

// Hardcode Codes First
#define MAX_IDS 100

// ERROR STATE DEFINITIONS
#define CANPROBE_MAX_ID_REACHED 30

struct CANTask_rx_probe
{
    int ids[MAX_IDS];
    uint64_t data[MAX_IDS];
    int idx;
};

int CANTask_probe_append_canid(struct CANTask_rx_probe *probe, struct can_frame *frame);
int CANTask_probe_update_canid(struct CANTask_rx_probe *probe, struct can_frame *frame, int idx);
int CANTask_probe_find_canid(struct CANTask_rx_probe *probe, int canid);
int CANTask_probe_update_state(struct CANTask_rx_probe *probe, struct can_frame *frame);

#endif
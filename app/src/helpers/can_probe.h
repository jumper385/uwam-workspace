#ifndef _CAN_PROBE_H_
#define _CAN_PROBE_H_

#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>

// Hardcode Codes First
#define MAX_IDS 9
#define CANPROBE_ERROR_LL_INSERT_ERROR 100
#define CANPROBE_ERROR_LL_EMPTY 101
#define CAN_PROBE_LL_ID_NOT_PRESENT 102

// ERROR STATE DEFINITIONS
#define CANPROBE_MAX_ID_REACHED 30

struct CANProbe_node
{
    void *next;
    uint16_t id;
    uint64_t data;
};

struct CANProbe_probe
{
    int cnt;
    struct CANProbe_node *canid;
    struct CANProbe_node *start;
    struct CANProbe_node *end;
};

struct CANTask_rx_probe
{
    int ids[MAX_IDS];
    uint64_t data[MAX_IDS];
    int idx;
};

int CANTask_probe_update_state(struct CANTask_rx_probe *probe, struct can_frame *frame);
int CANProbe_init(struct CANProbe_probe *canid);
int CANProbe_insert_node(struct CANProbe_probe *canid, struct can_frame *frame);

#endif
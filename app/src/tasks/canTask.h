#ifndef _CAN_TASK_H_
#define _CAN_TASK_H_

#define MAX_IDS 10

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>
#include <zephyr/crypto/crypto.h>

#include "appTask.h"
#include "../helpers/can_probe.h"



struct CANTask
{
    struct AppTask super;
    struct device *can1, *can2;
    struct can_frame tx_frame;
    struct CANTask_rx_probe can1_probe, can2_probe;
};

void CANTask_init(struct CANTask *task);
void CANTask_thread(struct CANTask *task, void *p2, void *p3);
int CANTask_emit_test_can1_tx(struct CANTask *task);
int CANTask_emit_test_can2_tx(struct CANTask *task);

#endif
#ifndef _CAN_TASK_H_
#define _CAN_TASK_H_

#include <zephyr/device.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/byteorder.h>
#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>
#include <zephyr/logging/log.h>

#include "appTask.h"

struct CANTask
{
    struct AppTask super;
    struct device *can1, *can2;
    struct can_frame tx_frame;
};

void CANTask_init(struct CANTask *task);
void CANTask_thread(struct CANTask *task, void *p2, void *p3);
int CANTask_emit_test_can1_tx(struct CANTask *task);

#endif
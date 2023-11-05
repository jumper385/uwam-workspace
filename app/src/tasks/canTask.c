#include "canTask.h"

LOG_MODULE_REGISTER(can);

void CANTask_init(struct CANTask *task)
{
    AppTask_init((struct AppTask *)task);

    task->can1 = DEVICE_DT_GET(DT_NODELABEL(can1));
    task->can2 = DEVICE_DT_GET(DT_NODELABEL(can2));

    int ret = can_set_mode(task->can1, CAN_MODE_NORMAL);
    if (ret < 0)
    {
        LOG_ERR("can_set_mode() failed for can1: %d", ret);
        return;
    }

    ret = can_start(task->can1);
    if (ret != 0)
    {
        LOG_ERR("can_start failed for can1: %d");
        return;
    }
    else
    {
        LOG_INF("CAN Bus 1 Started...");
    }

    ret = can_set_mode(task->can2, CAN_MODE_NORMAL);
    if (ret < 0)
    {
        LOG_ERR("can_set_mode() failed for can2: %d", ret);
        return;
    }

    ret = can_start(task->can2);
    if (ret != 0)
    {
        LOG_ERR("can_start failed for can2: %d");
        return;
    }
    else
    {
        LOG_INF("CAN Bus 2 Started...");
    }
}

/**
 * EXTERNAL ACCESS FUNCTIONS
 * These set of functions allow for external control to emit different
 * events for the thread to operate on.
 */
int CANTask_emit_test_can1_tx(struct CANTask *task)
{
    int err = k_event_post(&task->super.events, 0b1U);
    return err;
}

int CANTask_emit_test_can2_tx(struct CANTask *task)
{
    int err = k_event_post(&task->super.events, 0b10U);
    return err;
}

/**
 * OPERATOR FUNCTIONS
 * These events act on emitted events form external access functions to
 * then effect changes to the peripheral.
 */
int CANTask_operate_test_can1_tx(struct CANTask *task)
{
    struct can_frame frame =
        {
            .data = {0x1a, 0x2b},
            .id = 0xdd,
            .dlc = 2,
        };

    LOG_INF("Sending CAN1 TX");

    int ret = can_send(task->can1, &frame, K_NO_WAIT, NULL, NULL);
    return ret;
}

int CANTask_operate_test_can2_tx(struct CANTask *task)
{
    struct can_frame frame =
        {
            .data = {0x3f, 0x8e},
            .id = 0xde,
            .dlc = 2,
        };

    LOG_INF("Sending CAN2 TX");

    int ret = can_send(task->can2, &frame, K_NO_WAIT, NULL, NULL);
    return ret;
}

// Thread Definition

void CANTask_thread(struct CANTask *task, void *p2, void *p3)
{
    struct AppTask *appTask = (struct AppTask *)task;
    int event = k_event_wait(&appTask->events, 0b111U, false, K_NO_WAIT);

    LOG_INF("CAN Thread Ready...");

    int ret;

    LOG_INF("CAN Thread Loop Started...");

    for (;;)
    {

        int event = k_event_wait(&appTask->events, 0b111U, false, K_NO_WAIT);

        if ((0b1U == (0b1u & event)) != 0U)
        {
            ret = CANTask_operate_test_can1_tx(task);
            k_event_clear(&appTask->events, 0b1U);
        }

        if ((0b10U == (0b10u & event)) != 0U)
        {
            ret = CANTask_operate_test_can2_tx(task);
            k_event_clear(&appTask->events, 0b10U);
        }

        k_yield();
    }
}
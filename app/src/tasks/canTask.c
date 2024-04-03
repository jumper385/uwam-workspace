#include <zephyr/random/rand32.h>
#include <zephyr/shell/shell.h>

#include "canTask.h"

LOG_MODULE_REGISTER(can);
CAN_MSGQ_DEFINE(can1q, 2);
CAN_MSGQ_DEFINE(can2q, 2);

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
    uint16_t tx_length = sys_rand32_get() >> 29;
    uint32_t id = sys_rand32_get() >> 28;

    struct can_frame frame =
    {
        .id = id,
        .dlc = tx_length,
    };

    for (int i = 0; i < tx_length; i++)
    {
        frame.data[i] = sys_rand32_get();
    }

    int ret = can_send(task->can1, &frame, K_NO_WAIT, NULL, NULL);
    return ret;
}

int CANTask_operate_test_can2_tx(struct CANTask *task)
{
    uint16_t tx_length = sys_rand32_get() >> 29;
    uint32_t id = sys_rand32_get() >> 28;

    struct can_frame frame =
    {
        .id = id,
        .dlc = tx_length,
    };

    for (int i = 0; i < tx_length; i++)
    {
        frame.data[i] = sys_rand32_get();
    }

    int ret = can_send(task->can2, &frame, K_NO_WAIT, NULL, NULL);
    return ret;
}

// Thread Definition
void CANTask_thread(struct CANTask *task, void *p2, void *p3)
{

    struct AppTask *appTask = (struct AppTask *)task;

    const struct can_filter filter = {
        .flags = CAN_FILTER_DISABLE,
    };

    struct can_frame frame_can1, frame_can2;

    can_add_rx_filter_msgq(task->can2, &can2q, &filter);
    can_add_rx_filter_msgq(task->can1, &can1q, &filter);

    task->ch1_rx = NULL;
    task->ch2_rx = NULL;

    ll_obj_init(task->ch1_rx);
    ll_obj_init(task->ch2_rx);

    for (;;)
    {
        int ret;
        ret = k_msgq_get(&can2q, &frame_can2, K_NO_WAIT);

        if (ret == 0)
        {
            task->ch2_rx = bst_node_insert(task->ch2_rx, frame_can2.id, *frame_can2.data);
            bst_node_search(task->ch2_rx, frame_can2.id);
            struct bst_node *ch2_search = bst_node_search(task->ch2_rx, frame_can2.id);
            LOG_INF("ID STATE: %d", ch2_search->key);
            ll_print(ch2_search->data->head);
        }

        ret = k_msgq_get(&can1q, &frame_can1, K_NO_WAIT);

        if (ret == 0)
        {
            task->ch1_rx = bst_node_insert(task->ch1_rx, frame_can1.id, *frame_can1.data);
            bst_node_search(task->ch1_rx, frame_can1.id);
            struct bst_node *ch1_search = bst_node_search(task->ch1_rx, frame_can1.id);
            LOG_INF("ID STATE: %d", ch1_search->key);
            ll_print(ch1_search->data->head);
        }

        int event = k_event_wait(&appTask->events, 0b111111U, false, K_NO_WAIT);

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

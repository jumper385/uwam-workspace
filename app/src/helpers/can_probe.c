#include "can_probe.h"

/**
 * Appends a CAN ID to the probe's list of IDs if there is space.
 * It also stores the corresponding data from the CAN frame.
 *
 * @param probe Pointer to the CANTask_rx_probe structure.
 * @param frame Pointer to the CAN frame containing the ID and data to append.
 * @return int Returns 0 on success, or -CANPROBE_MAX_ID_REACHED if the probe's ID list is full.
 */
int CANTask_probe_append_canid(struct CANTask_rx_probe *probe, struct can_frame *frame)
{
    if (probe->idx < MAX_IDS)
    {
        probe->ids[probe->idx] = frame->id;
        memcpy(&probe->data[probe->idx], &frame->data, sizeof(uint64_t));
        probe->idx++;
    }
    else
    {
        return -CANPROBE_MAX_ID_REACHED;
    }

    return 0;
}

/**
 * Updates the CAN ID's data in the probe at the specified index.
 *
 * @param probe Pointer to the CANTask_rx_probe structure.
 * @param frame Pointer to the CAN frame containing the new data.
 * @param idx Index at which the data should be updated.
 * @return int Always returns 0.
 */
int CANTask_probe_update_canid(struct CANTask_rx_probe *probe, struct can_frame *frame, int idx)
{
    memcpy(&probe->data[idx], &frame->data, sizeof(uint64_t));
    return 0;
}

/**
 * Searches for the given CAN ID within the probe's list of IDs.
 *
 * @param probe Pointer to the CANTask_rx_probe structure.
 * @param canid The CAN ID to find.
 * @return int Index of the found CAN ID; -1 if not found.
 */
int CANTask_probe_find_canid(struct CANTask_rx_probe *probe, int canid)
{
    /**
     * Checks if the ID array has an ID match
     * if canid >= 0, ID was found. If canid < 0; ID wasnt found
     */
    int id = -1;
    for (int i = 0; i < MAX_IDS; i++)
    {
        if (probe->ids[i] == canid)
        {
            id = i;
            break;
        }
    }
    return id;
}

/**
 * Updates the state of the probe with the data from the given CAN frame.
 * If the CAN ID is new, it appends it to the probe; if it exists, updates the data.
 *
 * @param probe Pointer to the CANTask_rx_probe structure.
 * @param frame Pointer to the CAN frame containing the ID and data.
 * @return int Returns 0 on success, or -CANPROBE_MAX_ID_REACHED if a new ID cannot be appended.
 */
int CANTask_probe_update_state(struct CANTask_rx_probe *probe, struct can_frame *frame)
{

    int id = CANTask_probe_find_canid(probe, frame->id);

    if (id < 0)
    {
        int err = CANTask_probe_append_canid(probe, frame);
        if (err < 0)
        {
            return err;
        }
    }
    else
    {
        CANTask_probe_update_canid(probe, frame, id);
        return 0;
    }
}

int CANProbe_init(struct CANProbe_probe *canid)
{
    canid->canid = NULL;
    canid->start = NULL;
    canid->end = NULL;
    canid->cnt = 0;
    return 0;
}

int CANProbe_node_id_exists(struct CANProbe_probe *canid, uint16_t id)
{
    if (canid->start == NULL)
    {
        return -CANPROBE_ERROR_LL_EMPTY; // List is empty
    }

    struct CANProbe_node *current = canid->start;
    for (int i = 0; i < canid->cnt; i++)
    {
        if (current->id == id)
        {
            return 0; // ID found
        }
        printk("ID[%d]: %d\n", i, current->id);
        current = current->next;
    }

    return -CAN_PROBE_LL_ID_NOT_PRESENT; // ID not found
}

// add node
int CANProbe_insert_node(struct CANProbe_probe *canid, struct can_frame *frame)
{
    int ret = CANProbe_node_id_exists(canid, frame->id);

    // check if head is null...
    struct CANProbe_node *new;
    // if list is empty
    if (canid->start == NULL)
    {
        // create the new node
        new = k_malloc(sizeof(struct CANProbe_node));
        if (new == NULL)
        {
            return -CANPROBE_ERROR_LL_INSERT_ERROR;
        }
        memcpy(&new->data, &frame->data, sizeof(uint64_t));
        new->id = frame->id;

        new->next = NULL;
        canid->start = new;
        canid->end = new;
    }
    else
    {
        new = k_malloc(sizeof(struct CANProbe_node));
        if (new == NULL)
        {
            return -CANPROBE_ERROR_LL_INSERT_ERROR;
        }
        memcpy(&new->data, &frame->data, sizeof(uint64_t));
        new->id = frame->id;

        new->next = canid->start;
        canid->start = new;
    }

    canid->cnt++;

    return 0;
}

//  find node with id
//  remove node
//  insert node into arb. position in list
//  remove node in arb. position in list
//  pop last node
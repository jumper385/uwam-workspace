#include "can_probe.h"

int CANTask_probe_init(struct CANTask_rx_probe *probe)
{
    for (int i = 0; i < MAX_IDS; i++)
    {
        probe->dtypes->type = TYPE_UINT;
    }
}

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

int CANTask_probe_register_id(struct CANTask_rx_probe *probe, uint32_t id, enum CAN_DTypes dtype)
{
    probe->dtypes[id].type = dtype;
}

float CANTask_probe_id_get_float(struct CANTask_rx_probe *probe, uint32_t id)
{
    union CAN_Data_Union buff;
    buff.u64 = probe->data[id];
    return buff.d;
}

uint64_t CANTask_probe_id_get_u64(struct CANTask_rx_probe *probe, uint32_t id)
{
    union CAN_Data_Union buff;
    buff.u64 = probe->data[id];
    return buff.u64;
}
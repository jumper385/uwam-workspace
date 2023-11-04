#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>
#include <zephyr/device.h>

#include <zephyr/kernel.h>
#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>

#include "./tasks/canTask.h"

LOG_MODULE_REGISTER(main);

// Task Definitions
struct CANTask canTask;

// Thread Memory Instantiations
K_THREAD_STACK_DEFINE(can_task_data, 2048);

#define LED0 DT_NODELABEL(DT_ALIAS(led0))
struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int main(void)
{

	// Initialize Threads
	CANTask_init(&canTask);

	// Start Thread Operation
	canTask.super.tid = k_thread_create(
	    &canTask.super.thread,
	    can_task_data, K_THREAD_STACK_SIZEOF(can_task_data),
	    CANTask_thread, &canTask, NULL, NULL, 2, 0, K_NO_WAIT);

	// Debug Blinky...

	while (1)
	{
		CANTask_emit_test_can1_tx(&canTask);
		gpio_pin_toggle_dt(&led0);
		k_msleep(50);
	}
	return 0;
}

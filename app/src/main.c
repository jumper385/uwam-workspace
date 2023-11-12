#include <zephyr/kernel.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <string.h>

#include "./tasks/canTask.h"
#include "./helpers/can_probe.h"

LOG_MODULE_REGISTER(main);

// Setup Blinky...
struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);
struct gpio_dt_spec rgb_red = GPIO_DT_SPEC_GET(DT_NODELABEL(rgb_red), gpios);
struct gpio_dt_spec rgb_green = GPIO_DT_SPEC_GET(DT_NODELABEL(rgb_green), gpios);
struct gpio_dt_spec rgb_blue = GPIO_DT_SPEC_GET(DT_NODELABEL(rgb_blue), gpios);

// Task Definitions
struct CANTask canTask;

// Thread Memory Instantiations
K_THREAD_STACK_DEFINE(can_task_data, 2048);

// Setup Timers
struct k_timer test_can_timer;
struct k_timer red_led_timer;
struct k_timer green_led_timer;
struct k_timer blue_led_timer;

void shellcmd_cantask_idcnt(const struct shell *shell, size_t argc, char *argv[])
{
	char dest[40] = "";
	strcpy(dest, argv[1]);
	if (strcmp(dest, "can1") == 0)
	{
		shell_print(shell, "%s - %d Different ID's", dest, canTask.can1_probe.idx);
	}
	else if (strcmp(dest, "can2") == 0)
	{
		shell_print(shell, "%s - %d Different ID's", dest, canTask.can2_probe.idx);
	}
	else
	{
		LOG_ERR("Select correct channel - either can1 or can2");
	}
}

void shellcmd_cantask_listids(const struct shell *shell, size_t argc, char *argv[])
{
	char dest[40] = "";
	strcpy(dest, argv[1]);
	if (strcmp(dest, "can1") == 0)
	{
		shell_print(shell, "CAN1 COLLECTED %d IDs", canTask.can1_probe.idx);
		for (int i = 0; i < canTask.can1_probe.idx; i++)
		{
			shell_print(shell, "ID: %x - %.2f", canTask.can1_probe.ids[i], CANTask_probe_id_get_float(&canTask.can1_probe, i));
		}
	}
	else if (strcmp(dest, "can2") == 0)
	{
		shell_print(shell, "CAN2 COLLECTED %d IDs", canTask.can2_probe.idx);
		for (int i = 0; i < canTask.can2_probe.idx; i++)
		{
			shell_print(shell, "ID: %x - %x", canTask.can2_probe.ids[i], canTask.can2_probe.data[i]);
		}
	}
	else
	{
		LOG_ERR("Select correct channel - either can1 or can2");
	}
}

SHELL_STATIC_SUBCMD_SET_CREATE(candbg,
							   SHELL_CMD_ARG(idcnt, NULL,
											 "Show the number of unique ID's detected by the device",
											 shellcmd_cantask_idcnt, 2, NULL),
							   SHELL_CMD_ARG(listids, NULL,
											 "Show a full list of ID's for the specified can channel",
											 shellcmd_cantask_listids, 2, NULL),
							   SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(candbg, &candbg, "CAN Debugger", NULL);

void test_can_expiry_fn(struct k_timer *timer)
{
	CANTask_emit_test_can1_tx(&canTask);
}

void can2_expiry_fn(struct k_timer *timer)
{
	CANTask_emit_test_can2_tx(&canTask);
}

void red_led_expiry_fn(struct k_timer *timer)
{
	gpio_pin_toggle_dt(&rgb_red);
}

void green_led_expiry_fn(struct k_timer *timer)
{
	gpio_pin_toggle_dt(&rgb_green);
}

void blue_led_expiry_fn(struct k_timer *timer)
{
	gpio_pin_toggle_dt(&rgb_blue);
}

K_TIMER_DEFINE(test_can_timer, test_can_expiry_fn, NULL);
K_TIMER_DEFINE(can2_tx_timer, can2_expiry_fn, NULL);
K_TIMER_DEFINE(red_led_timer, red_led_expiry_fn, NULL);
K_TIMER_DEFINE(green_led_timer, green_led_expiry_fn, NULL);
K_TIMER_DEFINE(blue_led_timer, blue_led_expiry_fn, NULL);

int main(void)
{

	// Initialize Threads
	CANTask_init(&canTask);

	// Start Thread Operation
	canTask.super.tid = k_thread_create(
		&canTask.super.thread,
		can_task_data, K_THREAD_STACK_SIZEOF(can_task_data),
		CANTask_thread, &canTask, NULL, NULL, 100, 0, K_NO_WAIT);

	// Setup Timers
	k_timer_start(&test_can_timer, K_NO_WAIT, K_MSEC(100));
	k_timer_start(&can2_tx_timer, K_NO_WAIT, K_MSEC(100));
	k_timer_start(&red_led_timer, K_NO_WAIT, K_MSEC(325));
	k_timer_start(&green_led_timer, K_NO_WAIT, K_MSEC(350));
	k_timer_start(&blue_led_timer, K_NO_WAIT, K_MSEC(300));

	// Do the Blinky...
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&rgb_red, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&rgb_green, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&rgb_blue, GPIO_OUTPUT_ACTIVE);

	while (1)
	{
		gpio_pin_toggle_dt(&led0);
		k_msleep(50);
	}
	return 0;
}
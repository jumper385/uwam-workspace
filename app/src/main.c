#include <zephyr/kernel.h>
#include "./tasks/canTask.h"

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

CAN_MSGQ_DEFINE(can1q, 2);
CAN_MSGQ_DEFINE(can2q, 2);

void test_can_expiry_fn(struct k_timer *timer)
{
	CANTask_emit_test_can1_tx(&canTask);
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
	k_timer_start(&test_can_timer, K_NO_WAIT, K_MSEC(1000));
	k_timer_start(&red_led_timer, K_NO_WAIT, K_MSEC(325));
	k_timer_start(&green_led_timer, K_NO_WAIT, K_MSEC(350));
	k_timer_start(&blue_led_timer, K_NO_WAIT, K_MSEC(300));

	// Do the Blinky...
	gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&rgb_red, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&rgb_green, GPIO_OUTPUT_ACTIVE);
	gpio_pin_configure_dt(&rgb_blue, GPIO_OUTPUT_ACTIVE);

	const struct can_filter filter = {
		.flags = CAN_FILTER_DATA | CAN_FILTER_IDE,
		.id = 0xdd,
	};

	struct can_frame frame;
	int filter_id = can_add_rx_filter_msgq(canTask.can2, &can2q, &filter);

	while (1)
	{

		int ret = k_msgq_get(&can2q, &frame, K_NO_WAIT);

		LOG_INF("%d", frame.dlc);

		gpio_pin_toggle_dt(&led0);
		k_msleep(50);
	}
	return 0;
}
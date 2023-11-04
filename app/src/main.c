#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

#include <zephyr/usb/usb_device.h>
#include <zephyr/usb/usbd.h>

#include <zephyr/kernel.h>
#include <zephyr/net/canbus.h>
#include <zephyr/drivers/can.h>
#include <zephyr/random/rand32.h>

LOG_MODULE_REGISTER(main);

const struct device *const can_dev = DEVICE_DT_GET(DT_CHOSEN(zephyr_canbus));

#define LED0 DT_NODELABEL(DT_ALIAS(led0))
struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int a, b;

void cmd_adder_seta_handler(const struct shell *shell, size_t argc, char *argv[])
{
	long dout = strtol(argv[1], NULL, NULL);
	a = dout;
	LOG_INF("A Data: %ld", dout);
}

void cmd_adder_setb_handler(const struct shell *shell, size_t argc, char *argv[])
{
	long dout = strtol(argv[1], NULL, NULL);
	b = dout;
	LOG_INF("B Data: %ld", dout);
}

void cmd_adder_get_sum_handler(const struct shell *shell, size_t argc, char *argv[])
{
	LOG_INF("SUM: %ld", a + b);
}

SHELL_STATIC_SUBCMD_SET_CREATE(adder,
							   SHELL_CMD_ARG(seta, NULL,
											 "Set the first argument of the addition",
											 cmd_adder_seta_handler, 2, NULL),
							   SHELL_CMD_ARG(setb, NULL,
											 "Set the second argument of the addition",
											 cmd_adder_setb_handler, 2, NULL),
							   SHELL_CMD(sum, NULL,
										 "Get the sum of a and b",
										 cmd_adder_get_sum_handler),
							   SHELL_SUBCMD_SET_END);

SHELL_CMD_REGISTER(funtions, &adder, "Fun-Tions", NULL);

int main(void)
{
	int ret;

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);
	ret = can_set_mode(can_dev, CAN_MODE_LOOPBACK);

	if (ret != 0) {
		LOG_ERR("can_set_mode failed %d");
		return;
	}

	ret = can_start(can_dev);
	if (ret != 0) {
		LOG_ERR("can_start failed %d");
		return;
	}

	while (1)
	{
		struct can_frame frame = {
			.data = {0xfc, 0x1c}, 
			.id = 10, 
			.dlc = 3
		};

		can_send(can_dev, &frame, K_NO_WAIT, NULL, NULL);
		gpio_pin_toggle_dt(&led0);
		k_msleep(100);
	}
	return 0;
}

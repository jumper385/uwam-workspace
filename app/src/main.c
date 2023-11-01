#include <zephyr/kernel.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <stdlib.h>

LOG_MODULE_REGISTER(main);

#define LED0 DT_NODELABEL(DT_ALIAS(led0))
struct gpio_dt_spec led0 = GPIO_DT_SPEC_GET(DT_ALIAS(led0), gpios);

int a, b;

void cmd_adder_seta_handler(const struct shell *shell, size_t argc, char *argv[]) {
	long dout = strtol(argv[1], NULL, NULL);
	a = dout;
	LOG_INF("A Data: %ld", dout);
}

void cmd_adder_setb_handler(const struct shell *shell, size_t argc, char *argv[]) {
	long dout = strtol(argv[1], NULL, NULL);
	b = dout;
	LOG_INF("B Data: %ld", dout);
}

void cmd_adder_get_sum_handler(const struct shell *shell, size_t argc, char *argv[]) {
	LOG_INF("SUM: %ld", a + b);
}

SHELL_STATIC_SUBCMD_SET_CREATE( adder,
	SHELL_CMD_ARG(seta, NULL, 
		"Set the first argument of the addition",
		cmd_adder_seta_handler, 2, NULL),
	SHELL_CMD_ARG(setb, NULL, 
		"Set the second argument of the addition",
		cmd_adder_setb_handler, 2, NULL),
	SHELL_CMD(sum, NULL, 
		"Get the sum of a and b",
		cmd_adder_get_sum_handler),
	SHELL_SUBCMD_SET_END
);

SHELL_CMD_REGISTER(funtions, &adder, "Fun-Tions", NULL);

int main(void)
{
	int ret;

	ret = gpio_pin_configure_dt(&led0, GPIO_OUTPUT_ACTIVE);

	while (1)
	{
		gpio_pin_toggle_dt(&led0);
		k_msleep(1000);
	}
	return 0;
}

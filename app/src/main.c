#include <zephyr/kernel.h>

int main(void)
{

	while (1)
	{
		printk("Hello world\n");
		k_msleep(100);	
	}
	return 0;
}

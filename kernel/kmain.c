#include "kernel.h"

void	kmain(void)
{
	print_erro("this is error\n");
	print_warn("this is warnning\n");
	print_info("this is info\n");
	put_nbr(99);
	put_nbr(-99);
	while (1)
		;
}

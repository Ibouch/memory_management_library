#include <memory_management.h>

int	main(void)
{
	void	*n[100];
	void	*p[100];
	void	*x[100];

	for (int i = 0; i < 100; i++)
	{
		n[i] = malloc(64);
		p[i] = malloc(1024);
		x[i] = malloc(2048);
	}
	for (int i = 0; i < 100; i++)
	{
		free(n[i]);
		reallocf(n[i], 40);
		free(p[i]);
		free(x[i]);
	}
	show_alloc_mem();
	return (0);
}

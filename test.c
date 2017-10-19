#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>

int	main(int ac, char **av)
{
	int	i = (-1);
	int	nb = atoi(av[2]);
	char	*str = NULL;

	str = (char *)mmap(0, nb + 1, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	while (++i < nb)
		str[i] = av[1][0];
	str[i] = '\0';
	printf("%s\n", str);
	return (0);
}


#include <memory_management.h>

static void	*main_realloc_func(void *ptr, size_t size, bool f)
{
	uint8_t		id;
	t_area		*a;
	void		*new;

	if (ptr == NULL)
		return (malloc(size));
	if (size == 0)
	{
		free(ptr);
		return (NULL);
	}
	mutex_action(INIT_MUTEX);
	//pthread_mutex_lock(&(g_ptmu));
	id = 0;
	while (id < N_AREA)
	{
		a = g_data.areas[id];
		while (a != NULL)
		{
			if ((new = parse_area_bis(ptr, size, a, f)))
				return (new);
			a = a->next;
		}
		++id;
	}
	return (NULL);
}

void	*realloc(void *ptr, size_t size)
{
	return (return_pointer(main_realloc_func(ptr, size, false)));
}

void	*reallocf(void *ptr, size_t size)
{
	return (return_pointer(main_realloc_func(ptr, size, true)));
}

#include <memory_management.h>

static void	init_(void)
{
	mutex_action(INIT_MUTEX);
	if (!(g_data.initialized))
	{
		ft_putstr("On init");
		g_data.initialized = true;
		g_data.err_abort = ((getenv("MallocErrorAbort")) ? true : false);
		P_SIZE = getpagesize();
		getrlimit(RLIMIT_AS, &(g_data.rlm));
		g_data.alloc_max[TINY] = 64;
		g_data.alloc_max[SMALL] = 1024;
	}
}

static void	get_id(size_t size)
{
	if (size > 1 && size <= g_data.alloc_max[TINY])
		g_data.id = TINY;
	else if (size > g_data.alloc_max[TINY] && size <= g_data.alloc_max[SMALL])
		g_data.id = SMALL;
	else if (size)
		g_data.id = LARGE;
}

static void	*find_available_free_block(t_area *a, size_t size)
{
	size_t	i;
	size_t	segment_size;

	i = 0;
	while (i < a->memory_allocated)
	{
		segment_size = MDATA_BLOCK_SIZE;
		if (MDATA_BLOCK_FREE == true && size <= segment_size)
		{
			if (NEW_BLOCK < segment_size)
			{
				((t_header *)((size_t)a->ptr +
					(OVER_MDATA + size)))->size = segment_size - NEW_BLOCK;
				((t_header *)((size_t)a->ptr
					+ (OVER_MDATA + size)))->is_free = true;
				MDATA_BLOCK_SIZE = size;
				MDATA_BLOCK_FREE = false;
			}
			else
				MDATA_BLOCK_FREE = false;
			return ((void *)((size_t)a->ptr + OVER_MDATA));
		}
		i += BLOCK_SIZE;
	}
	return (NULL);
}

void		*malloc(size_t size)
{
	t_area	*area;
	void	*block;

	if (!size)
		return (NULL);
	init_();
	get_id(size);
//	pthread_mutex_lock(&(g_ptmu));
	if (g_data.areas[g_data.id] == NULL)
		return (add_new_area(&(g_data.areas[g_data.id]), size));
	area = g_data.areas[g_data.id];
	while (area)
	{
		if ((block = find_available_free_block(area, size)) != NULL)
			return (return_pointer(block));
		else if ((area->memory_allocated + NEW_BLOCK) <= area->memory_available)
			return (get_available_block(area, size));
		else if (area->next == NULL)
			return (add_new_area(&(area->next), size));
		area = area->next;
	}
	return (return_pointer(NULL));
}

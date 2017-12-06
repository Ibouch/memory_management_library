#include <ft_printf.h>
#include <memory_management.h>
#include <sys/mman.h>

# define META_DATA (sizeof(struct s_header))
# define BLOCK_SIZE (META_DATA + segment_size)
# define NEW_BLOCK (META_DATA + size)
# define OVER_MDATA (i + META_DATA)
# define OVER_BLOCK (i + BLOCK_SIZE)

void	*return_pointer(void *ptr)
{
	pthread_mutex_unlock(&(g_ptmu));
	//pthread_mutex_destroy(&(g_ptmu));
	ft_printf(" | returnded : %p\n", ptr);
	//show_alloc_mem();
	return (ptr);
}

void	*get_available_block(t_area *area, size_t size)
{
	((struct s_header *)((size_t)area->ptr + area->memory_allocated))->size = size;
	((struct s_header *)((size_t)area->ptr + area->memory_allocated))->is_free = false;
	area->memory_allocated += NEW_BLOCK;
	ft_printf(" | allocated : %zu", area->memory_allocated);
	return (return_pointer((void *)((size_t)area->ptr + (area->memory_allocated - size))));
}

void	*add_new_area(t_area **area, size_t size)
{
	g_data.pagesize = getpagesize();
	g_data.areas_size[TINY] = (g_data.alloc_max[TINY] + META_DATA) * 100;
	g_data.areas_size[SMALL] = (g_data.alloc_max[SMALL] + META_DATA) * 100;
	g_data.areas_size[LARGE] = NEW_BLOCK;

	g_data.areas_size[g_data.id] = (size_t)(((int)(g_data.areas_size[g_data.id] + sizeof(t_area)) + g_data.pagesize - 1) / g_data.pagesize * g_data.pagesize);
	
	if ((g_data.areas_size[g_data.id] + g_data.total_allocated >= g_data.rlm.rlim_cur
	|| (size + g_data.total_allocated) >= g_data.rlm.rlim_cur)
	|| (g_data.areas_size[g_data.id] < NEW_BLOCK || g_data.areas_size[g_data.id] < size))
		return (return_pointer(NULL));
	
	ft_printf("\n______NEW ZONE_____ : %zu | total_allocated : %llu vs %llu\n", g_data.areas_size[g_data.id], g_data.total_allocated, g_data.rlm.rlim_cur);
	*area = (t_area *)mmap(0, g_data.areas_size[g_data.id], PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (*area == MAP_FAILED)
		return (return_pointer(NULL));

	(*area)->memory_available = g_data.areas_size[g_data.id] - sizeof(t_area);
	(*area)->memory_allocated = 0;
	(*area)->ptr = (void *)((size_t)(*area) + sizeof(t_area));
	(*area)->next = NULL;
	ft_printf("\nEND OF STRUCT AREA : %p AND PTR : %p\n", &((*area)->next), &((*area)->ptr));
	g_data.total_allocated += g_data.areas_size[g_data.id];
	return (get_available_block((*area), size));
}

static void	show_area_mem(t_area *addr, const char area[6], size_t *total)
{
	size_t	i;
	size_t	segment_size;
	bool	is_free;

	if (addr && addr->ptr != NULL)
		ft_printf("\n%s : %p\n", area, addr->ptr);
	while (addr && addr->ptr != NULL)
	{
		ft_printf("\n---------------------- ZONE ----------------------\n- Total : %zu octets\n- Allocated : %zu octets\n- Available : %zu octets\n\n", addr->memory_available, addr->memory_allocated, addr->memory_available - addr->memory_allocated);
		i = 0;
		while (i < addr->memory_allocated)
		{
			segment_size = ((struct s_header *)((size_t)addr->ptr + i))->size;
			is_free = ((struct s_header *)((size_t)addr->ptr + i))->is_free;
			ft_printf("%p - %p : %zu octets free : %u\n", ((size_t)addr->ptr + OVER_MDATA), ((size_t)addr->ptr + OVER_BLOCK), segment_size, is_free);
			i += BLOCK_SIZE;
			*total += segment_size;
		}
		addr = addr->next;
	}
}

void	show_alloc_mem(void)
{
	size_t	total;

	total = 0;
	ft_printf("______________________________________________ SHOW ALLOC MEM ______________________________________________\n");
	show_area_mem(g_data.areas[TINY], "TINY", &total);
	show_area_mem(g_data.areas[SMALL], "SMALL", &total);
	show_area_mem(g_data.areas[LARGE], "LARGE", &total);
	ft_printf("Total : %zu octets\n", total);
}


static void	*find_available_free_block(t_area *a, size_t size)
{
	size_t	i;
	size_t	segment_size;

	i = 0;
	while (i < a->memory_allocated) // catch free space
	{
		ft_printf("\nbegin addr size : %p value : %zu\n", &(((struct s_header *)((size_t)a->ptr + i))->size), (((struct s_header *)((size_t)a->ptr + i))->size));
		segment_size = ((struct s_header *)((size_t)a->ptr + i))->size;
		if (((struct s_header *)((size_t)a->ptr + i))->is_free == true && size <= segment_size)
		{
			if (NEW_BLOCK < segment_size)
			{
				((struct s_header *)((size_t)a->ptr + (OVER_MDATA + size)))->size = segment_size - NEW_BLOCK;
				((struct s_header *)((size_t)a->ptr + (OVER_MDATA + size)))->is_free = true;
				((struct s_header *)((size_t)a->ptr + i))->size = size;
				((struct s_header *)((size_t)a->ptr + i))->is_free = false;
			}
			else
				((struct s_header *)((size_t)a->ptr + i))->is_free = false;
			return ((void *)((size_t)a->ptr + OVER_MDATA));
		}
		i += BLOCK_SIZE;
	}
	return (NULL);
}

void	*malloc(size_t size)
{
	t_area	*area;
	void	*block;

	ft_printf("Malloc : %zu", size);
	getrlimit(RLIMIT_AS, &(g_data.rlm));
	g_data.alloc_max[TINY] = 64;
	g_data.alloc_max[SMALL] = 1024;
	if (size > 1 && size <= g_data.alloc_max[TINY])
		g_data.id = TINY;
	else if (size > g_data.alloc_max[TINY] && size <= g_data.alloc_max[SMALL])
		g_data.id = SMALL;
	else if (size)
		g_data.id = LARGE;
	else
		return (NULL);
	pthread_mutex_lock(&(g_ptmu));
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

void	*merge_data_blocks(void	**start, void *to_merge)
{
	size_t	segment_size;

	segment_size = ((struct s_header *)(to_merge))->size;
	((struct s_header *)(*start))->size += BLOCK_SIZE;
	return (*(start));
}

void	free(void *ptr)
{
	size_t		i;
	size_t		segment_size;
	u_int8_t	id;
	bool		free_blocks;
	void		*previous;
	void		*block;
	t_area		*a;
	t_area		*tmp;

	pthread_mutex_lock(&(g_ptmu));
	ft_printf("Free: %p\n", ptr);
	id = 0;
	while (id < N_AREA)
	{
		tmp = NULL;
		a = g_data.areas[id];
		while (a != NULL)
		{
			i = 0;
			free_blocks = true;
			while (i < a->memory_allocated)
			{
				previous = ((i > 0) ? (void *)((size_t)a->ptr + (i - BLOCK_SIZE)) : NULL);
				segment_size = ((struct s_header *)((size_t)a->ptr + i))->size;
				if ((size_t)ptr >= ((size_t)a->ptr + OVER_MDATA) && (size_t)ptr <= ((size_t)a->ptr + OVER_BLOCK))
				{
					((struct s_header *)((size_t)a->ptr + i))->is_free = true;
					block = (void *)((size_t)a->ptr + i);
					if (previous != NULL && ((struct s_header *)(previous))->is_free == true)
						block = merge_data_blocks(&(previous), (void *)((size_t)a->ptr + i));
					if (OVER_BLOCK < a->memory_allocated && ((struct s_header *)((size_t)a->ptr + OVER_BLOCK))->is_free == true)
						merge_data_blocks(&(block), (void *)((size_t)a->ptr + OVER_BLOCK));
				}
				free_blocks = (((struct s_header *)((size_t)a->ptr + i))->is_free == false) ? false : free_blocks;
				i += BLOCK_SIZE;
			}
			if ((free_blocks == true && (id != LARGE
			&& (a->memory_allocated + META_DATA + g_data.alloc_max[id]) > a->memory_available))
			|| (free_blocks == true && id == LARGE))
			{
				if (tmp != NULL)
					tmp->next = a->next;
				else
					g_data.areas[id] = a->next;
				ft_printf("JEEEE MUNNMAP\n");
				munmap(a, a->memory_available + sizeof(t_area));
				pthread_mutex_unlock(&(g_ptmu));
				return ;
			}
			tmp = a;
			a = a->next;
		}
		++id;
	}
	pthread_mutex_unlock(&(g_ptmu));
}

void	*realloc(void *ptr, size_t size)
{
	size_t		i;
	size_t		segment_size;
	u_int8_t	id;
	t_area		*a;
	void		*new;

	ft_printf("Realloc : %p | %zu\n", ptr, size);
	if (ptr == NULL)
		return (malloc(size));
	if (size == 0)
	{
		free(ptr);
		return (NULL);
	}
	pthread_mutex_lock(&(g_ptmu));
	id = 0;
	while (id < N_AREA)
	{
		a = g_data.areas[id];
		while (a != NULL)
		{
			i = 0;
			while (i < a->memory_allocated)
			{
				segment_size = ((struct s_header *)((size_t)a->ptr + i))->size;
				ft_printf("LE PROBELEME VIEN D'ICI ----- SEGMENT SIZE VAUT : %zu\n", segment_size);
				if ((size_t)ptr >= ((size_t)a->ptr + OVER_MDATA) && (size_t)ptr <= ((size_t)a->ptr + OVER_BLOCK))
				{
					if (OVER_BLOCK + size <= a->memory_available)
					{
						if (OVER_BLOCK == a->memory_allocated) // last block
						{
							((struct s_header *)((size_t)a->ptr + i))->size += size;
							((struct s_header *)((size_t)a->ptr + i))->is_free = false;
							a->memory_allocated += size;

							return (return_pointer((void *)((size_t)a->ptr + OVER_MDATA)));
						}
						else if (((struct s_header *)((size_t)a->ptr + OVER_BLOCK))->is_free == true)
						{
							size = ((size < META_DATA) ? META_DATA : size); // Ne pas couper un header
							((struct s_header *)((size_t)a->ptr + i))->size += size;
							((struct s_header *)((size_t)a->ptr + i))->is_free = false;
							a->memory_allocated += size;

							return (return_pointer((void *)((size_t)a->ptr + OVER_MDATA)));
						}
					}
					pthread_mutex_unlock(&(g_ptmu));
					if ((new = malloc(segment_size + size)) != NULL)
					{
						ft_printf("We gonna call memcpy with %zu\n", segment_size);
						ft_memcpy(new, (void *)((size_t)a->ptr + OVER_MDATA), segment_size);
						ft_printf("Memcpy success !!!!\n");
						free((void *)((size_t)a->ptr + OVER_MDATA));
					}
					return (new);
				}
				i += BLOCK_SIZE;
			}
			a = a->next;
		}
		++id;
	}
	return (return_pointer(NULL));
}

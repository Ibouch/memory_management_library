#include <ft_printf.h>
#include <memory_management.h>
#include <sys/mman.h>
#include <pthread.h>

# define META_DATA (sizeof(struct s_header))
# define BLOCK_SIZE (META_DATA + size)
# define align4(x) (((((x) - 1) >> 2) << 2) + 4)

void	*get_available_block(t_area *area, size_t size)
{
	ft_printf("On ecrit size a l'addr : %p |  is_free : %p\n", &((struct s_header *)(area->ptr + area->memory_allocated))->size, &((struct s_header *)(area->ptr + area->memory_allocated))->is_free);

	((struct s_header *)(area->ptr + area->memory_allocated))->size = size;
	((struct s_header *)(area->ptr + area->memory_allocated))->is_free[0] = TRUE;

//	memcpy((area->ptr + area->memory_allocated), &size, sizeof(size_t));
//	memcpy((area->ptr + (area->memory_allocated + sizeof(size_t))), 0, 8);

	area->memory_allocated += BLOCK_SIZE;
	ft_printf("Pointer returned : %p\n", (area->ptr + (area->memory_allocated - size)));
	return ((area->ptr + (area->memory_allocated - size)));
}

void	*add_new_area(t_area **area, size_t size)
{
	g_data.areas_size[g_data.id] = ((g_data.areas_size[g_data.id] + sizeof(t_area)) + g_data.pagesize - 1) / g_data.pagesize * g_data.pagesize;
	*area = (t_area *)mmap(0, g_data.areas_size[g_data.id], PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (*area == MAP_FAILED)
		return (NULL);
	(*area)->memory_available = g_data.areas_size[g_data.id] - sizeof(t_area);
	(*area)->memory_allocated = 0;
	(*area)->ptr = (void *)(*area + sizeof(t_area));
	(*area)->next = NULL;
	ft_printf("--- Init Memory --- : Area : %u | Size : %zu\n", g_data.id, g_data.areas_size[g_data.id]);
	ft_printf("Start Addr : %p | End Addr %p\n", (*area)->ptr, ((*area)->ptr + (*area)->memory_available));
	return (get_available_block(*area, size));
}

void	*init_memory(t_area **area_addr, size_t size)
{
	g_data.pagesize = getpagesize();
	g_data.areas_size[TINY] = (64 + META_DATA) * 100;
	g_data.areas_size[SMALL] = (1024 + META_DATA) * 100;
	g_data.areas_initialized[g_data.id] = TRUE;
	return (add_new_area(area_addr, size));
}

void	show_area_mem(t_area *addr, char *area, size_t *total)
{
	size_t	i;
	size_t	size;
	t_area	*begin;

	i = 0;
	size = 0;
	begin = addr;

	if (addr && addr->ptr != NULL)
		ft_printf("%s : %p\n", area, addr->ptr);
	while (addr && addr->ptr != NULL)
	{
		while (i < addr->memory_allocated)
		{
			size = ((struct s_header *)(addr->ptr + i))->size;
			i += BLOCK_SIZE;
			*total += size;
			ft_printf("%p - %p : %zu octets free : %u\n", (addr->ptr + (i - size)), (addr->ptr + i), size, ((struct s_header *)(addr->ptr + i))->is_free[0]);
		}
		i = 0;
		addr = addr->next;
	}
	addr = begin;
}

void	show_alloc_mem(void)
{
	size_t	total;

	total = 0;
	show_area_mem(g_data.areas[TINY], "TINY", &total);
	show_area_mem(g_data.areas[SMALL], "SMALL", &total);
	show_area_mem(g_data.areas[LARGE], "LARGE", &total);
	ft_printf("Total : %zu octets\n", total);
}

void	*malloc(size_t size)
{
	t_area	*begin;
	void	*block;

	//size = align4(size);
	if (size > 1 && size <= 64)
		g_data.id = TINY;
	else if (size > 64 && size <= 1024)
		g_data.id = SMALL;
	else if (size)
	{
		g_data.id = LARGE;
		g_data.areas_size[LARGE] = BLOCK_SIZE;
	}
	else
		return (NULL);
	//g_data.mem_lock = PTHREAD_MUTEX_INITIALIZER;
	//pthread_mutex_lock(&(g_data.mem_lock));
	if (g_data.areas_initialized[g_data.id] == FALSE)
	{
		block = init_memory(&(g_data.areas[g_data.id]), size);
	//	pthread_mutex_unlock(&(g_data.mem_lock));
		return (block);
	}

	begin = g_data.areas[g_data.id];
	block = NULL;	

	ft_printf("Taille demandée : %zu | Taille Totale : %zu | Taille allouée : %zu | Taille disponible : %zu\n", size, begin->memory_available, begin->memory_allocated, begin->memory_available - begin->memory_allocated);

	while (g_data.areas[g_data.id])
	{
		if (g_data.areas[g_data.id]->memory_available >= (g_data.areas[g_data.id]->memory_allocated + BLOCK_SIZE))
		{
			block = get_available_block(g_data.areas[g_data.id], size);
			break ;
		}
		else if (g_data.areas[g_data.id]->next == NULL)
		{
			block = add_new_area(&(g_data.areas[g_data.id]->next), size);
			break ;
		}
		g_data.areas[g_data.id] = g_data.areas[g_data.id]->next;
	}
	g_data.areas[g_data.id] = begin;
//	pthread_mutex_unlock(&(g_data.mem_lock));
	return (block);
}

void	free(void *ptr);

void	*realloc(void *ptr, size_t size);
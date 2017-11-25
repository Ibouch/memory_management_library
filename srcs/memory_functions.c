#include <ft_printf.h>
#include <memory_management.h>
#include <sys/mman.h>
#include <pthread.h>

# define META_DATA (sizeof(struct s_header))
# define BLOCK_SIZE (META_DATA + size)

void	*get_available_block(t_area *area, size_t size)
{
	((struct s_header *)((size_t)area->ptr + area->memory_allocated))->size = size;
	((struct s_header *)((size_t)area->ptr + area->memory_allocated))->is_free = FALSE;
	area->memory_allocated += BLOCK_SIZE;
	return ((void *)((size_t)area->ptr + (area->memory_allocated - size)));
}

void	*add_new_area(t_area **area, size_t size)
{
	g_data.pagesize = getpagesize();
	g_data.areas_size[TINY] = (64 + META_DATA) * 100;
	g_data.areas_size[SMALL] = (1024 + META_DATA) * 100;
	g_data.areas_size[LARGE] = BLOCK_SIZE;

	g_data.areas_size[g_data.id] = ((g_data.areas_size[g_data.id] + sizeof(t_area)) + g_data.pagesize - 1) / g_data.pagesize * g_data.pagesize;
	if ((g_data.areas_size[g_data.id] + g_data.total_allocated) >= g_data.rlm.rlim_cur || (size + g_data.total_allocated) >= g_data.rlm.rlim_cur)
		return (NULL);
	ft_printf("______NEW ZONE_____ : %zu | total_allocated : %llu vs %llu\n", g_data.areas_size[g_data.id], g_data.total_allocated, (char *)g_data.rlm.rlim_cur);
	*area = (t_area *)mmap(0, g_data.areas_size[g_data.id], PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (*area == MAP_FAILED)
		return (NULL);
	ft_printf("\nOK\n");
	(*area)->memory_available = g_data.areas_size[g_data.id] - sizeof(t_area);
	(*area)->memory_allocated = 0;
	(*area)->ptr = (void *)((size_t)(*area) + sizeof(t_area));
	(*area)->next = NULL;
	g_data.total_allocated += g_data.areas_size[g_data.id];
	return (get_available_block((*area), size));
}

void	show_area_mem(t_area *addr, char *area, size_t *total)
{
	size_t	i;
	size_t	size;
	t_bool	is_free;
	t_area	*begin;

	i = 0;
	begin = addr;
	if (addr && addr->ptr != NULL)
		ft_printf("%s : %p\n", area, addr->ptr);
	while (addr && addr->ptr != NULL)
	{
		while (i < addr->memory_allocated)
		{
			size = ((struct s_header *)((size_t)addr->ptr + i))->size;
			is_free = ((struct s_header *)((size_t)addr->ptr + i))->is_free;
			i += BLOCK_SIZE;
			*total += size;
			ft_printf("%p - %p : %zu octets free : %u\n", ((size_t)addr->ptr + (i - size)), ((size_t)addr->ptr + i), size, is_free);
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
	t_area			*area;

	getrlimit(RLIMIT_AS, &(g_data.rlm));
	if (size > 1 && size <= 64)
		g_data.id = TINY;
	else if (size > 64 && size <= 1024)
		g_data.id = SMALL;
	else if (size)
		g_data.id = LARGE;
	else
		return (NULL);
	if (g_data.areas[g_data.id] == NULL)
		return (add_new_area(&(g_data.areas[g_data.id]), size));
	area = g_data.areas[g_data.id];
	while (area)
	{
		if (area->memory_available >= (area->memory_allocated + BLOCK_SIZE))
			return (get_available_block(area, size));
		else if (area->next == NULL)
			return (add_new_area(&(area->next), size));
		area = area->next;
	}
	return (NULL);
}

void	free(void *ptr);

void	*realloc(void *ptr, size_t size);

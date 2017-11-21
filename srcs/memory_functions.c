#include <ft_printf.h>
#include <memory_management.h>
#include <sys/mman.h>

# define META_DATA (sizeof(struct s_block))
# define BLOCK_SIZE (META_DATA + size)
# define align4(x) (((((x) - 1) >> 2) << 2) + 4)

void	*get_available_block(t_area *area, size_t size)
{
	ft_printf("Je vais ecrire a l'adresse suivante : %p\n", (area->ptr + area->memory_allocated));
	area->meta = (struct s_block *)(area->ptr + area->memory_allocated);
	area->meta->size = size;
	area->meta->is_free = FALSE;
//	((struct s_block *)(area->ptr + area->memory_allocated))->size = size;
//	((struct s_block *)(area->ptr + area->memory_allocated))->is_free = FALSE;

//	memcpy((area->ptr + area->memory_allocated), &size, sizeof(size_t));
//	memcpy((area->ptr + (area->memory_allocated + sizeof(size_t))), (void *)FALSE, sizeof(long));

	area->memory_allocated += BLOCK_SIZE;
	ft_printf("Pointer returned : %p\n", (area->ptr + (area->memory_allocated - size)));
	return ((area->ptr + (area->memory_allocated - size)));
}

void	*add_new_area(t_area **area, size_t size)
{
	/*	
	if (addr->mmap == NULL)
	{
		(*addr)->size = size;
		(*addr)->memory_allocated = 0;
		(*addr)->mmap = mmap(0, BLOCK_SIZE, PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		(*addr)->next = NULL;
		return (get_available_block(*addr, size));
	}*/
	*area = (t_area *)mmap(0, (sizeof(t_area) + g_data.areas_size[g_data.id]), PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	(*area)->size = g_data.areas_size[g_data.id];
	(*area)->memory_allocated = 0;
	(*area)->ptr = (void *)(*area + sizeof(t_area));
	(*area)->next = NULL;
	ft_printf("Start Addr : %p | End Addr %p\n", g_data.areas[g_data.id]->ptr, (g_data.areas[g_data.id]->ptr + g_data.areas_size[g_data.id]));
	return (get_available_block(*area, size));
}

void	init_memory(void)
{
	if (g_data.id != LARGE && g_data.areas_initialized[g_data.id] == FALSE)
	{
		g_data.areas_size[TINY] = (64 + META_DATA) * 100;
		g_data.areas_size[SMALL] = (1024 + META_DATA) * 100;

		ft_printf("--- Init Memory --- : Area : %u | Size : %zu\n", g_data.id, g_data.areas_size[g_data.id]);
		g_data.areas[g_data.id] = (t_area *)mmap(0, (sizeof(t_area) + g_data.areas_size[g_data.id]),
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
		g_data.areas[g_data.id]->size = g_data.areas_size[g_data.id];	
		g_data.areas[g_data.id]->memory_allocated = 0;
		g_data.areas[g_data.id]->ptr = (void *)(g_data.areas[g_data.id] + sizeof(t_area));
		g_data.areas[g_data.id]->next = NULL;
		g_data.areas_initialized[g_data.id] = TRUE;
		ft_printf("Start Addr : %p | End Addr %p\n", g_data.areas[g_data.id]->ptr, (g_data.areas[g_data.id]->ptr + g_data.areas_size[g_data.id]));
	}
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
			size = ((struct s_block *)(addr->ptr + i))->size;
			i += BLOCK_SIZE;
			*total += size;
			ft_printf("%p - %p : %zu octets\n", (addr->ptr + (i - size)), (addr->ptr + i), size);
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
	//t_area	*tt;

	if (size > 1 && size <= 64)
		g_data.id = TINY;
	else if (size > 64 && size <= 1024)
		g_data.id = SMALL;
	else
		g_data.id = 0;
	init_memory();
	/*else if (size)
	{
		g_data.id = LARGE;
		begin = &(g_data.areas[LARGE]);
		while (g_data.areas[LARGE])
		{
			if (g_data.areas[LARGE].next == NULL)
				break ;
			g_data.areas[LARGE] = g_data.areas[LARGE].next;
		}
		ptr = add_new_area(&g_data.areas[LARGE], size);
		g_data.areas
	}
	*/

	//tt = g_data.areas[g_data.id];	
	//ft_printf("Taille demandée : %zu | Taille Totale : %zu | Taille allouée : %zu | Taille disponible : %zu\n", size, tt->size, tt->memory_allocated, tt->size - tt->memory_allocated);
	while (g_data.areas[g_data.id])
	{
		if (g_data.areas[g_data.id]->size >= (g_data.areas[g_data.id]->memory_allocated + BLOCK_SIZE))
			return (get_available_block(g_data.areas[g_data.id], size));
		if (g_data.areas[g_data.id]->next == NULL)
			break ;
		g_data.areas[g_data.id] = g_data.areas[g_data.id]->next;
	}
	return (add_new_area(&(g_data.areas[g_data.id]->next), size));
}

void	free(void *ptr);

void	*realloc(void *ptr, size_t size);
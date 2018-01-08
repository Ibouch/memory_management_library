/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_long_fd.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2016/02/19 18:52:07 by ibouchla          #+#    #+#             */
/*   Updated: 2016/02/19 18:52:55 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>
#include <sys/mman.h>

void	print_digit_addr(size_t p)
{
	const char	*s;

	s = "0123456789abcdef";
	if (p > 0)
	{
		print_digit_addr(p / 16);
		ft_putchar(s[(p % 16)]);
	}
}

void	print_addr(size_t p)
{
	ft_putstr("\033[1;96m0x");
	print_digit_addr(p);
}

void	print_digit_size(size_t nb)
{
	if (nb >= 10)
	{
		print_digit_size(nb / 10);
		ft_putchar(nb % 10 + '0');
	}
	else
		ft_putchar((char)nb + '0');
}

void	print_size(size_t nb)
{
	ft_putstr(H_WHITE);
	print_digit_size(nb);
}

void	*return_pointer(void *ptr)
{
	pthread_mutex_unlock(&(g_ptmu));
	pthread_mutex_destroy(&(g_ptmu));
	return (ptr);
}

void	*get_available_block(t_area *area, size_t size)
{
	((t_header *)((size_t)area->ptr + area->memory_allocated))->size = size;
	((t_header *)((size_t)area->ptr + area->memory_allocated))->is_free = false;
	area->memory_allocated += NEW_BLOCK;
	return (return_pointer((void *)((size_t)area->ptr
	+ (area->memory_allocated - size))));
}

static bool	max_mem_alloc(size_t size)
{
	return ((g_data.areas_size[g_data.id]
	+ g_data.total_allocated >= g_data.rlm.rlim_cur
	|| (size + g_data.total_allocated) >= g_data.rlm.rlim_cur)
		|| (g_data.areas_size[g_data.id] < NEW_BLOCK
			|| g_data.areas_size[g_data.id] < size));
}

void	*add_new_area(t_area **area, size_t size)
{
	g_data.areas_size[TINY] = (g_data.alloc_max[TINY] + META_DATA) * 100;
	g_data.areas_size[SMALL] = (g_data.alloc_max[SMALL] + META_DATA) * 100;
	g_data.areas_size[LARGE] = NEW_BLOCK;
	g_data.areas_size[g_data.id] = (size_t)(((int)(g_data.areas_size[g_data.id]
	+ sizeof(t_area)) + P_SIZE - 1) / P_SIZE * P_SIZE);
	if ((max_mem_alloc(size)) == true)
	{
		ft_putstr("MAXX ALLLOCCCC");//sdsd
		return (return_pointer(NULL));
	}
	*area = (t_area *)mmap(0, g_data.areas_size[g_data.id],
		PROT_READ | PROT_WRITE, MAP_ANON | MAP_PRIVATE, -1, 0);
	if (*area == MAP_FAILED)
		return (return_pointer(NULL));
	(*area)->memory_available = g_data.areas_size[g_data.id] - sizeof(t_area);
	(*area)->memory_allocated = 0;
	(*area)->ptr = (void *)((size_t)(*area) + sizeof(t_area));
	(*area)->next = NULL;
	g_data.total_allocated += g_data.areas_size[g_data.id];
	return (get_available_block((*area), size));
}

static void	print_area_name(const char area[6], void *ptr)
{
	ft_putstr("\033[1;95m\n");
	ft_putstr(area);
	ft_putstr(" : ");
	print_addr((size_t)ptr);
	ft_putchar('\n');
}

static void	show_data(size_t memory_available, size_t memory_allocated)
{
	ft_putstr("\n\033[1;93m------------------------------------------\n");
	ft_putstr("- Total : ");
	print_size(memory_available);
	ft_putendl(" octets");
	ft_putstr("\033[1;93m- Allocated : ");
	print_size(memory_allocated);
	ft_putendl(" octets");
	ft_putstr("\033[1;93m- Available : ");
	print_size(memory_available - memory_allocated);
	ft_putendl(" octets");
	ft_putendl("\033[1;93m------------------------------------------\n");
}

static void	show_block_range(void *ptr, size_t i,
	size_t segment_size, bool is_free)
{
	print_addr(((size_t)ptr + OVER_MDATA));
	ft_putstr(" - ");
	print_addr(((size_t)ptr + OVER_BLOCK));
	ft_putstr(" : ");
	print_size(segment_size);
	ft_putstr(" octets");
	(is_free) ? ft_strcolor_fd(" [✓]", H_GREEN, 1, true)
	: ft_strcolor_fd(" [✘]", H_RED, 1, true);
}

static void	show_area_mem(t_area *addr, const char area[6], size_t *total)
{
	size_t	i;
	size_t	segment_size;
	bool	is_free;

	if (addr && addr->ptr != NULL)
		print_area_name(area, addr->ptr);
	while (addr && addr->ptr != NULL)
	{
		show_data(addr->memory_available, addr->memory_allocated);
		i = 0;
		while (i < addr->memory_allocated)
		{
			segment_size = ((t_header *)((size_t)addr->ptr + i))->size;
			is_free = ((t_header *)((size_t)addr->ptr + i))->is_free;
			show_block_range(addr->ptr, i, segment_size, is_free);
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
	show_area_mem(g_data.areas[TINY], "TINY", &total);
	show_area_mem(g_data.areas[SMALL], "SMALL", &total);
	show_area_mem(g_data.areas[LARGE], "LARGE", &total);
	ft_putstr("\033[1;93m \n▶  Total : ");
	print_size(total);
	ft_putendl(" octets");
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

static void	init_(size_t size)
{
	P_SIZE = getpagesize();
	getrlimit(RLIMIT_AS, &(g_data.rlm));
	g_data.alloc_max[TINY] = 64;
	g_data.alloc_max[SMALL] = 1024;
	if (size > 1 && size <= g_data.alloc_max[TINY])
		g_data.id = TINY;
	else if (size > g_data.alloc_max[TINY] && size <= g_data.alloc_max[SMALL])
		g_data.id = SMALL;
	else if (size)
		g_data.id = LARGE;
}

void	*malloc(size_t size)
{
	t_area	*area;
	void	*block;

	if (!size)
		return (NULL);
	init_(size);
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

void	*merge_data_blocks(void **start, void *to_merge)
{
	size_t	segment_size;

	segment_size = ((t_header *)(to_merge))->size;
	((t_header *)(*start))->size += BLOCK_SIZE;
	return (*(start));
}

bool	check_unmap_area(t_iterator *it, t_area *a, uint8_t id)
{
	return ((it->free_blocks == true
		&& (id != LARGE && (a->memory_allocated + META_DATA
		+ g_data.alloc_max[id] > a->memory_available)))
		|| (it->free_blocks == true && id == LARGE));
}

uint8_t	unmap_area(t_iterator *it, t_area *a, uint8_t id)
{
	if ((check_unmap_area()) == true)
	{
		if (it->tmp != NULL)
			it->tmp->next = a->next;
		else
			g_data.areas[id] = a->next;
		munmap(a, (a->memory_available + sizeof(t_area)));
		pthread_mutex_unlock(&(g_ptmu));
		return (1);
	}
	return (0);
}

void	parse_area(t_iterator *it, t_area *a, void *ptr)
{
	size_t		i;
	size_t		segment_size;

	i = 0;
	while (i < a->memory_allocated)
	{
		it->previous = ((i > 0) ?
		(void *)((size_t)a->ptr + (i - BLOCK_SIZE)) : NULL);
		segment_size = MDATA_BLOCK_SIZE;
		if ((size_t)ptr >= ((size_t)a->ptr + OVER_MDATA)
			&& (size_t)ptr <= ((size_t)a->ptr + OVER_BLOCK))
		{
			MDATA_BLOCK_FREE = true;
			it->block = (void *)((size_t)a->ptr + i);
			if (it->previous != NULL && ((t_header *)(it->previous))->is_free == true)
				it->block = merge_data_blocks(&(it->previous), (void *)((size_t)a->ptr + i));
			if (OVER_BLOCK < a->memory_allocated
			&& ((t_header *)((size_t)a->ptr + OVER_BLOCK))->is_free == true)
			{
				merge_data_blocks(&(it->block), (void *)((size_t)a->ptr + OVER_BLOCK));
				segment_size += ((t_header *)((size_t)a->ptr + OVER_BLOCK))->size + META_DATA;
			}
		}
		it->free_blocks = (MDATA_BLOCK_FREE == false) ? false : it->free_blocks;
		i += BLOCK_SIZE;
	}
}

void	free(void *ptr)
{
	uint8_t		id;
	t_area		*a;
	t_iterator	it;

	pthread_mutex_lock(&(g_ptmu));
	id = 0;
	while (id < N_AREA)
	{
		it.tmp = NULL;
		a = g_data.areas[id];
		while (a != NULL)
		{
			it.free_blocks = true;
			parse_area(&it, a, ptr);
			if ((unmap_area(&it, a, id)))
				return ;
			it.tmp = a;
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
	size_t		next_size;
	uint8_t	id;
	t_area		*a;
	void		*new;

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
				segment_size = MDATA_BLOCK_SIZE;
				if ((size_t)ptr >= ((size_t)a->ptr + OVER_MDATA) && (size_t)ptr <= ((size_t)a->ptr + OVER_BLOCK))
				{
					if (OVER_BLOCK == a->memory_allocated && OVER_BLOCK + size <= a->memory_available)
					{
						MDATA_BLOCK_SIZE += size;
						MDATA_BLOCK_FREE = false;
						a->memory_allocated += size;
						return (return_pointer((void *)((size_t)a->ptr + OVER_MDATA)));
					}
					else if (OVER_BLOCK < a->memory_allocated && ((t_header *)((size_t)a->ptr + OVER_BLOCK))->is_free == true)
					{
						next_size = ((t_header *)((size_t)a->ptr + OVER_BLOCK))->size;
						if (size < next_size)
						{
							((t_header *)((size_t)a->ptr + OVER_BLOCK + size))->size = next_size - size;
							((t_header *)((size_t)a->ptr + OVER_BLOCK + size))->is_free = true;
							MDATA_BLOCK_SIZE += size;
							MDATA_BLOCK_FREE = false;
							return (return_pointer((void *)((size_t)a->ptr + OVER_MDATA)));
						}
						else if (size <= (META_DATA + next_size))
						{
							MDATA_BLOCK_SIZE += (META_DATA + next_size);
							MDATA_BLOCK_FREE = false;
							return (return_pointer((void *)((size_t)a->ptr + OVER_MDATA)));
						}
					}
					pthread_mutex_unlock(&(g_ptmu));
					if ((new = malloc(segment_size + size)) != NULL)
					{
						memcpy(new, (void *)((size_t)a->ptr + OVER_MDATA), segment_size);
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

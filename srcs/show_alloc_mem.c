/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/10 17:07:03 by ibouchla          #+#    #+#             */
/*   Updated: 2018/01/10 17:07:08 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>

extern t_memory			g_data;
extern pthread_mutex_t	g_ptmu;

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
	ft_putstr(" octets\n");
	ft_putstr("\033[1;93m- Allocated : ");
	print_size(memory_allocated);
	ft_putstr(" octets\n");
	ft_putstr("\033[1;93m- Available : ");
	print_size(memory_available - memory_allocated);
	ft_putstr(" octets\n");
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

void		show_alloc_mem(void)
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

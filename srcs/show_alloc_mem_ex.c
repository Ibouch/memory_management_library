/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   show_alloc_mem_ex.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/07 12:58:55 by ibouchla          #+#    #+#             */
/*   Updated: 2018/03/07 12:59:00 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>

extern t_memory			g_data;
extern pthread_mutex_t	g_ptmu;

static void	print_digit_addr(size_t p, uint8_t base)
{
	const char	*s;

	s = "0123456789abcdef";
	if (p > 0)
	{
		print_digit_addr(p / base, base);
		ft_putchar(s[(p % base)]);
	}
}

static void	show_digit_ptr(size_t addr, uint8_t n_char)
{
	size_t	len;
	size_t	addr_len;

	addr_len = ft_udigitlen(addr, 16);
	len = (addr_len > n_char) ? 0 : n_char - addr_len;
	len = (addr == 0) ? len + 1 : len;
	while (len--)
		ft_putchar('0');
	print_digit_addr(addr, 16);
}

static void	area_hexdump(t_area *a, size_t addr)
{
	size_t	i;

	i = 0;
	while (i < a->memory_allocated)
	{
		if ((i % 16) == 0)
		{
			ft_putchar('\n');
			ft_putstr("\033[1;34m");
			show_digit_ptr((size_t)addr + i, 8);
			ft_putstr("\033[1;37m");
			ft_putchar('\t');
		}
		show_digit_ptr(*((unsigned char *)(size_t)a->ptr + i), 2);
		ft_putchar(' ');
		++i;
	}
}

void		show_alloc_mem_ex(void)
{
	uint8_t	id;
	size_t	addr;
	t_area	*a;

	mutex_action(INIT_MUTEX);
	id = 0;
	addr = 0;
	while (id < N_AREA)
	{
		a = g_data.areas[id];
		while (a != NULL)
		{
			area_hexdump(a, addr);
			addr += a->memory_allocated;
			a = a->next;
		}
		++id;
	}
	ft_putchar('\n');
	mutex_action(ULOCK_DESTROY);
}

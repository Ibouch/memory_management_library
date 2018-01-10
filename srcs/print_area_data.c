/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   print_area_data.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/10 17:06:46 by ibouchla          #+#    #+#             */
/*   Updated: 2018/01/10 17:06:48 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>

static void	print_digit_addr(size_t p)
{
	const char	*s;

	s = "0123456789abcdef";
	if (p > 0)
	{
		print_digit_addr(p / 16);
		ft_putchar(s[(p % 16)]);
	}
}

static void	print_digit_size(size_t nb)
{
	if (nb >= 10)
	{
		print_digit_size(nb / 10);
		ft_putchar(nb % 10 + '0');
	}
	else
		ft_putchar((char)nb + '0');
}

void		print_addr(size_t p)
{
	ft_putstr("\033[1;96m0x");
	print_digit_addr(p);
}

void		print_size(size_t nb)
{
	ft_putstr(H_WHITE);
	print_digit_size(nb);
}

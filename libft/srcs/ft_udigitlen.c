/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_udigitlen.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/03/07 13:21:33 by ibouchla          #+#    #+#             */
/*   Updated: 2018/03/07 13:21:36 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <libft.h>

size_t	ft_udigitlen(uintmax_t c, uint8_t base)
{
	size_t	ret;

	ret = 0;
	if (c == 0)
		return (1);
	while (c > 0)
	{
		c /= base;
		++ret;
	}
	return (ret);
}

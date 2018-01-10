/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   calloc.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/10 17:05:28 by ibouchla          #+#    #+#             */
/*   Updated: 2018/01/10 17:05:59 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>

extern pthread_mutex_t	g_ptmu;

void	*calloc(size_t count, size_t size)
{
	void	*ptr;

	if (((size_t)-1 / count) < size)
		return (NULL);
	ptr = malloc(count * size);
	mutex_action(INIT_MUTEX);
	if (ptr)
		ft_bzero(ptr, count * size);
	return (return_pointer(ptr));
}

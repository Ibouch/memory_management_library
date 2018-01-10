/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   memory_functions.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/10 17:06:29 by ibouchla          #+#    #+#             */
/*   Updated: 2018/01/10 17:06:32 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>
#include <sys/mman.h>

extern t_memory			g_data;
extern pthread_mutex_t	g_ptmu;

static bool	max_mem_alloc(size_t size)
{
	return ((g_data.areas_size[g_data.id]
	+ g_data.total_allocated >= g_data.rlm.rlim_cur
	|| (size + g_data.total_allocated) >= g_data.rlm.rlim_cur)
		|| (g_data.areas_size[g_data.id] < NEW_BLOCK
			|| g_data.areas_size[g_data.id] < size));
}

void		mutex_action(uint8_t action)
{
	if (action == INIT_MUTEX && (pthread_mutex_init(&(g_ptmu), NULL)) != 0)
		abort();
	else if (action == ULOCK_DESTROY)
	{
		pthread_mutex_unlock(&(g_ptmu));
		pthread_mutex_destroy(&(g_ptmu));
	}
	else
		pthread_mutex_lock(&(g_ptmu));
}

void		*return_pointer(void *ptr)
{
	pthread_mutex_unlock(&(g_ptmu));
	pthread_mutex_destroy(&(g_ptmu));
	return (ptr);
}

void		*get_available_block(t_area *area, size_t size)
{
	((t_header *)((size_t)area->ptr + area->memory_allocated))->size = size;
	((t_header *)((size_t)area->ptr + area->memory_allocated))->is_free = false;
	area->memory_allocated += NEW_BLOCK;
	return (return_pointer((void *)((size_t)area->ptr
	+ (area->memory_allocated - size))));
}

void		*add_new_area(t_area **area, size_t size)
{
	g_data.areas_size[TINY] = (g_data.alloc_max[TINY] + META_DATA) * 100;
	g_data.areas_size[SMALL] = (g_data.alloc_max[SMALL] + META_DATA) * 100;
	g_data.areas_size[LARGE] = NEW_BLOCK;
	g_data.areas_size[g_data.id] = (size_t)(((int)(g_data.areas_size[g_data.id]
	+ sizeof(t_area)) + P_SIZE - 1) / P_SIZE * P_SIZE);
	if ((max_mem_alloc(size)) == true)
	{
		if (g_data.err_abort == true)
			abort();
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

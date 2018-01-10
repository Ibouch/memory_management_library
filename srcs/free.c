/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibouchla <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2018/01/10 17:06:05 by ibouchla          #+#    #+#             */
/*   Updated: 2018/01/10 17:06:07 by ibouchla         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <memory_management.h>
#include <sys/mman.h>

extern t_memory			g_data;
extern pthread_mutex_t	g_ptmu;

static void		*merge_data_blocks(void **start, void *to_merge)
{
	size_t	segment_size;

	segment_size = ((t_header *)(to_merge))->size;
	((t_header *)(*start))->size += BLOCK_SIZE;
	return (*(start));
}

static uint8_t	unmap_area(t_iterator *it, t_area *a, uint8_t id)
{
	if ((it->free_blocks == true
	&& (id != LARGE && (a->memory_allocated + META_DATA
	+ g_data.alloc_max[id] > a->memory_available)))
	|| (it->free_blocks == true && id == LARGE))
	{
		if (it->tmp != NULL)
			it->tmp->next = a->next;
		else
			g_data.areas[id] = a->next;
		munmap(a, (a->memory_available + sizeof(t_area)));
		mutex_action(ULOCK_DESTROY);
		return (1);
	}
	return (0);
}

static size_t	parse_free_block(t_area *a, t_iterator *it,
				size_t segment_size, size_t i)
{
	if (g_data.err_abort == true && MDATA_BLOCK_FREE == true)
		abort();
	MDATA_BLOCK_FREE = true;
	it->block = (void *)((size_t)a->ptr + i);
	if (it->previous != NULL && ((t_header *)(it->previous))->is_free == true)
		it->block = merge_data_blocks(&(it->previous),
		(void *)((size_t)a->ptr + i));
	if (OVER_BLOCK < a->memory_allocated
	&& ((t_header *)((size_t)a->ptr + OVER_BLOCK))->is_free == true)
	{
		merge_data_blocks(&(it->block), (void *)((size_t)a->ptr + OVER_BLOCK));
		segment_size += ((t_header *)((size_t)a->ptr + OVER_BLOCK))->size
		+ META_DATA;
	}
	return (segment_size);
}

static void		parse_area(t_iterator *it, t_area *a, void *ptr)
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
			segment_size = parse_free_block(a, it, segment_size, i);
		it->free_blocks = (MDATA_BLOCK_FREE == false) ? false : it->free_blocks;
		i += BLOCK_SIZE;
	}
}

void			free(void *ptr)
{
	uint8_t		id;
	t_area		*a;
	t_iterator	it;

	mutex_action(INIT_MUTEX);
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
	mutex_action(ULOCK_DESTROY);
}

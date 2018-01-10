
#include <memory_management.h>

static void	*realloc_last_block(t_area *a, size_t size, size_t i)
{
	MDATA_BLOCK_SIZE += size;
	MDATA_BLOCK_FREE = false;
	a->memory_allocated += size;
	return (return_pointer((void *)((size_t)a->ptr + OVER_MDATA)));
}

static void	*overwrite_next_free_block(t_area *a, size_t size,
			size_t segment_size, size_t i)
{
	size_t	next_size;

	next_size = ((t_header *)((size_t)a->ptr + OVER_BLOCK))->size;
	if (size < next_size)
	{
		((t_header *)((size_t)a->ptr + OVER_BLOCK + size))->size =\
		next_size - size;
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
	return (NULL);
}

static void	init_ns(t_nem *n, t_area *a, void *ptr, bool f)
{
	n->a = a;
	n->ptr = ptr;
	n->f = f;
}

static void	*copy_on_new_mem(t_nem *n, size_t size,
			size_t segment_size, size_t i)
{
	void	*new;

	mutex_action(ULOCK_DESTROY);
	//pthread_mutex_unlock(&(g_ptmu));
	if (n->f == true)
	{
		free(n->ptr);
		return (NULL);
	}
	if ((new = malloc(segment_size + size)) != NULL)
	{
		memcpy(new, (void *)((size_t)n->a->ptr + OVER_MDATA), segment_size);//ft_memcpy
		free((void *)((size_t)n->a->ptr + OVER_MDATA));
	}
	return (new);
}

void		*parse_area_bis(void *ptr, size_t size,
			t_area *a, bool f)
{
	size_t	i;
	size_t	segment_size;
	void	*new;
	t_nem	n;

	i = 0;
	while (i < a->memory_allocated)
	{
		segment_size = MDATA_BLOCK_SIZE;
		if ((size_t)ptr >= ((size_t)a->ptr + OVER_MDATA)
		&& (size_t)ptr <= ((size_t)a->ptr + OVER_BLOCK))
		{
			if (OVER_BLOCK == a->memory_allocated
			&& OVER_BLOCK + size <= a->memory_available)
				return (realloc_last_block(a, size, i));
			else if (OVER_BLOCK < a->memory_allocated
			&& ((t_header *)((size_t)a->ptr + OVER_BLOCK))->is_free == true)
				if ((new = overwrite_next_free_block(a, size, segment_size, i)))
					return (new);
			init_ns(&n, a, ptr, f);
			return (copy_on_new_mem(&n, size, segment_size, i));
		}
		i += BLOCK_SIZE;
	}
	return (NULL);
}

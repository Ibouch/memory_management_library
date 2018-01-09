
void	*calloc(size_t count, size_t size)
{
	void	*ptr;

	if (((size_t)-1 / count) < size)
		return (NULL);
	ptr = malloc(count * size);
	if (ptr)
		ft_bzero(ptr, count * size);
	return (ptr);
}

/*
	** Page allocated 3/4
	**	Nb alloc max restant
	** Total nb de pages utilise 
	** getenv pour le abort du free
	**	reallocf


#ifndef MEMORY_MANAGEMENT_H
# define MEMORY_MANAGEMENT_H
# include <libft.h>
# include <pthread.h>

# define N_AREA 3
# define TINY 0
# define SMALL 1
# define LARGE 2

# define META_DATA (sizeof(struct s_header))
# define BLOCK_SIZE (META_DATA + segment_size)
# define NEW_BLOCK (META_DATA + size)
# define OVER_MDATA (i + META_DATA)
# define OVER_BLOCK (i + BLOCK_SIZE)
# define MDATA_BLOCK_FREE ((t_header *)((size_t)a->ptr + i))->is_free
# define MDATA_BLOCK_SIZE ((t_header *)((size_t)a->ptr + i))->size
# define P_SIZE g_data.pagesize
# define ULOCK_DESTROY 0
# define INIT_MUTEX 1

typedef struct		s_header
{
	size_t			size;
	bool			is_free;
}					t_header;

typedef struct		s_area
{
	size_t			memory_available;
	size_t			memory_allocated;
	void			*ptr;
	struct s_area	*next;
}					t_area;

typedef struct		s_nem
{
	t_area			*a;
	void			*ptr;
	bool			f;
}					t_nem;

typedef struct		s_iterator
{
	bool			free_blocks;
	t_area			*tmp;
	void			*previous;
	void			*block;
}					t_iterator;

typedef struct		s_memory
{
	int				id;
	int				pagesize;
	size_t			areas_size[N_AREA];
	size_t			alloc_max[2];
	struct s_area	*areas[N_AREA];
	struct rlimit	rlm;
	bool			err_abort;
	bool			initialized;
	uint64_t		total_allocated;
}					t_memory;

pthread_mutex_t		g_ptmu;

/*
**	Memory library functions
*/

void				*reallocf(void *ptr, size_t size);
void				*realloc(void *ptr, size_t size);
void				*calloc(size_t count, size_t size);
void				*malloc(size_t size);
void				free(void *ptr);

/*
**	Print data functions
*/

void				print_size(size_t nb);
void				print_addr(size_t p);
void				show_alloc_mem(void);

/*
**	Other functions
*/

void				*parse_area_bis(void *ptr, size_t size, t_area *a, bool f);
void				*get_available_block(t_area *area, size_t size);
void				*add_new_area(t_area **area, size_t size);
void				*return_pointer(void *ptr);
void				mutex_action(uint8_t action);

#endif

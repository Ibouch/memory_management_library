
#ifndef MEMORY_MANAGEMENT_H
# define MEMORY_MANAGEMENT_H
# include <libft.h>

# define N_AREA 3
# define TINY 0
# define SMALL 1
# define LARGE 2

typedef struct		s_header
{
	size_t			size;
	t_bool			is_free;
}					t_header;

typedef struct		s_area
{
	size_t			memory_available;
	size_t			memory_allocated;
	void			*ptr;
	struct s_area	*next;
}					t_area;

typedef struct		s_memory
{
	int				id;
	int				pagesize;
	size_t			areas_size[N_AREA];
	struct s_area	*areas[N_AREA];
	struct rlimit	rlm;
	u_int64_t		total_allocated;
}					t_memory;

static t_memory		g_data;

/*
**	Memory library functions
*/

void				*realloc(void *ptr, size_t size);
void				*malloc(size_t size);
void				free(void *ptr);

void				show_area_mem(t_area *addr, char *area, size_t *total);
void				*get_available_block(t_area *area, size_t size);
void				*add_new_area(t_area **area, size_t size);
void				show_alloc_mem(void);

#endif

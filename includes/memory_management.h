
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
	t_bool			is_free[8];
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
	int				pagesize;
	u_int8_t		id;
	t_bool			areas_initialized[N_AREA];
	size_t			areas_size[N_AREA];
	struct s_area	*areas[N_AREA];
//	pthread_mutex_t	mem_lock;
}					t_memory;

static t_memory		g_data;

#endif
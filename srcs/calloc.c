
#include <memory_management.h>

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

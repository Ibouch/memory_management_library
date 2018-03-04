#include <libft.h>

size_t	ft_digitlen(intmax_t c, uint8_t base)
{
	size_t	ret;

	ret = 0;
	if (c == 0)
		return (1);
	if (c < 0)
		c = c * (-1);
	while (c > 0)
	{
		c /= base;
		++ret;
	}
	return (ret);
}

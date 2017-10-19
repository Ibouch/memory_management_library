make re
gcc main.c -L./ -lft_malloc -I libft/includes
clear
echo "\n" && ./a.out | cat -e && echo "\n"
rm a.out
make re
gcc -g -Wall -Wextra main.c -L./ -lft_malloc -I libft/includes -o bin
rm -rf bin.dSYM

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: ibouchla <ibouchla@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2015/12/17 12:21:30 by ibouchla          #+#    #+#              #
#    Updated: 2016/04/14 22:29:55 by ibouchla         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

ifeq ($(HOSTTYPE),)
	HOSTTYPE := $(shell uname -m)_$(shell uname -s)
endif

NAME		=	libft_malloc_$(HOSTTYPE).so

INC_PATH	=	-I includes/ -I libft/includes/ -I ft_printf/includes/
SRC_PATH	=	srcs

SRC_NAME	=	memory_functions.c
SRC			=	$(addprefix $(SRC_PATH)/,$(SRC_NAME))
OBJET		=	$(SRC:.c=.o)

CC_FLAGS	=	gcc -Wall -Wextra -Werror -Wformat=2 -Wswitch-default -Wcast-align -Wpointer-arith \
				-Wbad-function-cast -Wstrict-prototypes -Wstrict-overflow=5 -Winline -Wundef -Wnested-externs \
				-Wcast-qual -Wshadow -Wwrite-strings -Wconversion -Wunreachable-code \
				-fno-common -fstrict-aliasing \
				-std=c11 -pedantic -O0 -ggdb3

CREATE_LIB	=	$(CC_FLAGS) -shared -o $(NAME) $(OBJET) -L libft -lft -L ft_printf -lftprintf
CREATE_SLN	=	ln -s $(NAME) libft_malloc.so

RED			=	\033[1;31m
BLUE		=	\033[1;34m
GREEN		=	\033[0;32m
YELLOW		=	\033[1;33m

all: $(NAME)

$(NAME): $(OBJET)
	@echo "➜	$(BLUE)Compilation of object files is complete.\n"
	@echo "➜	$(YELLOW)Compilation of additional libraries in progress.."
	@make -C libft/ > /dev/null
	@make -C ft_printf/ > /dev/null
	@echo "➜	$(BLUE)Compilation of additional libraries is complete.\n"
	@echo "➜	$(YELLOW)Creation of the library in progress.."
	@$(CREATE_LIB)
	@echo "➜	$(BLUE)Creation of the library is complete.\n"
	@echo "➜	$(YELLOW)Creation of the symbolic link in progress.."
	@$(CREATE_SLN)
	@echo "➜	$(BLUE)Creation of the symbolic link is complete.\n"
	@echo "\033[1;32m➜	Done"

%.o: %.c
	@$(CC_FLAGS) -c $< $(INC_PATH) -o $@
	@echo "\033[1;32m➜	Compilation of object file from the library source file : $(GREEN)Success"
clean:
	@echo "\n$(RED)➜	Deleting object files of the library"
	@$(RM) $(OBJET)
	@echo "\n$(RED)➜	Deleting additional libraries"
	@make fclean -C libft/ > /dev/null
	@make fclean -C ft_printf/ > /dev/null

fclean: clean
	@echo "➜	$(RED)Remove the library"
	@$(RM) $(NAME)
	@echo "➜	$(RED)Remove the symbolic link\n"
	@$(RM) libft_malloc.so

re: fclean all

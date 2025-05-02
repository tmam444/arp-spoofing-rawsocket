# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/11/17 23:23:13 by chulee            #+#    #+#              #
#    Updated: 2024/06/15 10:11:51 by chulee           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME=libft.a
SRCS=ft_atoi.c \
	 ft_itoa.c \
	 ft_bzero.c  \
	 ft_calloc.c \
	 ft_isalpha.c \
	 ft_isdigit.c \
	 ft_isalnum.c \
	 ft_isascii.c \
	 ft_isprint.c \
	 ft_toupper.c \
	 ft_tolower.c \
	 ft_memset.c \
	 ft_memcpy.c \
	 ft_memmove.c \
	 ft_memchr.c \
	 ft_memcmp.c \
	 ft_strlen.c \
	 ft_strlcpy.c \
	 ft_strlcat.c \
	 ft_strchr.c \
	 ft_strrchr.c \
	 ft_strncmp.c \
	 ft_strnstr.c \
	 ft_strdup.c \
	 ft_strjoin.c \
	 ft_strtrim.c \
	 ft_strmapi.c \
	 ft_striteri.c \
	 ft_substr.c \
	 ft_split.c \
	 ft_putchar_fd.c \
	 ft_putstr_fd.c \
	 ft_putendl_fd.c \
	 ft_putnbr_fd.c
SRCS_B=ft_lstnew.c \
	 ft_lstadd_front.c \
	 ft_lstadd_back.c \
	 ft_lstclear.c \
	 ft_lstdelone.c \
	 ft_lstsize.c \
	 ft_lstlast.c \
	 ft_lstiter.c \
	 ft_lstmap.c \
	 ft_lstindex.c
OBJS = $(SRCS:.c=.o)
OBJS_B = $(SRCS_B:.c=.o)
CC = cc
CFLAGS = -Wall -Wextra -Werror # -g3 -fsanitize=address

$(NAME) : $(OBJS)
		ar -rcs $@ $^

%.o : %.c
		$(CC) $(CFLAGS) -c $< -o $@

clean :
		rm -f $(OBJS) $(OBJS_B)

fclean : clean
		rm -f $(NAME)

all : $(NAME)

re : fclean all

bonus : $(OBJS) $(OBJS_B)
		ar -rcs $(NAME) $^

.PHONY: clean fclean all re bonus

# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2023/09/04 10:37:37 by chulee            #+#    #+#              #
#    Updated: 2024/10/25 09:16:14 by chulee           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME=ft_malcolm
SRCS=srcs/main.c \
		 srcs/utils.c \
		 srcs/interface.c \
		 srcs/pcap.c \
		 srcs/spoof_gateway.c
OBJS = $(addprefix objs/, $(notdir $(SRCS:.c=.o)))
HEADERS= -I ./includes
CC = cc
CFLAGS = -Wall -Wextra -Werror
LIBFT_PATH=./libft
LIBFT=$(LIBFT_PATH)/libft.a
# FSANITIZE = -fsanitize=address -g

$(NAME) : $(LIBFT) $(OBJS)
		$(CC) $(HEADERS) $(FSANITIZE) $(CFLAGS) -o $(NAME) $(OBJS) $(LIBFT)

$(LIBFT) :
		$(MAKE) bonus -C $(LIBFT_PATH)

objs/%.o : srcs/%.c
		$(CC) $(HEADERS) $(FSANITIZE) $(CFLAGS) -c $< -o $@

clean :
		$(MAKE) clean -C $(LIBFT_PATH)
		rm -f $(OBJS)

fclean : clean
		$(MAKE) fclean -C $(LIBFT_PATH)
		rm -f $(NAME)

all : $(NAME)

re : fclean all

.PHONY: clean fclean all re test

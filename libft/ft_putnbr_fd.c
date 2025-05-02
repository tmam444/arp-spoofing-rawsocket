/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_putnbr_fd.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/16 13:57:05 by chulee            #+#    #+#             */
/*   Updated: 2021/11/18 14:58:32 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_recursive_write(int n, int fd)
{
	char	c;

	if (n / 10 == 0)
	{
		if (n < 0)
		{
			n *= -1;
			write(fd, "-", 1);
		}
		c = '0' + n;
		write(fd, &c, 1);
		return ;
	}
	ft_recursive_write(n / 10, fd);
	n = n % 10;
	if (n < 0)
		n *= -1;
	c = '0' + n;
	write(fd, &c, 1);
}

void	ft_putnbr_fd(int n, int fd)
{
	if (fd < 0)
		return ;
	ft_recursive_write(n, fd);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_atoi.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/13 13:56:10 by chulee            #+#    #+#             */
/*   Updated: 2021/11/17 23:31:15 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_isspace(char c)
{
	if (c == ' ' || c == '\t' || c == '\n' || c == '\f' \
			|| c == '\r' || c == '\v')
		return (1);
	else
		return (0);
}

int	ft_atoi(const char *str)
{
	int	ret;
	int	sign;
	int	len;

	len = 0;
	ret = 0;
	sign = 1;
	while (ft_isspace(*str))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		if (sign == 1 && ++len >= 19)
			return (-1);
		if (sign == -1 && ++len >= 19)
			return (0);
		ret = ret * 10 + *str - '0';
		str++;
	}
	return (ret * sign);
}

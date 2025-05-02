/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/12 23:27:18 by chulee            #+#    #+#             */
/*   Updated: 2021/11/12 23:43:43 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memchr(const void *s, int c, size_t n)
{
	size_t	i;
	void	*ret;

	i = 0;
	ret = (void *)s;
	while (i < n)
	{
		if (((unsigned char *)s)[i] == (unsigned char)c)
			return (ret + i);
		i++;
	}
	return (NULL);
}

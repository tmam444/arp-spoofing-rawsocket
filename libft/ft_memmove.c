/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memmove.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/12 14:48:25 by chulee            #+#    #+#             */
/*   Updated: 2021/11/14 18:00:05 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

void	*ft_memmove(void *dst, const void *src, size_t len)
{
	size_t	i;

	if (dst == src)
		return (dst);
	i = 0;
	while (i < len)
	{
		if (dst < src)
			((unsigned char *)dst)[i] = ((unsigned char *)src)[i];
		else
			((unsigned char *)dst)[len - i - 1] \
				= ((unsigned char *)src)[len - i - 1];
		i++;
	}
	return (dst);
}

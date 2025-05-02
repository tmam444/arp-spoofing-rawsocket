/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strlcat.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/12 16:55:38 by chulee            #+#    #+#             */
/*   Updated: 2021/11/16 20:12:55 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

size_t	ft_strlcat(char *dst, const char *src, size_t dstsize)
{
	size_t	i_dest;
	size_t	i_src;

	i_dest = 0;
	while (dst[i_dest] != '\0' && i_dest < dstsize)
		i_dest++;
	i_src = 0;
	while (src[i_src] != '\0' && i_dest + 1 < dstsize)
	{
		dst[i_dest] = src[i_src];
		i_src++;
		i_dest++;
	}
	if (i_dest < dstsize)
		dst[i_dest] = '\0';
	while (src[i_src] != '\0')
	{
		i_src++;
		i_dest++;
	}
	return (i_dest);
}

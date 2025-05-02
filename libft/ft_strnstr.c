/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strnstr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/13 08:58:57 by chulee            #+#    #+#             */
/*   Updated: 2021/11/16 20:46:35 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strnstr(const char *haystack, const char *needle, size_t len)
{
	size_t	h_i;
	size_t	n_i;

	if (needle[0] == '\0')
		return ((char *)haystack);
	h_i = 0;
	while (haystack[h_i] != '\0' && h_i < len)
	{
		n_i = 0;
		if (haystack[h_i] == needle[n_i])
		{
			while (needle[n_i] != '\0' && h_i + n_i < len)
			{
				if (needle[n_i] != haystack[h_i + n_i])
					break ;
				n_i++;
			}
			if (needle[n_i] == '\0')
				return ((char *)haystack + h_i);
		}
		h_i++;
	}
	return (NULL);
}

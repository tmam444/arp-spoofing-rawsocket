/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strrchr.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/12 21:19:14 by chulee            #+#    #+#             */
/*   Updated: 2021/11/12 21:36:23 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strrchr(const char *s, int c)
{
	int		i;
	char	*ret;

	ret = (char *)s;
	i = 0;
	while (ret[i] != '\0')
		i++;
	while (i >= 0 && ret[i] != (char)c)
		i--;
	if (i == -1)
		return (NULL);
	return (ret + i);
}

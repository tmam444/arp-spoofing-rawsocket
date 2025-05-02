/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_substr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/08 16:48:11 by chulee            #+#    #+#             */
/*   Updated: 2021/11/14 20:26:17 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_substr(char const *s, unsigned int start, size_t len)
{
	char	*ret;
	size_t	i;

	if (!s)
		return (NULL);
	ret = malloc(len + 1);
	if (!ret)
		return (NULL);
	i = 0;
	while (start <= ft_strlen(s) && i < len && s[i] != '\0')
		ret[i++] = s[start++];
	ret[i] = '\0';
	return (ret);
}

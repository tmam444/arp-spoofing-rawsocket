/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strtrim.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/13 22:52:26 by chulee            #+#    #+#             */
/*   Updated: 2021/11/17 19:18:27 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static void	ft_set_index(char const *s1, char const *set, int *start, int *end)
{
	int	i;
	int	j;

	i = 0;
	while (s1[i] != '\0')
	{
		j = 0;
		while (set[j] != s1[i] && set[j] != '\0')
			j++;
		if (set[j] == '\0')
			break ;
		i++;
	}
	*start = i;
	while (s1[i] != '\0')
		i++;
	while (i - 1 >= *start && --i >= *start)
	{
		j = 0;
		while (set[j] != s1[i] && set[j] != '\0')
			j++;
		if (set[j] == '\0')
			break ;
	}
	*end = i;
}

char	*ft_strtrim(char const *s1, char const *set)
{
	int		i;
	int		start;
	int		end;
	char	*ret;

	if (!s1 || !set)
		return (NULL);
	ft_set_index(s1, set, &start, &end);
	ret = malloc(end - start + 2);
	if (!ret)
		return (NULL);
	i = 0;
	while (start <= end)
		ret[i++] = s1[start++];
	ret[i] = '\0';
	return (ret);
}

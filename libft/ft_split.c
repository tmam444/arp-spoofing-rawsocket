/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/11/14 20:32:02 by chulee            #+#    #+#             */
/*   Updated: 2021/11/16 01:57:35 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static	int	count_words(char const *s, const char c)
{
	int	ret;
	int	flag;

	ret = 0;
	flag = 1;
	while (*s)
	{
		if (*s == c)
			flag = 1;
		else
		{
			if (flag)
			{
				ret++;
				flag = 0;
			}
		}
		s++;
	}
	return (ret);
}

static	int	find_start(char const *s, char c)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] != c)
			break ;
		i++;
	}
	return (i);
}

static	int	find_len(char const *s, char c)
{
	int	i;

	i = 0;
	while (s[i] != '\0')
	{
		if (s[i] == c)
			break ;
		i++;
	}
	return (i);
}

char	**ft_split(char const *s, char c)
{
	char	**ret;
	int		word_count;
	int		start;
	int		len;
	int		i;

	if (!s)
		return (NULL);
	word_count = count_words(s, c);
	ret = malloc(sizeof(char *) * (word_count + 1));
	if (!ret)
		return (NULL);
	i = 0;
	start = 0;
	while (i < word_count)
	{
		start += find_start(s + start, c);
		len = find_len(s + start, c);
		ret[i] = ft_substr(s, start, len);
		start += len;
		i++;
	}
	ret[i] = NULL;
	return (ret);
}

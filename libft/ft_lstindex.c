/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstindex.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: chulee <chulee@student.42seoul.kr>         +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/12/31 21:29:11 by chulee            #+#    #+#             */
/*   Updated: 2023/12/31 22:04:32 by chulee           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list *ft_lstindex(t_list *lst, const size_t index)
{
    size_t i;

    i = 0;
    while (i != index)
    {
        if (!lst)
            return (NULL);
        lst = lst->next;
        i++;
    }
    return (lst);
}

/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hgoncalv <hgoncalv@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/10/21 11:33:26 by mda-cruz          #+#    #+#             */
/*   Updated: 2022/10/26 19:11:21 by hgoncalv         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

void	ft_free_cmd(char ***cmd)
{
	int	i;
	int	j;

	i = 0;
	j = 0;
	while (cmd[i] != NULL)
		i++;
	free(cmd[i--]);
	while (i > -1)
	{
		j = 0;
		while (cmd[i][j] && cmd[i][j] != NULL)
			j++;
		while (j > -1)
		{
			free(cmd[i][j--]);
		}
		free(cmd[i--]);
	}
	free(cmd);
}

void	ft_free_loop(char ***cmd, char **pipes, char *line)
{
	ft_free_cmd(cmd);
	free(pipes);
	//void(pipes);
	if (line != NULL)
		free(line);
}

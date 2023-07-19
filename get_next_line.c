/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: fcasaubo <fcasaubo@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/06/29 12:56:08 by fcasaubo          #+#    #+#             */
/*   Updated: 2023/07/10 10:25:01 by fcasaubo         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	manage_last_remain(t_struct *data)
{
	int		i;

	i = 0;
	free(data->buffer);
	data->buffer = NULL;
	data->line = NULL;
	if (data->rd == -1)
	{
		free(data->remain);
		data->remain = NULL;
		return ;
	}
	while (data->remain[i] != '\n' && data->remain[i])
		i++;
	data->line = gnl_strjoin(data->line, data->remain, i);
	if (data->remain[i] == '\n')
	{
		if (data->remain[i + 1])
		{
			data->remain = gnl_truncate(data->remain, i + 1);
			return ;
		}
	}
	free(data->remain);
	data->remain = NULL;
}

int	ft_remain_copy(t_struct *data)
{
	if (data->remain)
	{
		data->line = gnl_strdup(data->remain, 0);
		if (!data->line)
			return (gnl_str_cleaner(data, 1, 1, 1), 0);
		data->remain = NULL;
	}
	else if (data->line)
	{
		free(data->line);
		data->line = NULL;
	}
	return (1);
}

void	line_reader(t_struct *data, int i)
{
	while (data->rd > 0)
	{
		i = 0;
		while (data->buffer[i] != '\n' && data->buffer[i] != '\0')
			i++;
		data->line = gnl_strjoin(data->line, data->buffer, i);
		if (!data->line)
			return (gnl_str_cleaner(data, 1, 1, 1));
		if (data->buffer[i] == '\n')
		{
			if (data->buffer[i + 1])
			{
				data->remain = gnl_strdup(data->buffer, i + 1);
				if (!data->remain)
					return (gnl_str_cleaner(data, 1, 1, 1));
			}
			else
				gnl_str_cleaner(data, 1, 0, 1);
			return ;
		}
		data->rd = (int)read(data->fd, data->buffer, BUFFER_SIZE);
		data->buffer[data->rd] = '\0';
	}
	gnl_str_cleaner(data, 1, 0, 1);
}

int	remain_new_line_search(t_struct *data)
{
	int	i;

	i = 0;
	if (!data->remain)
		return (0);
	if (data->rd == -1)
		return (gnl_str_cleaner(data, 1, 1, 0), 1);
	while (data->remain[i] && data->remain[i] != '\n')
		i++;
	if (!data->remain[i])
		return (0);
	data->line = gnl_strjoin(data->line, data->remain, i);
	if (data->remain[i + 1])
		data->remain = gnl_truncate(data->remain, i + 1);
	else
	{
		free(data->remain);
		data->remain = NULL;
	}
	return (1);
}

char	*get_next_line(int fd)
{
	static t_struct	data = {NULL, NULL, NULL, 1, -1};

	if (fd < 0)
		return (NULL);
	data.fd = fd;
	data.line = NULL;
	if (remain_new_line_search(&data), data.line)
		return (data.line);
	data.buffer = malloc((BUFFER_SIZE + 1) * sizeof(char));
	if (!data.buffer)
		return (gnl_str_cleaner(&data, 1, 1, 1), NULL);
	data.rd = read(data.fd, data.buffer, BUFFER_SIZE);
	if (data.rd < 0 || (data.rd == 0 && !data.remain))
		return (gnl_str_cleaner(&data, 0, 0, 1), NULL);
	data.buffer[data.rd] = '\0';
	if (data.rd > 0)
	{
		if (!ft_remain_copy(&data))
			return (NULL);
		return (line_reader(&data, 0), data.line);
	}
	else if (data.remain && data.rd == 0)
		return (manage_last_remain(&data), data.line);
	return (NULL);
}

# include <stdlib.h>
# include <unistd.h>
# include <stdio.h>
# include <limits.h>
# include <stdbool.h>
# include <fcntl.h>

int		ft_strlen(char *str)
{
	int i;

	i = 0;
	while (str[i] != '\0')
		i++;
	return (i);
}


int	count_words(char *str, char c)
{
	int		i;

	i = 0;
	while (*str)
	{
		while (*str == c && *str)
			str++;
		if (*str == 0)
			return (i);
		while (*str && *str != c )
			str++;
		if (*str == 0)
			return (i + 1);
		i++;
	}
	return (i);
}

char		*ft_strdup(char *str)
{
	char	*dest;
	int		i;

	dest = malloc(ft_strlen(str) * sizeof(char) + 1);
	if (dest == NULL)
		return (0);
	i = 0;
	while (str[i] != '\0')
	{
		dest[i] = str[i];
		i++;
	}
	dest[i] = '\0';
	return (dest);
}


char	*ft_substr(char *str, int begg, int l)
{
	char	*sub;
	int		i;
	int		strl;

	if (!str)
		return (0);
	strl = ft_strlen(str);
	if (begg >= strl)
	{
		sub = malloc(sizeof(char));
		if (!sub)
			return (0);
		*sub = '\0';
		return (sub);
	}
	if (strl < l)
		return (ft_strdup((char *)str + begg));
	i = 0;
	sub = (char *)malloc(l + 1 * sizeof(char));
	if (!sub)
		return (0);
	while (begg < strl && i < l)
		sub[i++] = str[begg++];
	sub[i] = '\0';
	return (sub);
}

char		**ft_strsplit(char const *str, char c)
{
	char	**split;
	int		i;
	int		i2;
	int		cnt;

	i = 0;
	i2 = 0;
	cnt = 0;
	if (!(split = (char **)malloc(sizeof(char *) * (count_words((char *)str, c) +1))))
		return (0);
	while (cnt < count_words((char *)str, c) && count_words((char *)str, c) > 0)
	{
		while (str[i] == c && str[i])
			i++;
		i2 = i;
		while (str[i] != c && str[i])
			i++;
		split[cnt++] = ft_substr((char *)str, i2, i -i2);
	}
	split[cnt] = 0;
	return (split);
}

char	*ft_strnstr(const char	*str, const char *find, size_t len)
{
	size_t	i;
	size_t	j;

	i = 0;
	if (find[i] == '\0')
		return ((char *)str);
	while (str[i] && i < len)
	{
		j = 0;
		while (str[i + j] == find[j] && i + j < len)
		{
			if (find[j + 1] == '\0')
				return ((char *)str + i);
			j++;
		}
		i++;
	}
	return (0);
}



/* links env with command*/
char *ft_env_cm(char **env, char *cm)
{
	char **list_path;
	char *tmp_path;
	char *cm_path;

	while (*env)
	{
		if (ft_strnstr(*env, "PATH", 4) != 0)
			break;
		env++;
	}
	list_path = ft_strsplit(*env, ':');
	while (*list_path)
	{
		tmp_path = ft_strjoin(*list_path, "/");
		cm_path = ft_strjoin(tmp_path, cm);
		free(tmp_path);
		if (access(cm_path, F_OK) == 0)
			return (cm_path);
		else
			list_path++;
	}
	return (0);
}

/* execute command by finding command and env link in ft_execute_env_cm */
void ft_execve(char *ag, char **env)
{
	char **list_cm;

	list_cm = ft_strsplit(ag, ' ');
	if (execve(ft_env_cm(env, list_cm[0]), list_cm, env) == -1)
	{
		perror("ft_exceve");
		exit(1);
	}
}

/* OPEN document and send output through pipe/parent_p */
void child_p(int *fd, char **ag, char **env)
{
	int filefd;

	filefd = open(ag[1], O_RDONLY, 0777);
	if (filefd == -1)
	{
		perror("child_p- make sure file exist");
		exit(2); 
	}
	dup2(fd[1], 1);
	dup2(filefd, 0);
	close(fd[0]);
	ft_execve(ag[2], env);
}

/* READS from pipe, ST_INPUT is ST_OUT of child_p */
void parent_p(int *fd, char **ag, char **env)
{
	int filefd;

	// filefd = open("mytesting.txt", O_RDWR | O_CREAT | O_TRUNC,  0777);
	filefd = open(ag[4], O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (filefd == -1)
	{
		perror("parent_p");
		exit(2);
	}
	dup2(fd[0], 0);
	dup2(filefd, 1);
	close(fd[1]);
	ft_execve(ag[3], env);
}

/* MAIN function, creates a pipe, forks/creates 2 running procceses same time,
waits for the child to finish, pass output to paretnt, stores output in ag[4] */

//SIGNAL CHILD TO PARENT-> cat | ls = child_new_line, send signal to parent, close pipe |, will probably need KILL()
int ft_pipe(char **ag, char **env)
{
	int fd[2];
	int pid;

	pipe(fd);
	pid = fork();
	if (pid == 0)
		child_p(fd, ag, env);
	else
	{
		waitpid(pid, NULL, 0);
		parent_p(fd, ag, env);
	}
	close(fd[0]);
	close(fd[1]);

	return (0);
}

/* NOTES
void _file_close(int fd[2])
{
	dup2(fd[1], 1);
	close(fd[0]);
	close(fd[1]);
}

// O_MODE == O_TRUNC: truncate the file to 0 bytes
// O_MODE == O_APPEND: append to the end of the file

int _file_create(int fd[2], char *path, int mode)
{
	fd[0] = open(path, O_CREAT | O_WRONLY | mode, 0644);
	if (fd[0] > 0)
	{
		fd[1] = dup(1);
		dup2(fd[0], 1);
	}
	else
		printf("Minishell: %s: Permission denied\n", path);
	return (!(fd[0] > 2));
}

struct s_file
{
	int (*create)(int fd[2], char *path, int mode);
	void (*close)(int fd[2]);
};

t_file _file(void)
{
	static t_file file = {
		_file_create,
		_file_close};

	return (file);
}



*/
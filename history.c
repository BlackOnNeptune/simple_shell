#include "shell.h"

/**
 * get_rec_file - gets the record file
 * @info: parameter struct
 *
 * Return: allocated string containing the record file
 */
char *get_rec_file(info_t *info)
{
	char *buf, *dir;

	dir = _getenv(info, "HOME=");
	if (!dir)
		return (NULL);
	buf = malloc(sizeof(char) * (_strlen(dir) + _strlen(RECORD_FILE) + 2));
	if (!buf)
		return (NULL);
	buf[0] = 0;
	_strcpy(buf, dir);
	_strcat(buf, "/");
	_strcat(buf, RECORD_FILE);
	return (buf);
}

/**
 * write_rec - writes to the record file
 * @info: the parameter struct
 *
 * Return: 1 on success, else -1
 */
int write_rec(info_t *info)
{
	ssize_t fd;
	char *filename = get_rec_file(info);
	list_t *node = NULL;

	if (!filename)
		return (-1);

	fd = open(filename, O_CREAT | O_TRUNC | O_RDWR, 0644);
	free(filename);
	if (fd == -1)
		return (-1);
	for (node = info->history; node; node = node->next)
	{
		_putsfd(node->str, fd);
		_putfd('\n', fd);
	}
	_putfd(BUF_FLUSH, fd);
	close(fd);
	return (1);
}

/**
 * read_rec - reads the record file
 * @info: the parameter struct
 *
 * Return: histcount on success, 0 otherwise
 */
int read_rec(info_t *info)
{
	int i, last = 0, linecount = 0;
	ssize_t fd, rdlen, fsize = 0;
	struct stat st;
	char *buf = NULL, *filename = get_rec_file(info);

	if (!filename)
		return (0);

	fd = open(filename, O_RDONLY);
	free(filename);
	if (fd == -1)
		return (0);
	if (!fstat(fd, &st))
		fsize = st.st_size;
	if (fsize < 2)
		return (0);
	buf = malloc(sizeof(char) * (fsize + 1));
	if (!buf)
		return (0);
	rdlen = read(fd, buf, fsize);
	buf[fsize] = 0;
	if (rdlen <= 0)
		return (free(buf), 0);
	close(fd);
	for (i = 0; i < fsize; i++)
		if (buf[i] == '\n')
		{
			buf[i] = 0;
			build_rec_list(info, buf + last, linecount++);
			last = i + 1;
		}
	if (last != i)
		build_record_list(info, buf + last, linecount++);
	free(buf);
	info->histcount = linecount;
	while (info->histcount-- >= RECORD_MAX)
		delete_node_at_index(&(info->history), 0);
	renumber_record(info);
	return (info->histcount);
}

/**
 * build_rec_list - adds entry to a record linked list
 * @info: Structure containing potential arguments
 * @buf: buffer
 * @linecount: the record linecount, histcount
 *
 * Return: Always 0
 */
int build_rec_list(info_t *info, char *buf, int linecount)
{
	list_t *node = NULL;

	if (info->history)
		node = info->history;
	add_node_end(&node, buf, linecount);

	if (!info->history)
		info->history = node;
	return (0);
}

/**
 * renumber_rec - renumbers the record linked list after changes
 * @info: Structure containing potential arguments
 *
 * Return: the new histcount
 */
int renumber_rec(info_t *info)
{
	list_t *node = info->history;
	int i = 0;

	while (node)
	{
		node->num = i++;
		node = node->next;
	}
	return (info->histcount = i);
}

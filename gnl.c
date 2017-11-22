/*
** gnl.c for  in /home/romain.pillot/gnl
** 
** Made by romain pillot
** Login   <romain.pillot@epitech.net>
** 
** Started on  Tue Nov 21 23:58:43 2017 romain pillot
** Last update Wed Nov 22 01:29:11 2017 romain pillot
*/

#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdint.h>
#include "gnl.h"

/**
 * Append to the string a, the string b.
 *
 * @return	a dynamic string concatenated of a and b.
 **/
static char	*str_concat(char *a, char *b, bool free_a, bool free_b)
{
  char		*new = NULL;
  size_t	a_len = 0;
  size_t	b_len = 0;
  size_t	new_len;

  while (a && a[a_len])
    ++a_len;
  while (b && b[b_len])
    ++b_len;
  new_len = a_len + b_len;
  if (!(new = malloc(sizeof(char) * (new_len + 1))))
    return (NULL);
  new[new_len] = 0;
  while (b_len > 0)
    new[--new_len] = b[--b_len];
  while (a_len > 0)
    new[--new_len] = a[--a_len];
  if (free_a && a)
    free(a);
  if (free_b && b)
    free(b);
  return (new);
}

/**
 * This function is looking for split the given string result unto
 * two strings. When a newline character is found, the result is split.
 * The string before the newline character got the index 0, the rest got 1.
 *
 * @param result	a given string that may contain newlines.
 * @param split		an empty array of string that will be fill.
 **/
static void	split_newline(const char *result, char *split[2])
{
  size_t	line_length = 0;
  size_t	rest_length = 0;
  uint32_t	index = -1;

  split[0] = NULL;
  split[1] = NULL;
  while (result && result[line_length] && result[line_length] != '\n')
    ++line_length;
  if (line_length == 0 || !(split[0] = malloc(sizeof(char) * (line_length + 1))))
    return;
  split[0][line_length] = 0;
  while (++index < line_length)
    split[0][index] = result[index];
  index = line_length;
  while (result[index++])
    ++rest_length;
  if (rest_length == 0 || !(split[1] = malloc(sizeof(char) * (rest_length))))
    return;
  split[1][rest_length - 1] = 0;
  index = -1;
  while (++index < rest_length - 1)
    split[1][index] = result[line_length + 1 + index];
}

/**
 * @param str	the string which gonna be analized.
 * @param c	the char that will be searched into the string.
 * @return	true if the given string contains the given chararacter.
 **/
static bool	str_contains(const char *str, const char c)
{
  while (str && *str)
    if (*str++ == c)
      return (true);
  return (false);
}

/**
 * This function's looking for a new line.
 * If there is a new line, it's returned and the rest is put into the hold var.
 *
 * @param str	a string that may contain a new line.
 * @param hold	a buffer that may stock characters after the new line.
 * @return	the new line string if found, NULL otherwise.
 **/
static char	*apply_strategy(const char *str, char hold[])
{
  char		*split[2];
  uint32_t	index = -1;

  if (!str_contains(str, '\n'))
    return (NULL);
  split_newline(str, split);
  if (split[1] != NULL) {
    while (split[1][++index])
      hold[index] = split[1][index];
    hold[index] = 0;
    free(split[1]);
  }
  return (split[0]);
}
  
/**
 * @param fd    the given file descriptor (>= 0).
 *
 * @return the next line read on a given file descriptor.
 **/
char		*get_next_line(const int fd)
{
  static char	hold[READ_SIZE + 1] = {0};
  static int	last_fd = -1;
  char		buffer[READ_SIZE + 1];
  size_t	bytes;
  char		*str = NULL;
  char		*next_line;

  if (fd < 0)
    return (NULL);
  else if (fd != -1 && fd != last_fd)
    *hold = 0;
  last_fd = fd;
  if ((next_line = apply_strategy(hold, hold)))
    return (next_line);
  str = str_concat(hold, str, false, false);
  while (!(next_line = apply_strategy(str, hold))) {
    if ((bytes = read(fd, buffer, READ_SIZE)) <= 0)
      break;
    buffer[bytes] = 0;
    str = str_concat(str, buffer, true, false);
  }
  if (str && *str && bytes == 0) {
    *hold = 0;
    return (str);
  }
  free(str);
  return (next_line);
}

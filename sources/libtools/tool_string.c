#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "tool_string.h"

void str_trim_right(char *str, char c)
{
    size_t len = strlen(str);

	while (len > 0)
	{
		if (str[len - 1] == c)
		{
			str[len - 1] = '\0';
			len--;
			continue;
		}
		break;
	}
}

void rand_string(char *buffer, size_t length)
{
#ifdef UNIX
	unsigned int seed = 0;
	int fd = 0;
	if ((fd = open("/dev/urandom", 0)) < 0)
	{
		seed = time(0);
	}
	else
	{
		read(fd, &seed, sizeof(unsigned int));
		close(fd);
	}
#else
	time_t seed = time(0);
#endif

	char *string = "abcdefghijklmnopqrstuvwxyz0123456789";
	size_t string_len = strlen(string);

	srand((unsigned int)seed);

	for (size_t i = 0; i < length; i++)
	{
		buffer[i] = string[rand() % string_len];
	}
}

int string_split(const char *str, const char separator, string_split_options opt, char **tokens, size_t total_tokens)
{
    size_t token_pos = 0;          // 当前解析到的token的位置
	int token_len = 0;          // token的长度
	int num_tokens = 0;         // 总的token数量
	size_t total = strlen(str);
	for (size_t i = 0; i < total; i++)
	{
		if(str[i] == separator)
		{
			if(token_len == 0)
			{
				if(opt == SSO_None)
				{
					tokens[num_tokens] = NULL;
				}
				else
				{
					token_pos = i + 1;
					continue;
				}
			}
			else
			{
				char *token = (char*)calloc(1, token_len + 1);
				strncpy(token, str + token_pos, token_len);
				tokens[num_tokens] = token;
				token_len = 0;
				num_tokens++;
			}
			token_pos = i + 1;
		}
		else
		{
			token_len++;
		}
	}

	if(token_len > 0)
	{
		char *token = (char*)calloc(1, token_len + 1);
		strncpy(token, str + token_pos, token_len);
		tokens[num_tokens] = token;
		num_tokens++;
	}

	return num_tokens;
}

void string_split_print(int num, char **tokens)
{
    for(int i = 0; i < num; i++)
	{
		printf("token%d = %s, length = %zd\n", i + 1, tokens[i], strlen(tokens[i]));
	}
}

void string_split_free(size_t num_tokens, char **tokens)
{
    for (size_t i = 0; i < num_tokens; i++)
	{
		if(tokens[i] != NULL)
		{
			free(tokens[i]);
		}
	}
}

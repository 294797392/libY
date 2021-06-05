#ifndef __TOOL_STRING_H__
#define __TOOL_STRING_H__

#include "toolbase.h"

typedef enum string_split_options_e
{
    SSO_None = 0,
    SSO_RemoveEmptyEntries = 1
}string_split_options;

#ifdef __cplusplus
extern "C" {
#endif

    TOOLSAPI void str_trim_right(char *str, char c);

    TOOLSAPI void rand_string(char *buffer, size_t length);

    TOOLSAPI int string_split(const char *str, const char separator, string_split_options opt, char **tokens, size_t total_tokens);

    TOOLSAPI void string_split_print(int num, char **tokens);

    TOOLSAPI void string_split_free(size_t num_tokens, char **tokens);

#ifdef __cplusplus
}
#endif

#endif
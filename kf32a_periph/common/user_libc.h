#ifndef _USER_LIBC_H_
#define _USER_LIBC_H_

#include <stdarg.h>
#include <stddef.h>
  
int user_vsnprintf(char *buf, size_t size, const char *fmt, va_list args);
int user_snprintf(char * buf, size_t size, const char *fmt, ...);

#endif


#include <stddef.h>
#include <stdarg.h>
#include <sys/cdefs.h>

__BEGIN_DECLS
#define __PRINTFLIKE(__fmt,__varargs) __attribute__((__format__ (__printf__, __fmt, __varargs)))

void kprintf(const char *format, ...) __PRINTFLIKE(1, 2);

int snprintf(char *buf, size_t size, const char *format, ...) __PRINTFLIKE(3, 4);
int vsnprintf(char *buf, size_t size, const char *format, va_list args);

typedef void (*vcprintf_callback_t)(void *state, char c);

void cprintf(vcprintf_callback_t callback, void *state, const char* format,
	     ...) __PRINTFLIKE(3, 4);

void vcprintf(vcprintf_callback_t callback, void *state, const char* format,
	      va_list args);
__END_DECLS
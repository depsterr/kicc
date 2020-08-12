#include <stdlib.h>

#define CLR_CLEAR  "\033[m"
#define CLR_RED    "\033[1;31m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_BLUE   "\033[1;34m"

#define die(...) _m(CLR_RED ":(" CLR_CLEAR, __FILE__, __LINE__, __VA_ARGS__),exit(1)
#define msg(...) _m(CLR_GREEN "OK" CLR_CLEAR, __FILE__, __LINE__, __VA_ARGS__)

/*
   stolen from dylan araps k
   used to print debug messages
   t is type of message
   f is the file the message arose from
   l is the line the message arose from
   fmt and .. are passed as args to printf
 */
void _m(const char* t, const char *f, const int l, const char *fmt, ...);

/*
   used to match a string against multiple others.
   comp is the string to compare
   count is the amount of strings to compare against
   ... are the strings to compare against
 */
int match_strings(const char* comp, unsigned int count, ...);

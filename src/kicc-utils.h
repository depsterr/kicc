/* licensed under The MIT License */
#include <stdlib.h>

#define CLR_CLEAR  "\033[m"
#define CLR_RED    "\033[1;31m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_BLUE   "\033[1;34m"

#define die(...) _m(CLR_RED ":(" CLR_CLEAR, __FILE__, __func__, __LINE__, __VA_ARGS__),exit(1)
#define msg(...) _m(CLR_GREEN "OK" CLR_CLEAR, __FILE__, __func__, __LINE__, __VA_ARGS__)

/*
   stolen from dylan araps k
   used to print debug messages
   t is type of message
   f is the file the message arose from
   fu is the function the message arose from
   l is the line the message arose from
   fmt and .. are passed as args to printf
 */
void _m(const char* t, const char *f, const char *fu, const int l, const char *fmt, ...);

/*
   get env using first arg, as key and if
   no value is found return the second arg
 */
char* xget_env(char* name, char* alt);

/*
   same as malloc but dies on failure
 */
void* xmalloc(size_t size);

/*
   same as realloc but dies on failure
 */
void* xrealloc(void* ptr, size_t size);

/*
   used to match a string against multiple others.
   comp is the string to compare
   count is the amount of strings to compare against
   ... are the strings to compare against

   returns -1 on failure and 1 indexed string
   index on successful match.
 */
int match_strings(const char* comp, unsigned int count, ...);

/*
   returns a null terminated array of strings
   representing the paths to all of the
   available kiss extensions. (binaries in path
   named kiss-*).

   REMEMBER TO FREE RETURNED VALUES AFTER USE.
 */
char** get_kiss_extentions(void);

/*
   prints usage and extensions and then exits
   with the exit status 0
 */
void usage_and_extentions(void);

/* licensed under The MIT License */
#include <stdlib.h>
#include <stdbool.h>

#define CLR_CLEAR  "\033[m"
#define CLR_RED    "\033[1;31m"
#define CLR_GREEN  "\033[1;32m"
#define CLR_YELLOW "\033[1;33m"
#define CLR_BLUE   "\033[1;34m"

/*
   stolen from dylan araps k
   prints a message (printf formatting), file,
   function, and the line of code that the
   macro was triggered. Then exits with
   a status of 1.
 */
#define die(...) _m(CLR_RED ":(" CLR_CLEAR, __FILE__, __func__, __LINE__, __VA_ARGS__),exit(1)

/*
   stolen from dylan araps k
   identical to die, except it doesn't exit.
   usefull for debugging
 */
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
   getenv using first arg, as key and if
   no value is found return the second arg
 */
char* xgetenv(char* name, char* alt);

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
   appends a char to the end of a string
 */
void strapp(char* str, char c);

/*
   returns a null terminated array of strings
   representing the paths to all of the
   available kiss extensions. (binaries in path
   named kiss-*).

   REMEMBER TO FREE RETURNED VALUES AFTER USE.
 */
char** get_kiss_extentions(void);

/*
   returns a null terminated array of strings
   containing the names of all installed
   packages.

   REMEMBER TO FREE RETURNED VALUES AFTER USE.
 */
char** get_installed_packages(void);

/*
   returns a null terminated array of strings
   containing the paths to all packages
   in KISS_PATH matching the given name.

   REMEMBER TO FREE RETURNED VALUES AFTER USE.
 */
char** get_package_paths(char* pkg);

/*
   takes the name of a package as a string
   and returns true if it's installed and
   false if it's not installed
 */
bool is_installed(char* pkg);

/*
   A null terminated array of strings which
   represent the paths in KISS_PATH
 */
extern char** KISS_PATH;

/*
   The ROOT directory to use for all operations,
   this makes creating chroots very easy.
 */
extern char* KISS_ROOT;

/*
   The path to the package database relative to
   the root directory.
 */
extern char* pkg_db;

/*
   The full path to the package database.
 */
extern char* sys_db;

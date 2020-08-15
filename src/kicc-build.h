/*
   this is the main build function which handles prompts
   and then calling the functions which actually build
   the package.

   it takes two arguments, an array of strings as well
   as the length of the list.
 */
void build(char** pkg_names, int pkg_count);

/*
   this is a function which is responsible for handeling
   depends at the higest level. it recieves a list of package
   names which it then resolver the depends for and allocates
   memory for a new list to store all depends in. It also
   takes a count argument which is modified after depends
   are added to the new list.
 */
char** add_depends(char** pkg_names, int* pkg_count);

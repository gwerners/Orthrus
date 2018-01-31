 /* example.i */
 %module example
 %{
 /* Put header files here or function declarations like below */
 extern int GetDescriptor();
 extern void CloseDescriptor(int fd);
 %}
 
 extern int GetDescriptor();
 extern void CloseDescriptor(int fd);

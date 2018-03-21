#include <algorithm>
#include <iostream>
#include <iterator>
#include <map>
#include <regex.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/types.h>

/*
       #include <regex.h>
       #include <sys/types.h>

       int regcomp(regex_t *preg, const char *regex, int cflags);

       int regexec(const regex_t *preg, const char *string, size_t nmatch,
                   regmatch_t pmatch[], int eflags);

       size_t regerror(int errcode, const regex_t *preg, char *errbuf,
                       size_t errbuf_size);

       void regfree(regex_t *preg);

--[[
AddBusinessObject parameters:
1- Service Name
   - identifier for this service
2 - Object type
   BO_JAVA - will run a jvm for each instance
   BO_CPP  - will run a shared library
   BO_LUA  - will run a lua script
3 - regular expression
   - match this expression against rest path to fire one instance
4 - number of instances
   - how many objects will be running to handle connections
5 - path to binary
   BO_JAVA - must end with entry point object_name.class
   BO_CPP  - must end with object_name.so
   BO_LUA  - must end with object_name.lua
]]
AddBusinessObject("Servico SMS",BO_JAVA,"/SMS/[0-9]*
/[0-9]*",5,"BO/JAVA/Sample.class")
*/
/*
std::string ServiceName
   int type
   std::string regexp path
   int instances
   std::string object path

*/

typedef struct Service_t
{
  int type;
  int instances;
  regex_t preg;
  std::string regexp;
  std::string object;
} Service;

const char*
str_regcomp_error(int vlr)
{
  switch (vlr) {
    case REG_BADBR:
      return "Invalid use of back reference operator.";
    case REG_BADPAT:
      return "Invalid use of pattern operators such as group or list.";
    case REG_BADRPT:
      return "Invalid use of repetition operators such as using '*' as the "
             "first character.";
    case REG_EBRACE:
      return "Un-matched brace interval operators.";
    case REG_EBRACK:
      return "Un-matched bracket list operators.";
    case REG_ECOLLATE:
      return "Invalid collating element.";
    case REG_ECTYPE:
      return "Unknown character class name.";
    case REG_EEND:
      return "Nonspecific error. This is not defined by POSIX.2.";
    case REG_EESCAPE:
      return "Trailing backslash.";
    case REG_EPAREN:
      return "Un-matched parenthesis group operators.";
    case REG_ERANGE:
      return "Invalid use of the range operator, e.g., the ending point of the "
             "range occurs prior to the starting point.";
    case REG_ESIZE:
      return "Compiled regular expression requires a pattern buffer larger "
             "than 64Kb. This is not defined by POSIX.2.";
    case REG_ESPACE:
      return "The regex routines ran out of memory.";
    case REG_ESUBREG:
      return "Invalid back reference to a subexpression.";
  }
  return "unknow error code!";
}
int
main()
{
  std::map<std::string, Service*> services;
  std::map<std::string, Service*>::iterator itor;
  Service* p_service;
  const char* expression;
  const char* match;
  int ret;
  size_t nmatch = 1;
  regmatch_t pmatch[1];

  p_service = new Service;
  p_service->instances = 1;
  expression = "/tefmock/comanda"; //"[0-9]+";
  ret = regcomp(&p_service->preg, expression, REG_EXTENDED);
  if (ret) {
    printf("error compiling expression[%s](%d:%s)",
           expression,
           ret,
           str_regcomp_error(ret));
  }
  p_service->regexp = expression;
  p_service->object = "/path/obj.class";
  services.insert(std::pair<std::string, Service*>("SMS", p_service));
  //////////////////////////////////////////////////////////////////////
  p_service = new Service;
  p_service->instances = 34;
  expression = "[a-z]+";
  ret = regcomp(&p_service->preg, expression, REG_EXTENDED);
  if (ret) {
    printf("error compiling expression[%s](%d:%s)\n",
           expression,
           ret,
           str_regcomp_error(ret));
    exit(EXIT_FAILURE);
  }
  p_service->regexp = expression;
  p_service->object = "/path/obj.so";
  services.insert(std::pair<std::string, Service*>("URA", p_service));

  // match
  match = "/tefmock/comanda";
  for (itor = services.begin(); itor != services.end(); itor++) {
    p_service = itor->second;

    if (0 != (ret = regexec(&p_service->preg, match, nmatch, pmatch, 0))) {
      printf("Failed to match '%s' with '%s',returning %d.\n",
             match,
             p_service->regexp.c_str(),
             ret);
    } else {
      printf("With the whole expression, "
             "a matched substring \"%.*s\" is found at position %d to %d.\n",
             pmatch[0].rm_eo - pmatch[0].rm_so,
             &match[pmatch[0].rm_so],
             pmatch[0].rm_so,
             pmatch[0].rm_eo - 1);
      printf("matched[%s] against [%s]\n", match, p_service->regexp.c_str());
      break;
    }
  }
  match = "dfgasgas";
  for (itor = services.begin(); itor != services.end(); itor++) {
    p_service = itor->second;

    if (0 != (ret = regexec(&p_service->preg, match, nmatch, pmatch, 0))) {
      printf("Failed to match '%s' with '%s',returning %d.\n",
             match,
             p_service->regexp.c_str(),
             ret);
    } else {
      printf("With the whole expression, "
             "a matched substring \"%.*s\" is found at position %d to %d.\n",
             pmatch[0].rm_eo - pmatch[0].rm_so,
             &match[pmatch[0].rm_so],
             pmatch[0].rm_so,
             pmatch[0].rm_eo - 1);
      printf("matched[%s] against [%s]\n", match, p_service->regexp.c_str());
      break;
    }
  }
  /*
     std::map<std::string,int>::iterator it = my_map.find("x");
     if (it != my_map.end()) std::cout << "x: " << it->second << "\n";

     it = my_map.find("z");
     if (it != my_map.end()) std::cout << "z1: " << it->second << "\n";

     // Accessing a non-existing element creates it
     if (my_map["z"] == 42) std::cout << "Oha!\n";

     it = my_map.find("z");
     if (it != my_map.end()) std::cout << "z2: " << it->second << "\n";*/
  for (itor = services.begin(); itor != services.end(); itor++) {
    std::cout << "Servico[" << itor->first << "]" << std::endl;
    p_service = itor->second;
    std::cout << "\tinstancias=" << p_service->instances << ";" << std::endl;
    std::cout << "\tregexp=" << p_service->regexp << ";" << std::endl;
    std::cout << "\tobject=" << p_service->object << ";" << std::endl;
  }
  itor = services.find("URA");
  if (itor != services.end()) {
    std::cout << "erase URA" << std::endl;
    p_service = itor->second;
    regfree(&p_service->preg);
    delete p_service;
    services.erase(itor);
  }
  std::cout << "#############################################" << std::endl;
  for (itor = services.begin(); itor != services.end(); itor++) {
    std::cout << "Servico[" << itor->first << "]" << std::endl;
    p_service = itor->second;
    std::cout << "\tinstancias=" << p_service->instances << ";" << std::endl;
    std::cout << "\tregexp=" << p_service->regexp << ";" << std::endl;
    std::cout << "\tobject=" << p_service->object << ";" << std::endl;
  }
  for (itor = services.begin(); itor != services.end(); itor++) {
    p_service = itor->second;
    regfree(&p_service->preg);
    delete p_service;
  }
  return 0;
}

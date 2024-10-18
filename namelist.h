#ifndef NAMELIST_H
#define NAMELIST_H
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <inttypes.h>
#define NUMARGS(...)  (sizeof((char*[]){__VA_ARGS__})/sizeof(char*))

//INFO:https://stackoverflow.com/questions/2124339/c-preprocessor-va-args-number-of-arguments
#define GET_VAR(name,def,nml,type) name = get_variable_from_name(#name,def,nml,type);
//def == default value, nml == namelist, from which to get the value
//




//TODO:first argument in NUMARGS_MY should be filename
//then in the functions that parse these arguments we should skip
//the first argument
//In counter NUMARGS2 we should return a number less by 1 then the total num. of args

#define LENGTH_OF_VALIST 2056
#define BUFF_SIZE 256
enum types
{
  TYPE_INT32_T,
  TYPE_DOUBLE,
  TYPE_CHAR_PTR

};

typedef union 
{
  int32_t int_value;
  double double_value;
  char  str_value[BUFF_SIZE];
} value_union;

struct variable{
  char name[BUFF_SIZE];
  enum types type;
  value_union data;
  bool initialized;
};

struct namelist
{
  char* name;
  struct variable *nml_variables;
  size_t num_of_variables;
};

//NOTE:struc namelist should contain only real user-supplied variables
//then it should be assigned only if such a variable is found in parse_namelist_from_file
#define XSTR(...) STR(__VA_ARGS__)
#define STR(...) #__VA_ARGS__



#define TP(x) _Generic((x),    \
      int32_t* : TYPE_INT32_T, \
      double* : TYPE_DOUBLE,\
      char* : TYPE_CHAR_PTR,\
      default : -10\
)
#define ADVAR(x) TP(x),x


#define NUMARGS2(...) number_of_occurences(#__VA_ARGS__)

#define ASSIGN_NAMELIST(...) do { printf("Variables in NUMARGS_MY: %s\n",#__VA_ARGS__); \
  printf("Number of variable arguments: %zu\n",number_of_occurences(#__VA_ARGS__));\
  struct namelist * nml =  create_namelist(NUMARGS2(#__VA_ARGS__),"example");\
  fill_namelist_from_user_supplied_variable_list(nml,NUMARGS2(#__VA_ARGS__),#__VA_ARGS__ );\
  parse_namelist_from_file_scanf(nml,NML_NAME);\
  assign_values(NUMARGS2(#__VA_ARGS__),nml,__VA_ARGS__);\
  free_namelist(nml);\
}while(0)

//void parse_namelist_from_file(struct namelist * nml, char * name, FILE * file);

#endif // !NAMELIST_H


#ifdef NAMELIST_IMPLEMENTATION



size_t number_of_occurences(char * str)
{
  size_t i;
  for (i=0; str[i]; str[i]==',' ? i++ : *str++);
  return i+1;
}


void assign_values(size_t N,struct namelist * nml, ...)
{
  va_list(var_args);
  va_start(var_args,nml);
  enum types type;
  value_union value;
  
  for (size_t i = 0; i < 2*N; ++i)
  {

    if (i % 2 == 0)
    {
      type = va_arg(var_args,enum types);
      //printf("from assign: type %d\n",type);
    }
    else 
    {
      if (nml->nml_variables[ (i-1)/2].initialized == true)
      {
        if (type == TYPE_INT32_T)
          *va_arg(var_args,int32_t*) = nml->nml_variables[ (i-1)/2 ].data.int_value; 
        if (type == TYPE_DOUBLE)
          *va_arg(var_args,double*) = nml->nml_variables[ (i-1)/2 ].data.double_value; 

        if (type == TYPE_CHAR_PTR)
        {
          //printf("%s\n",nml->nml_variables[ (i-1)/2 ].data.str_value);
          strcpy(va_arg(var_args,char*), nml->nml_variables[ (i-1)/2 ].data.str_value); 
        }
      }
      else
      {
        
        if (type == TYPE_INT32_T)
          *va_arg(var_args,int32_t*);// = nml->nml_variables[ (i-1)/2 ].data.int_value; 
        if (type == TYPE_DOUBLE)
          *va_arg(var_args,double*);// = nml->nml_variables[ (i-1)/2 ].data.double_value; 

        if (type == TYPE_CHAR_PTR)
        {
          va_arg(var_args,char*);
        }
      }
       
    } 
  }
//  for (size_t i =0; i < N; ++i)
//  {
//    if (nml->nml_variables[i].type == TYPE_INT32_T)
//      *va_arg(var_args,int32_t*) = *(int32_t*)nml->nml_variables[i].data ;
//    else if (nml->nml_variables[i].type == TYPE_DOUBLE)
//      *va_arg(var_args,double*) = *(double*)nml->nml_variables[i].data ;
//    else if (nml->nml_variables[i].type == TYPE_CHAR_PTR) 
//      *va_arg(var_args,char**) = *(char**)nml->nml_variables[i].data ;
//    else 
//      printf("ERROR in assign_values()!!!!!\n");
//
//  }
//
  va_end(var_args);
}

void fill_namelist_from_user_supplied_variable_list(struct namelist * nml,size_t N,const char * va_string )
{
  char *  pattern = ",()& ";
  const size_t bufsize = LENGTH_OF_VALIST;
  char buffer[bufsize];
  strcpy(buffer, va_string);
  char * p = strtok(buffer,pattern);
  size_t i = 0;
  struct variable * var = (struct variable*)malloc(sizeof(struct variable)*N);
  printf("N from fill... %zu %zu\n",N, nml->num_of_variables);
  while (p != NULL)
  {
    if (++i % 2 == 0)
    {
      //printf("%s %zu\n",p,strlen(p));
      strcpy(var[ i/2-1 ].name,p);
      var[ i/2-1 ].initialized = false;
    }
    p = strtok(NULL, pattern);
  }

  for (i = 0; i < N; ++i)
    printf("%s  ",var[i].name);

  nml->nml_variables = var;
  printf("\n");
  //for (size_t i = 0; i < N; ++i)
  //{
  //  nml->nml_variables[i].name = va_string_list[i]; 

  //}
}

//TODO: rewrite the lexical analyzer so that it supports different namelists,
//comma-separated variables placed in one line (a=3, f=3, d=sdsdds)
//FIXME:Something wrong with reading float numbers
void parse_namelist_from_file_scanf(struct namelist * nml , char * name)
{

  FILE * file = fopen(NML_FILENAME,"r");
  nml->name = name;
  printf("Name of the namelist: %s\n",nml->name); 

  const size_t bufsize = BUFF_SIZE;
  char buffer[bufsize];
  char * pattern = "= \t";

  char var_name[bufsize];

  value_union value;
  enum types type;;

  while(fgets(buffer, bufsize, file))
  {
    if (sscanf(buffer, "%s = %"SCNd32, var_name, &value.int_value) == 2) {
     // .type = VAR_INT;
      //da_append(&vars, v);
      printf("INTEGER %s = %"PRId32"\n",var_name,value.int_value);
      type = TYPE_INT32_T;
    } else if (sscanf(buffer, "%s = %lf", var_name, &value.double_value) == 2) {
      //v.type = VAR_FLOAT64;
      //da_append(&vars, v);
      printf("DOUBLE %s = %lf\n",var_name,value.double_value);
      type = TYPE_DOUBLE;
    } else if (sscanf(buffer, "%s = %s", var_name, value.str_value) == 2) {
      //v.type = VAR_STRING;
      //value.str_value = strdup(str_read);
      printf("STRING %s = %s\n",var_name,value.str_value);
      type = TYPE_CHAR_PTR;
    }

    for (size_t i = 0; i < nml->num_of_variables; ++i)
    {
      if (strcmp(var_name, nml->nml_variables[i].name) == 0)
      {
        printf("NAME: %s\n",var_name);
        nml->nml_variables[i].type = type;
        if ((type == TYPE_INT32_T) || (type == TYPE_DOUBLE))
          nml->nml_variables[i].data = value;
        else if (type == TYPE_CHAR_PTR)
          strcpy(nml->nml_variables[i].data.str_value,value.str_value);

        nml->nml_variables[i].initialized = true;

      }
    }

    //printf("%"PRId32"\n",TP(&dble_read));
  }
  fclose(file);
}

void parse_namelist_from_file(struct namelist * nml, char * name){

  FILE * file = fopen(NML_FILENAME,"r");


  nml->name = name;
  printf("Name of the namelist: %s\n",nml->name); 
  const size_t bufsize = BUFF_SIZE;
  char buffer[bufsize];
  char * pattern = "= \t";
  while(fgets(buffer, bufsize, file))
  {
    printf("%s",buffer);
    char * p;// strtok(buffer,pattern);
    {//TODO: set appropriate flags to switch between variable and assigned value
      //for simplicity probably should take into account only the case where 1 variable is getting 
      //assigned in each line. Just for now...
      p = strtok(NULL,pattern);
      printf("Lexems: %s\n",p);
    }while (p != NULL);
  }

  fclose(file);
}

struct namelist * create_namelist(size_t number_of_elements,char * name)
{
  struct namelist * nml =  malloc(number_of_elements * sizeof(struct namelist));
  nml->name = name;
  nml->num_of_variables = number_of_elements;
  return nml;
}

void free_namelist(struct namelist * nml)
{
  free(nml->nml_variables);
  free(nml);
  return;
}


#endif


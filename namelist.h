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
#include <assert.h>

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
  TYPE_FLOAT64_T,
  TYPE_CHAR_PTR

};

typedef union 
{
  int32_t int_value;
  double float64_value;
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
      double* : TYPE_FLOAT64_T,   \
      char* : TYPE_CHAR_PTR,   \
      default : -10            \
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
        if (type == TYPE_FLOAT64_T)
          *va_arg(var_args,double*) = nml->nml_variables[ (i-1)/2 ].data.float64_value; 

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
        if (type == TYPE_FLOAT64_T)
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

char *strndup_strip_spaces(char *buffer, size_t n)
// Copies at most 'n' bytes but skips over leading spaces 
{
    int i = 0;
    for (; buffer[i] == ' '; i++);

    int j = n;
    for (; j >= 0 && buffer[j] == ' '; j--);

    return strndup(buffer + i, j - i + 1); 
}

bool try_parse_float64(char *buffer, char **var_name, value_union *value)
{
    size_t buffer_len = strlen(buffer);
    char *endptr;

    char *pos = strchr(buffer, '=');
    if (pos == NULL) {
        fprintf(stderr, "ERROR: could not locate '=' in the namelist string\n");
        exit(1); 
    }

    *var_name = strndup_strip_spaces(buffer, (pos-1) - buffer); // 'pos-1' to offset from '=' character

    char *var_value = pos + 1; // skip '=' character
    double float64_value = strtod(var_value, &endptr);

    if (endptr == NULL) return false;
    
    // check if there are characters in the line to be parsed 
    for ( ; *endptr == ' '; endptr++);

    if ((endptr - buffer) + 1 != buffer_len) {
        // fprintf(stderr, "ERROR: some characters are left in the string which could not be parsed\n");
        return false; 
    }

    value->float64_value = float64_value;
}

bool try_parse_int(char *buffer, char **var_name, value_union *value)
{
    size_t buffer_len = strlen(buffer);
    char *endptr;

    char *pos = strchr(buffer, '=');
    if (pos == NULL) {
        fprintf(stderr, "ERROR: could not locate '=' in the namelist string\n");
        exit(1); 
    }

    *var_name = strndup_strip_spaces(buffer, (pos-1) - buffer); // 'pos-1' to offset from '=' character

    char *var_value = pos + 1; // skip '=' character
    long int_value = strtol(var_value, &endptr, 10);

    if (endptr == NULL) return false;

    if ((endptr - buffer) + 1 != buffer_len) {
        // fprintf(stderr, "ERROR: some characters are left in the string which could not be parsed\n");
        return false; 
    }

    value->int_value = (int) int_value;

    return true;
}

//TODO: rewrite the lexical analyzer so that it supports different namelists,
//comma-separated variables placed in one line (a=3, f=3, d=sdsdds)
//FIXME:Something wrong with reading float numbers
void parse_namelist_from_file_scanf(struct namelist * nml , char * name)
{
    FILE *file = fopen(NML_FILENAME, "r");
    size_t lineno = 0;

    nml->name = name;
    printf("Name of the namelist: %s\n",nml->name); 

    const size_t bufsize = BUFF_SIZE;
    char buffer[bufsize];
    char * pattern = "= \t";

    char *var_name;

    value_union value;
    enum types type;

    while(fgets(buffer, bufsize, file)) 
    {
        lineno++;

        if (try_parse_int(buffer, &var_name, &value)) {
            printf("INTEGER '%s' = %"PRId32"\n", var_name, value.int_value);
            type = TYPE_INT32_T;
        } else if (try_parse_float64(buffer, &var_name, &value)) {
            printf("FLOAT64 '%s' = %lf\n", var_name, value.float64_value);
            type = TYPE_FLOAT64_T;
        } else if (sscanf(buffer, "%s = %s", var_name, value.str_value) == 2) {
            // TODO: we should probably wrap strings in quotes 
            //       because otherwise some erroneous numbers could be easily be parsed as strings:
            //       EXAMPLE: '129.0 2' could be parsed as string '129.0'. This should probably raise parsing error?
            printf("STRING %s = %s\n",var_name,value.str_value);
            type = TYPE_CHAR_PTR;
        } else { 
            fprintf(stderr, "%s:%zu: ERROR: could not parse\n", NML_FILENAME, lineno);
            fprintf(stderr, "%s\n", buffer);
            exit(1);
        }

        for (size_t i = 0; i < nml->num_of_variables; ++i)
        {
            if (strncmp(var_name, nml->nml_variables[i].name, strlen(nml->nml_variables[i].name)) == 0)
            {
                printf("NAME: %s\n",var_name);
                nml->nml_variables[i].type = type;
                if ((type == TYPE_INT32_T) || (type == TYPE_FLOAT64_T))
                    nml->nml_variables[i].data = value;
                else if (type == TYPE_CHAR_PTR)
                    strcpy(nml->nml_variables[i].data.str_value, value.str_value);

                nml->nml_variables[i].initialized = true;

            }
        }
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


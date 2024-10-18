
#define NML_FILENAME "nml.txt"
#define NML_NAME "example"

#define NAMELIST_IMPLEMENTATION
#include "namelist.h"




int main(int argc, char *argv[])
{

  int32_t a = 5;
  int32_t b = 6;
  int32_t val = 98;
  char str[49] = "strstr";
  int32_t ii = 11;
  char dd[32] = "valval";

  //ASSIGN_NAMELIST(ADVAR(&a,0),ADVAR(&b,0), ADVAR(&val,0), ADVAR(&ii,0), ADVAR(dd,"s"));
  ASSIGN_NAMELIST(ADVAR(&a),ADVAR(&b), ADVAR(&val), ADVAR(&ii), ADVAR(dd), ADVAR(str));

  printf("a = %"PRId32"\n",a);
  printf("ii = %"PRId32"\n",ii);
  printf("b = %"PRId32"\n",b);
  printf("val = %"PRId32"\n",val);
  //printf("b = %lf\n",b);
  //printf("val = %lf\n",val);
  printf("dd = %s\n",dd);
  printf("str = %s\n",str);


  return EXIT_SUCCESS;
}

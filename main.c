
#define NML_FILENAME "nml.txt"
#define NML_NAME "example"

#define NAMELIST_IMPLEMENTATION
#include "namelist.h"




int main(int argc, char *argv[])
{

  int32_t a = 5;
  double b = 6;
  int32_t val = 98;
  char str[49] = "strstr";
  int32_t ii = 11;
  char dd[32] = "valval";

  ASSIGN_NAMELIST(ADVAR(&a),ADVAR(&b), ADVAR(&val), ADVAR(&ii), ADVAR(dd), ADVAR(str));

  printf("a = %"PRId32"\n",a);
  printf("ii = %"PRId32"\n",ii);
  printf("b = %lf\n",b);
  printf("val = %"PRId32"\n",val);
  printf("dd = %s\n",dd);
  printf("str = %s\n",str);


  return EXIT_SUCCESS;
}

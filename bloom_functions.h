#ifndef FUNCTIONS_H_
#define FUNCTIONS_H_
#include <stdint.h>
#include "fnv.h"

typedef struct program_arguments Arguments;
struct program_arguments
{
	int size;
	int plithos_hash;
};
int check_bit(char*,uint32_t,int);
void set_bit(char*,uint32_t,int);
void printbincharpad(int,char*);
void initialize_args(Arguments*,int,int);
void initialize_bloom_filter(Arguments*,char*);
Fnv32_t fnv_32_str(char *str, Fnv32_t hval);



#endif /* FUNCTIONS_H_ */

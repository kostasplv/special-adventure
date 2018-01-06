#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "bloom_functions.h"
#include <string.h>
#include <inttypes.h>
#include "fnv.h"

/*
 * voithitiki sunartisi ektuposhs ton bits enos pinaka
 * den ziteitai stin ekfonisi ,tin xrisimopoiisa gia debugging
 * tin afino mipos xreiastei stin proforiki
 *
 * tin exo parei autousia apo to link pou anafero sto readme!
 */
void printbincharpad(int size,char* c)
{
	int i,j;
	for(j=0;j<size;j++)
	{
		//printf("Keli %d\n",j);
		for (i = CHAR_BIT-1; i >= 0; i--)
		{
			putchar( (c[j] & (1 << i)) ? '1' : '0' );
		}
		//putchar('\n');
	}
	putchar('\n');
}

/*
 * sunartisi pou analoga tin timi epistrofis tis hash function
 * thetei 1 to antistoixo bit sto bloom filter
 */
void set_bit(char* c,uint32_t hash_result,int size)
{

	uint64_t temp=(hash_result%(CHAR_BIT*size))/CHAR_BIT,temp1=(hash_result%(CHAR_BIT*size))%CHAR_BIT;
	c[temp]|=(1<<temp1);
}

/*
 * sunartisi pou analoga tin timi epistrofis tis hash function
 * tsekarei an einai 1 h 0 to antistoixo bit
 * kai epistrefei 1 an to bit einai 1 ,0 allios
 */
int check_bit(char* c,uint32_t hash_result,int size)
{
	//printf("sting=%s\n",c);
	//printf("hash_result=%" PRIu32 "\n",hash_result);
	uint64_t temp=(hash_result%(CHAR_BIT*size))/CHAR_BIT,temp1=(hash_result%(CHAR_BIT*size))%CHAR_BIT;

	if((c[temp]>>temp1)&1)
		return 1;
	return 0;
}

/*
 * sunartisi pou thetei 0 ola ta bits enos pinaka
 */

void initialize_bloom_filter(Arguments* args,char* filter)
{
	int i;
	for(i=0;i<args->size;i++)
	{
		filter[i]&=0;
	}
}

/*
 * sunartisi pou arxikopoiei tin struct pou fulao ta orismata pou exei dosei
 * o xristis apo tin grammi entoilon
 */
void initialize_args(Arguments* orismata,int size,int hash)
{
	orismata->plithos_hash=hash;
	orismata->size=size;
}
uint32_t jenkins_one_at_a_time_hash(char *key, size_t len)
{
    uint32_t hash, i;
    for(hash = i = 0; i < len; ++i)
    {
        hash += key[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);
    return hash;
}
Fnv32_t fnv_32_str(char *str, Fnv32_t hval)		//XRHSIMOPOIEITAI GIA THN HASH-FUNCTION//
{
    unsigned char *s = (unsigned char *)str;	/* unsigned string */

    /*
     * FNV-1 hash each octet in the buffer
     */
    while (*s) {

	/* multiply by the 32 bit FNV magic prime mod 2^32 */
#if defined(NO_FNV_GCC_OPTIMIZATION)
	hval *= FNV_32_PRIME;
#else
	hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
#endif

	/* xor the bottom with the current octet */
	hval ^= (Fnv32_t)*s++;
    }

    /* return our new hash value */
    return hval;
}


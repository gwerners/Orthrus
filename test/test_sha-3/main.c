#include <stdio.h>
#include <string.h>

void FIPS202_SHAKE128(const unsigned char *input, unsigned int inputByteLen, unsigned char *output, int outputByteLen);
void FIPS202_SHA3_512(const unsigned char *input, unsigned int inputByteLen, unsigned char *output);

int main(int argc, char**argv)
{
   int i;
   const char * input="The quick brown fox jumps over the lazy dog";
   char output[128];
   memset(output,0,128);
   printf("begin test keccak - sha-3\n");
   FIPS202_SHAKE128(input,strlen(input),output,128);
   printf("%s - ",input);
   for(i=0;i<32;i++)
      printf("%02x",output[i]&0xff);
   printf("\n");


//f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e
//f4202e3c5852f9182a0430fd8144f0a74b95e7417ecae17db0f8cfeed0e3e66e
  

   return 0;
}


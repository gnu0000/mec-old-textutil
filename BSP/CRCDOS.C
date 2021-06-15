#include <stdio.h>
#include <stdlib.h>


#define   MASK   0xA0000001
#define   NUM unsigned long int


NUM CRC32 (NUM ulCRC, int c)
   {
   int i;

   for (i=0; i<8; i++)
      {
      if((ulCRC ^ c) & 1)
         ulCRC = (ulCRC>>1) ^ MASK;
      else
         ulCRC>>=1;
      c>>=1;
      }
   return ulCRC;
   }


main (argc, argv)
   int  argc;
   char *argv[];
   {
   NUM  uCRC;
   int  c;

   uCRC = 0;
   while ((c = getchar ()) != EOF)
      uCRC = CRC32 (uCRC, c);
   printf ("CRC-32 = %lx\n", uCRC);
   return 0;
   }



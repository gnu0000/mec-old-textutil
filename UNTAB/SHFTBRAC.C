#include <stdlib.h>
#include <stdio.h>

main (int argc, char *argv[])
   {
   int   c;
   long  l = 0;

   while ((c = getchar ()) != EOF)
      {
      if (c == '{' || c == '}')
         {
         fputs ("   ", stdout);
         l++;
         }
      fputc (c, stdout);
      }
   fprintf (stderr, "%ld braces shifted\n", l);
   return 0;
   }

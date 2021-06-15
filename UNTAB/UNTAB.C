#include <stdlib.h>
#include <stdio.h>


main (int argc, char *argv[])
   {
   int   c;
   long  l = 0;

   while ((c = getchar ()) != EOF)
      {
      if (c == '\t')
         {
         fputs ("   ", stdout);
         l++;
         }
      else
         fputc (c, stdout);
      }
   fprintf (stderr, "%ld tabs converted\n", l);
   return 0;
   }

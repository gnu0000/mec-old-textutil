#include <stdio.h>

#define TMPFILE      "TMPUD$$$.TMP"
#define BUFFSIZE     32768U

char szBuff [BUFFSIZE + 1];

main (int argc, char *argv[])
   {
   FILE  *fpIn, *fpOut;
   int   i, u, j =0;

   if (argc < 2)
      return puts ("DU  DOS to UNIX file converter. Params are files to cvt");

   for (i=1; i < argc; i++)
      {
      printf ("Converting %s\n", argv[i]);

      if (!(fpIn = fopen (argv[i], "rt")))
         {
         printf ("Unable to open input file :%s\n", argv[i]);
         continue;
         }

      if (!(fpOut = fopen (TMPFILE, "wb")))
         return puts ("Error opening output.");

      do
         {
         u = fread (szBuff, 1, BUFFSIZE, fpIn);
         fwrite (szBuff, 1, u, fpOut);
         } while (u == BUFFSIZE);

      fclose (fpIn);
      fclose (fpOut);

      unlink (argv[i]);
      rename (TMPFILE, argv[i]);

      j++;
      }

   printf ("%d files processed.", j);
   return 0;
   }

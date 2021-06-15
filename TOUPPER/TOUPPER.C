#include <stdio.h>
#include <ctype.h>

#define TMPFILE      "TMPUD$$$.TMP"
#define BUFFSIZE     32768U
#define USHORT       unsigned int

char szBuff [BUFFSIZE + 1];

main (USHORT argc, char *argv[])
   {
   FILE   *fpIn, *fpOut;
   USHORT i, u, uIdx, j =0;

   if (argc < 2)
      return puts ("TOUPPER  file data case converter. Params are files to cvt");

   for (i=1; i < argc; i++)
      {
      printf ("Converting %s\n", argv[i]);

      if (!(fpIn = fopen (argv[i], "rt")))
         {
         printf ("Unable to open input file :%s\n", argv[i]);
         continue;
         }

      if (!(fpOut = fopen (TMPFILE, "wt")))
         return puts ("Error opening output.");

      do
         {
         u = fread (szBuff, 1, BUFFSIZE, fpIn);
         for (uIdx=0; uIdx<u; uIdx++)
            szBuff[uIdx] = (char)toupper(szBuff[uIdx]);
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

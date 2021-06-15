/*
 *
 * lookat.c
 * Monday, 10/30/1995.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <GnuType.h>


int main (int argc, char *argv[])
   {
   char   b;
   USHORT w;
   ULONG  l;
   FILE *fp;
   ULONG ulOffset;

   if (argc != 4)
      return printf ("Usage: lookat file offset [b|w|l]");

   if (!(fp = fopen (argv[1], "rb")))
      return printf ("can't open file.");

   ulOffset = atol (argv[2]);
   fseek (fp, ulOffset, SEEK_SET);

   switch (toupper (*argv[3]))
      {
      case 'B':
         fread (&b, 1, 1, fp);
         printf ("b=%u (%x)\n", (USHORT)b, (USHORT)b);
         break;

      case 'L':
         fread (&l, 1, 1, fp);
         printf ("l=%lu (%lx)\n", l, l);
         break;

      case 'W':
      default :
         fread (&w, 1, 1, fp);
         printf ("w=%u (%x)\n", w, w);
         break;
      }


   return 0;
   }


/*
 *
 * clearnul.c
 * Monday, 8/22/1994.
 *
 */

#include <os2.h>
#include <stdio.h>
#include <io.h>
#include <stdlib.h>


void Usage (void)
   {
   printf ("This program clears nulls from a file and converts them to spaces.\n\n");
   printf ("USAGE: CLEARNUL File\n\n");
   printf ("WHERE: File - is the file to convert.\n\n");
   exit (1);
   }


int main (int argc, char *argv[])
   {
   ULONG l, ulLen, ulCvts;
   FILE  *fp;
   USHORT c;

   if (argc != 2)
      Usage ();
   if (!(fp = fopen (argv[1], "rb+")))
      return printf ("Unable to open file: %s", argv[1]);

   printf ("Working...\n");
   ulLen = filelength (fileno (fp));

   fseek (fp, 0, SEEK_SET);
   for (l=ulCvts=0; l<ulLen; l++)
      {
      fseek (fp, l, SEEK_SET);
      if (!(c = getc (fp)))
         {
         fseek (fp, l, SEEK_SET);
         putc (' ', fp);
         ulCvts++;
         }
      }
   fclose (fp);
   printf ("Done. %lu of %lu characters converted.\n", ulCvts, ulLen);
   return 0;
   }


#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHUNK 32767
#define SIZE  100000
char buf [32769];

typedef unsigned long int ULONG;



void dofile (char *pszFile, ULONG ulPiece)
   {
   char szBase [60];
   char szFile [60];
   FILE *fpIn;
   FILE *fpOut;
   char *p;
   ULONG  i;      /*-- file extension ---*/
   ULONG  iSiz;   /*-- read block size---*/
   ULONG  iBlock; /*--remaining siz   ---*/
   ULONG  iRet;   /*-- return from fwrite---*/

   i      = 1;
   strcpy (szBase, pszFile);

   /*--- Write extension in file 0 ---*/
   if (p=strchr (szBase, '.'))
      {
      *p='\0';
      sprintf (szFile, "%s.000", szBase);
      printf ("Writing %s\n", szFile);
      if (!(fpOut = fopen (szFile, "wb")))
         {
         printf ("Unable to open output.\n");
         exit (1);
         }
      fprintf (fpOut, "%s", p+1);
      fclose (fpOut);
      }

   if (!(fpIn = fopen (pszFile, "rb")))
      {
      printf ("Unable to open input.\n");
      exit (1);
      }


   while (1)
      {
      sprintf (szFile, "%s.%3.3d", szBase, i++);
      printf ("Writing %s\n", szFile);
      if (!(fpOut = fopen (szFile, "wb")))
         {
         printf ("Unable to open output.\n");
         exit (1);
         }

      iBlock = ulPiece;
      iRet   = 0;
      iSiz   = 0;

      while (iBlock && iRet == iSiz)
         {
         if (iBlock > CHUNK)
            iSiz = CHUNK;
         else
            iSiz = iBlock;
         iBlock = iBlock - iSiz;

         iRet = fread (buf, (size_t) 1, (size_t) iSiz, fpIn);
         fwrite (buf, (size_t)1, (size_t)iRet, fpOut);
         }
      fclose (fpOut);

      if (iRet != iSiz)
         return;
      }
   }



main (int argc, char *argv[])
   {
   ULONG   iPiece;
   ULONG   i;
   char *p;

   iPiece = SIZE;

   if (argc == 1)
      {
      printf ("USAGE: BSP [-Size] file [file ...]\n");
      exit (0);
      }

   for (i=1; argv[i]; i++)
      {
      p = argv[i];
      if (*p == '-')
         iPiece = atol (p+1);
      else
         dofile (p, iPiece);
      }
   return 0;
   }


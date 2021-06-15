#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHUNK 32767
#define SIZE  100000
char buf [32769];

#define NUM unsigned long int


void dofile (char *pszFile, NUM iPiece)
//   char *pszFile;
//   NUM iPiece;
   {
   char szBase [60];
   char szFile [60];
   FILE *fpIn;
   FILE *fpOut;
   char *p;
   NUM  i;      /*-- file extension ---*/
   NUM  iSiz;   /*-- read block size---*/
   NUM  iBlock; /*--remaining siz   ---*/
   NUM  iRet;   /*-- return from fwrite---*/

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

      iBlock = iPiece;
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



main (argc, argv)
   int  argc;
   char *argv[];
   {
   NUM  iPiece; /*-- out file size  ---*/
   NUM  i;
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
         iPiece = atoi (p+1);
      else
         dofile (p, iPiece);
      }
   return 0;
   }


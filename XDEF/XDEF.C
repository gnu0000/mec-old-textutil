#include <os2.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <conio.h>
#include "stuff.h"

//   bWrite = !!strstr (pszInFile, ".c") || strstr (pszInFile, ".C");


#define INCLUDE   "D:\\C\\include\\"
#define NHINCLUDE "..\\include\\"


typedef struct _def 
   {
   PSZ pszName;
   struct _def *Next;
   } DEFN;
typedef DEFN *PDEFN;


PDEFN pDefs = NULL;

char szTmp  [256];
char szTmp2 [256];







Error (PSZ psz1, PSZ psz2)
   {
   int c;

   while (kbhit ())
      c = getchar ();


   printf (psz1, psz2);
//   return exit (0);
   printf ("\nIs This True ? >");
   c = getchar ();
   return (toupper (c) == 'Y'); 
   }




PSZ Skip (PSZ pszStr, PSZ pszSkip)
   {
   while (strchr (pszSkip, *pszStr))
      pszStr++;
   return pszStr;
   }



/*
 * File Name to parse.
 * should include path if needed
 * 1st file is stripped of conditions
 */


PSZ GetFileName (PSZ pszLn)
   {
   char  szName[80];
   PSZ   pszTmp2;

   *szTmp = '\0';
   if (*(pszTmp2 = Skip (strcpy (szTmp2, pszLn), " \t")) == '<')
      strcpy (szTmp, INCLUDE);
   else
      strcpy (szTmp, NHINCLUDE);
   pszTmp2 = Skip (pszTmp2, " \t\"<");
   GetWord (&pszTmp2, szName, " \t>\"", 0, 0);
   strcat (szTmp, szName);
   return szTmp;
   }


PDEFN MakeDef (PSZ psz)
   {
   PDEFN pTmp, pNew;
   PSZ  pszLn;

   pszLn = strcpy (szTmp2, psz);
   GetWord (&pszLn, szTmp, " \t\n", 0, 0);
   pNew = (PDEFN) malloc (sizeof (DEFN));
   pNew->Next    = NULL;
   pNew->pszName = strdup (szTmp);
   pTmp = pDefs;
   if (!pTmp)
      pDefs = pNew;
   else
      {
      while (pTmp->Next)
         pTmp = pTmp->Next;
      pTmp->Next = pNew;
      }
   return pNew;
   }



BOOL RemoveDef (PSZ psz)
   {
   PDEFN pTmp1, pTmp2;
   PSZ  pszLn;

   pTmp1 = pTmp2 = pDefs;

   pszLn = strcpy (szTmp2, psz);
   GetWord (&pszLn, szTmp, " \t\n", 0, 0);
   if (!pTmp1)
      return FALSE;
   while (pTmp1)
      {
      if (!strcmp (szTmp, pTmp1->pszName))
         {
         if (pTmp1 == pDefs)
            pDefs = pTmp1->Next;
         else
            pTmp2->Next = pTmp1->Next;
         free (pTmp1);
         }
      pTmp2 = pTmp1;
      pTmp1 = pTmp1->Next;
      }
   }


PDEFN IsDefined (PSZ psz)
   {
   PDEFN pTmp1;
   PSZ   pszLn;

   pTmp1 = pDefs;
   pszLn = strcpy (szTmp2, psz);

   GetWord (&pszLn, szTmp, " \t\n", 0, 0);
   if (!pTmp1)
      return FALSE;
   while (pTmp1)
      {
      if (!strcmp (szTmp, pTmp1->pszName))
         return pTmp1;
      pTmp1 = pTmp1->Next;
      }
   return NULL;
   }



BOOL IsTrue (PSZ psz)
   {
   PSZ   pszLn;
   BOOL  bNot;

   pszLn = Skip (strcpy (szTmp2, psz), " \t");
   if (bNot = (*pszLn == '!'))
      pszLn++;
   GetWord (&pszLn, szTmp, " \t\n()", 0, 0);
   if (strcmp (szTmp, "defined"))
      return Error ("Unknown Test: %s", psz);
   pszLn = Skip (pszLn, " \t(");
   GetWord (&pszLn, szTmp, ")\n", 0, 0);
   pszLn = Skip (pszLn, " \t");
   if (*pszLn != ')')
      return Error ("Expecting ')': %s", psz);
   pszLn = Skip (pszLn++, " \t");
   if (*pszLn == '|' || *pszLn == '&')
      return Error ("Cant handle : %s", psz);
   StripString (szTmp, " \t");
   ClipString (szTmp, " \t");

   return (bNot ? !IsDefined (szTmp) : !!IsDefined (szTmp));
   }


/*
 * 1: skip until #else or #endif of same level
 * 2: skip until #endif of same level
 *
 */
BOOL SkipTo (FILE *fpIn, USHORT uCmd)
   {
   PSZ    pszLine;

   while (ReadLine (fpIn, szTmp, NULL, sizeof szTmp) != -1)
      {
      if (szTmp[0] != '#')
         continue;

      pszLine = Skip (szTmp+1, " \t");

           if (!strncmp (pszLine, "ifdef", 5))
         SkipTo (fpIn, 2);
      else if (!strncmp (pszLine, "ifndef", 6))
         SkipTo (fpIn, 2);
      else if (!strncmp (pszLine, "if", 2))
         SkipTo (fpIn, 2);
      else if (!strncmp (pszLine, "else", 4) && uCmd == 1)
         break;
      else if (!strncmp (pszLine, "endif", 5))
         break;
      else if (!strncmp (pszLine, "elif", 4) && uCmd == 1)
         {
         Error ("Cant Handle %s", szTmp);
         exit (1);
         }

      }
   return TRUE;
   }



void Xdef (PSZ pszInFile, BOOL bWrite)
   {
   FILE   *fpIn, *fpOut;
   char   szXTmp  [256];
   char   szLine [512];
   PSZ    pszLine;
   USHORT uIfLevel = 0;

   strcat (strcpy (szXTmp, pszInFile), "c");
   if (!(fpIn  = fopen (pszInFile, "rt")))
      Error ("Unable to open %s", pszInFile);
   if (bWrite && !(fpOut = fopen (szXTmp, "wt")))
      Error ("Unable to open %s", szXTmp);

   while (ReadLine (fpIn, szLine, NULL, sizeof szLine) != -1)
      {
      if (szLine[0] == '#')
         {
         printf ("%s> %s\n", pszInFile, szLine);
         pszLine = Skip (szLine+1, " \t");

         if (!strncmp (pszLine, "include", 7))
            {
            strcpy (szXTmp, GetFileName (pszLine+7));
            Xdef (szXTmp, FALSE);
            }

         else if (!strncmp (pszLine, "define", 6))
            MakeDef (pszLine + 6);

         else if (!strncmp (pszLine, "undef", 5))
            RemoveDef (pszLine + 5);

         else if (!strncmp (pszLine, "ifdef", 5))
            {
            if (!IsDefined (pszLine +5))
               SkipTo (fpIn, 1);
            continue;
            }

         else if (!strncmp (pszLine, "ifndef", 6))
            {
            if (!!IsDefined (pszLine +6))
               SkipTo (fpIn, 1);
            continue;
            }

         else if (!strncmp (pszLine, "if", 2))
            {
            if (!IsTrue (pszLine +2))
               SkipTo (fpIn, 1);
            continue;
            }

         else if (!strncmp (pszLine, "else", 4))
            {
            SkipTo (fpIn, 2);
            continue;
            }

         else if (!strncmp (pszLine, "endif", 5))
            continue;

         else 
            {
            Error ("Unknown preprocessor line : %s\n", szLine);
            exit (1);
            }
         }
      if (bWrite)
         {
         fputs (szLine, fpOut);
         fputc ('\n', fpOut);
         }
      }
   fclose (fpIn);
   if (bWrite)
      fclose (fpOut);
   }




main (int argc, char *argv[])
   {
   if (argc < 2)
      exit (0);

   Xdef (argv[1], TRUE);
   return 0;
   }



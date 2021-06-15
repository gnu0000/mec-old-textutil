/*
 * Stuff.c
 *
 *
 * Craig Fitzgerald
 *
 * This file is used by the EBS module
 *
 * This file provides various file and string functions
 * that i wish were part of the standard library set.
 *
 */


#include <os2.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "stuff.h"


ULONG GLOBALLINE = 1L;


/*
 * Struct for PushPos and PopPos
 */
typedef struct
   {
   FILE   *fp;
   fpos_t *pos;
   USHORT    i;
   } POSSTK;
POSSTK  *stk = NULL;



/***********************************************************************/
/****                                                               ****/
/***                        FILE FUNCTIONS                           ***/
/****                                                               ****/
/***********************************************************************/
//
// int getc2 (FILE *fp);
// int ungetc2 (int c, FILE *fp);
// int fputstr (PSZ psz, FILE *fp);
//
// USHORT ReadLine (*fpIn, pszStr, pszComments, uMaxLen)
// USHORT ReadTo   (*fpIn, pszStr, pszDelim, uMaxLen, bEatDelim)
// USHORT ReadWhile(*fpIn, pszStr, pszChars, uMaxLen, bEatDelim)
// USHORT ReadWord (*fpIn, pszWord, pszSkip, pszDelim, uMaxLen, bEatDelim)
// USHORT PushPos  (*fp)
// USHORT PopPos   (*fp, bSet)
//



int getc2 (FILE *fp)
   {
   int c;

   c = getc (fp);
   GLOBALLINE += (c == '\n');
   return c;
   }


int ungetc2 (int c, FILE *fp)
   {
   GLOBALLINE -= (c == '\n');
   return ungetc (c, fp);
   }



/*
 * -> Reads a line from fpIn.
 * -> Returns -1 if EOF before string could be read.
 * -> Returns the length of the line read in.
 * -> pszComments is a string of characters that are considered
 *     to be comment delimeters if at the beginning of the line. If the
 *     line has a comment char as its first non whitespace char. That line
 *     is skipped and the next is read. if \n is included in pszComments
 *     then blank lines are considered comments too.
 * -> if the line is longer than uMaxLen, the rest of the line is thrown
 *     away.
 * -> newline itself is read and discarded
 */
USHORT ReadLine (FILE   *fpIn,
                 PSZ    pszStr,
                 PSZ    pszComments,
                 USHORT uMaxLen)
   {
   USHORT i;
   int    c;

   if (pszStr)
      *pszStr = '\0';
   while (1)
      {
      i = 0;
      while ((c = getc2 (fpIn)) == (int)' ' || c == (int)'\t')
         if (i < uMaxLen && pszStr) pszStr[i++] = (char)c;

      if (c == EOF) return -1;

      if (!pszComments || strchr (pszComments, c)) break;
      while ((c = getc2 (fpIn)) != '\n' && c != EOF);
      }
   ungetc2 (c, fpIn);

   /*---This could be speeded up if this were done manually ---*/
   return i + ReadTo (fpIn, pszStr + i, "\n", uMaxLen - i, 1);

   while ((c = getc2 (fpIn)) != '\n' && c != EOF)
         if (i < uMaxLen && pszStr) pszStr[i++] = (char)c;

   if ((i -= (i && i == uMaxLen)) && pszStr)
      pszStr [i] = '\0';
   return i;

   }


/* -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads until it gets up to one of pszDelim or EOF
 * -> if uMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> of pszStr comes in NULL all chars to delim are discarded
 * -> If bEatDelim != 0 the delimiting char is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipto function
 */
USHORT ReadTo (FILE   *fpIn,
               PSZ    pszStr,
               PSZ    pszDelim,
               USHORT uMaxLen,
               BOOL   bEatDelim)
   {
   USHORT i = 0;
   int    c;

   if (pszStr) *pszStr = '\0';
   while (!strchr (pszDelim, c = getc2 (fpIn)) && c != EOF)
      if ((i < uMaxLen) && pszStr) 
         pszStr[i++] = (char)c;

   if (pszStr && (i -= (i && i == uMaxLen)))
      pszStr [i] = '\0';

   if (!bEatDelim) ungetc2 (c, fpIn);
   return i;
   }



/* -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads while char is one of pszChars or EOF
 * -> if uMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> If bEatDelim != 0 the delimiting char is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipwhile function
 */
USHORT ReadWhile (FILE *fpIn,
               PSZ pszStr,
               PSZ pszChars,
               USHORT  uMaxLen,
               USHORT  bEatDelim)
   {
   USHORT i = 0;
   int    c;

   while (strchr (pszChars, c = getc2 (fpIn)) && c != EOF)
         if (i < uMaxLen && pszStr) pszStr[i++] = (char)c;

   if (!bEatDelim) ungetc2 (c, fpIn);
   if (i -= (i && i == uMaxLen))
      pszStr [i] = '\0';
   return i;
   }





/* 0> returns -1 if it reads EOF before it gets a word.
 * 1> Skips pszSkip chars (normally spaces and tabs and maybe newlines)
 * 2> Reads word up to uMaxLen chars
 * 3> Skips remaining chars if uMaxLen Reached   
 * 4> If bEatDelim != 0 the delimiting char is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszWord may be null
 */
USHORT ReadWord (FILE *fpIn,
              PSZ pszWord,
              PSZ pszSkip,
              PSZ pszDelim,
              USHORT  uMaxLen,
              USHORT  bEatDelim)
   {
   if (ReadWhile (fpIn, NULL, pszSkip, 32767, 0) == -1)
      return -1;
   return ReadTo (fpIn, pszWord, pszDelim, uMaxLen, bEatDelim);
   }





/* -> pushes the file position for FILE
 * -> a separate stack is kept for each FILE
 *     so pushing different FILEs wil not cause a conflict
 * -> returns the depth of the stack for FILE
 */
USHORT PushPos (FILE *fp)
   {
   fpos_t pos;
   USHORT    i = 0;
   POSSTK  *tmpstk;

   if (fp == NULL) return 0;
   if (fgetpos (fp, &pos)) return 0;
//   if (stk == NULL)
//      {
//      stk = malloc (sizeof (POSSTK));
//      stk->fp = NULL;
//      }
   tmpstk = stk;
   while (tmpstk && tmpstk->fp)
      {
      if (tmpstk->fp == fp)
         {
         tmpstk->pos = realloc (tmpstk->pos, sizeof (fpos_t)*(++tmpstk->i));
         tmpstk->pos[tmpstk->i -1] = pos;
         return tmpstk->i;
         }
      tmpstk++, i++;
      }
   stk = realloc (tmpstk, sizeof (POSSTK) * (i + 2));
   stk[i+1].fp = NULL;

   stk[i].fp = fp;
   stk[i].pos = malloc (sizeof (fpos_t));
   stk[i].pos[0] = pos;
   stk[i].i = 1;
   return 1;
   }




/* -> pops the file position for FILE
 * -> id uSet != 0 the FILE position is set to the last push position
 *     otherwise the position is popped and discarded.
 * -> returns the depth of the stack for FILE or -1 if there is
 *     no stack for FILE
 */
USHORT PopPos (FILE *fp, BOOL bSet)
   {
   fpos_t pos;
   POSSTK  *tmpstk;

   if (fp == NULL || stk == NULL) return -1;
   tmpstk = stk;
   while (tmpstk->fp)
      {
      if (tmpstk->fp == fp)
         {
         if (!tmpstk->i) return -1;
         pos = tmpstk->pos[--tmpstk->i];
         tmpstk->pos = realloc (tmpstk->pos, sizeof (fpos_t)*(tmpstk->i));
         if (bSet) fsetpos (fp, &pos);
         return tmpstk->i;
         }
      tmpstk++;
      }
   return -1;
   }



USHORT Peek (FILE *fp)
   {
   return (ungetc2 (getc2 (fp), fp));
   }




/* this is like fputs except
 * a null is ok
 * a \0 is written at the end
 */
int fputstr (PSZ psz, FILE *fp)
   {
   USHORT uRet;

   uRet = (psz ? fputs (psz, fp) : 0);
   fputc ('\0', fp);   
   return uRet;
   }





/*
 * This function reads in a csv fiels from the stream
 * if fp comes in EOF 1 is returned.
 * supporting quotes are stripped
 * LastItem determines if fiels should be terminated by a
 * comma or an \n | \EOF.  If the delimeter is of the incorrect
 * type, 2 is returned.
 * pszStr may be null, in which case the field is discarded.
 *
 * return values:
 *   0 - ok
 *   1 - eof on fp
 *   2 - comma expected
 *   3 - eol/eof expected
 */
USHORT ReadCSVField (FILE *fp, PSZ pszStr, USHORT uMaxLen, BOOL bLastItem)
   {
   BOOL   bQuoting = FALSE;
   USHORT i = 0;
   int    c;

   while (TRUE)
      {
      c = getc2 (fp);
      if (c == EOF || c == '\n'  || c == ',' && !bQuoting)
         break;
      if (++i >= uMaxLen)
         {
         ReadTo (fp, NULL, (bLastItem ? "\n" : ",\n"), 32767, TRUE);
         c = (bLastItem ? '\n' : ',');
         break;
         }

      if (c == '"')
         {
         if ('"' == (c = getc2 (fp)))
            {
            if (pszStr) *pszStr++ = (char)c;
            continue;
            }
         ungetc2 (c, fp);
         bQuoting = !bQuoting;
         continue;
         }
      if (pszStr) *pszStr++ = (char)c;
      }
   if (pszStr) 
      *pszStr = '\0';

   if (!i && !bLastItem && c == EOF)
      return 1;

   if (c == ',' && !bLastItem)
      return 0;
   if ((c == '\n' || c == EOF) && bLastItem)
      return 0;
   return (bLastItem ? 3 : 2);
   }




/***********************************************************************/
/****                                                               ****/
/***                       STRING FUNCTIONS                          ***/
/****                                                               ****/
/***********************************************************************/
//
//
//USHORT GetWord (PSZ *ppsz, ...
//USHORT EatWord (PSZ *ppszStr, PSZ pszTarget)
//PSZ ClipString (PSZ pszStr, PSZ pszClipList)
//PSZ StripString (PSZ pszStr, PSZ pszStripList)
//PSZ ExtractString (PSZ pszStr, PSZ pszExtractList)



/*
 * Eats a word from the front of a string and returns
 * that word and the rest of the string.
 *
 * 1> skip spaces and tabs
 * 2> read to delimeter character (exclusive)
 * 3> word ends at char before delim, string starts at char at/after delim
 * 4> returns word read and remaining string as parameters
 * 5> delimeter char is returns as fn val.
 *
 * returns -1 when there is nothing to return
 */
USHORT GetWord (PSZ     *ppsz,
                PSZ     pszWord,
                PSZ     pszDelimeters,
                USHORT  bHasQuotes,
                USHORT  bEatDelim)
   {
   USHORT bInQuotes;

   bInQuotes = FALSE;

   *pszWord = '\0';
//   StripString (*ppsz, " \t");
   StripString (*ppsz, " ");

   if (*ppsz == NULL || **ppsz == '\0')
      return -1;

   while (**ppsz != '\0' &&
          (bInQuotes || strchr (pszDelimeters, **ppsz) == NULL))
      {
      if (bHasQuotes)
         if (**ppsz == '"')
            bInQuotes = !bInQuotes;

      *(pszWord++) = *((*ppsz)++);
      }
   *pszWord = '\0';

   if (bEatDelim)
      return (int) (**ppsz ? *((*ppsz)++): 0);
   else
      return (int) (**ppsz ? *(*ppsz): 0);
   }



/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
USHORT EatWord (PSZ *ppszStr, PSZ pszTarget)
   {
   char szWord [512];

   if ((GetWord (ppszStr, szWord, " ,\t\n", 0, 1)) == -1)
      return 2;
   return !!stricmp (pszTarget, szWord);
   }



/*
 * Removes trailing chars from str
 */
PSZ ClipString (PSZ pszStr, PSZ pszClipList)
   {
   int i;

   i = strlen (pszStr);
   while (i >= 0 && strchr (pszClipList, pszStr[i]) != NULL)
      pszStr[i--] = '\0';
   return pszStr;
   }



/*
 * Removes leading chars from str
 * Moves memory, not ptr;
 */
PSZ StripString (PSZ pszStr, PSZ pszStripList)
   {
   USHORT uLen, i = 0;

   if (!(uLen = strlen (pszStr)))
      return pszStr;
   while (i < uLen && strchr (pszStripList, pszStr[i]) != NULL)
      i++;
   if (i)
      memmove (pszStr, pszStr + i, uLen - i + 1);
   return pszStr;
   }



/*
 * Removes chars from anywhere in string
 *
 */
PSZ ExtractString (PSZ pszStr, PSZ pszExtractList)
   {
   PSZ pszPtrSrc, pszPtrDest;

   if (pszStr == NULL)
      return NULL;

   pszPtrSrc = pszPtrDest = pszStr;
   while (*pszPtrSrc != '\0')
      {
      if (strchr (pszExtractList, *pszPtrSrc) == NULL)
         *pszPtrDest++ = *pszPtrSrc;
      pszPtrSrc++;
      }
   *pszPtrDest = '\0';
   return pszStr;
   }


BOOL BlankLine (PSZ pszLine)
   {
   for (; *pszLine == ' ' || *pszLine == '\t'; pszLine++)
      ;
   return !*pszLine;
   }


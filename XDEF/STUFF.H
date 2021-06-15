/*
 * Stuff.h
 *
 * (C) 1992 Info Tech Inc.
 * This file is part the EBS module
 */


extern ULONG GLOBALLINE;

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
// USHORT ReadLine (*fpIn, pszStr, pszComments, iMaxLen)
// USHORT ReadTo   (*fpIn, pszStr, pszDelim, iMaxLen, bEatDelim)
// USHORT ReadWhile(*fpIn, pszStr, pszChars, iMaxLen, bEatDelim)
// USHORT ReadWord (*fpIn, pszWord, pszSkip, pszDelim, iMaxLen, bEatDelim)
// USHORT PushPos  (*fp)
// USHORT PopPos   (*fp, bSet)
// USHORT ReadCSVField (FILE *fp, PSZ pszStr, USHORT uMaxLen, BOOL bLastItem);
//


/* just like thier c lib counterparts except that they update
 * the GLOBALLINE variable on a \n
 */
int getc2 (FILE *fp);
int ungetc2 (int c, FILE *fp);


/*
 * -> Reads a line from fpIn.
 * -> Returns -1 if EOF before string could be read.
 * -> Returns the length of the line read in.
 * -> pszComments is a string of characters that are considered
 *     to be comment delimeters if at the beginning of the line. If the
 *     line has a comment char as its first non whitespace char. That line
 *     is skipped and the next is read. if \n is included in pszComments
 *     then blank lines are considered comments too.
 * -> if the line is longer than iMaxLen, the rest of the line is thrown
 *     away.
 * -> newline itself is read and discarded
 */
USHORT ReadLine (FILE *fpIn,
              PSZ pszStr,
              PSZ pszComments,
              USHORT  iMaxLen);




/* -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads until it gets up to one of pszDelim or EOF
 * -> if iMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> If bEatDelim != 0 the delimiting char is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipto function
 */
USHORT ReadTo (FILE *fpIn,
            PSZ pszStr,
            PSZ pszDelim,
            USHORT  iMaxLen,
            USHORT  bEatDelim);


/* -> Returns count of chars read into pszStr
 * -> Returns -1 if fpIn comes in with EOF
 * -> Reads while char is one of pszChars or EOF
 * -> if iMaxLen to small, the remaining chars are discarded
 * -> similar to readline.
 * -> If bEatDelim != 0 the delimiting char is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszStr may be NULL, in which case this function performs
 *     as a skipwhile function
 */
USHORT ReadWhile (FILE *fpIn,
               PSZ pszStr,
               PSZ pszChars,
               USHORT  iMaxLen,
               USHORT  bEatDelim);




/* 0> returns -1 if it reads EOF before it gets a word.
 * 1> Skips pszSkip chars (normally spaces and tabs and maybe newlines)
 * 2> Reads word up to iMaxLen chars
 * 3> Skips remaining chars if iMaxLen Reached   
 * 4> If bEatDelim != 0 the delimiting char is read and discarded.
 *     otherwise the delimeter itself is not read.
 * -> pszWord may be null
 */
USHORT ReadWord (FILE *fpIn,
              PSZ pszWord,
              PSZ pszSkip,
              PSZ pszDelim,
              USHORT  iMaxLen,
              USHORT  bEatDelim);




/* -> pushes the file position for FILE
 * -> a separate stack is kept for each FILE
 *     so pushing different FILEs wil not cause a conflict
 * -> returns the depth of the stack for FILE
 */
USHORT PushPos (FILE *fp);




/* -> pops the file position for FILE
 * -> id uSet != 0 the FILE position is set to the last push position
 *     otherwise the position is popped and discarded.
 * -> returns the depth of the stack for FILE or -1 if there is
 *     no stack for FILE
 */
USHORT PopPos (FILE *fp, USHORT bSet);


USHORT Peek (FILE *fp);


/* this is like fputs except
 * a null is ok
 * a \0 is written at the end
 */
int fputstr (PSZ psz, FILE *fp);



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
 *   2 - incorrect terminator
 *
 */
USHORT ReadCSVField (FILE *fp, PSZ pszStr, USHORT uMaxLen, BOOL bLastItem);

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
//BOOL BlankLine (PSZ pszLine)



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
USHORT GetWord (PSZ *ppsz,
             PSZ pszWord,
             PSZ pszDelimeters,
             USHORT  bHasQuotes,
             USHORT  bEatDelim);


/*
 * 0 = match
 * 1 = no match
 * 2 = end of string
 */
USHORT EatWord (PSZ *ppszStr, PSZ pszTarget);


/*
 * Removes trailing chars from str
 */
PSZ ClipString (PSZ pszStr, PSZ pszClipList);



/*
 * Removes leading chars from str
 * Moves memory, not ptr;
 */
PSZ StripString (PSZ pszStr, PSZ pszStripList);



/*
 * Removes chars from anywhere in string
 *
 */
PSZ ExtractString (PSZ pszStr, PSZ pszExtractList);





BOOL BlankLine (PSZ pszLine);


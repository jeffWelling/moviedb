/*============================================================================
 *
 *  Program: mkdb.c 
 *
 *  Version: 3.24
 *
 *  Purpose: make databases from list files 
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996-2004 The Internet Movie Database Inc. 
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 *
 *  Options:
 *         -acr  update actors database
 *         -acs  update actress database
 *         -dir  update directors database
 *       -write  update writers database  
 *        -comp  update composers database
 *        -cine  update cinematographers database
 *         -mrr  update movie ratings database
 *         -aka  update alternative titles database
 *       -movie  update movies database
 *        -triv  update trivia database
 *        -plot  update plot summaries database
 *       -crazy  update crazy credits database
 *         -bio  update biographies database
 *        -edit  update editors database
 *      -prodes  update production designers database
 *     -costdes  update costume designers database
 *       -votes  update votes database from votelog file
 *        -goof  update goofs database
 *       -quote  update quotes database
 *      -strack  update soundtracks database
 *       -prdcr  update producers database
 *        -time  update running times database
 *        -misc  update miscellaneous filmographies database
 *        -naka  update alternative names database
 *        -cert  update certificates database
 *       -genre  update genres database
 *     -keyword  update keywords database
 *      -prodco  update production companies database
 *       -color  update color information database
 *         -mix  update sound mix database
 *       -cntry  update countries database
 *         -rel  update release date database
 *         -loc  update locations database
 *         -lit  update literature database
 *        -tech  update technical database
 *        -link  update movie links database
 *         -tag  update tag lines database
 *     -castcom  update complete cast database
 *        -lang  update language database
 *        -vers  update alternate versions database
 *       -sfxco  update special effects companies database
 *         -bus  update business database
 *          -ld  update laserdisc database
 *        -dist  update distributors database
 *     -crewcom  update complete crew database
 * -akaf <file>  update alternative titles from a local file
 *      -create  generate name, title and attribute indices and check all
 *                 database files exist
 *       -debug  show debugging information
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include "moviedb.h"
#include "dbutils.h"

#define MKDB_USAGE1 "usage: mkdb [-acr|-acs|-dir|-write|-comp|-cine|-edit|-prodes|-costdes|-prdcr|"
#define MKDB_USAGE2 "            -misc|-movie|-time|-plot|-bio|-triv|-crazy|-goof|-quote|-strack|"
#define MKDB_USAGE3 "            -cert|-genre|-keyword|-mrr|-votes|-aka|-akaf <file>|-naka|-prodco|"
#define MKDB_USAGE4 "            -color|-mix|-cntry|-rel|-loc|-lit|-tech|-link|-tag|-castcom|-vers|"
#define MKDB_USAGE5 "            -lang|-sfxco|-bus|-ld|-dist|-crewcom] [-m -debug -create]"

static int debugFlag = FALSE ;

static struct titleKeyOffset *sharedTitleIndex ;
static size_t sharedTitleIndexSize ;
static struct attrIndexRec *attributeIndex ;
static size_t attributeIndexSize ;

int titleIndexRecSort (struct titleIndexRec *t1, struct titleIndexRec *t2)
{
  return ( caseCompare(t1->title, t2->title) ) ;
}

TitleID readMoviesList (struct titleIndexRec *titles)
{ 
  FILE *listFp ; 
  char *tptr ;
  char line [ MXLINELEN ] ; 
  int  inMovie = FALSE ; 
  TitleID  indexKey = 0 ;
  
  listFp = openFile ( MOVIELIST ) ;
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && ( strncmp ( line, "-------", 5 ) == 0 ) ) 
      break ;
    else if ( inMovie && *line == '"' ) 
    {
       if ( ( tptr = strchr ( line, '\t' ) ) != NULL )
       {
          *tptr = '\0' ;
          titles [ indexKey ] . title = duplicateString ( line ) ; 
          titles [ indexKey ] . titleKey = indexKey ;
          indexKey++ ;
       }
    }
    else 
      if ( strncmp ( line, "MOVIES LIST", 11 ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }

  (void) fseek ( listFp, 0L, SEEK_SET ) ;
  inMovie = FALSE ;
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && *line == '"' ) 
      break ;
    else if ( inMovie ) 
    {
       if ( ( tptr = strchr ( line, '\t' ) ) != NULL )
       {
          *tptr = '\0' ;
          titles [ indexKey ] . title = duplicateString ( line ) ; 
          titles [ indexKey ] . titleKey = indexKey ;
          indexKey++ ;
       }
    }
    else 
      if ( strncmp ( line, "MOVIES LIST", 11 ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }
 
  (void) fclose ( listFp ) ; 

  /* the movies.list is sorted slightly differently concerning special characters */ 
  /* than the caseCompare-function that is used for binary search */
  (void) qsort ( (void*) titles, (size_t) indexKey, sizeof ( struct titleIndexRec ), (int (*) (const void*, const void*)) titleIndexRecSort ) ;

  return ( indexKey ) ;
} 


int nameKeyOffsetSort (struct nameKeyOffset *n1, struct nameKeyOffset *n2)
{
  return ( n1 -> nameKey - n2 -> nameKey ) ;
}


int titleKeyOffsetSort (struct titleKeyOffset *t1, struct titleKeyOffset *t2)
{
  return ( t1 -> titleKey - t2 -> titleKey ) ;
}


int attrKeyOffsetSort (struct attrKeyOffset *a1, struct attrKeyOffset *a2)
{
  return ( a1 -> attrKey - a2 -> attrKey ) ;
}


void writeTitleIndexKey ( TitleID titleCount )
{ 
  TitleID i, count = 0 ;
  FILE *indexFp, *keyFp ; 
  struct titleKeyOffset *titleIndex ;
  long lastOffset = 0 ;
  char line [ MXLINELEN ] ; 
  char *p ;

  titleIndex = (struct titleKeyOffset *) calloc ( titleCount + 5, sizeof ( struct titleKeyOffset ) ) ;
  if ( titleIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate titles index" ) ;

  keyFp = openFile ( TITLEKEY ) ;
  indexFp = writeFile ( TITLEIDX ) ;

  while ( fgets ( line, MXLINELEN, keyFp ) != NULL ) 
  {
     if ( ( p = strchr ( line, FSEP ) ) == NULL )
       moviedbError ( "mkdb: badly formed title key file" ) ;
     titleIndex [ count ] . titleKey = strtol ( ++p, (char **) NULL, 16) ;
     titleIndex [ count++ ] . offset = lastOffset ;
     lastOffset = ftell ( keyFp ) ;
  }

  (void) qsort ( (void*) titleIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  for ( i = 0 ; i < count ; i++ )
    putFullOffset ( titleIndex [ i ] . offset, indexFp ) ;

  (void) fclose ( keyFp ) ;
  (void) fclose ( indexFp ) ;

  free ( (void*) titleIndex ) ;
}


void writeAttrIndexKey ( AttributeID attrCount )
{ 
  AttributeID i, count = 0 ;
  FILE *indexFp, *keyFp ; 
  struct attrKeyOffset *attrIndex  ;
  long lastOffset = 0 ;
  char line [ MXLINELEN ] ; 
  char *p ;

  attrIndex = (struct attrKeyOffset *) calloc ( attrCount + 5, sizeof ( struct attrKeyOffset ) ) ;
  if ( attrIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate attributes index" ) ;

  keyFp = openFile ( ATTRKEY ) ;
  indexFp = writeFile ( ATTRIDX ) ;

  while ( fgets ( line, MXLINELEN, keyFp ) != NULL ) 
  {
     if ( ( p = strchr ( line, FSEP ) ) == NULL ) {
       printf ( "%s\n", line ) ;
       moviedbError ( "mkdb: badly formed key file" ) ;
     }
     attrIndex [ count ] . attrKey = strtol ( ++p, (char **) NULL, 16) ;
     attrIndex [ count++ ] . offset = lastOffset ;
     lastOffset = ftell ( keyFp ) ;
  }

  (void) qsort ( (void*) attrIndex, (size_t) count, sizeof ( struct attrKeyOffset ), (int (*) (const void*, const void*)) attrKeyOffsetSort ) ;

  for ( i = 0 ; i < count ; i++ )
    putFullOffset ( attrIndex [ i ] . offset, indexFp ) ;

  (void) fclose ( keyFp ) ;
  (void) fclose ( indexFp ) ;
}


void writeNameIndexKey ( NameID nameCount )
{ 
  NameID i, count = 0 ;
  FILE *indexFp, *keyFp ; 
  struct nameKeyOffset *namesIndex ;
  long lastOffset = 0 ;
  char line [ MXLINELEN ] ; 
  char *p ;

  namesIndex = (struct nameKeyOffset *) calloc ( nameCount + 5, sizeof ( struct nameKeyOffset ) ) ;
  if ( namesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate names index" ) ;

  keyFp = openFile ( NAMEKEY ) ;
  indexFp = writeFile ( NAMEIDX ) ;

  while ( fgets ( line, MXLINELEN, keyFp ) != NULL ) 
  {
     if ( ( p = strchr ( line, FSEP ) ) == NULL )
       moviedbError ( "mkdb: key file corrupt" ) ;
     namesIndex [ count ] . nameKey = strtol ( ++p, (char **) NULL, 16) ;
     namesIndex [ count++ ] . offset = lastOffset ;
     lastOffset = ftell ( keyFp ) ;
  }

  (void) qsort ( (void*) namesIndex, (size_t) count, sizeof ( struct nameKeyOffset ), (int (*) (const void*, const void*)) nameKeyOffsetSort ) ;

  for ( i = 0 ; i < count ; i++ )
    putFullOffset ( namesIndex [ i ] . offset, indexFp ) ;

  (void) fclose ( keyFp ) ;
  (void) fclose ( indexFp ) ;
  free ( (void*) namesIndex ) ;
}


void writeTitleAlphaKey ( TitleID titleCount, struct titleIndexRec *titles )
{ 
  TitleID i ;
  FILE *indexFp ; 

  indexFp = writeFile ( TITLEKEY ) ;

  for ( i = 0 ; i < titleCount ; i++ )
    (void) fprintf ( indexFp , "%s|%x\n", titles [ i ] . title, titles [ i ] . titleKey ) ;

  (void) fclose ( indexFp ) ;
}


void writeAttrAlphaKey ( AttributeID attrCount )
{ 
  AttributeID i ;
  FILE *indexFp ; 

  indexFp = writeFile ( ATTRKEY ) ;

  for ( i = 0 ; i < attrCount ; i++ )
    (void) fprintf ( indexFp , "%s|%lx\n", attributeIndex [ i ] . attr, attributeIndex [ i ] . attrKey ) ;

  (void) fclose ( indexFp ) ;
}


TitleID readTitleAlphaKey ( struct titleIndexRec *titles )
{
  FILE *listFP ; 
  char line [ MXLINELEN ] ; 
  char *p ;
  TitleID i = 0 ;

  if ( isReadable ( TITLEKEY ) )
  {
    listFP = openFile ( TITLEKEY ) ;
    while ( fgets ( line, MXLINELEN, listFP ) != NULL ) 
    {
      if ( ( p = strchr ( line, FSEP ) ) == NULL )
        moviedbError ( "mkdb: titles key file corrupt" ) ;
      *p = '\0' ;
      titles [ i ] . title = duplicateString ( line ) ;
      titles [ i++ ] . titleKey = strtol ( p + 1, (char **) NULL, 16) ;
      if ( i >= MAXTITLES ) 
        moviedbError ( "mkdb: too many titles -- increase MAXTITLES" ) ;
    }
   (void) fclose ( listFP ) ;
  }
  return ( i ) ; 
}


AttributeID readAttrAlphaKey ( void )
{
  struct attrIndexRec *attributes = attributeIndex ;
  FILE *listFP ; 
  char line [ MXLINELEN ] ; 
  char *p ;
  AttributeID i = 0 ;

  if ( isReadable ( ATTRKEY ) )
  {
    listFP = openFile ( ATTRKEY ) ;
    while ( fgets ( line, MXLINELEN, listFP ) != NULL ) 
    {
      if ( ( p = strchr ( line, FSEP ) ) == NULL )
        moviedbError ( "mkdb: attributes key file corrupt" ) ;
      *p = '\0' ;
      attributes [ i ] . attr = duplicateString ( line ) ;
      attributes [ i++ ] . attrKey = strtol ( p + 1, (char **) NULL, 16) ;
      if ( i >= attributeIndexSize ) {
	attributeIndexSize += ATTRGROW ;
	attributes = realloc ( attributeIndex, sizeof ( struct attrIndexRec ) * attributeIndexSize ) ;
	if ( attributes == NULL )
	  moviedbError ( "mkdb: not enough memory to generate attributes index" ) ;
	attributeIndex = attributes ;
      }
    }
   (void) fclose ( listFP ) ;
  }
  return ( i ) ; 
}


struct titleIndexRec *findTitleKeyPos (char *str, struct titleIndexRec *titles, TitleID titleCount, TitleID *insert)
{
  TitleID lower = 0, upper = titleCount, mid = 0 ;
  int found = FALSE, compare = 0 ;

  if ( titleCount == 0 )
  {
    *insert = 0 ;
    return ( NULL ) ;
  }

  do
  {
     mid = ( upper + lower ) / 2 ;
     compare = caseCompare ( titles [ mid ] . title, str ) ;
     if ( compare == 0 )
       found = TRUE ;
    else if ( compare < 0 )
      lower = mid + 1 ;
    else
      upper = mid ;
  }
  while ( !found && lower < upper ) ;

  if ( found )
    return ( &( titles [ mid ] ) ) ;
  else
    if ( compare < 0 )
      *insert = mid + 1;
    else
      *insert = mid ;
  return ( NULL ) ;
}


TitleID titleKeyLookup (char *str, struct titleIndexRec *titles, TitleID *titleCount )
{
   struct titleIndexRec *matched ;
   TitleID indexKey = *titleCount, insert, i ;
   char *ptr ;

   if ( ( ptr = strchr ( str, FSEP ) ) != NULL )
     *ptr = '\0' ;

   if ( indexKey == 0 )
   {
     titles [ 0 ] . title = duplicateString ( str ) ; 
     titles [ 0 ] . titleKey = 0 ;
     *titleCount = 1 ;
     return ( 0 ) ;
   }
     
   matched = findTitleKeyPos ( str, titles, indexKey, &insert ) ;
   if ( matched != NULL )
     return ( matched -> titleKey ) ;
   else
   {
     for ( i = indexKey ; i > insert ; i-- ) 
       titles [ i ] = titles [ i - 1 ] ; 
     titles [ insert ] . title = duplicateString ( str ) ; 
     titles [ insert ] . titleKey = indexKey ;
     *titleCount = *titleCount + 1 ;
     if ( *titleCount >= MAXTITLES ) 
       moviedbError ( "mkdb: too many titles -- increase MAXTITLES" ) ;
     return ( indexKey ) ;
   }
}

TitleID titleKeyLookupReadOnly (char *str, struct titleIndexRec *titles, TitleID titleCount)
{
  struct titleIndexRec *matched ;
  TitleID dummy, i ;
  char *ptr ;

  if ( ( ptr = strchr ( str, FSEP ) ) != NULL )
    *ptr = '\0' ;

  if ( titleCount == 0 )
    return ( NOTITLE );
     
  matched = findTitleKeyPos ( str, titles, titleCount, &dummy ) ;
  if ( matched != NULL )
    return ( matched -> titleKey ) ;
  else
    return ( NOTITLE );
}

struct attrIndexRec *findAttrKeyPos (char *attr, struct attrIndexRec *attributes, AttributeID attrCount, AttributeID *insert)
{
  AttributeID lower = 0, upper = attrCount, mid = 0 ;
  int found = FALSE, compare = 0 ;

  if ( attrCount == 0 )
  {
    *insert = 0 ;
    return ( NULL ) ;
  }

  do
  {
     mid = ( upper + lower ) / 2 ;
     compare = caseCompare ( attributes [ mid ] . attr, attr ) ;
     if ( compare == 0 )
       found = TRUE ;
    else if ( compare < 0 )
      lower = mid + 1 ;
    else
      upper = mid ;
  }
  while ( !found && lower < upper ) ;

  if ( found )
    return ( &( attributes [ mid ] ) ) ;
  else
    if ( compare < 0 )
      *insert = mid + 1;
    else
      *insert = mid ;
  return ( NULL ) ;
}


AttributeID attrKeyLookup (char *attr, AttributeID *attrCount)
{
   struct attrIndexRec *attributes = attributeIndex ;
   struct attrIndexRec *matched ;
   AttributeID attrKey = *attrCount, insert, i ;
   char *ptr ;

   if ( ( ptr = strchr ( attr, FSEP ) ) != NULL )
     *ptr = '\0' ;

   if ( attrKey == 0 )
   {
     attributes [ 0 ] . attr = duplicateString ( attr ) ; 
     attributes [ 0 ] . attrKey = 0 ;
     *attrCount = 1 ;
     return ( 0 ) ;
   }
     
   matched = findAttrKeyPos ( attr, attributes, attrKey, &insert ) ;
   if ( matched != NULL )
     return ( matched -> attrKey ) ;
   else
   {
     for ( i = attrKey ; i > insert ; i-- ) 
       attributes [ i ] = attributes [ i - 1 ] ; 
     attributes [ insert ] . attr = duplicateString ( attr ) ; 
     attributes [ insert ] . attrKey = attrKey ;
     *attrCount = *attrCount + 1 ;
     if ( *attrCount >= attributeIndexSize )  {
	attributeIndexSize += ATTRGROW ;
	attributes = realloc ( attributeIndex, sizeof ( struct attrIndexRec ) * attributeIndexSize ) ;
	if ( attributes == NULL )
	  moviedbError ( "mkdb: not enough memory to generate attributes index" ) ;
	attributeIndex = attributes ;
     }
     return ( attrKey ) ;
   }
}


int yearDataSort (struct yearData *y1, struct yearData *y2)
{
  return ( y1 -> titleKey - y2 -> titleKey ) ;
}


TitleID processMoviesList (struct titleIndexRec *titles, TitleID *titleCount, AttributeID *attrCount)
{
  char  *yrptr, *attrptr, *p ;
  FILE  *listFp, *dbFp ; 
  struct yearData *years ;
  char  line [ MXLINELEN ] ; 
  int   inMovie = FALSE ;
  TitleID count = 0, i ;

  years = malloc ( MAXTITLES * sizeof (struct yearData) ) ;
  if ( ! years )
    moviedbError ( "mkdb: cannot allocate years structure" ) ;

  if ( *titleCount == 0 )
    *titleCount = readMoviesList ( titles ) ;

  listFp = openFile ( MOVIELIST ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && ( strncmp ( line, "-------", 5 ) == 0 ) ) 
      break ;
    else if ( inMovie ) 
    {
       if ( ( yrptr = strchr ( line, '\t' ) ) != NULL )
       {
          *yrptr = '\0' ;
          if ( debugFlag )
            (void) printf ( "%s\n", line ) ;
          years [ count ] . titleKey = titleKeyLookup ( line, titles, titleCount ) ;

          while ( *++yrptr == '\t' ) ;
          if ( ( attrptr = strchr ( yrptr, '\t' ) ) != NULL )
          {
            attrptr++ ;
            if ( ( p = strchr ( attrptr, '\n' ) ) == NULL )
              moviedbError ( "mkdb: movies list corrupt" ) ;
            *p = '\0' ;
            years [ count ] . attrKey = attrKeyLookup ( attrptr, attrCount ) ;
          }
          else
            years [ count ] . attrKey = NOATTR ;

          if ( *(yrptr+4) == '-' )
          {
            years [ count ] . yrfrom = atoi ( yrptr ) ;
            years [ count ] . yrto =  atoi ( yrptr + 5 ) ;
          }
          else
          {
            years [ count ] . yrfrom = atoi ( yrptr ) ;
            years [ count ] . yrto = 0 ;
          }
          if ( years [ count ] . yrfrom < 1890 || years [ count ] . yrfrom > 2100 )
            years [ count ] . yrfrom = 0 ;
          if ( years [ count ] . yrto < 1890 || years [ count ] . yrto > 2100 )
            years [ count ] . yrto = 0 ;
          count++ ;     
	  if (count >= MAXTITLES)
	    moviedbError ( "mkdb: too many titles -- increase MAXTITLES" );
       }
    }
    else 
      if ( strncmp ( line, "MOVIES LIST", 11 ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }
  
  (void) fclose ( listFp ) ;

  (void) qsort ( (void*) years, (size_t) count, sizeof ( struct yearData ), (int (*) (const void*, const void*)) yearDataSort ) ;

  dbFp = writeFile ( MOVIEDB ) ;

  for ( i = 0 ; i < count ; i++ )
  {
    putTitle ( years [ i ] . titleKey, dbFp ) ;
    putInt ( years [ i ] . yrfrom, dbFp ) ;
    putInt ( years [ i ] . yrto, dbFp ) ;
    putAttr ( years [ i ] . attrKey, dbFp ) ;
  }
  (void) fclose ( dbFp ) ;
  return ( count ) ;
}


char *splitAttribute (char *title)
{
  char   *attr = NULL ;
  char   *bracketptr ;
  char   *akaptr ;
  int    nest ;

  if ( ( akaptr = strstr ( title, "(aka " ) ) != NULL )
  {
    *(akaptr - 1) = '\0' ;
    if ( *(akaptr - 2) == ' ' )
      *(akaptr - 2) = '\0' ;
    nest = 1 ; 
    while ( *akaptr++ && nest )
      if ( *akaptr == '(' )
        nest++;
      else if ( *akaptr == ')' )
        nest--;
    if ( *akaptr == '\0' ) 
      return ( NULL ) ;
    else
    {
      if ( *(akaptr + 1) == ' ' )
        return ( akaptr + 2 ) ;
      else
        return ( akaptr + 1 ) ;
    }
  }
  else if ( ( attr = strstr ( title, "  (" ) ) != NULL )
  {
     *attr = '\0' ;
     attr += 2 ;
  }
  return ( attr ) ;
}


int isValidListLine (char *line)
{
   if ( *line == '\0' )
     return ( FALSE ) ;

   if ( strncmp ( line, "\t\t\t", 3 ) == 0 )
     return ( TRUE ) ;

   if ( strchr ( line, '\t' ) != NULL && *line != '\t'  )
     return ( TRUE ) ;

   return ( FALSE ) ;

}


char *splitChar ( char *line ) 
{
  char *ptr, *endptr ;

  if ( ( ptr = strchr ( line, '[' ) ) == NULL )
    return ( NULL ) ;
  else
    if ( ( endptr = strrchr ( ptr, ']' ) ) == NULL )
      return ( NULL ) ;

/* string length is stored in one byte; chop it if it's too long */
  if ( ( endptr - ptr ) > UCHAR_MAX ) {
    endptr = ptr + UCHAR_MAX ;
  }
  *endptr = '\0' ;
  *(ptr-2) = '\0' ;
  return ( ++ptr ) ;
}


int splitPosition ( char *line ) 
{
  char *ptr, *endptr ;
  int retval ;

  if ( ( ptr = strchr ( line, '<' ) ) == NULL )
    return ( NOPOS ) ;
  else
    if ( ( endptr = strrchr ( ptr, '>' ) ) == NULL )
      return ( NOPOS ) ;

  *endptr = '\0' ;
  *(ptr-2) = '\0' ;
  retval = atoi ( ++ptr  ) ;
  if ( retval == 0 || retval > MAXPOS )
    return ( NOPOS ) ;
  else
    return ( retval ) ;
}

    
int readCastEntry (struct castFilmography *entry, FILE *stream)
{
  int i, len ;

  entry -> nameKey = getName ( stream ) ;
  if ( feof ( stream ) )
    return ( FALSE ) ;
  getFilmographyCounts ( stream, &(entry -> noWithAttr), &(entry -> noWithoutAttr) ) ;
  for ( i = 0 ; i < entry -> noWithAttr ; i++ )
  {
    entry -> withAttrs [ i ] . titleKey = getTitle ( stream ) ;
    entry -> withAttrs [ i ] . attrKey = getAttr ( stream ) ;
    len = getByte ( stream ) ;
    if ( len > 0 )
      (void) fseek ( stream, len, SEEK_CUR ) ;
    entry -> withAttrs [ i ] . position = getPosition ( stream ) ;
  }
  for ( i = 0 ; i < entry -> noWithoutAttr ; i++ )
  { 
    entry -> withoutAttrs [ i ] . titleKey = getTitle ( stream ) ;
    len = getByte ( stream ) ;
    if ( len > 0 )
      (void) fseek ( stream, len, SEEK_CUR ) ;
    entry -> withoutAttrs [ i ] . position = getPosition ( stream ) ;
  }
  return ( TRUE ) ;
}


void makeCastDatabaseNamesIndex ( int listId, NameID namesOnList )
{
  FILE *dbFp, *ndxFp ;
  struct castFilmography entry ;
  struct nameKeyOffset *namesIndex ;
  long offset = 0 ;
  long count = 0, i ;
  char fn [ MAXPATHLEN ] ;

  namesIndex = (struct nameKeyOffset *) calloc ( namesOnList + 5, sizeof ( struct nameKeyOffset ) ) ;
  if ( namesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate names index" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;
  while ( readCastEntry ( &entry, dbFp ) )
  {
     namesIndex [ count ] . nameKey = entry . nameKey ;
     namesIndex [ count ] . offset = offset ;
     offset = ftell ( dbFp ) ;
     count++ ;
  }

  (void) qsort ( (void*) namesIndex, (size_t) count, sizeof ( struct nameKeyOffset ), (int (*) (const void*, const void*)) nameKeyOffsetSort ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, NDXEXT ) ;
  ndxFp = writeFile ( fn ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putName ( namesIndex [ i ] . nameKey, ndxFp ) ;
     putFullOffset ( namesIndex [ i ] . offset, ndxFp ) ;
  }

  (void) fclose ( dbFp ) ;
  (void) fclose ( ndxFp ) ;
  free ( (void*) namesIndex ) ;
}


void makeCastDatabaseTitlesIndex ( int listId, long nentries )
{
  FILE *dbFp, *tdxFp ;
  struct castFilmography entry ;
  struct titleKeyOffset *titlesIndex ;
  long count = 0, offset = 0, i ;
  int j ;
  char fn [ MAXPATHLEN ] ;

  titlesIndex = (struct titleKeyOffset *) calloc ( nentries + 5, sizeof ( struct titleKeyOffset ) ) ;
  if ( titlesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate titles index" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;

  while ( readCastEntry ( &entry, dbFp ) )
  {
    for ( j = 0 ; j < entry . noWithAttr ; j++ )
    {
      titlesIndex [ count ] . titleKey = entry . withAttrs [ j ] . titleKey ;
      titlesIndex [ count ] . offset = offset ;
      count++ ;
    }
    for ( j = 0 ; j < entry . noWithoutAttr ; j++ )
    {
      titlesIndex [ count ] . titleKey = entry . withoutAttrs [ j ] . titleKey  ;
      titlesIndex [ count ] . offset = offset ;
      count++ ;
    }
    offset = ftell ( dbFp ) ;
  }

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, TDXEXT ) ;
  tdxFp = writeFile ( fn ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, tdxFp ) ;
     putFullOffset ( titlesIndex [ i ] . offset, tdxFp ) ;
  }

  (void) fclose ( dbFp ) ;
  (void) fclose ( tdxFp ) ;
  free ( (void*) titlesIndex ) ;
}


void writeCastEntry (struct castFilmography *currentEntry, FILE *stream)
{
  int i ;

  if ( currentEntry -> noWithAttr == 0 && currentEntry -> noWithoutAttr == 0 )
    return ;
  else
  {
    putName ( currentEntry -> nameKey, stream ) ;
    putFilmographyCounts ( currentEntry -> noWithAttr, currentEntry -> noWithoutAttr, stream ) ;
    for ( i = 0 ; i < currentEntry -> noWithAttr ; i++ )
    {
      putTitle ( currentEntry -> withAttrs [ i ] . titleKey, stream ) ;
      putAttr ( currentEntry -> withAttrs [ i ] . attrKey, stream ) ;
      if ( currentEntry -> withAttrs [ i ] . cname == NULL )
        putByte ( 0, stream ) ;
      else
      {
        putByte ( strlen ( currentEntry -> withAttrs [ i ] . cname ), stream ) ;
        putString ( currentEntry -> withAttrs [ i ] . cname, stream ) ;
        free ( (void*) currentEntry -> withAttrs [ i ] . cname ) ;
      }
      putPosition ( currentEntry -> withAttrs [ i ] . position, stream ) ;
    }
    for ( i = 0 ; i < currentEntry -> noWithoutAttr ; i++ )
    {
      putTitle ( currentEntry -> withoutAttrs [ i ] . titleKey, stream ) ;
      if ( currentEntry -> withoutAttrs [ i ] . cname == NULL )
        putByte ( 0, stream ) ;
      else
      {
        putByte ( strlen ( currentEntry -> withoutAttrs [ i ] . cname ), stream ) ;
        putString ( currentEntry -> withoutAttrs [ i ] . cname, stream ) ;
        free ( (void*) currentEntry -> withoutAttrs [ i ] . cname ) ;
      }
      putPosition ( currentEntry -> withoutAttrs [ i ] . position, stream ) ;
    }
  }
}


long processCastList ( NameID *nameCount, struct titleIndexRec *titles, TitleID *titleCount, AttributeID *attrCount, int listId, int moviesOnly, int nochar )
{
  FILE   *listFp, *dbFp, *nameKeyFp, *tmpFp ;
  char   line [ MXLINELEN ] ;
  char   keyFileData [ MXLINELEN ] ;
  char   prevName [ MXLINELEN ] ;
  char   fn [ MAXPATHLEN ] ;
  char   *title, *attr, *cname, *keyPtr = NULL ;
  long   nentries = 0 ;
  NameID namesOnList = 0 ;
  int    foundMark = FALSE, inList = FALSE, skipMode = FALSE, compare ;
  struct castFilmography currentEntry ;
  int    position ;

  currentEntry . noWithAttr = 0 ;
  currentEntry . noWithoutAttr = 0 ;

  prevName [ 0 ] = '\0' ;

  listFp = openFile ( filmographyDefs [ listId ] . list ) ;

  if ( isReadable ( NAMEKEY ) )
  {
    tmpFp = copyFile ( NAMEKEY ) ;
    (void) fgets ( keyFileData, MXLINELEN, tmpFp ) ;
    if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
      *keyPtr++ = '\0' ;
  }
  else
    tmpFp = NULL ;

  nameKeyFp = fopen ( NAMEKEY, "w" ) ;
 
  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = writeFile ( fn ) ;

  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
  {
    if ( inList )
    {
      if ( isValidListLine ( line ) )
      {
        stripEOL ( line ) ;
        if ( ( title = strchr ( line, '\t' ) ) != NULL )
        {
          *title++ = '\0' ;
          if ( *line != '\0' )
          {
            if ( caseCompare ( prevName, line ) > 0 ) 
            {
              skipMode = TRUE ;
              if ( debugFlag )
                printf ( "skipped: %s\n", line ) ;
              continue ;
            }
            else
              (void) strcpy ( prevName, line ) ;

            writeCastEntry ( &currentEntry, dbFp ) ;
            skipMode = FALSE ;
            namesOnList++ ;
            if ( tmpFp == NULL )
            {
              currentEntry . nameKey = (*nameCount)++ ;
              (void) fprintf ( nameKeyFp, "%s|%lx\n", line, currentEntry . nameKey ) ;
            }
            else
            {
              compare = caseCompare ( keyFileData, line ) ;
              while ( compare < 0 )
              {
                (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
                if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
                {
                  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                    *keyPtr++ = '\0' ;
                  compare = caseCompare ( keyFileData, line ) ;
                }
                else
                {
                  compare = 1 ;
                  (void) fclose ( tmpFp ) ;
                  tmpFp = NULL ;
                }
              }
              if ( compare == 0 )
              {
                currentEntry . nameKey = strtol ( keyPtr, (char **) NULL, 16) ;
                (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
                if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
                {
                  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                    *keyPtr++ = '\0' ;
                }
                else
                {
                  (void) fclose ( tmpFp ) ;
                  tmpFp = NULL ;
                }
              }
              else
              {
                currentEntry . nameKey = (*nameCount)++ ;
                (void) fprintf ( nameKeyFp, "%s|%lx\n", line, currentEntry . nameKey ) ;
              }
            }
            currentEntry . noWithAttr = 0 ;
            currentEntry . noWithoutAttr = 0 ;
            if ( debugFlag )
              (void) printf ( "%s\n", line ) ;
          } 
          while ( *title == '\t' ) 
            title++ ;
          position = splitPosition ( title ) ;
          cname = splitChar ( title ) ;
          attr = splitAttribute ( title ) ;
  
          if ( ( !skipMode ) && ( ( !moviesOnly ) || ( moviesOnly && *title != '"' ) ) )
          {
            if ( attr == NULL || currentEntry . noWithAttr >= MAXWITHATTRS )
            {
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . titleKey = titleKeyLookup ( title, titles, titleCount ) ;
              if ( cname == NULL || nochar ) 
                currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . cname = NULL ;
              else
                currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . cname = duplicateString ( cname ) ;
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . position = position ;
              currentEntry . noWithoutAttr++ ; 
            }
            else
            {
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . titleKey = titleKeyLookup ( title, titles, titleCount ) ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . attrKey = attrKeyLookup ( attr, attrCount ) ;
              if ( cname == NULL || nochar ) 
                currentEntry . withAttrs [ currentEntry . noWithAttr ] . cname = NULL ;
              else
                currentEntry . withAttrs [ currentEntry . noWithAttr ] . cname = duplicateString ( cname ) ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . position = position ;
              currentEntry . noWithAttr++ ; 
            }
            nentries++ ;
          }
        }
      }
    }
    else
    {
      if ( strncmp ( line, "Name", 4 ) == 0 )
       	foundMark = TRUE ;
      else if ( foundMark && strncmp ( line, "----", 4 ) == 0 )
	inList = TRUE ;
      else
	foundMark = FALSE ;
    }
  }
  writeCastEntry ( &currentEntry, dbFp ) ;
  (void) fclose ( listFp ) ;
  (void) fclose ( dbFp ) ;
  if ( tmpFp != NULL )
  {
    (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
    while ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      (void) fprintf ( nameKeyFp, "%s", keyFileData ) ;
    (void) fclose ( tmpFp ) ;
  }
  (void) fclose ( nameKeyFp ) ;

  makeCastDatabaseNamesIndex ( listId, namesOnList ) ;
  makeCastDatabaseTitlesIndex ( listId, nentries ) ;

  return ( nentries ) ;
}



int readFilmographyEntry (struct filmography *entry, FILE *stream)
{
  int i ;

  entry -> nameKey = getName ( stream ) ;
  if ( feof ( stream ) )
    return ( FALSE ) ;
  getFilmographyCounts ( stream, &(entry -> noWithAttr), &(entry -> noWithoutAttr) ) ;
  for ( i = 0 ; i < entry -> noWithAttr ; i++ )
  {
    entry -> withAttrs [ i ] . titleKey = getTitle ( stream ) ;
    entry -> withAttrs [ i ] . attrKey = getAttr ( stream ) ;
  }
  for ( i = 0 ; i < entry -> noWithoutAttr ; i++ )
     entry -> withoutAttrs [ i ] = getTitle ( stream ) ;

  return ( TRUE ) ;
}


void makeDatabaseNamesIndex ( int listId, NameID namesOnList )
{
  FILE *dbFp, *ndxFp ;
  struct filmography entry ;
  struct nameKeyOffset *namesIndex ;
  long offset = 0 ;
  long count = 0, i ;
  char fn [ MAXPATHLEN ] ;

  namesIndex = (struct nameKeyOffset *) calloc ( namesOnList + 5, sizeof ( struct nameKeyOffset ) ) ;
  if ( namesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate names index" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;
  while ( readFilmographyEntry ( &entry, dbFp ) )
  {
     namesIndex [ count ] . nameKey = entry . nameKey ;
     namesIndex [ count ] . offset = offset ;
     offset = ftell ( dbFp ) ;
     count++ ;
  }

  (void) qsort ( (void*) namesIndex, (size_t) count, sizeof ( struct nameKeyOffset ), (int (*) (const void*, const void*)) nameKeyOffsetSort ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, NDXEXT ) ;
  ndxFp = writeFile ( fn ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putName ( namesIndex [ i ] . nameKey, ndxFp ) ;
     putFullOffset ( namesIndex [ i ] . offset, ndxFp ) ;
  }

  (void) fclose ( dbFp ) ;
  (void) fclose ( ndxFp ) ;
  free ( (void*) namesIndex ) ;
}


void makeDatabaseTitlesIndex ( int listId, long nentries )
{
  FILE *dbFp, *tdxFp ;
  struct filmography entry ;
  struct titleKeyOffset *titlesIndex ;
  long count = 0, offset = 0, i ;
  int j ;
  char fn [ MAXPATHLEN ] ;

  titlesIndex = (struct titleKeyOffset *) calloc ( nentries + 5, sizeof ( struct titleKeyOffset ) ) ;
  if ( titlesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate titles index" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;

  while ( readFilmographyEntry ( &entry, dbFp ) )
  {
    for ( j = 0 ; j < entry . noWithAttr ; j++ )
    {
      titlesIndex [ count ] . titleKey = entry . withAttrs [ j ] . titleKey ;
      titlesIndex [ count ] . offset = offset ;
      count++ ;
    }
    for ( j = 0 ; j < entry . noWithoutAttr ; j++ )
    {
      titlesIndex [ count ] . titleKey = entry . withoutAttrs [ j ]  ;
      titlesIndex [ count ] . offset = offset ;
      count++ ;
    }
    offset = ftell ( dbFp ) ;
  }

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, TDXEXT ) ;
  tdxFp = writeFile ( fn ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, tdxFp ) ;
     putFullOffset ( titlesIndex [ i ] . offset, tdxFp ) ;
  }

  (void) fclose ( dbFp ) ;
  (void) fclose ( tdxFp ) ;
  free ( (void*) titlesIndex ) ;
}


void writeFilmographyEntry (struct filmography *currentEntry, FILE *stream)
{
  int i ;

  if ( currentEntry -> noWithAttr == 0 && currentEntry -> noWithoutAttr == 0 )
    return ;
  else
  {
    putName ( currentEntry -> nameKey, stream ) ;
    putFilmographyCounts ( currentEntry -> noWithAttr, currentEntry -> noWithoutAttr, stream ) ;
    for ( i = 0 ; i < currentEntry -> noWithAttr ; i++ )
    {
      putTitle ( currentEntry -> withAttrs [ i ] . titleKey, stream ) ;
      putAttr ( currentEntry -> withAttrs [ i ] . attrKey, stream ) ;
    }
    for ( i = 0 ; i < currentEntry -> noWithoutAttr ; i++ )
      putTitle ( currentEntry -> withoutAttrs [ i ], stream ) ;
  }
}


long processFilmographyList ( NameID *nameCount, struct titleIndexRec *titles, TitleID *titleCount, AttributeID *attrCount, int listId, int moviesOnly )
{
  FILE   *listFp, *dbFp, *nameKeyFp, *tmpFp ;
  char   line [ MXLINELEN ] ;
  char   keyFileData [ MXLINELEN ] ;
  char   prevName [ MXLINELEN ] ;
  char   fn [ MAXPATHLEN ] ;
  char   *title, *attr, *keyPtr = NULL ;
  long   nentries = 0 ;
  NameID namesOnList = 0 ;
  int    foundMark = FALSE, inList = FALSE, skipMode = FALSE, compare ;
  struct filmography currentEntry ;

  currentEntry . noWithAttr = 0 ;
  currentEntry . noWithoutAttr = 0 ;

  prevName [ 0 ] = '\0' ;

  listFp = openFile ( filmographyDefs [ listId ] . list ) ;

  if ( isReadable ( NAMEKEY ) )
  {
    tmpFp = copyFile ( NAMEKEY ) ;
    (void) fgets ( keyFileData, MXLINELEN, tmpFp ) ;
    if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
      *keyPtr++ = '\0' ;
  }
  else
    tmpFp = NULL ;

  nameKeyFp = fopen ( NAMEKEY, "w" ) ;
 
  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = writeFile ( fn ) ;

  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
  {
    if ( inList )
    {
      if ( isValidListLine ( line ) )
      {
        stripEOL ( line ) ;
        if ( ( title = strchr ( line, '\t' ) ) != NULL )
        {
          *title++ = '\0' ;
          if ( *line != '\0' )
          {
            if ( caseCompare ( prevName, line ) > 0 ) 
            {
              skipMode = TRUE ;
              if ( debugFlag )
                printf ( "skipped: %s\n", line ) ;
              continue ;
            }
            else
              (void) strcpy ( prevName, line ) ;

            writeFilmographyEntry ( &currentEntry, dbFp ) ;
            skipMode = FALSE ;
            namesOnList++ ;
            if ( tmpFp == NULL )
            {
              currentEntry . nameKey = (*nameCount)++ ;
              (void) fprintf ( nameKeyFp, "%s|%lx\n", line, currentEntry . nameKey ) ;
            }
            else
            {
              compare = caseCompare ( keyFileData, line ) ;
              while ( compare < 0 )
              {
                (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
                if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
                {
                  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                    *keyPtr++ = '\0' ;
                  compare = caseCompare ( keyFileData, line ) ;
                }
                else
                {
                  compare = 1 ;
                  (void) fclose ( tmpFp ) ;
                  tmpFp = NULL ;
                }
              }
              if ( compare == 0 )
              {
                currentEntry . nameKey = strtol ( keyPtr, (char **) NULL, 16) ;
                (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
                if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
                {
                  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                    *keyPtr++ = '\0' ;
                }
                else
                {
                  (void) fclose ( tmpFp ) ;
                  tmpFp = NULL ;
                }
              }
              else
              {
                currentEntry . nameKey = (*nameCount)++ ;
                (void) fprintf ( nameKeyFp, "%s|%lx\n", line, currentEntry . nameKey ) ;
              }
            }
            currentEntry . noWithAttr = 0 ;
            currentEntry . noWithoutAttr = 0 ;
            if ( debugFlag )
              (void) printf ( "%s\n", line ) ;
          } 
          while ( *title == '\t' ) 
            title++ ;
          attr = splitAttribute ( title ) ;
  
          if ( ( !skipMode ) && ( ( !moviesOnly ) || ( moviesOnly && *title != '"' ) ) )
          {
            if ( attr == NULL || currentEntry . noWithAttr >= MAXWITHATTRS )
            {
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] = titleKeyLookup ( title, titles, titleCount ) ;
              currentEntry . noWithoutAttr++ ; 
            }
            else
            {
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . titleKey = titleKeyLookup ( title, titles, titleCount ) ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . attrKey = attrKeyLookup ( attr, attrCount ) ;
              currentEntry . noWithAttr++ ; 
            }
            nentries++ ;
          }
        }
      }
    }
    else
    {
      if ( strncmp ( line, "Name", 4 ) == 0 )
       	foundMark = TRUE ;
      else if ( foundMark && strncmp ( line, "----", 4 ) == 0 )
	inList = TRUE ;
      else
	foundMark = FALSE ;
    }
  }
  writeFilmographyEntry ( &currentEntry, dbFp ) ;
  (void) fclose ( listFp ) ;
  (void) fclose ( dbFp ) ;
  if ( tmpFp != NULL )
  {
    (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
    while ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      (void) fprintf ( nameKeyFp, "%s", keyFileData ) ;
    (void) fclose ( tmpFp ) ;
  }
  (void) fclose ( nameKeyFp ) ;

  makeDatabaseNamesIndex ( listId, namesOnList ) ;
  makeDatabaseTitlesIndex ( listId, nentries ) ;

  return ( nentries ) ;
}


void splitWriterPosition ( char *line, int *lineOrder, int *groupOrder, int *subgroupOrder ) 
{
  char *ptr, *endptr, *groupPtr, *subPtr ;
 
  *lineOrder = NOPOS ;
  *groupOrder = NOPOS ;
  *subgroupOrder = NOPOS ;

  if ( ( ptr = strchr ( line, '<' ) ) == NULL )
    return ;
  else
    if ( ( endptr = strrchr ( ptr, '>' ) ) == NULL )
      return ;

  *lineOrder = strtol ( ptr + 1, &groupPtr, 10 ) ;
  *groupOrder = strtol ( groupPtr + 1, &subPtr, 10 ) ;
  *subgroupOrder = strtol ( subPtr + 1, NULL, 10 ) ;
  if ( debugFlag )
    (void) printf ( "%d,%d,%d\n", *lineOrder, *groupOrder, *subgroupOrder ) ;

  *endptr = '\0' ;
  *(ptr-2) = '\0' ;
}


int readWriterFilmographyEntry (struct writerFilmography *entry, FILE *stream)
{
  int i ;

  entry -> nameKey = getName ( stream ) ;
  if ( feof ( stream ) )
    return ( FALSE ) ;
  getFilmographyCounts ( stream, &(entry -> noWithAttr), &(entry -> noWithoutAttr) ) ;
  for ( i = 0 ; i < entry -> noWithAttr ; i++ )
  {
    entry -> withAttrs [ i ] . titleKey = getTitle ( stream ) ;
    entry -> withAttrs [ i ] . attrKey = getAttr ( stream ) ;
    entry -> withAttrs [ i ] . lineOrder = getPosition ( stream ) ;
    entry -> withAttrs [ i ] . groupOrder = getPosition ( stream ) ;
    entry -> withAttrs [ i ] . subgroupOrder = getPosition ( stream ) ;
  }
  for ( i = 0 ; i < entry -> noWithoutAttr ; i++ )
  {
    entry -> withoutAttrs [ i ] . titleKey = getTitle ( stream ) ;
    entry -> withoutAttrs [ i ] . lineOrder = getPosition ( stream ) ;
    entry -> withoutAttrs [ i ] . groupOrder = getPosition ( stream ) ;
    entry -> withoutAttrs [ i ] . subgroupOrder = getPosition ( stream ) ;
  }

  return ( TRUE ) ;
}


void makeWriterDatabaseNamesIndex ( int listId, NameID namesOnList )
{
  FILE *dbFp, *ndxFp ;
  struct writerFilmography entry ;
  struct nameKeyOffset *namesIndex ;
  long offset = 0 ;
  long count = 0, i ;
  char fn [ MAXPATHLEN ] ;

  namesIndex = (struct nameKeyOffset *) calloc ( namesOnList + 5, sizeof ( struct nameKeyOffset ) ) ;
  if ( namesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate names index" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;
  while ( readWriterFilmographyEntry ( &entry, dbFp ) )
  {
     namesIndex [ count ] . nameKey = entry . nameKey ;
     namesIndex [ count ] . offset = offset ;
     offset = ftell ( dbFp ) ;
     count++ ;
  }

  (void) qsort ( (void*) namesIndex, (size_t) count, sizeof ( struct nameKeyOffset ), (int (*) (const void*, const void*)) nameKeyOffsetSort ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, NDXEXT ) ;
  ndxFp = writeFile ( fn ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putName ( namesIndex [ i ] . nameKey, ndxFp ) ;
     putFullOffset ( namesIndex [ i ] . offset, ndxFp ) ;
  }

  (void) fclose ( dbFp ) ;
  (void) fclose ( ndxFp ) ;
  free ( (void*) namesIndex ) ;
}


void makeWriterDatabaseTitlesIndex ( int listId, long nentries )
{
  FILE *dbFp, *tdxFp ;
  struct writerFilmography entry ;
  struct titleKeyOffset *titlesIndex ;
  long count = 0, offset = 0, i ;
  int j ;
  char fn [ MAXPATHLEN ] ;

  titlesIndex = (struct titleKeyOffset *) calloc ( nentries + 5, sizeof ( struct titleKeyOffset ) ) ;
  if ( titlesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate titles index" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;

  while ( readWriterFilmographyEntry ( &entry, dbFp ) )
  {
    for ( j = 0 ; j < entry . noWithAttr ; j++ )
    {
      titlesIndex [ count ] . titleKey = entry . withAttrs [ j ] . titleKey ;
      titlesIndex [ count ] . offset = offset ;
      count++ ;
    }
    for ( j = 0 ; j < entry . noWithoutAttr ; j++ )
    {
      titlesIndex [ count ] . titleKey = entry . withoutAttrs [ j ] . titleKey  ;
      titlesIndex [ count ] . offset = offset ;
      count++ ;
    }
    offset = ftell ( dbFp ) ;
  }

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, TDXEXT ) ;
  tdxFp = writeFile ( fn ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, tdxFp ) ;
     putFullOffset ( titlesIndex [ i ] . offset, tdxFp ) ;
  }

  (void) fclose ( dbFp ) ;
  (void) fclose ( tdxFp ) ;
  free ( (void*) titlesIndex ) ;
}


void writeWriterFilmographyEntry (struct writerFilmography *currentEntry, FILE *stream)
{
  int i ;

  if ( currentEntry -> noWithAttr == 0 && currentEntry -> noWithoutAttr == 0 )
    return ;
  else
  {
    putName ( currentEntry -> nameKey, stream ) ;
    putFilmographyCounts ( currentEntry -> noWithAttr, currentEntry -> noWithoutAttr, stream ) ;
    for ( i = 0 ; i < currentEntry -> noWithAttr ; i++ )
    {
      putTitle ( currentEntry -> withAttrs [ i ] . titleKey, stream ) ;
      putAttr ( currentEntry -> withAttrs [ i ] . attrKey, stream ) ;
      putPosition ( currentEntry -> withAttrs [ i ] . lineOrder, stream ) ;
      putPosition ( currentEntry -> withAttrs [ i ] . groupOrder, stream ) ;
      putPosition ( currentEntry -> withAttrs [ i ] . subgroupOrder, stream ) ;
    }
    for ( i = 0 ; i < currentEntry -> noWithoutAttr ; i++ )
    {
      putTitle ( currentEntry -> withoutAttrs [ i ] . titleKey, stream ) ;
      putPosition ( currentEntry -> withoutAttrs [ i ] . lineOrder, stream ) ;
      putPosition ( currentEntry -> withoutAttrs [ i ] . groupOrder, stream ) ;
      putPosition ( currentEntry -> withoutAttrs [ i ] . subgroupOrder, stream ) ;
    }
  }
}


long processWriterFilmographyList ( NameID *nameCount, struct titleIndexRec *titles, TitleID *titleCount, AttributeID *attrCount, int listId, int moviesOnly )
{
  FILE   *listFp, *dbFp, *nameKeyFp, *tmpFp ;
  char   line [ MXLINELEN ] ;
  char   keyFileData [ MXLINELEN ] ;
  char   prevName [ MXLINELEN ] ;
  char   fn [ MAXPATHLEN ] ;
  char   *title, *attr, *keyPtr = NULL ;
  long   nentries = 0 ;
  NameID namesOnList = 0 ;
  int    foundMark = FALSE, inList = FALSE, skipMode = FALSE, compare ;
  struct writerFilmography currentEntry ;
  int    lineOrder, groupOrder, subgroupOrder ;

  currentEntry . noWithAttr = 0 ;
  currentEntry . noWithoutAttr = 0 ;

  prevName [ 0 ] = '\0' ;

  listFp = openFile ( filmographyDefs [ listId ] . list ) ;

  if ( isReadable ( NAMEKEY ) )
  {
    tmpFp = copyFile ( NAMEKEY ) ;
    (void) fgets ( keyFileData, MXLINELEN, tmpFp ) ;
    if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
      *keyPtr++ = '\0' ;
  }
  else
    tmpFp = NULL ;

  nameKeyFp = fopen ( NAMEKEY, "w" ) ;
 
  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = writeFile ( fn ) ;

  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
  {
    if ( inList )
    {
      if ( isValidListLine ( line ) )
      {
        stripEOL ( line ) ;
        if ( ( title = strchr ( line, '\t' ) ) != NULL )
        {
          *title++ = '\0' ;
          if ( *line != '\0' )
          {
            if ( caseCompare ( prevName, line ) > 0 ) 
            {
              skipMode = TRUE ;
              if ( debugFlag )
                printf ( "skipped: %s\n", line ) ;
              continue ;
            }
            else
              (void) strcpy ( prevName, line ) ;

            writeWriterFilmographyEntry ( &currentEntry, dbFp ) ;
            skipMode = FALSE ;
            namesOnList++ ;
            if ( tmpFp == NULL )
            {
              currentEntry . nameKey = (*nameCount)++ ;
              (void) fprintf ( nameKeyFp, "%s|%lx\n", line, currentEntry . nameKey ) ;
            }
            else
            {
              compare = caseCompare ( keyFileData, line ) ;
              while ( compare < 0 )
              {
                (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
                if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
                {
                  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                    *keyPtr++ = '\0' ;
                  compare = caseCompare ( keyFileData, line ) ;
                }
                else
                {
                  compare = 1 ;
                  (void) fclose ( tmpFp ) ;
                  tmpFp = NULL ;
                }
              }
              if ( compare == 0 )
              {
                currentEntry . nameKey = strtol ( keyPtr, (char **) NULL, 16) ;
                (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
                if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
                {
                  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                    *keyPtr++ = '\0' ;
                }
                else
                {
                  (void) fclose ( tmpFp ) ;
                  tmpFp = NULL ;
                }
              }
              else
              {
                currentEntry . nameKey = (*nameCount)++ ;
                (void) fprintf ( nameKeyFp, "%s|%lx\n", line, currentEntry . nameKey ) ;
              }
            }
            currentEntry . noWithAttr = 0 ;
            currentEntry . noWithoutAttr = 0 ;
            if ( debugFlag )
              (void) printf ( "%s\n", line ) ;
          } 
          while ( *title == '\t' ) 
            title++ ;
          splitWriterPosition ( title, &lineOrder, &groupOrder, &subgroupOrder ) ;
          attr = splitAttribute ( title ) ;
  
          if ( ( !skipMode ) && ( ( !moviesOnly ) || ( moviesOnly && *title != '"' ) ) )
          {
            if ( attr == NULL || currentEntry . noWithAttr >= MAXWITHATTRS )
            {
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . titleKey = titleKeyLookup ( title, titles, titleCount ) ;
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . lineOrder = lineOrder ;
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . groupOrder = groupOrder ;
              currentEntry . withoutAttrs [ currentEntry . noWithoutAttr ] . subgroupOrder = subgroupOrder ;
              currentEntry . noWithoutAttr++ ; 
            }
            else
            {
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . titleKey = titleKeyLookup ( title, titles, titleCount ) ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . attrKey = attrKeyLookup ( attr, attrCount ) ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . lineOrder = lineOrder ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . groupOrder = groupOrder ;
              currentEntry . withAttrs [ currentEntry . noWithAttr ] . subgroupOrder = subgroupOrder ;
              currentEntry . noWithAttr++ ; 
            }
            nentries++ ;
          }
        }
      }
    }
    else
    {
      if ( strncmp ( line, "Name", 4 ) == 0 )
       	foundMark = TRUE ;
      else if ( foundMark && strncmp ( line, "----", 4 ) == 0 )
	inList = TRUE ;
      else
	foundMark = FALSE ;
    }
  }
  writeWriterFilmographyEntry ( &currentEntry, dbFp ) ;
  (void) fclose ( listFp ) ;
  (void) fclose ( dbFp ) ;
  if ( tmpFp != NULL )
  {
    (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
    while ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      (void) fprintf ( nameKeyFp, "%s", keyFileData ) ;
    (void) fclose ( tmpFp ) ;
  }
  (void) fclose ( nameKeyFp ) ;

  makeWriterDatabaseNamesIndex ( listId, namesOnList ) ;
  makeWriterDatabaseTitlesIndex ( listId, nentries ) ;

  return ( nentries ) ;
}



TitleID processTriviaList (struct titleIndexRec *titles, TitleID *titleCount, int listId)
{ 
  FILE *dbFp, *listFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char line [ MXLINELEN ] ; 
  int  indata = FALSE ; 
  int  inheader = FALSE ;
  int  notdone = TRUE ; 
  TitleID count = 0, i ;
  long currentOffset ;
  
  listFp = openFile ( triviaDefs [ listId ] . list ) ;
  dbFp = writeFile ( triviaDefs [ listId ] . db ) ;
   
  while ( notdone && fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( indata ) 
      if ( strncmp ( line, "----------", 10 ) == 0 ) 
         notdone = FALSE ; 
       else 
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "# ", 2 ) == 0 )
         {
           stripEOL ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 2 ) ;
           titlesIndex [ count ] . titleKey = titleKeyLookup ( line + 2, titles, titleCount ) ;
           titlesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= sharedTitleIndexSize)
	   {
	     sharedTitleIndexSize += TIGROW ;
	     titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	      if ( titlesIndex == NULL )
		moviedbError ( "mkdb: not enough memory to generate trivia index" ) ;
	      sharedTitleIndex = titlesIndex ;
	   }
         }
       }
    else 
      if ( strcmp ( line, triviaDefs [ listId ] . start ) == 0 ) 
        inheader = TRUE ; 
      else if ( inheader && strncmp ( line, "# ", 2 ) == 0 )
      {
        indata = TRUE ;
        (void) fprintf ( dbFp, "%s", line ) ; 
        stripEOL ( line ) ;
        titlesIndex [ 0 ] . titleKey = titleKeyLookup ( line + 2, titles, titleCount ) ;
        titlesIndex [ 0 ] . offset = 0 ;
        count = 1 ;
      }

  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  indexFp = writeFile ( triviaDefs [ listId ] . index ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putFullOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  return ( count ) ;
} 


TitleID processPlotList (struct titleIndexRec *titles, TitleID *titleCount)
{ 
  FILE *dbFp, *listFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char line [ MXLINELEN ] ; 
  int  inplot = FALSE ; 
  TitleID  count = 0, i ;
  long currentOffset ;

  listFp = openFile ( PLOTLIST ) ;
  dbFp = writeFile ( PLOTDB ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inplot ) 
    {
       if ( line [ 0 ] != '-' && line [ 0 ] != '\n' )
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "MV: ", 4 ) == 0 )
         {
           stripEOL ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 4 ) ;
           titlesIndex [ count ] . titleKey = titleKeyLookup ( line + 4, titles, titleCount ) ;
           titlesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= sharedTitleIndexSize)
	   {
	     sharedTitleIndexSize += TIGROW ;
	     titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	      if ( titlesIndex == NULL )
		moviedbError ( "mkdb: not enough memory to generate plot index" ) ;
	      sharedTitleIndex = titlesIndex ;
           }
         }
       }
    }
    else 
      if ( strncmp ( line, "PLOT SUMMARIES", 14 ) == 0 ) 
      {
        inplot = TRUE ; 
        (void) fprintf ( dbFp, "%s", line ) ; 
      }
 
  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  indexFp = writeFile ( PLOTIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putFullOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  return ( count ) ;
} 

#ifdef INTERNAL
TitleID processOutlineList (struct titleIndexRec *titles, TitleID *titleCount)
{ 
  FILE *dbFp, *listFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char line [ MXLINELEN ] ; 
  int  inplot = FALSE ; 
  TitleID  count = 0, i ;
  long currentOffset ;

  listFp = openFile ( OUTLLIST ) ;
  dbFp = writeFile ( OUTLDB ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inplot ) 
    {
       if ( line [ 0 ] != '-' && line [ 0 ] != '\n' )
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "MV: ", 4 ) == 0 )
         {
           stripEOL ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 4 ) ;
           titlesIndex [ count ] . titleKey = titleKeyLookup ( line + 4, titles, titleCount ) ;
           titlesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= sharedTitleIndexSize)
	   {
	     sharedTitleIndexSize += TIGROW ;
	     titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	      if ( titlesIndex == NULL )
		moviedbError ( "mkdb: not enough memory to generate outline index" ) ;
	      sharedTitleIndex = titlesIndex ;
           }
         }
       }
    }
    else 
      if ( strncmp ( line, "OUTLINES LIST", 13 ) == 0 ) 
      {
        inplot = TRUE ; 
        (void) fprintf ( dbFp, "%s", line ) ; 
      }
 
  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  indexFp = writeFile ( OUTLIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putFullOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  return ( count ) ;
} 
#endif


NameID processBiographiesList ( NameID *nameCount )
{ 
  FILE *dbFp, *listFp, *indexFp, *nameKeyFp, *tmpFp ; 
  struct nameKeyOffset *namesIndex;
  char line [ MXLINELEN ] ; 
  char   keyFileData [ MXLINELEN ] ;
  char   prevName [ MXLINELEN ] ;
  char   *keyPtr = NULL ;
  int  inbio = FALSE, skipMode = FALSE, compare ; 
  NameID count = 0, i ;
  long currentOffset ;

  namesIndex = (struct nameKeyOffset*) calloc ( MAXBIOENTRIES, sizeof ( struct nameKeyOffset ) ) ;

  if ( namesIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate names index" ) ;

  prevName [ 0 ] = '\0' ;

  if ( isReadable ( NAMEKEY ) )
  {
    tmpFp = copyFile ( NAMEKEY ) ;
    (void) fgets ( keyFileData, MXLINELEN, tmpFp ) ;
    if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
      *keyPtr++ = '\0' ;
  }
  else
    tmpFp = NULL ;

  nameKeyFp = writeFile ( NAMEKEY ) ;

  listFp = openFile ( BIOLIST ) ;
  dbFp = writeFile ( BIODB ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inbio ) 
    {
       if ( line [ 0 ] != '-' && line [ 0 ] != '\n' )
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "NM: ", 4 ) == 0 )
         {
           stripEOL ( line ) ;
           stripSep ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 4 ) ;
           if ( caseCompare ( prevName, line + 4 ) >= 0 )
           {
             skipMode = TRUE ;
             if ( debugFlag ) 
               printf ( "skipped: %s\n", line + 4 ) ;
             continue ;
           }
           else
             (void) strcpy ( prevName, line + 4 ) ;
           skipMode = FALSE ;

           if ( tmpFp == NULL )
           {
             namesIndex [ count ] . nameKey = (*nameCount)++ ;
             (void) fprintf ( nameKeyFp, "%s|%lx\n", line + 4, namesIndex [ count ] . nameKey ) ;
           }
           else
           {
             compare = caseCompare ( keyFileData, line + 4 ) ;
             while ( compare < 0 )
             {
               (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
               if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
               {
                 if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                   *keyPtr++ = '\0' ;
                 compare = caseCompare ( keyFileData, line + 4 ) ;
               }
               else
               {
                 compare = 1 ;
                 (void) fclose ( tmpFp ) ;
                 tmpFp = NULL ;
               }
             }
             if ( compare == 0 )
             {
               namesIndex [ count ] . nameKey = strtol ( keyPtr, (char **) NULL, 16) ;
               (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
               if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
               {
                 if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                   *keyPtr++ = '\0' ;
               }
               else
               {
                 (void) fclose ( tmpFp ) ;
                 tmpFp = NULL ;
               }
             }
             else
             {
               namesIndex [ count ] . nameKey = (*nameCount)++ ;
               (void) fprintf ( nameKeyFp, "%s|%lx\n", line + 4, namesIndex [ count ] . nameKey ) ;
             }
           }
           namesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= MAXBIOENTRIES)
	     moviedbError ( "mkdb: too many bios -- increase MAXBIOENTRIES" ) ;
         }
       }
    }
    else 
      if ( strncmp ( line, "BIOGRAPHY LIST", 14 ) == 0 ) 
      {
        inbio = TRUE ; 
        (void) fprintf ( dbFp, "%s", line ) ; 
      }
 
  (void) fclose ( listFp ) ; 
  (void) fclose ( dbFp ) ; 
  if ( tmpFp != NULL )
  {
    (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
    while ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      (void) fprintf ( nameKeyFp, "%s", keyFileData ) ;
    (void) fclose ( tmpFp ) ;
  }
  (void) fclose ( nameKeyFp ) ;

  (void) qsort ( (void*) namesIndex, (size_t) count, sizeof ( struct nameKeyOffset ), (int (*) (const void*, const void*)) nameKeyOffsetSort ) ;

  indexFp = writeFile ( BIOIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putName ( namesIndex [ i ] . nameKey, indexFp ) ;
     putFullOffset ( namesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  free ( (void*) namesIndex ) ;
  return ( count ) ;
} 


int mrrDataSort (struct mrrData *m1, struct mrrData *m2)
{
  return ( m1 -> titleKey - m2 -> titleKey ) ;
}


int isReportLine (char *line)
{
  if ( strspn ( line+6, ".01234567890*" ) != 10 )
    return ( FALSE ) ;

  if ( strspn ( line+17, " 1234567890" ) < 8 )
    return ( FALSE ) ;

  if ( strspn ( line+26, " .1234567890" ) < 5 )
    return ( FALSE ) ;

  return ( TRUE ) ;
}


int mrrTitleCompare (char *mrrtitle, char *title)
{
  for ( ; upperCase (*mrrtitle) == upperCase (*title) ; mrrtitle++, title++ )
    if ( *mrrtitle == '\0' )
      return ( 0 ) ;
  if ( *mrrtitle == '\0' && *title == ' ' && *(title+1) == '(' && *(title+2) != '1' )
    return ( 0 ) ;
  return ( upperCase (*mrrtitle) - upperCase (*title) ) ;
}


TitleID processMovieRatings (struct titleIndexRec *titles, TitleID *titleCount)
{ 
  FILE *dbFp, *listFp ; 
  struct mrrData *ratingsReport ;
  char line [ MXLINELEN ] ; 
  int  inmrr = FALSE ; 
  TitleID count = 0, i, insert, nratings ;
  int  votes ;
  float rating ;
  struct titleIndexRec *matched ;

  listFp = openFile ( MRRLIST ) ;
  (void) fseek ( listFp, 0, SEEK_END ) ;
  nratings = ftell ( listFp ) / MRRSIZE ;
  (void) fseek ( listFp, 0, SEEK_SET ) ;
  if ( ( ratingsReport = calloc ( nratings, sizeof ( struct mrrData ) ) ) == NULL )
     moviedbError ( "out of memory" ) ;

  while ( fgets ( line, MXLINELEN, listFp ) != NULL )
  {
    if ( ! inmrr ) 
    {
      if ( strcmp ( line, "MOVIE RATINGS REPORT\n" ) == 0 ) 
        inmrr = TRUE ; 
    }
    else if ( isReportLine ( line ) )
    {
      stripEOL ( line ) ;
      (void) sscanf ( line + 16, "%d%4e", &votes, &rating ) ;
      matched = findTitleKeyPos ( line +32, titles, *titleCount, &insert ) ;
      if ( matched != NULL )
      {
        ratingsReport [ count ] . titleKey = matched -> titleKey ;
        ratingsReport [ count ] . votes = votes ;
        ratingsReport [ count ] . rating = rating * 10 ;
        (void) strncpy ( ratingsReport [ count ] . distribution, line + 6, 10 ) ;
        ratingsReport [ count ] . distribution [ 10 ] = '\0' ;
        count ++ ;
	if (count >= nratings)
	  moviedbError ( "mkdb: too many ratings -- decrease MRRSIZE" ) ;
      }
      else
        if ( debugFlag )
          (void) printf ( "skipped:%s\n", line ) ;

    }
  }

  (void) qsort ( (void*) ratingsReport, (size_t) count, sizeof ( struct mrrData ), (int (*) (const void*, const void*)) mrrDataSort ) ;

  dbFp = writeFile ( MRRDB ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( ratingsReport [ i ] . titleKey, dbFp ) ;
     putString ( ratingsReport [ i ] . distribution, dbFp ) ;
     putInt ( ratingsReport [ i ] . votes, dbFp ) ;
     putByte ( ratingsReport [ i ] . rating, dbFp ) ;
  }

  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 
  return ( count ) ;
}


int voteDataSort (struct voteData *v1, struct voteData *v2)
{
  return ( v1 -> titleKey - v2 -> titleKey ) ;
}


TitleID processVotesList (struct titleIndexRec *titles, TitleID *titleCount)
{ 
  FILE *dbFp, *listFp ; 
  struct voteData *votes;
  char line [ MXLINELEN ] ; 
  struct titleIndexRec *matched ;
  int compare ; 
  TitleID count = 0, i = 1, insert ;
  int vote ;
  char *mrrtitle ;

  votes = (struct voteData*) calloc ( MAXTITLES, sizeof ( struct voteData ) ) ;

  if ( votes == NULL )
    moviedbError ( "mkdb: not enough memory to generate votes index" ) ;

  listFp = openFile ( VOTELIST ) ;

  while ( fgets ( line, MXLINELEN, listFp ) != NULL )
    if ( strncmp ( line, "vote", 4 ) == 0 )
    { 
      stripEOL ( line ) ;
      *(line + 8 ) = '\0' ;
      vote = atoi ( line + 4 ) ;
      if ( vote > 0 && vote <= 10 )
      {
        mrrtitle = line + 9 ;
        upperCaseString ( mrrtitle ) ;

        matched = findTitleKeyPos ( mrrtitle, titles, *titleCount, &insert ) ;
        if ( matched != NULL )
        {
          votes [ count ] . titleKey = matched -> titleKey ;
          votes [ count ] . vote = vote ;
          count ++ ;
        }
        else
        {
          compare = mrrTitleCompare ( mrrtitle, titles [ 0 ] . title ) ;
          for ( i = 0 ; compare > 0 && i < *titleCount ; i++ )
            compare = mrrTitleCompare ( mrrtitle, titles [ i ] . title ) ;
          if ( compare == 0 )
          {
            votes [ count ] . titleKey = titles [ i - 1 ] . titleKey ;
            votes [ count ] . vote = vote ;
            count ++ ;
          }
          else
            if ( debugFlag )
              (void) printf ( "skipped:%s%s\n", line, mrrtitle ) ;
        }
      }
    }

  (void) qsort ( (void*) votes, (size_t) count, sizeof ( struct voteData ), (int (*) (const void*, const void*)) voteDataSort ) ;

  dbFp = writeFile ( VOTEDB ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( votes [ i ] . titleKey, dbFp ) ;
     putByte ( votes [ i ] . vote, dbFp ) ;
  }

  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 
  free ( (void *) votes ) ;
  return ( count ) ;
}


int akaDataSort (struct akaData *a1, struct akaData *a2)
{
  if ( a1 -> primary != a2 -> primary )
    return ( a1 -> primary - a2 -> primary ) ;
  else
    return ( a1 -> aka - a2 -> aka ) ;
}


int akaIndexDataSort (struct akaData *a1, struct akaData *a2)
{
  return ( a1 -> aka - a2 -> aka ) ;
}


TitleID processAkaList (struct titleIndexRec *titles, TitleID *titleCount, AttributeID *attrCount, char *akaFile )
{ 
  FILE *dbFp, *listFp ; 
  struct akaData *aka ;
  size_t akaIndexSize = AKASTART ;
  char line [ MXLINELEN ] ; 
  int inaka = FALSE, enddata = FALSE ;
  TitleID count = 0, processed = 0, sortedTo = 0, i ;
  TitleID primaryKey = NOTITLE ;
  char *ptr ;

  TitleID secondaryKey = NOTITLE ;
  TitleID titleCountValid = *titleCount;
  TitleID startSearchTitle = *titleCount;

  aka = malloc ( sizeof (struct akaData ) * akaIndexSize ) ;
  if ( isReadable ( AKADB ) )
  {
    dbFp = openFile ( AKADB ) ;
    while ( !feof ( dbFp ) )
    {
      aka [ count ] . primary = getTitle ( dbFp ) ;
      aka [ count ] . aka = getTitle ( dbFp ) ;
      aka [ count ] . attrKey = getAttr ( dbFp ) ;
      count++ ;
      if (count >= akaIndexSize )
      {
	  struct akaData *akaIndex = aka ;

	  akaIndexSize += AKAGROW ;
	  aka = realloc ( akaIndex, sizeof (struct akaData) * akaIndexSize ) ;
	  if ( aka == NULL )
	    moviedbError ( "mkdb: not enough memory to generate aka index" ) ;
      }

    }
    count-- ;
    sortedTo = count ;
    (void) fclose ( dbFp ) ;
  }

  listFp = openFile ( akaFile ) ;
  while ( !enddata && fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inaka ) 
    {
      if ( line [ 0 ] == '\n' )
	continue ;
      else if ( strncmp ( line, "-------", 5 ) == 0 ) 
	enddata = TRUE ;
      else if ( line [ 0 ] == ' ' )
      {
	/* skip aka-lines for movies that are not in the movies.list */
	if (NOTITLE == primaryKey)
	  continue;
        stripEOL ( line ) ;
        if ( ( ptr = strchr ( line, '\t' ) ) != NULL )
        {
          *(ptr - 1) = '\0' ;
          aka [ count ] . attrKey = attrKeyLookup ( ++ptr, attrCount ) ;
        }
        else
        {
          aka [ count ] . attrKey = NOATTR ;
          if ( ( ptr = strrchr ( line, ')' ) ) == NULL )
            continue ;
          *ptr = '\0' ;
        }
        if ( debugFlag )
           (void) printf ( "%s\n", line ) ;
	secondaryKey = titleKeyLookupReadOnly ( line + 8, titles, titleCountValid ) ;
	if (NOTITLE == secondaryKey)
	  {
	    /* binary search failed. Try to locate title in the appended akas */
	    /* for this primary title (linear search) */
	    TitleID i;
	    for (i = startSearchTitle; i < *titleCount; i++)
	      if (0 == caseCompare ( titles [ i ] . title, line + 8 ))
		{
		  secondaryKey = i;
		  break;
		}

	    if (NOTITLE == secondaryKey)
	      {
		/* data is not in titleIndex. Append to titleIndex instead of insert */
		/* We use titleCountValid for lookups so this should be no problem. */
		secondaryKey = *titleCount;
		titles [ secondaryKey ] . title = duplicateString ( line + 8 ) ; 
		titles [ secondaryKey ] . titleKey = secondaryKey ;
		*titleCount = *titleCount + 1 ;
		if ( *titleCount >= MAXTITLES ) 
		  moviedbError ( "mkdb: too many titles -- increase MAXTITLES" ) ;
	      }
	  }

	aka [ count ] . aka = secondaryKey;
        aka [ count ] . primary = primaryKey ;
        processed++ ;
        if ( sortedTo == 0 ) 
          count++ ;
        else
          if ( bsearch ( (void*) &(aka [ count ]) , (void*) aka, (size_t) sortedTo, sizeof ( struct akaData ), (int (*) (const void*, const void*)) akaDataSort ) == NULL )
            count++ ;
	if (count >= akaIndexSize )
	{
	    struct akaData *akaIndex = aka ;

	    akaIndexSize += AKAGROW ;
	    aka = realloc ( akaIndex, sizeof (struct akaData) * akaIndexSize ) ;
	    if ( aka == NULL )
	      moviedbError ( "mkdb: not enough memory to generate aka index" ) ;
	}
      }
      else
      {
        stripEOL ( line ) ;
	primaryKey = titleKeyLookupReadOnly ( line, titles, titleCountValid ) ;
	if (NOTITLE == primaryKey)
	  printf ("WARNING: %s not in movies.list\n", line);
	startSearchTitle = *titleCount;	    
      }
    }
    else 
      if ( strncmp ( line, "AKA", 3 ) == 0 ) 
      {
	(void) fgets ( line, MXLINELEN, listFp ) ;
        if ( strncmp ( line, "===", 3 ) == 0 )
          inaka = TRUE ; 
      }
 
  (void) fclose ( listFp ) ;

  /* now sort titles index */
  (void) qsort ( (void*) titles, (size_t) *titleCount, sizeof ( struct titleIndexRec ), (int (*) (const void*, const void*)) titleIndexRecSort ) ;
  /* sanity check */
  if (1)
    {
      TitleID i;
      for (i = 1; i < *titleCount; i++)
	if (0 == caseCompare(titles [i-1].title, titles [i].title))
	  printf ("WARNING: titles.key contains 2 identical movies (%s)\n", titles [i].title);
    }

  (void) qsort ( (void*) aka, (size_t) count, sizeof ( struct akaData ), (int (*) (const void*, const void*)) akaDataSort ) ;

  dbFp = writeFile ( AKADB ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( aka [ i ] . primary, dbFp ) ;
     putTitle ( aka [ i ] . aka, dbFp ) ;
     putAttr ( aka [ i ] . attrKey, dbFp ) ;
  }
  (void) fclose ( dbFp ) ; 

  (void) qsort ( (void*) aka, (size_t) count, sizeof ( struct akaData ), (int (*) (const void*, const void*)) akaIndexDataSort ) ;

  dbFp = writeFile ( AKAIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( aka [ i ] . aka, dbFp ) ;
     putTitle ( aka [ i ] . primary, dbFp ) ;
  }
  (void) fclose ( dbFp ) ; 

  return ( processed ) ;
}


int akaNameDataAlphaSort (struct akaNameData *a1, struct akaNameData *a2)
{
    return ( caseCompare ( a1 -> akaString, a2 -> akaString ) ) ;
}


int akaNameDataSort (struct akaNameData *a1, struct akaNameData *a2)
{
  if ( a1 -> primary != a2 -> primary )
    return ( a1 -> primary - a2 -> primary ) ;
  else
    return ( a1 -> aka - a2 -> aka ) ;
}


int akaNameIndexDataSort (struct akaNameData *a1, struct akaNameData *a2)
{
  return ( a1 -> aka - a2 -> aka ) ;
}


NameID processAkaNamesList ( NameID *nameCount )
{ 
  FILE *dbFp, *listFp, *nameKeyFp, *tmpFp ; 
  struct akaNameData *naka ;
  char line [ MXLINELEN ] ; 
  char   keyFileData [ MXLINELEN ] ;
  char   prevName [ MXLINELEN ] ;
  char   *keyPtr = NULL ;
  int inaka = FALSE, enddata = FALSE, skipMode, compare ;
  NameID count = 0, i ;
  NameID primaryKey = NONAME ;
  char *ptr ;

  naka = (struct akaNameData*) calloc ( MAXNAKAENTRIES, sizeof ( struct akaNameData ) ) ;
  if ( naka == NULL )
    moviedbError ( "mkdb: not enough memory to generate aka names list" ) ;

  prevName [ 0 ] = '\0' ;

  if ( isReadable ( NAMEKEY ) )
  {
    tmpFp = copyFile ( NAMEKEY ) ;
    (void) fgets ( keyFileData, MXLINELEN, tmpFp ) ;
    if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
      *keyPtr++ = '\0' ;
  }
  else
    tmpFp = NULL ;

  nameKeyFp = writeFile ( NAMEKEY ) ;

  listFp = openFile ( NAKALIST ) ;
  while ( !enddata && fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inaka ) 
    {
      if ( line [ 0 ] == '\n' )
	continue ;
      else if (line [ 0 ] == '-' )
	enddata = TRUE ;
      else if ( line [ 0 ] != ' ' )
      {
        stripEOL ( line ) ;
        if ( debugFlag )
          (void) printf ( "%s\n", line ) ;
        if ( caseCompare ( prevName, line ) >= 0 )
        {
          skipMode = TRUE ;
          if ( debugFlag ) 
            printf ( "skipped: %s\n", line ) ;
          continue ;
        }
        else
          (void) strcpy ( prevName, line ) ;
        skipMode = FALSE ;

        if ( tmpFp == NULL )
        {
           primaryKey = (*nameCount)++ ;
           (void) fprintf ( nameKeyFp, "%s|%lx\n", line, primaryKey ) ;
        }
        else
        {
           compare = caseCompare ( keyFileData, line ) ;
           while ( compare < 0 )
           {
             (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
             if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
             {
               if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                 *keyPtr++ = '\0' ;
               compare = caseCompare ( keyFileData, line ) ;
             }
             else
             {
               compare = 1 ;
               (void) fclose ( tmpFp ) ;
               tmpFp = NULL ;
             }
           }
           if ( compare == 0 )
           {
             primaryKey = strtol ( keyPtr, (char **) NULL, 16) ;
             (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
             if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
             {
               if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
                 *keyPtr++ = '\0' ;
             }
             else
             {
               (void) fclose ( tmpFp ) ;
               tmpFp = NULL ;
             }
           }
           else
           {
             primaryKey = (*nameCount)++ ;
             (void) fprintf ( nameKeyFp, "%s|%lx\n", line, primaryKey ) ;
           }
        }
      }
      else
      {
        if ( ( ptr = strrchr ( line, ')' ) ) != NULL )
        {
          *ptr = '\0' ;
           naka [ count ] . akaString = duplicateString ( line + 8 ) ;
           naka [ count ] . primary = primaryKey ;
           count++ ;
	   if (count >= MAXNAKAENTRIES)
	     moviedbError ( "mkdb: too many alternate names -- increase MAXNAKAENTRIES" ) ;
        }
      }
    }
    else 
      if ( strncmp ( line, "AKA", 3 ) == 0 ) 
      {
	(void) fgets ( line, MXLINELEN, listFp ) ;
        if ( strncmp ( line, "===", 3 ) == 0 )
          inaka = TRUE ; 
      }

  (void) fclose ( listFp ) ;
  if ( tmpFp != NULL )
  {
    (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
    while ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      (void) fprintf ( nameKeyFp, "%s", keyFileData ) ;
    (void) fclose ( tmpFp ) ;
  }
  (void) fclose ( nameKeyFp ) ;

  (void) qsort ( (void*) naka, (size_t) count, sizeof ( struct akaNameData ), (int (*) (const void*, const void*)) akaNameDataAlphaSort ) ;

  prevName [ 0 ] = '\0' ;
  tmpFp = copyFile ( NAMEKEY ) ;
  (void) fgets ( keyFileData, MXLINELEN, tmpFp ) ;
  if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
    *keyPtr++ = '\0' ;

  nameKeyFp = writeFile ( NAMEKEY ) ;

  for ( i = 0 ; i < count ; i++ )
  {
    (void) strcpy ( prevName, naka [ i ] . akaString ) ;
    compare = caseCompare ( keyFileData, naka [ i ] . akaString ) ;
    while ( compare < 0 )
    {
      (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
      if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      {
        if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
          *keyPtr++ = '\0' ;
        compare = caseCompare ( keyFileData, naka [ i ] . akaString ) ;
      }
      else
      {
        compare = 1 ;
        (void) fclose ( tmpFp ) ;
        tmpFp = NULL ;
      }
    }
    if ( compare == 0 )
    {
      naka [ i ] . aka = strtol ( keyPtr, (char **) NULL, 16) ;
      (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
      if ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      {
        if ( ( keyPtr = strchr ( keyFileData, FSEP ) ) != NULL )
          *keyPtr++ = '\0' ;
      }
      else
      {
        (void) fclose ( tmpFp ) ;
        tmpFp = NULL ;
      }
    }
    else
    {
      naka [ i ] . aka = (*nameCount)++ ;
      (void) fprintf ( nameKeyFp, "%s|%lx\n", naka [ i ] . akaString, naka [ i ] . aka ) ;
    }
  }
  if ( tmpFp != NULL )
  {
    (void) fprintf ( nameKeyFp, "%s|%s", keyFileData, keyPtr ) ;
    while ( fgets ( keyFileData, MXLINELEN, tmpFp ) != NULL )
      (void) fprintf ( nameKeyFp, "%s", keyFileData ) ;
    (void) fclose ( tmpFp ) ;
  }
  (void) fclose ( nameKeyFp ) ;

  (void) qsort ( (void*) naka, (size_t) count, sizeof ( struct akaNameData ), (int (*) (const void*, const void*)) akaNameDataSort ) ;

  dbFp = writeFile ( NAKADB ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putName ( naka [ i ] . primary, dbFp ) ;
     putName ( naka [ i ] . aka, dbFp ) ;
  }
  (void) fclose ( dbFp ) ; 

  (void) qsort ( (void*) naka, (size_t) count, sizeof ( struct akaNameData ), (int (*) (const void*, const void*)) akaNameIndexDataSort ) ;

  dbFp = writeFile ( NAKAIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putName ( naka [ i ] . aka, dbFp ) ;
     putName ( naka [ i ] . primary, dbFp ) ;
  }
  (void) fclose ( dbFp ) ; 
  free ( (void *) naka ) ;

  return ( count ) ;
}


TitleID processTitleInfoList ( struct titleIndexRec *titles, TitleID *titleCount, AttributeID *attrCount, int listId )
{
  char  *tmptr, *attrptr ;
  FILE  *listFp, *dbFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char  line [ MXLINELEN ] ; 
  int   inMovie = FALSE ;
  TitleID   count = 0, i, titleKey, prevTitleKey = NOTITLE, idxCount = 0 ;
  AttributeID attrKey ;

  listFp = openFile ( titleInfoDefs [ listId ] . list ) ;
  dbFp = writeFile ( titleInfoDefs [ listId ] . db ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && ( strncmp ( line, "-------", 5 ) == 0 ) ) 
      break ;
    else if ( inMovie ) 
    {
       stripEOL ( line ) ;
       if ( ( tmptr = strchr ( line, '\t' ) ) != NULL )
       {
          *tmptr = '\0' ;
          if ( debugFlag )
            (void) printf ( "%s\n", line ) ;
          titleKey = titleKeyLookup ( line, titles, titleCount ) ;
          if ( titleKey != prevTitleKey )
          {
            titlesIndex [ idxCount ] . titleKey = titleKey ;
            titlesIndex [ idxCount++ ] .  offset = ftell ( dbFp ) ;
            prevTitleKey = titleKey ;
          }

          while ( *++tmptr == '\t' ) ;
          if ( ( attrptr = strchr ( tmptr, '\t' ) ) != NULL )
          {
            *attrptr = '\0' ;
            attrptr++ ;
            attrKey = attrKeyLookup ( attrptr, attrCount ) ;
          }
          else
            attrKey = NOATTR ;
          putTitle ( titleKey, dbFp ) ;
          putByte ( strlen ( tmptr), dbFp ) ;
          putString ( tmptr, dbFp ) ;
          putAttr ( attrKey, dbFp ) ;
          count++ ;     
	 if (count >= sharedTitleIndexSize)
	 {
	   sharedTitleIndexSize += TIGROW ;
	   titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	    if ( titlesIndex == NULL )
	      moviedbError ( "mkdb: not enough memory to generate title info index" ) ;
	    sharedTitleIndex = titlesIndex ;
         }
       }
    }
    else 
      if ( strcmp ( line, titleInfoDefs [ listId ] . start ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }
  
  (void) fclose ( listFp ) ;
  (void) fclose ( dbFp ) ;

  (void) qsort ( (void*) titlesIndex, (size_t) idxCount, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;
  indexFp = writeFile ( titleInfoDefs [ listId ] . index ) ;
  for ( i = 0 ; i < idxCount ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }
  (void) fclose ( indexFp ) ; 

  return ( count ) ;
}


TitleID processBusinessList ( struct titleIndexRec *titles, TitleID *titleCount )
{ 
  FILE *dbFp, *listFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char line [ MXLINELEN ] ; 
  int  inLit = FALSE ; 
  TitleID  count = 0, i ;
  long currentOffset ;

  listFp = openFile ( BUSLIST ) ;
  dbFp = writeFile ( BUSDB ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inLit ) 
    {
       if ( line [ 0 ] != '-' )
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "MV: ", 4 ) == 0 )
         {
           stripEOL ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 4 ) ;
           titlesIndex [ count ] . titleKey = titleKeyLookup ( line + 4, titles, titleCount ) ;
           titlesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= sharedTitleIndexSize)
	   {
	     sharedTitleIndexSize += TIGROW ;
	     titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	      if ( titlesIndex == NULL )
		moviedbError ( "mkdb: not enough memory to generate business index" ) ;
	      sharedTitleIndex = titlesIndex ;
           }
         }
       }
    }
    else 
      if ( strncmp ( line, "BUSINESS LIST", 13 ) == 0 ) 
      {
        inLit = TRUE ; 
        (void) fprintf ( dbFp, "%s", line ) ; 
      }
 
  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  indexFp = writeFile ( BUSIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  return ( count ) ;
} 


TitleID processLaserDiscList ( struct titleIndexRec *titles, TitleID *titleCount )
{ 
  FILE *dbFp, *listFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char line [ MXLINELEN ] ; 
  int  inLD = FALSE ; 
  TitleID  count = 0, i ;
  long currentOffset = 0 ;

  listFp = openFile ( LDLIST ) ;
  dbFp = writeFile ( LDDB ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inLD ) 
    {
       if ( line [ 0 ] != '-' )
       {
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "OT: ", 4 ) == 0 )
         {
           stripEOL ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 4 ) ;
           titlesIndex [ count ] . titleKey = titleKeyLookup ( line + 4, titles, titleCount ) ;
           titlesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= sharedTitleIndexSize)
	   {
	     sharedTitleIndexSize += TIGROW ;
	     titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	      if ( titlesIndex == NULL )
		moviedbError ( "mkdb: not enough memory to generate laser index" ) ;
	      sharedTitleIndex = titlesIndex ;
           }
         }
       }
       else
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "--\n", line ) ; 
       }
    }
    else 
      if ( strncmp ( line, "LASERDISC LIST", 14 ) == 0 ) 
      {
        inLD = TRUE ; 
        (void) fprintf ( dbFp, "%s", line ) ; 
      }
 
  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  indexFp = writeFile ( LDIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  return ( count ) ;
} 



TitleID processLiteratureList ( struct titleIndexRec *titles, TitleID *titleCount )
{ 
  FILE *dbFp, *listFp, *indexFp ; 
  struct titleKeyOffset *titlesIndex = sharedTitleIndex ;
  char line [ MXLINELEN ] ; 
  int  inLit = FALSE ; 
  TitleID  count = 0, i ;
  long currentOffset ;

  listFp = openFile ( LITLIST ) ;
  dbFp = writeFile ( LITDB ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inLit ) 
    {
       if ( line [ 0 ] != '-' )
       {
         currentOffset = ftell ( dbFp ) ;
         (void) fprintf ( dbFp, "%s", line ) ; 
         if ( strncmp ( line, "MOVI: ", 6 ) == 0 )
         {
           stripEOL ( line ) ;
           if ( debugFlag )
             (void) printf ( "%s\n", line + 6 ) ;
           titlesIndex [ count ] . titleKey = titleKeyLookup ( line + 6, titles, titleCount ) ;
           titlesIndex [ count ] . offset = currentOffset ;
           count++ ;
	   if (count >= sharedTitleIndexSize)
	   {
	     sharedTitleIndexSize += TIGROW ;
	     titlesIndex = realloc ( sharedTitleIndex, sizeof (struct titleKeyOffset) * sharedTitleIndexSize );
	      if ( titlesIndex == NULL )
		moviedbError ( "mkdb: not enough memory to generate lit index" ) ;
	      sharedTitleIndex = titlesIndex ;
           }
         }
       }
    }
    else 
      if ( strncmp ( line, "LITERATURE LIST", 14 ) == 0 ) 
      {
        inLit = TRUE ; 
        (void) fprintf ( dbFp, "%s", line ) ; 
      }
 
  (void) fclose ( dbFp ) ; 
  (void) fclose ( listFp ) ; 

  (void) qsort ( (void*) titlesIndex, (size_t) count, sizeof ( struct titleKeyOffset ), (int (*) (const void*, const void*)) titleKeyOffsetSort ) ;

  indexFp = writeFile ( LITIDX ) ;

  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( titlesIndex [ i ] . titleKey, indexFp ) ;
     putOffset ( titlesIndex [ i ] . offset, indexFp ) ;
  }

  (void) fclose ( indexFp ) ; 
  return ( count ) ;
} 


int compCastSort ( struct titleKeyOffset *t1, struct titleKeyOffset *t2 )
{
  return ( t1 -> titleKey - t2 -> titleKey ) ;
}


TitleID processCompleteCastList ( struct titleIndexRec *titles, TitleID *titleCount )
{
  char  *tmptr ;
  FILE  *listFp, *dbFp ; 
  struct compCastRec *list ;
  char  line [ MXLINELEN ] ; 
  int   inMovie = FALSE ;
  TitleID   count = 0, i ;

  list = (struct compCastRec*) calloc ( MAXTITLEINFO, sizeof ( struct compCastRec ) ) ;
  if ( list == NULL )
    moviedbError ( "mkdb: not enough memory to generate complete cast list" ) ;

  listFp = openFile ( CASTCOMLIST ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && ( strncmp ( line, "-------", 5 ) == 0 ) ) 
      break ;
    else if ( inMovie ) 
    {
       stripEOL ( line ) ;
       if ( ( tmptr = strchr ( line, '\t' ) ) != NULL )
       {
          *tmptr = '\0' ;
          while ( *++tmptr == '\t' ) ;
          if ( debugFlag )
            (void) printf ( "%s\n", line ) ;
          list [ count ] . titleKey = titleKeyLookup ( line, titles, titleCount ) ;
          if ( strncmp ( tmptr, "Complete+Verified", 17 ) == 0 )
            list [ count ] . status = verified ;
          else
            list [ count ] . status = completed ;
          count++ ;     
	  if (count >= MAXTITLEINFO)
	    moviedbError ( "mkdb: too many complete casts -- increase MAXTITLEINFO" ) ;
       }
    }
    else 
      if ( strcmp ( line, "CAST COVERAGE TRACKING LIST\n" ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }
  
  (void) fclose ( listFp ) ;

  (void) qsort ( (void*) list, (size_t) count, sizeof ( struct compCastRec ), (int (*) (const void*, const void*)) compCastSort ) ;
  dbFp = writeFile ( CASTCOMDB ) ;
  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( list [ i ] . titleKey, dbFp ) ;
     putByte ( list [ i ] . status, dbFp ) ;
  }
  (void) fclose ( dbFp ) ;
  free ( (void*) list ) ;

  return ( count ) ;
}


TitleID processCompleteCrewList ( struct titleIndexRec *titles, TitleID *titleCount )
{
  char  *tmptr ;
  FILE  *listFp, *dbFp ; 
  struct compCastRec *list ;
  char  line [ MXLINELEN ] ; 
  int   inMovie = FALSE ;
  TitleID   count = 0, i ;

  list = (struct compCastRec*) calloc ( MAXTITLEINFO, sizeof ( struct compCastRec ) ) ;
  if ( list == NULL )
    moviedbError ( "mkdb: not enough memory to generate complete cast crew list" ) ;

  listFp = openFile ( CREWCOMLIST ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && ( strncmp ( line, "-------", 5 ) == 0 ) ) 
      break ;
    else if ( inMovie ) 
    {
       stripEOL ( line ) ;
       if ( ( tmptr = strchr ( line, '\t' ) ) != NULL )
       {
          *tmptr = '\0' ;
          while ( *++tmptr == '\t' ) ;
          if ( debugFlag )
            (void) printf ( "%s\n", line ) ;
          list [ count ] . titleKey = titleKeyLookup ( line, titles, titleCount ) ;
          if ( strncmp ( tmptr, "Complete+Verified", 17 ) == 0 )
            list [ count ] . status = verified ;
          else
            list [ count ] . status = completed ;
          count++ ;     
	  if (count >= MAXTITLEINFO)
	    moviedbError ( "mkdb: too many complete crews -- increase MAXTITLEINFO" ) ;
       }
    }
    else 
      if ( strcmp ( line, "CREW COVERAGE TRACKING LIST\n" ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }
  
  (void) fclose ( listFp ) ;

  (void) qsort ( (void*) list, (size_t) count, sizeof ( struct compCastRec ), (int (*) (const void*, const void*)) compCastSort ) ;
  dbFp = writeFile ( CREWCOMDB ) ;
  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( list [ i ] . titleKey, dbFp ) ;
     putByte ( list [ i ] . status, dbFp ) ;
  }
  (void) fclose ( dbFp ) ;
  free ( (void*) list ) ;

  return ( count ) ;
}



int movieLinkRecSort ( struct movieLinkDbRec *ml1, struct movieLinkDbRec *ml2 )
{
  if ( ml1 -> fromTitleKey != ml2 -> fromTitleKey )
    return ( ml1 -> fromTitleKey - ml2 -> fromTitleKey ) ;
  else
    return ( ml1 -> position - ml2 -> position ) ;
}


TitleID processMovieLinksList ( struct titleIndexRec *titles, TitleID *titleCount )
{
  FILE  *listFp, *dbFp ; 
  struct movieLinkDbRec *list ;
  size_t linkSize = LINKSTART ;
  char  line [ MXLINELEN ] ; 
  char *ptr ;
  int   inMovie = FALSE, i, position = 0 ;
  TitleID   count = 0, currentTitleKey = NOTITLE ;

  list = (struct movieLinkDbRec*) calloc ( LINKSTART, sizeof ( struct movieLinkDbRec ) ) ;
  if ( list == NULL )
    moviedbError ( "mkdb: not enough memory to generate movie links list" ) ;

  listFp = openFile ( LINKLIST ) ;
   
  while ( fgets ( line, MXLINELEN, listFp ) != NULL ) 
    if ( inMovie && ( strncmp ( line, "-------", 5 ) == 0 ) ) 
      break ;
    else if ( line [ 0 ] == '\n' )
      continue ;
    else if ( inMovie ) 
    {
       if ( line [ 0 ] != ' ' )
       {
         stripEOL ( line ) ;
         currentTitleKey = titleKeyLookup ( line, titles, titleCount ) ;
         position = 0 ;
         if ( debugFlag )
           (void) printf ( "%s\n", line ) ;
       }
       else
       {
         list [ count ] . fromTitleKey = currentTitleKey ;
         for ( i = 0 ; i < NO_OF_LINK_TYPES - 1 ; i++ )
           if ( strncmp ( line + 3, movieLinkDefs [ i ] . string, movieLinkDefs [ i ] . length ) == 0 )
           {
             list [ count ] . link = movieLinkDefs [ i ] . link ;
             if ( ( ptr = strrchr ( line, ')' ) ) != NULL )
             {
               *ptr = '\0' ;
               list [ count ] . toTitleKey = titleKeyLookup ( line + 4 + movieLinkDefs [ i ] . length, titles, titleCount ) ;
               list [ count ] . position = position++ ;
               count++ ;
	       if ( count >= linkSize )
	       {
		 struct movieLinkDbRec *links ;

		 linkSize += LINKGROW ;
		 links = realloc ( list, sizeof (struct movieLinkDbRec) * linkSize ) ;
		 if ( links == NULL )
		   moviedbError ( "mkdb: not enough memory to generate link index" ) ;
	       }
             }
             break ;
           }
       }
    }
    else 
      if ( strcmp ( line, "MOVIE LINKS LIST\n" ) == 0 ) 
      {
        inMovie = TRUE ; 
        (void) fgets ( line, MXLINELEN, listFp ) ;
      }
  
  (void) fclose ( listFp ) ;

  dbFp = writeFile ( LINKDB ) ;
  (void) qsort ( (void*) list, (size_t) count, sizeof ( struct movieLinkDbRec ), (int (*) (const void*, const void*)) movieLinkRecSort ) ;
  for ( i = 0 ; i < count ; i++ )
  {
     putTitle ( list [ i ] . fromTitleKey, dbFp ) ;
     putByte ( list [ i ] . link, dbFp ) ;
     putTitle ( list [ i ] . toTitleKey, dbFp ) ;
  }
  (void) fclose ( dbFp ) ; 
  free ( (void*) list ) ;

  return ( count ) ;
}


void touchFile ( char *filename )
{
  FILE *fp ;

  if ( ! isReadable ( filename ) )
  {
    fp = writeFile ( filename ) ;
    (void) fclose ( fp ) ;
  }
}


void touchDatabases ( NameID nameCount, TitleID titleCount, AttributeID attrCount )
{
  int i ;
  char fn [ MAXPATHLEN ] ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    (void) constructFilename ( fn, filmographyDefs [ i ] . stem, DBSEXT ) ;
    touchFile ( fn ) ;
    (void) constructFilename ( fn, filmographyDefs [ i ] . stem, NDXEXT ) ;
    touchFile ( fn ) ;
    (void) constructFilename ( fn, filmographyDefs [ i ] . stem, TDXEXT ) ;
    touchFile ( fn ) ;
  }

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
  {
    touchFile ( triviaDefs [ i ] . db ) ;
    touchFile ( triviaDefs [ i ] . index ) ;
  }

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
  {
    touchFile ( titleInfoDefs [ i ] . db )  ;
    touchFile ( titleInfoDefs [ i ] . index )  ;
  }

  touchFile ( MRRDB ) ;
  touchFile ( VOTEDB ) ;
  touchFile ( MOVIEDB ) ;
  touchFile ( TIMESDB ) ;
  touchFile ( AKADB ) ;
  touchFile ( AKAIDX ) ;
  touchFile ( NAKADB ) ;
  touchFile ( NAKAIDX ) ;
  touchFile ( PLOTDB ) ;
  touchFile ( PLOTIDX ) ;
#ifdef INTERNAL
  touchFile ( OUTLDB ) ;
  touchFile ( OUTLIDX ) ;
#endif
  touchFile ( BIODB ) ;
  touchFile ( BIOIDX ) ;
  touchFile ( LITDB ) ;
  touchFile ( LITIDX ) ;
  touchFile ( BUSDB ) ;
  touchFile ( BUSIDX ) ;
  touchFile ( LDDB ) ;
  touchFile ( LDIDX ) ;
  touchFile ( CASTCOMDB ) ;
  touchFile ( CREWCOMDB ) ;
  touchFile ( LINKDB ) ;

  writeNameIndexKey ( nameCount ) ;
  writeTitleIndexKey ( titleCount ) ;
  writeAttrIndexKey ( attrCount ) ;
}


void readKeyCounts ( NameID *nameCount, TitleID *titleCount, AttributeID *attrCount )
{
  FILE *fp ;

  if ( ( fp = fopen ( KEYCOUNTS, "r" ) ) == NULL ) 
  {
     *nameCount = 0 ;
     *titleCount = 0 ;
     *attrCount = 0 ;
  }
  else
  {
     *nameCount = getName ( fp ) ;
     *titleCount = getTitle ( fp ) ;
     *attrCount = getAttr ( fp ) ;
     (void) fclose ( fp ) ;
  }
}


void writeKeyCounts ( NameID nameCount, TitleID titleCount, AttributeID attrCount )
{
  FILE *fp ;

  if ( ( fp = fopen ( KEYCOUNTS, "w" ) ) != NULL ) 
  {
     putName ( nameCount, fp ) ;
     putTitle ( titleCount, fp ) ;
     putAttr ( attrCount, fp ) ;
  }
  else
    moviedbError ( "mkdb: error writing key counts file" ) ;
  (void) fclose ( fp ) ;
}


int main (int argc, char **argv)
{
  int  addaka = FALSE, addrate = FALSE, addmovie = FALSE, addnaka = FALSE ; 
  int  addplot = FALSE, addbio = FALSE, addvotes = FALSE, addakaf = FALSE ;
#ifdef INTERNAL
  int  addoutline = FALSE ;
#endif
  int  addlit = FALSE, addcastcom = FALSE, addlinks = FALSE, addbus = FALSE ;
  int  addld = FALSE, addcrewcom = FALSE, moviesOnly = FALSE, nochar = FALSE ;
  int  specific = FALSE ;
  int  createFlag = FALSE ;
  int  err = FALSE ;
  int  i, j, found ;
  long listCount ;
  TitleID  count ;
  int  addflags [ NO_OF_FILMOGRAPHY_LISTS ] ;
  int  trivflags [ NO_OF_TRIV_LISTS ] ;
  int  titleInfoFlags [ NO_OF_TITLE_INFO_LISTS ] ;
  int  loadTitles = FALSE, loadAttrs = FALSE ;
  char akaFile [ MAXPATHLEN ] ;

  static struct titleIndexRec titles [ MAXTITLES ] ;

  TitleID titleCount = 0 ;
  AttributeID attrCount = 0 ;
  NameID  nameCount = 0 ;

  sharedTitleIndexSize = TISTART ;
  sharedTitleIndex = malloc ( sizeof (struct titleKeyOffset) * sharedTitleIndexSize ) ;
  if ( sharedTitleIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate titles index" ) ;

  attributeIndexSize = ATTRSTART ;
  attributeIndex = malloc ( sizeof (struct attrIndexRec) * attributeIndexSize ) ;
  if ( attributeIndex == NULL )
    moviedbError ( "mkdb: not enough memory to generate attribute index" ) ;

  akaFile [ 0 ] = '\0' ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
    addflags [ i ] = FALSE ;

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    trivflags [ i ] = FALSE ;

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    titleInfoFlags [ i ] = FALSE ;

  for ( i = 1; i < argc; i++ )
    if ( strcmp ( "-movie", argv [ i ] ) == 0 )
    {
      addmovie =  TRUE ;
      loadTitles = TRUE ;
      loadAttrs = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-mrr", argv [ i ] ) == 0 )
    {
      addrate =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-aka", argv [ i ] ) == 0 )
    {
      addaka =  TRUE ;
      loadTitles = TRUE ;
      loadAttrs = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-naka", argv [ i ] ) == 0 )
    {
      addnaka =  TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-akaf", argv [ i ] ) == 0 )
    {
      if ( ++i < argc )
        (void) strcpy ( akaFile, argv [ i ] ) ;
      else
        err = TRUE ;
      addakaf =  TRUE ;
      loadTitles = TRUE ;
      loadAttrs = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-plot", argv [ i ] ) == 0 )
    {
      addplot =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
#ifdef INTERNAL
    else if ( strcmp ( "-outline", argv [ i ] ) == 0 )
    {
      addoutline =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
#endif
    else if ( strcmp ( "-bio", argv [ i ] ) == 0 )
    {
      addbio =  TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-votes", argv [ i ] ) == 0 )
    {
      addvotes =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-lit", argv [ i ] ) == 0 )
    {
      addlit =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-bus", argv [ i ] ) == 0 )
    {
      addbus =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-ld", argv [ i ] ) == 0 )
    {
      addld =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-link", argv [ i ] ) == 0 )
    {
      addlinks =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-castcom", argv [ i ] ) == 0 )
    {
      addcastcom =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-crewcom", argv [ i ] ) == 0 )
    {
      addcrewcom =  TRUE ;
      loadTitles = TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-m", argv [ i ] ) == 0 )
      moviesOnly =  TRUE ;
    else if ( strcmp ( "-nochar", argv [ i ] ) == 0 )
      nochar =  TRUE ;
    else if ( strcmp ( "-create", argv [ i ] ) == 0 )
    {
      createFlag =  TRUE ;
      specific = TRUE ;
    }
    else if ( strcmp ( "-debug", argv [ i ] ) == 0 )
      debugFlag =  TRUE ;
    else
    {
      found = FALSE ;
      for ( j = 0 ; j < NO_OF_FILMOGRAPHY_LISTS ; j++ )
        if ( strcmp ( filmographyDefs [ j ] . option, argv [ i ] ) == 0 )
        {
          found = TRUE ;
          addflags [ j ] = TRUE ;
          loadTitles = TRUE ;
          loadAttrs = TRUE ;
          specific = TRUE ;
          break ;
        }
      for ( j = 0 ; j < NO_OF_TRIV_LISTS ; j++ )
        if ( strcmp ( triviaDefs [ j ] . option, argv [ i ] ) == 0 )
        {
          found = TRUE ;
          trivflags [ j ] = TRUE ;
          loadTitles = TRUE ;
          specific = TRUE ;
          break ;
        }
      for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
        if ( strcmp ( titleInfoDefs [ j ] . option, argv [ i ] ) == 0 )
        {
          found = TRUE ;
          titleInfoFlags [ j ] = TRUE ;
          loadTitles = TRUE ;
          loadAttrs = TRUE ;
          specific = TRUE ;
          break ;
        }
      if ( ! found )
      {
        (void) fprintf ( stderr, "mkdb: unrecognised option %s\n", argv[i] ) ;
        err = TRUE ;
      }
    }

  if ( err ) 
    moviedbUsage ( MKDB_USAGE1, MKDB_USAGE2, MKDB_USAGE3, MKDB_USAGE4, MKDB_USAGE5, NULL ) ;

  if ( ! specific )
  {
    for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
      addflags [ i ] = TRUE ;
    for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
      trivflags [ i ] = TRUE ;
    for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
      titleInfoFlags [ i ] = TRUE ;
    addmovie = TRUE ;
    addaka = TRUE ;
    addnaka = TRUE ;
    addrate = TRUE ;
    addplot = TRUE ;
#ifdef INTERNAL
    addoutline = TRUE ;
#endif
    addbio = TRUE ;
    addvotes = TRUE ;
    addlit = TRUE ;
    addbus = TRUE ;
    addld = TRUE ;
    addcastcom = TRUE ;
    addcrewcom = TRUE ;
    addlinks = TRUE ;
    loadTitles = TRUE ;
    loadAttrs = TRUE ;
    createFlag = TRUE ;
  }

  readKeyCounts ( &nameCount, &titleCount, &attrCount ) ;

  if ( loadTitles )
    titleCount = readTitleAlphaKey ( titles ) ;
  if ( loadAttrs )
    attrCount = readAttrAlphaKey ( ) ;

  if ( addmovie && isReadable ( MOVIELIST ) )
  {
    (void) printf ( "Adding Movies List...\n" ) ;
    count = processMoviesList ( titles, &titleCount, &attrCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  for ( i = 0 ; i < NO_OF_CAST_LISTS ; i++ )
  {
    if ( addflags [ i ] && isReadable ( filmographyDefs [ i ] . list ) )
    {
      (void) printf ( "Adding %s...\n", filmographyDefs [ i ] . desc ) ;
      listCount = processCastList ( &nameCount, titles, &titleCount, &attrCount, i, moviesOnly, nochar ) ;
      (void) printf ( " ...%ld read\n", listCount ) ;
    }
  }

  for ( i = NO_OF_CAST_LISTS ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    if ( addflags [ i ] && isReadable ( filmographyDefs [ i ] . list ) )
    {
      (void) printf ( "Adding %s...\n", filmographyDefs [ i ] . desc ) ;
      if ( i == WRITER_LIST_ID )
        listCount = processWriterFilmographyList ( &nameCount, titles, &titleCount, &attrCount, i, moviesOnly ) ;
      else
        listCount = processFilmographyList ( &nameCount, titles, &titleCount, &attrCount, i, moviesOnly ) ;
      (void) printf ( " ...%ld read\n", listCount ) ;
    }
  }

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    if ( trivflags [ i ] && isReadable ( triviaDefs [ i ] . list ) )
    {
      (void) printf ( "Adding %s...\n", triviaDefs [ i ] . desc ) ;
      count = processTriviaList ( titles, &titleCount, i ) ;
      (void) printf ( " ...%d read\n", count ) ;
    }

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    if ( titleInfoFlags [ i ] && isReadable ( titleInfoDefs [ i ] . list ) )
    {
      (void) printf ( "Adding %s List...\n", titleInfoDefs [ i ] . desc ) ;
      count = processTitleInfoList ( titles, &titleCount, &attrCount, i ) ;
      (void) printf ( " ...%d read\n", count ) ;
    }

  if ( addaka && isReadable ( AKALIST ) )
  {
    (void) printf ( "Adding Aka Titles...\n" ) ;
    count = processAkaList ( titles, &titleCount, &attrCount, AKALIST ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addakaf && isReadable ( akaFile ) )
  {
    (void) printf ( "Adding Local Aka Titles...\n" ) ;
    count = processAkaList ( titles, &titleCount, &attrCount, akaFile ) ;
    (void) printf ( " ...%d read\n", count ) ;
    createFlag = TRUE ;
  }

  if ( addnaka && isReadable ( NAKALIST ) )
  {
    (void) printf ( "Adding Aka Names...\n" ) ;
    listCount = processAkaNamesList ( &nameCount ) ;
    (void) printf ( " ...%ld read\n", listCount ) ;
  }

  if ( addplot && isReadable ( PLOTLIST ) )
  {
    (void) printf ( "Adding Plot Summaries...\n" ) ;
    count = processPlotList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

#ifdef INTERNAL
  if ( addoutline && isReadable ( OUTLLIST ) )
  {
    (void) printf ( "Adding Plot Outlines...\n" ) ;
    count = processOutlineList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }
#endif

  if ( addbio && isReadable ( BIOLIST ) )
  {
    (void) printf ( "Adding Biographies...\n" ) ;
    listCount = processBiographiesList ( &nameCount ) ; 
    (void) printf ( " ...%ld read\n", listCount ) ;
  }

  if ( addrate && isReadable ( MRRLIST ) )
  {
    (void) printf ( "Adding Movie Ratings...\n" ) ;
    count = processMovieRatings ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addvotes && isReadable ( VOTELIST ) )
  {
    (void) printf ( "Adding Votes...\n" ) ;
    count = processVotesList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addlit && isReadable ( LITLIST ) )
  {
    (void) printf ( "Adding Literature List...\n" ) ;
    count = processLiteratureList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addbus && isReadable ( BUSLIST ) )
  {
    (void) printf ( "Adding Business List...\n" ) ;
    count = processBusinessList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addld && isReadable ( LDLIST ) )
  {
    (void) printf ( "Adding LaserDisc List...\n" ) ;
    count = processLaserDiscList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addcastcom && isReadable ( CASTCOMLIST ) )
  {
    (void) printf ( "Adding Cast Completion List...\n" ) ;
    count = processCompleteCastList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addcrewcom && isReadable ( CREWCOMLIST ) )
  {
    (void) printf ( "Adding Crew Completion List...\n" ) ;
    count = processCompleteCrewList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( addlinks && isReadable ( LINKLIST ) )
  {
    (void) printf ( "Adding Movie Links List...\n" ) ;
    count = processMovieLinksList ( titles, &titleCount ) ;
    (void) printf ( " ...%d read\n", count ) ;
  }

  if ( loadTitles )
    writeTitleAlphaKey ( titleCount, titles ) ;

  if ( loadAttrs )
    writeAttrAlphaKey ( attrCount ) ;

  if ( createFlag )
    touchDatabases ( nameCount, titleCount, attrCount ) ;

  writeKeyCounts ( nameCount, titleCount, attrCount ) ;

  return ( 0 ) ;
}

/*============================================================================
 *
 *  Program: alist.c 
 *
 *  Version: 3.7
 *
 *  Author:  C J Needham <col@imdb.com>
 *
 *  Copyright (c) 1996-1999 The Internet Movie Database Inc
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 * 
 *  Options:
 *
 *        -acr  search actors database
 *        -acs  search actress database
 *        -dir  search directors database
 *      -write  search writers database
 *       -comp  search composers database
 *       -cine  search cinematographers database
 *       -edit  search editors database
 *     -prodes  search production designers database
 *    -costdes  search costume designers database
 *      -prdcr  search producers database
 *       -misc  search miscellaneous database
 *
 *      -title  search titles database
 *
 *   -attr <a>  search for entries with attribute equal to string <a>
 *         -aa  search for academy award winners
 *        -aan  search for academy award nominess
 *         -gg  search for Golden Globe winners
 *        -ggn  search for Golden Globe nominees
 *
 *         -yr  add year of release info & sort chronologically
 * -yrfrom <y>  limit search to movies released from year <y> onwards
 *   -yrto <y>  limit search to movies released before year <y>
 *   -yreq <y>  limit search to movies released in year <y>
 *
 *          -m  movies only, ignore TV-series
 *          -i  case sensistive search
 *          -s  enable substring matching for -attr value only
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "moviedb.h"
#include "dbutils.h"
#include "years.h"

#define ALIST_USAGE1 "usage: alist [-acr -acs -dir -write -comp -cine -edit -prodes -costdes -prdcr" 
#define ALIST_USAGE2 "              -misc] [-title] [-yr -yrfrom <year> -yrto <year> -yreq <year>]"
#define ALIST_USAGE3 "             [-m -i -s] -attr <attr>|{-aa -aan}|{-gg -ggn}"

int alistYearSort ( struct alistResult *r1, struct alistResult *r2 )
{
  if ( r1 -> year != r2 -> year )
    return ( r1 -> year - r2 -> year ) ;
  else
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
}


void displayAlistSorted ( struct alistResult *results, int count ) 
{
  int i, len, currentYear = 0 ;

  qsort ( (void*) results, (size_t) count, sizeof ( struct alistResult ), (int (*) (const void*, const void*)) alistYearSort ) ;
  
  for ( i = 0 ; i < count ; i++ )
  {
    if ( currentYear != results [ i ] . year )
    {
      (void) printf ( "\n" ) ;
      currentYear = results [ i ] . year ;
    }
    (void) printf ( "%s", results [ i ] . name ) ;
    len = strlen ( results [ i ] . name ) ;
    if ( len >= 16 )
      (void) printf ( "\t" ) ;
    else if ( len >= 8 )
      (void) printf ( "\t\t" ) ;
    else
      (void) printf ( "\t\t\t" ) ;

    displayTitleYear ( results [ i ] . title, results [ i ] . year ) ;
    if ( results [ i ] . attr != NULL )
      (void) printf ( " %s\n", results [ i ] . attr ) ;
    else
      (void) printf ( "\n" ) ;
  }
}


int alistAttrTest ( struct attrSearchOptRec attrFlags, char *attr, int substring, int casesen, char *listAttr ) 
{
  if ( listAttr == NULL )
    return ( FALSE ) ;

  if ( attrFlags . string )
    if ( substring )
    {
      if ( casesen )
      {
        if ( strstr ( listAttr, attr ) != NULL )
          return ( TRUE ) ;
      }
      else
        if ( caseStrStr ( listAttr, attr ) != NULL )
          return ( TRUE ) ;
    }
    else
    {
      if ( casesen )
      {
        if ( strcmp ( listAttr, attr ) == 0 )
          return ( TRUE ) ;
      }
      else
        if ( caseCompare ( listAttr, attr ) == 0 )
          return ( TRUE ) ;
    }      

  if ( attrFlags . aa )
    if ( strstr ( listAttr, "AA)" ) != NULL )
      return ( TRUE ) ;

  if ( attrFlags . aan )
    if ( strstr ( listAttr, "AAN)" ) != NULL )
      return ( TRUE ) ;

  if ( attrFlags . gg )
    if ( strstr ( listAttr, "GG)" ) != NULL )
      return ( TRUE ) ;

  if ( attrFlags . ggn )
    if ( strstr ( listAttr, "GGN)" ) != NULL )
      return ( TRUE ) ;

  return ( FALSE ) ;
}


int titleKeyAlistCompare ( struct titleDbRec *search, struct titleDbRec *t1 )
{
  return ( search -> titleKey - t1 -> titleKey ) ;
}


int alistYearTest ( int yearFlag, int yrf, int yrt, TitleID titleKey, TitleID nyears, struct titleDbRec *years, int *yearValue ) 
{
  struct titleDbRec *trec, searchRec ;

  if ( yearFlag )
  {
    searchRec . titleKey = titleKey ;
    if ( ( trec = bsearch ( (void*) &searchRec, (void*) years, (size_t) nyears, sizeof ( struct titleDbRec ), (int (*) (const void*, const void*)) titleKeyAlistCompare ) ) != NULL )
      *yearValue = trec -> year ;
    else
      return ( FALSE ) ;
    if ( *yearValue < yrf )
      return ( FALSE ) ;
    if ( *yearValue > yrt )
      return ( FALSE ) ;
    return ( TRUE ) ;
  }
  *yearValue = 0 ;
  return ( TRUE ) ;
}


int alistMoviesOnlyTest ( int mvsonly, char *title )
{
  if ( mvsonly && *title == '"' )
    return ( FALSE ) ;
  else
    return ( TRUE ) ;
}


void alistNames ( int listId, char *attr, int substring, int casesen, struct attrSearchOptRec attrFlags, int mvsonly, int yr, int yrf, int yrt, TitleID nyears, struct titleDbRec *years )
{
  FILE *dbFp, *nameKeyFp, *nameIndexFp, *titleKeyFp, *titleIndexFp, *attrKeyFp, *attrIndexFp ;
  struct formatRec listData [ MAXFILMOGRAPHIES ] ;
  struct alistResult results [ MAXALISTRESULTS ] ;
  int i, count, len, noWithAttr, noWithoutAttr, rcount = 0 ;
  char fn [ MAXPATHLEN ] ;
  char *name ;
  NameID nameKey ;
  TitleID titleKey ;
  AttributeID attrKey ;
  int yearValue, keepFlag ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] .stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;
  nameKeyFp = openFile ( NAMEKEY ) ;
  nameIndexFp = openFile ( NAMEIDX ) ;
  titleKeyFp = openFile ( TITLEKEY ) ;
  titleIndexFp = openFile ( TITLEIDX ) ;
  attrKeyFp = openFile ( ATTRKEY ) ;
  attrIndexFp = openFile ( ATTRIDX ) ;

  while ( TRUE )
  {
    count = 0 ;
    nameKey = getName ( dbFp ) ;
    name = mapNameKeyToText ( nameKey, nameKeyFp, nameIndexFp ) ;
    if ( feof ( dbFp ) )
      break ;
    getFilmographyCounts ( dbFp, &noWithAttr, &noWithoutAttr ) ;
    for ( i = 0 ; i < noWithAttr ; i++ )
    {
      titleKey = getTitle ( dbFp ) ;
      attrKey = getAttr ( dbFp ) ;
      if ( listId < NO_OF_CAST_LISTS )
      {
        len = getByte ( dbFp ) ;
        if ( len > 0 )
          (void) fseek ( dbFp, len, SEEK_CUR ) ;
        (void) getPosition ( dbFp ) ;
      }
	else if ( listId == WRITER_LIST_ID )
      {
	(void) getPosition ( dbFp );
	(void) getPosition ( dbFp );
	(void) getPosition ( dbFp );
      }
      listData [ count ] . titleKey = titleKey ;
      listData [ count ] . attrKey = attrKey ;
      listData [ count ] . title = mapTitleKeyToText ( titleKey, titleKeyFp, titleIndexFp ) ;
      listData [ count ] . attr = mapAttrKeyToText ( attrKey, attrKeyFp, attrIndexFp ) ;
      keepFlag = alistMoviesOnlyTest ( mvsonly, listData [ count ] . title ) ;
      keepFlag = keepFlag && alistYearTest ( yr, yrf, yrt, titleKey, nyears, years, &yearValue ) ;
      keepFlag = keepFlag && alistAttrTest ( attrFlags, attr, substring, casesen, listData [ count ] . attr ) ;
      listData [ count ] . year = yearValue ;
      if ( keepFlag )
      {
        if ( yr )
        {
          results [ rcount ] . name = duplicateString ( name ) ;
          results [ rcount ] . title = listData [ count ] . title ;
          results [ rcount ] . attr = listData [ count ] . attr ;
          results [ rcount ] . year = yearValue ;
          rcount++ ;
        }
        count++ ;
      }
      else
      {
        free ( (void*) listData [ count ] . title ) ;
        free ( (void*) listData [ count ] . attr ) ;
      }
    }
    for ( i = 0 ; i < noWithoutAttr ; i++ )
    {
      (void) getTitle ( dbFp ) ;
      if ( listId < NO_OF_CAST_LISTS )
      {
        len = getByte ( dbFp ) ;
        if ( len > 0 )
          (void) fseek ( dbFp, len, SEEK_CUR ) ;
        (void) getPosition ( dbFp ) ;
      }
	else if ( listId == WRITER_LIST_ID )
      {
	(void) getPosition ( dbFp );
	(void) getPosition ( dbFp );
	(void) getPosition ( dbFp );
      }
    }
    if ( ! yr )
    {
      listData [ 0 ] . name = name ;
      displayFormatPlain ( listData, count ) ;
      freeFormatData ( listData, count ) ;
    }
  }
  if ( yr )
  {
    displayAlistSorted ( results, rcount ) ;
    for ( i = 0 ; i < rcount ; i++ )
    {
      free ( (void*) results [ i ] . name ) ;
      free ( (void*) results [ i ] . title ) ;
      free ( (void*) results [ i ] . attr ) ;
    }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( nameKeyFp ) ;
  (void) fclose ( nameIndexFp ) ;
  (void) fclose ( titleKeyFp ) ;
  (void) fclose ( titleIndexFp ) ;
  (void) fclose ( attrKeyFp ) ;
  (void) fclose ( attrIndexFp ) ;
}


int alistTitleYearSort ( struct alistTitleRes *r1, struct alistTitleRes *r2 )
{
  if ( r1 -> year != r2 -> year )
    return ( r1 -> year - r2 -> year ) ;
  else
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
}


void displayAlistTitleSorted ( struct alistTitleRes results [], int count ) 
{
  int i, currentYear = 0 ;

  qsort ( (void*) results, (size_t) count, sizeof ( struct alistTitleRes ), (int (*) (const void*, const void*)) alistTitleYearSort ) ;
  
  for ( i = 0 ; i < count ; i++ )
  {
    if ( currentYear != results [ i ] . year )
    {
      (void) printf ( "\n" ) ;
      currentYear = results [ i ] . year ;
    }
    displayTitleYear ( results [ i ] . title, results [ i ] . year ) ;
    if ( results [ i ] . attr != NULL )
      (void) printf ( " %s\n", results [ i ] . attr ) ;
    else
      (void) printf ( "\n" ) ;
  }
}


void alistTitles ( char *attr, int substring, int casesen, struct attrSearchOptRec attrFlags, int mvsonly, int yr, int yrf, int yrt )
{
  FILE *dbFp, *titleKeyFp, *titleIndexFp, *attrKeyFp, *attrIndexFp ;
  char *title, *dbAttr ;
  TitleID titleKey ;
  AttributeID attrKey ;
  int  year, keepFlag ;
  struct alistTitleRes results [ MAXALISTTITLES ] ;
  int count = 0, i;

  dbFp = openFile ( MOVIEDB ) ;
  titleKeyFp = openFile ( TITLEKEY ) ;
  titleIndexFp = openFile ( TITLEIDX ) ;
  attrKeyFp = openFile ( ATTRKEY ) ;
  attrIndexFp = openFile ( ATTRIDX ) ;

  while ( TRUE )
  {
    titleKey = getTitle ( dbFp ) ;
    if ( feof ( dbFp ) )
      break ;
    year = getInt ( dbFp ) ;
    (void) getInt ( dbFp ) ;
    attrKey = getAttr ( dbFp ) ;
    title = mapTitleKeyToText ( titleKey, titleKeyFp, titleIndexFp ) ;
    dbAttr = mapAttrKeyToText ( attrKey, attrKeyFp, attrIndexFp ) ;
    keepFlag = alistMoviesOnlyTest ( mvsonly, title ) ;
    keepFlag = keepFlag && alistAttrTest ( attrFlags, attr, substring, casesen, dbAttr ) ;
    if ( yr )
    {
      if ( year < yrf || year > yrt )
        keepFlag = FALSE ;
      if ( keepFlag )
      {
        results [ count ] . title = title ;
        results [ count ] . attr = dbAttr ;
        results [ count ] . year = year ;
        count++ ;
      }
    }
    else 
    {
      if ( keepFlag )
        printf ( "%s %s\n", title, dbAttr ) ;
      free ( (void*) title ) ;
      free ( (void*) dbAttr ) ;
    }
  }

  if ( yr && count > 0 )
  {
    displayAlistTitleSorted ( results, count ) ;
    for ( i = 0 ; i < count ; i++ )
    {
      free ( (void*) results [ i ] . title ) ;
      free ( (void*) results [ i ] . attr ) ;
    }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( titleKeyFp ) ;
  (void) fclose ( titleIndexFp ) ;
  (void) fclose ( attrKeyFp ) ;
  (void) fclose ( attrIndexFp ) ;
}


int main ( int argc, char **argv )
{
  int   err = FALSE ;
  int   i, j ;
  int   okopt ;
  int   casesen = FALSE ;
  int   substring = FALSE ;
  int   mvsonly = FALSE ;
  struct titleDbRec *years  ;
  char  attr [ MXLINELEN ] ;
  struct attrSearchOptRec  attrFlags = { 0, 0, 0, 0, 0 } ;
  int   yrf = 0 ;
  int   yrt = 9999 ;
  int   yropt = FALSE ;
  TitleID nyears = 0 ;
  int   listId = -1 ;
  int   titleFlag = FALSE ;

  years = (struct titleDbRec *) calloc ( MAXTITLES, sizeof ( struct titleDbRec ) ) ;
  if ( years == NULL )
    moviedbError ( "mkdb: not enough memory to generate titleDbRec" ) ;

  logProgram ( argv [ 0 ] ) ;

  if ( argc == 1 )
    err = TRUE ;

  attr [ 0 ] = '\0' ;

  for ( i = 1; i < argc; i++ )
    if ( strcmp ( "-i", argv[i] ) == 0 )
      casesen = TRUE ;
    else if ( strcmp ( "-m", argv[i] ) == 0 )
      mvsonly = TRUE ;
    else if ( strcmp ( "-s", argv[i] ) == 0 )
      substring = TRUE ;
    else if ( strcmp ( "-title", argv[i] ) == 0 )
      titleFlag = TRUE ;
    else if ( strcmp ( "-attr", argv[i] ) == 0 )
    {
      attrFlags . string = TRUE ;
      if ( ++i < argc )
	(void) strcpy ( attr, argv[i] ) ;
      else
	err = TRUE ;
    }
    else if ( strcmp ( "-yr", argv[i] ) == 0 )
      yropt = TRUE ;
    else if ( strcmp ( "-aa", argv[i] ) == 0 )
      attrFlags . aa = TRUE ;
    else if ( strcmp ( "-aan", argv[i] ) == 0 )
      attrFlags . aan = TRUE ;
    else if ( strcmp ( "-gg", argv[i] ) == 0 )
      attrFlags . gg = TRUE ;
    else if ( strcmp ( "-ggn", argv[i] ) == 0 )
      attrFlags . ggn = TRUE ;
    else if ( strcmp ( "-yrto", argv[i] ) == 0 )
    {
      yropt = TRUE ;
      if ( ++i < argc )
        yrt = atoi ( argv[i] ) ;
      else
        err = TRUE ;
    }
    else if ( strcmp ( "-yrfrom", argv[i] ) == 0 )
    {
      yropt = TRUE ;
      if ( ++i < argc )
        yrf = atoi ( argv[i] ) ;
      else
        err = TRUE ;
    }
    else if ( strcmp ( "-yreq", argv[i] ) == 0 )
    {
      yropt = TRUE ;
      if ( ++i < argc )
      {
        yrf = atoi ( argv[i] ) ;
        yrt = yrf ;
      }
      else
        err = TRUE ;
    }
    else
    {
      okopt = FALSE ;
      for ( j = 0 ; j < NO_OF_FILMOGRAPHY_LISTS ; j++ )
	if ( strcmp ( filmographyDefs [ j ] . option, argv [ i ] ) == 0 )
        {
          listId = j ;
	  okopt = TRUE ;
        }
      if ( ! okopt )
      {
         (void) fprintf ( stderr, "alist: unrecognised option %s\n", argv[i] ) ;
         err = TRUE ;
      }
    }

  if ( !attrFlags . string && !attrFlags . aa && !attrFlags . aan && !attrFlags . gg && !attrFlags . ggn )
  {
    (void) fprintf ( stderr, "alist: missing attribute to search for\n" ) ;
    err = TRUE ;
  }
  
  if ( !err && titleFlag == FALSE && listId < -1 )
  {
    (void) fprintf ( stderr, "alist: must pick -title or one database option\n" ) ;
    err = TRUE ;
  }

  if ( !err && titleFlag && listId >= 0 )
  {
    (void) fprintf ( stderr, "alist: can't pick -title and a database option\n" ) ;
    err = TRUE ;
  }

  if ( err )
    moviedbUsage ( ALIST_USAGE1, ALIST_USAGE2, ALIST_USAGE3, NULL, NULL, NULL ) ;

  if ( yropt )
    nyears = readTitleDb ( years ) ;

  if ( listId >= 0 )
    alistNames ( listId, attr, substring, casesen, attrFlags, mvsonly, yropt, yrf, yrt, nyears, years ) ;
  else
    alistTitles ( attr, substring, casesen, attrFlags, mvsonly, yropt, yrf, yrt ) ;

  return ( 0 ) ;
}

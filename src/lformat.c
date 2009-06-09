/*============================================================================
 *
 *  Program: lformat.c 
 *
 *  Version: 3.18
 *
 *  Purpose: format list databases
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996-2002 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 * 
 *  Options:
 *
 *        -acr  format actors database
 *        -acs  format actress database
 *        -dir  format directors database
 *      -write  format writers database
 *       -comp  format composers database
 *       -cine  format cinematographers database
 *       -edit  format editors database
 *     -prodes  format production designers database
 *    -costdes  format costume designers database
 *      -prdcr  format producers database
 *       -misc  format miscellaneous database
 *
 *    -mrr        add movie ratings report info to results
 *    -smrr       add movie ratings report info & sort by ratings
 *    -vmrr       add movie ratings report info & sort by votes
 *
 *    -yru        add year of release info
 *    -yr         add year of release info & sort chronologically
 *
 *    -m          movies only, ignore TV-series
 *    -r          raw, don't format results
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "moviedb.h"
#include "dbutils.h"
#include "years.h"
#include "ratings.h"

#define LFORMAT_USAGE1 "usage: lformat [-acr|-acs|-dir|-write|-comp|-cine|-edit|-prodes|-costdes|"
#define LFORMAT_USAGE2 "               -prdcr|-misc] [-mrr|-smrr|-vmrr] [-yr|-yru] [-m -r]"


void displayLformatRaw ( struct formatRec listData [ ], int count ) 
{
  int i ;

  for ( i = 0 ; i < count ; i++ )
  {
    (void) printf ( "%s|", listData [ i ] . name ) ;
    if ( listData [ i ] . year != 0 )
      displayTitleYear ( listData [ i ] . title, listData [ i ] . year ) ;
    else
      (void) printf ( "%s", listData [ i ] . title ) ;
    if ( listData [ i ] . attr != NULL )
      (void) printf ( "|%s\n", listData [ i ] . attr ) ;
    else
      (void) printf ( "|\n" ) ;
  }
}


void displayLformatYear ( struct formatRec listData [], int count )
{
  int i, len ;

  (void) printf ( "%s", listData [ 0 ] . name ) ;
  len = strlen ( listData [ 0 ] . name ) ;
  if ( len >= 16 )
    (void) printf ( "\t" ) ;
  else if ( len >= 8 )
    (void) printf ( "\t\t" ) ;
  else
    (void) printf ( "\t\t\t" ) ;
    
  displayTitleYear ( listData [ 0 ] . title, listData [ 0 ] . year ) ;
  if ( listData [ 0 ] . attr != NULL )
    (void) printf ( " %s\n", listData [ 0 ] . attr ) ;
  else
    (void) printf ( "\n" ) ;

  for ( i = 1 ; i < count ; i++ )
  {
    (void) printf ( "\t\t\t" ) ;
    displayTitleYear ( listData [ i ] . title, listData [ i ] . year ) ;
    if ( listData [ i ] . attr != NULL )
      (void) printf ( " %s\n", listData [ i ] . attr ) ;
    else
      (void) printf ( "\n" ) ;
  }

  (void) printf ( "\n" ) ;
}


void displayLformatRatings ( struct formatRec listData [], int count ) 
{
  int i ;

  (void) printf ( "%s\n\n", listData [ 0 ] . name ) ;

  for ( i = 0 ; i < count ; i++ )
  {
    if ( listData [ i ] . mrr . votes > 0 )
      (void) printf ( "      %s%8d   %2.1f  ", listData [ i ] . mrr . distribution, listData [ i ] . mrr . votes, listData [ i ] . mrr . rating ) ;
    else
      (void) printf ( "                                " ) ;

    if ( listData [ i ] . year != 0 )
      displayTitleYear ( listData [ i ] . title, listData [ i ] . year ) ;
    else
      (void) printf ( "%s", listData [ i ] . title ) ;
    if ( listData [ i ] . attr != NULL )
      (void) printf ( " %s\n", listData [ i ] . attr ) ;
    else
      (void) printf ( "\n" ) ;
  }
  (void) printf ( "\n" ) ;
}


void displayLformatData ( struct formatRec listData [], int count, int raw, int mrropt, int yropt ) 
{
  if ( raw )
    displayLformatRaw ( listData, count ) ;
  else if ( mrropt == NONE && yropt == NONE )
    displayFormatPlain ( listData, count ) ;
  else if ( mrropt == NONE && yropt != NONE )
    displayLformatYear ( listData, count ) ;
  else if ( mrropt != NONE )
    displayLformatRatings ( listData, count ) ;
}


int plainAlphaSort ( struct formatRec *r1, struct formatRec *r2 )
{
  if ( r1 -> title [ 0 ] == '"' )
    if ( r2 -> title [ 0 ] == '"' )
      return ( caseCompare ( r1 -> title, r2 -> title )  ) ;
    else
      return ( 1 ) ;
  else
    if ( r2 -> title [ 0 ] == '"' )
      return ( -1 ) ;
    else
      return ( caseCompare ( r1 -> title, r2 -> title )  ) ;
}


int titleKeyLformatCompare ( struct titleDbRec *search, struct titleDbRec *t1 )
{
  return ( search -> titleKey - t1 -> titleKey ) ;
}


int mrrKeyLformatCompare ( struct mrrDbRec *search, struct mrrDbRec *m1 )
{
  return ( search -> titleKey - m1 -> titleKey ) ;
}


void sortLformatData ( struct formatRec listData [ ], int count, int raw, int mrropt, int yropt, struct titleDbRec *years, TitleID nyears, struct mrrDbRec *ratings, TitleID nratings ) 
{
  struct titleDbRec *trec, tSearchRec ;
  struct mrrDbRec *mrec, mSearchRec ;
  int i ;

  if ( mrropt == NONE && yropt == NONE && !raw )
  {
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) plainAlphaSort ) ;
    return ;
  }

  if ( yropt != NONE )
  {
    for ( i = 0 ; i < count ; i++ )
    {
      tSearchRec . titleKey = listData [ i ] . titleKey ;
      if ( ( trec = bsearch ( (void*) &tSearchRec, (void*) years, (size_t) nyears, sizeof ( struct titleDbRec ), (int (*) (const void*, const void*)) titleKeyLformatCompare ) ) != NULL )
        listData [ i ] . year = trec -> year ;
      else
        listData [ i ] . year = 0 ;
    }
  }

  if ( mrropt != NONE )
  {
    for ( i = 0 ; i < count ; i++ )
    {
      mSearchRec . titleKey = listData [ i ] . titleKey ;
      if ( ( mrec = bsearch ( (void*) &mSearchRec, (void*) ratings, (size_t) nratings, sizeof ( struct mrrDbRec ), (int (*) (const void*, const void*)) mrrKeyLformatCompare ) ) != NULL )
        listData [ i ] . mrr = mrec -> mrr ;  
      else
      {
        listData [ i ] . mrr . votes = 0 ;  
        listData [ i ] . mrr . rating = 0.0 ;  
      }
    }
  }

  if ( mrropt == NONE ) 
    if ( yropt == YRU )
    {
      if ( !raw )
        qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) plainAlphaSort ) ;
    }
    else 
      qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) yearLformatSort ) ;

  if ( mrropt == SMRR ) 
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) smrrLformatSort ) ;
  else if ( mrropt == VMRR ) 
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) vmrrLformatSort ) ;
}


void lformatList ( int listId, int mvsonly, int raw, int mrropt, int yropt, struct titleDbRec *years, TitleID nyears, struct mrrDbRec *ratings, TitleID nratings )
{
  FILE *dbFp, *nameKeyFp, *nameIndexFp, *titleKeyFp, *titleIndexFp, *attrKeyFp, *attrIndexFp ;
  struct formatRec listData [ MAXFILMOGRAPHIES ] ;
  int i, count, len, noWithAttr, noWithoutAttr ;
  char fn [ MAXPATHLEN ] ;
  char *name ;
  NameID nameKey ;
  TitleID titleKey ;
  AttributeID attrKey ;

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
	(void) getPosition ( dbFp ) ;
	(void) getPosition ( dbFp ) ;
	(void) getPosition ( dbFp ) ;
      }
      listData [ count ] . titleKey = titleKey ;
      listData [ count ] . attrKey = attrKey ;
      listData [ count ] . title = mapTitleKeyToText ( titleKey, titleKeyFp, titleIndexFp ) ;
      if ( !mvsonly || listData [ count ] . title [ 0 ] != '"' )
      {
        listData [ count ] . name = duplicateString ( name ) ;
        listData [ count ] . attr = mapAttrKeyToText ( attrKey, attrKeyFp, attrIndexFp ) ;
        count++ ;
      }
      else
        free ( (void*) listData [ count ] . title ) ;
    }
    for ( i = 0 ; i < noWithoutAttr ; i++ )
    {
      titleKey = getTitle ( dbFp ) ;
      if ( listId < NO_OF_CAST_LISTS )
      {
        len = getByte ( dbFp ) ;
        if ( len > 0 )
          (void) fseek ( dbFp, len, SEEK_CUR ) ;
        (void) getPosition ( dbFp ) ;
      }
	else if ( listId == WRITER_LIST_ID )
      {
	(void) getPosition ( dbFp ) ;
	(void) getPosition ( dbFp ) ;
	(void) getPosition ( dbFp ) ;
      }
      listData [ count ] . attrKey = NOATTR ;
      listData [ count ] . titleKey = titleKey ;
      listData [ count ] . title = mapTitleKeyToText ( titleKey, titleKeyFp, titleIndexFp ) ;
      if ( !mvsonly || listData [ count ] . title [ 0 ] != '"' )
      {
        listData [ count ] . name = duplicateString ( name ) ;
        listData [ count ] . attr = NULL ;
        count++ ;
      }
      else
        free ( (void*) listData [ count ] . title ) ;
    }
    if ( count > 0 )
    {
      sortLformatData ( listData, count, raw, mrropt, yropt, years, nyears, ratings, nratings ) ;
      displayLformatData ( listData, count, raw, mrropt, yropt ) ;
      for ( i = 0 ; i < count ; i++ )
      {
        free ( (void*) listData [ i ] . name ) ;
        free ( (void*) listData [ i ] . title ) ;
        free ( (void*) listData [ i ] . attr ) ;
      }
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


int main ( int argc, char **argv )
{
  struct titleDbRec *years = NULL ;
  struct mrrDbRec   *ratings = NULL ;
  int    err = FALSE ;
  int    i, j, okopt ;
  int    mvsonly = FALSE ;
  int    raw = FALSE ;
  int    mrropt = NONE ;
  int    yropt = NONE ;
  int    listId = -1 ;
  TitleID  nyears = 0, nratings = 0 ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 1 ; i < argc ; i++ )
    if ( strcmp ( "-m", argv [ i ] ) == 0 )
      mvsonly = TRUE ;
    else if ( strcmp ( "-r", argv [ i ] ) == 0 )
      raw = TRUE ;
    else if ( strcmp ( "-mrr", argv [ i ] ) == 0 )
      mrropt = MRR ;
    else if ( strcmp ( "-smrr", argv [ i ] ) == 0 )
      mrropt = SMRR ;
    else if ( strcmp ( "-vmrr", argv [ i ] ) == 0 )
      mrropt = VMRR ;
    else if ( strcmp ( "-yr", argv [ i ] ) == 0 )
      yropt = YR ;
    else if ( strcmp ( "-yru", argv [ i ] ) == 0 )
      yropt = YRU ;
    else
    {
      okopt = FALSE ;
      for ( j = 0 ; j < NO_OF_FILMOGRAPHY_LISTS ; j++ )
	if ( strcmp ( filmographyDefs [ j ] . option, argv [ i ] ) == 0 )
        {
	  okopt = TRUE ;
          if ( listId != -1 )
            moviedbError ( "lformat: only one database option allowed" ) ;
	  listId = j ;
	}
      if ( ! okopt )
      {
         (void) fprintf ( stderr, "lformat: unrecognised option %s\n", argv[i] ) ;
         err = TRUE ;
      }
    }

  if ( !err && listId == -1 )
  {
    (void) fprintf ( stderr, "lformat: no database specified\n" ) ;
    err = TRUE ;
  }

  if ( err )
    moviedbUsage ( LFORMAT_USAGE1, LFORMAT_USAGE2, NULL, NULL, NULL, NULL ) ;

  if ( raw && mrropt != NONE )
    moviedbError ( "lformat: -r not available with ratings report options" ) ;

  if ( yropt != NONE )
  {
    if ( ( years = calloc ( MAXTITLES, sizeof ( struct titleDbRec ) ) ) == NULL )
      moviedbError ( "out of memory" ) ;
    nyears = readTitleDb ( years ) ;
  }

  if ( mrropt != NONE )
  {
    nratings = ratingsDbSize ( ) ;
    if ( ( ratings = calloc ( nratings, sizeof ( struct mrrDbRec ) ) ) == NULL )
      moviedbError ( "out of memory" ) ;
    nratings = readRatingsDb ( ratings ) ;
  }

  lformatList ( listId, mvsonly, raw, mrropt, yropt, years, nyears, ratings, nratings ) ;

  return ( 0 ) ;
}

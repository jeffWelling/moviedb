/*============================================================================
 *
 *  Program: lseen.c 
 *
 *  Version: 3.18
 *
 *  Purpose: track movies seen
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
 *        -acr    track actors database
 *        -acs    track actress database
 *        -dir    track directors database
 *      -write    track writers database
 *       -comp    track composers database
 *       -cine    track cinematographers database
 *       -edit    track editors database
 *     -prodes    track production designers database
 *    -costdes    track costume designers database
 *      -prdcr    track producers database
 *       -misc    track miscellaneous database
 *
 *    -mrr        add movie ratings report info to results
 *    -smrr       add movie ratings report info & sort by ratings
 *    -vmrr       add movie ratings report info & sort by votes
 *
 *    -yru        add year of release info
 *    -yr         add year of release info & sort chronologically
 *
 *    -min <val>  only report on people where you've seen <val> or more of
 *                 their movies
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

#define LSEEN_USAGE1 "usage: lseen [-acr|-acs|-dir|-write|-comp|-cine|-edit|-prodes|-costdes|-prdcr"
#define LSEEN_USAGE2 "              -misc] [-mrr|-smrr|-vmrr] [-yr|-yru] [-us] [-min <value>]"


void displayLseenYears ( struct formatRec listData [], int count ) 
{
  int i ;

  (void) printf ( "%s (%d)\n\n", listData [ 0 ] . name, count ) ;

  for ( i = 0 ; i < count ; i++ )
  {
    (void) printf ( "   " ) ;
    if ( listData [ i ] . year > 0 )
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


void displayLseenRatings ( struct formatRec listData [], int count ) 
{
  int i ;

  (void) printf ( "%s (%d)\n\n", listData [ 0 ] . name, count ) ;

  for ( i = 0 ; i < count ; i++ )
  {
    if ( listData [ i ] . mrr . votes > 0 )
      (void) printf ( "      %s%8d   %2.1f  ", listData [ i ] . mrr . distribution,
                 listData [ i ] . mrr . votes, listData [ i ] . mrr . rating ) ;
    else
      (void) printf ( "                                " ) ;

    if ( listData [ i ] . year > 0 )
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


void displayLseenData ( struct formatRec listData [], int count, int mrropt, int yropt ) 
{
  if ( mrropt != NONE )
    displayLseenRatings ( listData, count ) ;
  else if ( yropt != NONE )
    displayLseenYears ( listData, count ) ;
  else
    (void) printf ( "%s (%d)\n", listData [ 0 ] . name, count ) ;
}


int mrrTitleKeyLseenCompare ( struct mrrDbRec *search, struct mrrDbRec *m1 )
{
  return ( search -> titleKey - m1 -> titleKey ) ;
}


int yearTitleKeyLseenCompare ( struct titleDbRec *search, struct titleDbRec *t1 )
{
  return ( search -> titleKey - t1 -> titleKey ) ;
}


void mapLseenData ( struct formatRec listData [], int count, FILE *titleKeyFp, FILE *titleIndexFp, FILE *attrKeyFp, FILE *attrIndexFp )
{
  int i ;

  for ( i = 0 ; i < count ; i++ )
  {
    listData [ i ] . title = mapTitleKeyToText ( listData [ i ] . titleKey, titleKeyFp, titleIndexFp ) ;
    listData [ i ] . attr = mapAttrKeyToText ( listData [ i ] . attrKey, attrKeyFp, attrIndexFp ) ;
  }
}


void sortLseenData ( struct formatRec listData [], int count, int mrropt, int yropt, struct titleDbRec *years, TitleID nyears, struct mrrDbRec *ratings, TitleID nratings ) 
{
  struct mrrDbRec *mrec, mSearchRec ;
  struct titleDbRec *trec, tSearchRec ;
  int i ;

  if ( yropt != NONE )
  {
    for ( i = 0 ; i < count ; i++ )
    {
      tSearchRec . titleKey = listData [ i ] . titleKey ;
      if ( ( trec = bsearch ( (void*) &tSearchRec, (void*) years, (size_t) nyears, sizeof ( struct titleDbRec ), (int (*) (const void*, const void*)) yearTitleKeyLseenCompare ) ) != NULL )
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
      if ( ( mrec = bsearch ( (void*) &mSearchRec, (void*) ratings, (size_t) nratings, sizeof ( struct mrrDbRec ), (int (*) (const void*, const void*)) mrrTitleKeyLseenCompare ) ) != NULL )
        listData [ i ] . mrr = mrec -> mrr ;  
      else
      {
        listData [ i ] . mrr . votes = 0 ;  
        listData [ i ] . mrr . rating = 0.0 ;  
      }
    }
  }

  if ( mrropt == NONE && yropt == YR ) 
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) yearLformatSort ) ;
  else if ( mrropt == MRR && yropt == YR ) 
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) yearLformatSort ) ;
  else if ( mrropt == SMRR ) 
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) smrrLformatSort ) ;
  else if ( mrropt == VMRR ) 
    qsort ( (void*) listData, (size_t) count, sizeof ( struct formatRec ), (int (*) (const void*, const void*)) vmrrLformatSort ) ;
}


int voteTitleKeyLseenCompare ( struct voteDbRec *search, struct voteDbRec *v1 )
{
  return ( search -> titleKey - v1 -> titleKey ) ;
}


int readLseenData ( struct formatRec listData [], struct voteDbRec *votes, TitleID nvotes, FILE *dbFp, int listId ) 
{
  int count = 0, len, i ;
  int noWithAttr, noWithoutAttr ;
  struct voteDbRec searchRec ;
  NameID nameKey ;

  nameKey = getName ( dbFp ) ;
  getFilmographyCounts ( dbFp, &noWithAttr, &noWithoutAttr ) ;
  for ( i = 0 ; i < noWithAttr ; i++ )
  {
    listData [ count ] . nameKey = nameKey ;
    listData [ count ] . titleKey = getTitle ( dbFp ) ;
    listData [ count ] . attrKey = getAttr ( dbFp ) ;
    if ( listId < NO_OF_CAST_LISTS )
    {
      len = getByte ( dbFp ) ;
      if ( len > 0 )
      (void) fseek ( dbFp, len, SEEK_CUR ) ;
    }
    listData [ count ] . name = NULL ;
    listData [ count ] . title = NULL ;
    listData [ count ] . attr = NULL ;
    listData [ count ] . year = 0 ;
    searchRec . titleKey = listData [ count ] . titleKey ;
    if ( bsearch ( (void*) &searchRec, (void*) votes, (size_t) nvotes, sizeof ( struct voteDbRec ), (int (*) (const void*, const void*)) voteTitleKeyLseenCompare )  != NULL )
      count++ ;
  }
  for ( i = 0 ; i < noWithoutAttr ; i++ )
  {
    listData [ count ] . nameKey = nameKey ;
    listData [ count ] . titleKey = getTitle ( dbFp ) ;
    if ( listId < NO_OF_CAST_LISTS )
    {
      len = getByte ( dbFp ) ;
      if ( len > 0 )
      (void) fseek ( dbFp, len, SEEK_CUR ) ;
    }
    listData [ count ] . attrKey = NOATTR ;
    listData [ count ] . name = NULL ;
    listData [ count ] . title = NULL ;
    listData [ count ] . attr = NULL ;
    listData [ count ] . year = 0 ;
    searchRec . titleKey = listData [ count ] . titleKey ;
    if ( bsearch ( (void*) &searchRec, (void*) votes, (size_t) nvotes, sizeof ( struct voteDbRec ), (int (*) (const void*, const void*)) voteTitleKeyLseenCompare )  != NULL )
     count++ ;
  }
  return ( count ) ;
}


int lseenCountSort ( struct lseenCount *r1, struct lseenCount *r2 )
{
  return ( r2 -> count - r1 -> count ) ;
}


void displayLseenCounts ( struct lseenCount counts[], int ncount, int mrropt, int yropt, struct voteDbRec *votes, TitleID nvotes, struct titleDbRec *years, TitleID nyears, struct mrrDbRec *ratings, TitleID nratings, FILE *dbFp, int listId ) 
{
  FILE *titleKeyFp = NULL, *titleIndexFp = NULL, *attrKeyFp = NULL, *attrIndexFp = NULL ;
  int i, count ;
  struct formatRec listData [ MAXFILMOGRAPHIES ] ;

  if ( mrropt != NONE || yropt != NONE )
  {
    titleKeyFp = openFile ( TITLEKEY ) ;
    titleIndexFp = openFile ( TITLEIDX ) ;
    attrKeyFp = openFile ( ATTRKEY ) ;
    attrIndexFp = openFile ( ATTRIDX ) ;
  }
  qsort ( (void*) counts, (size_t) ncount, sizeof ( struct lseenCount ), (int (*) (const void*, const void*)) lseenCountSort ) ;
  for ( i = 0 ; i < ncount ; i++ )
  {
    if ( mrropt != NONE || yropt != NONE ) 
    {
      (void) fseek ( dbFp, counts [ i ] . offset, SEEK_SET ) ;
      count = readLseenData ( listData, votes, nvotes, dbFp, listId ) ;
      mapLseenData ( listData, count, titleKeyFp, titleIndexFp, attrKeyFp, attrIndexFp ) ;
      sortLseenData ( listData, count, mrropt, yropt, years, nyears, ratings, nratings ) ;
      listData [ 0 ] . name = counts [ i ] . name ;
      displayLseenData ( listData, count, mrropt, yropt ) ;
      freeFormatData ( listData, count ) ;
    }
    else
      (void) printf ( "%3d %s\n", counts [ i ] . count, counts [ i ] . name ) ;
  }
  if ( mrropt != NONE || yropt != NONE )
  {
    (void) fclose ( titleKeyFp ) ;
    (void) fclose ( titleIndexFp ) ;
    (void) fclose ( attrKeyFp ) ;
    (void) fclose ( attrIndexFp ) ;
  }
}


void lseenList ( int listId, int mrropt, int yropt, int usopt, int threshold, struct voteDbRec *votes, TitleID nvotes, struct titleDbRec *years, TitleID nyears, struct  mrrDbRec *ratings, TitleID nratings )
{
  FILE *dbFp, *nameKeyFp, *nameIndexFp, *titleKeyFp = NULL, *titleIndexFp = NULL, *attrKeyFp = NULL, *attrIndexFp = NULL ;
  struct formatRec listData [ MAXFILMOGRAPHIES ] ;
  struct lseenCount *counts = NULL ;
  struct voteDbRec searchRec ;
  int i, len, ncount = 0, count ;
  char fn [ MAXPATHLEN ] ;
  NameID nameKey ;
  long offset ;
  int noWithAttr, noWithoutAttr ;

  if ( usopt )
  {
    titleKeyFp = openFile ( TITLEKEY ) ;
    titleIndexFp = openFile ( TITLEIDX ) ;
    attrKeyFp = openFile ( ATTRKEY ) ;
    attrIndexFp = openFile ( ATTRIDX ) ;
  }
  else
    if ( ( counts = calloc ( MAXLSEEN, sizeof ( struct lseenCount ) ) ) == NULL )
      moviedbError ( "out of memory" ) ;

  (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, DBSEXT ) ;
  dbFp = openFile ( fn ) ;
  nameKeyFp = openFile ( NAMEKEY ) ;
  nameIndexFp = openFile ( NAMEIDX ) ;

  while ( TRUE )
  {
    count = 0 ;
    offset = ftell ( dbFp ) ;
    nameKey = getName ( dbFp ) ;
    if ( feof ( dbFp ) )
      break ;
    getFilmographyCounts ( dbFp, &noWithAttr, &noWithoutAttr ) ;
    for ( i = 0 ; i < noWithAttr ; i++ )
    {
      listData [ count ] . nameKey = nameKey ;
      listData [ count ] . titleKey = getTitle ( dbFp ) ;
      listData [ count ] . attrKey = getAttr ( dbFp ) ;
      if ( listId < NO_OF_CAST_LISTS )
      {
        len = getByte ( dbFp ) ;
        if ( len > 0 )
        (void) fseek ( dbFp, len, SEEK_CUR ) ;
      }
      listData [ count ] . name = NULL ;
      listData [ count ] . title = NULL ;
      listData [ count ] . attr = NULL ;
      listData [ count ] . year = 0 ;
      searchRec . titleKey = listData [ count ] . titleKey ;
      if ( bsearch ( (void*) &searchRec, (void*) votes, (size_t) nvotes, sizeof ( struct voteDbRec ), (int (*) (const void*, const void*)) voteTitleKeyLseenCompare )  != NULL )
        count++ ;
    }
    for ( i = 0 ; i < noWithoutAttr ; i++ )
    {
      listData [ count ] . nameKey = nameKey ;
      listData [ count ] . titleKey = getTitle ( dbFp ) ;
      if ( listId < NO_OF_CAST_LISTS )
      {
        len = getByte ( dbFp ) ;
        if ( len > 0 )
        (void) fseek ( dbFp, len, SEEK_CUR ) ;
      }
      listData [ count ] . attrKey = NOATTR ;
      listData [ count ] . name = NULL ;
      listData [ count ] . title = NULL ;
      listData [ count ] . attr = NULL ;
      listData [ count ] . year = 0 ;
      searchRec . titleKey = listData [ count ] . titleKey ;
      if ( bsearch ( (void*) &searchRec, (void*) votes, (size_t) nvotes, sizeof ( struct voteDbRec ), (int (*) (const void*, const void*)) voteTitleKeyLseenCompare )  != NULL )
      count++ ;
    }

    if ( count > 0 && count >= threshold )
      if ( !usopt ) 
      {
        counts [ ncount ] . count = count ;
        counts [ ncount ] . offset = offset ;
        counts [ ncount ] . nameKey = nameKey ;
        counts [ ncount ] . name = mapNameKeyToText ( nameKey, nameKeyFp, nameIndexFp ) ;
        if ( ++ncount > MAXLSEEN )
          moviedbError ( "out of space to store totals, use -min option" ) ;
      }
      else
      {
        mapLseenData ( listData, count, titleKeyFp, titleIndexFp, attrKeyFp, attrIndexFp ) ;
        sortLseenData ( listData, count, mrropt, yropt, years, nyears, ratings, nratings ) ;
        listData [ 0 ] . name = mapNameKeyToText ( nameKey, nameKeyFp, nameIndexFp ) ;
        displayLseenData ( listData, count, mrropt, yropt ) ;
        freeFormatData ( listData, count ) ;
      }
  }
  if ( ! usopt ) 
    displayLseenCounts ( counts, ncount, mrropt, yropt, votes, nvotes, years, nyears, ratings, nratings, dbFp, listId );

  (void) fclose ( dbFp ) ;
  (void) fclose ( nameKeyFp ) ;
  (void) fclose ( nameIndexFp ) ;
  if ( usopt )
  {
    (void) fclose ( titleKeyFp ) ;
    (void) fclose ( titleIndexFp ) ;
    (void) fclose ( attrKeyFp ) ;
    (void) fclose ( attrIndexFp ) ;
  }
}


int main ( int argc, char **argv )
{
  struct mrrDbRec   *ratings = NULL ;
  struct titleDbRec *years = NULL ;
  struct voteDbRec  *votes ;
  int    err = FALSE ;
  int    i, j, okopt ;
  int    mrropt = NONE ;
  int    yropt = NONE ;
  int    usopt = FALSE ;
  int    threshold = 0 ;
  int    listId = -1 ;
  TitleID    nyears = 0, nvotes = 0, nratings = 0 ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 1 ; i < argc ; i++ )
    if ( strcmp ( "-mrr", argv [ i ] ) == 0 )
      mrropt = MRR ;
    else if ( strcmp ( "-smrr", argv [ i ] ) == 0 )
      mrropt = SMRR ;
    else if ( strcmp ( "-vmrr", argv [ i ] ) == 0 )
      mrropt = VMRR ;
    else if ( strcmp ( "-yr", argv [ i ] ) == 0 )
      yropt = YR ;
    else if ( strcmp ( "-yru", argv [ i ] ) == 0 )
      yropt = YRU ;
    else if ( strcmp ( "-us", argv [ i ] ) == 0 )
      usopt = TRUE ;
    else if ( strcmp ( "-min", argv [ i ] ) == 0 )
    {
      if ( ++i < argc )
        threshold = atoi ( argv [ i ] ) ;
      else
        err = TRUE ;
    }
    else
    {
      okopt = FALSE ;
      for ( j = 0 ; j < NO_OF_FILMOGRAPHY_LISTS ; j++ )
	if ( strcmp ( filmographyDefs [ j ] . option, argv [ i ] ) == 0 )
        {
	  okopt = TRUE ;
	  if ( listId != -1 )
	    moviedbError ( "lseen: only one database option allowed" ) ;
	  listId = j ;
	}
      if ( ! okopt )
      {
         (void) fprintf ( stderr, "lseen: unrecognised option %s\n", argv[i] ) ;
         err = TRUE ;
      }
    }

  if ( !err && listId == -1 )
  {
    (void) fprintf ( stderr, "lseen: no database specified\n" ) ;
    err = TRUE ;
  }

  if ( err )
    moviedbUsage ( LSEEN_USAGE1, LSEEN_USAGE2, NULL, NULL, NULL, NULL ) ;

  if ( ( votes = calloc ( MAXTITLES, sizeof ( struct voteDbRec ) ) ) == NULL )
    moviedbError ( "out of memory" ) ;
  nvotes = readVotesDb ( votes ) ;

  if ( mrropt != NONE )
  {
    nratings = ratingsDbSize ( ) ;
    if ( ( ratings = calloc ( nratings, sizeof ( struct mrrDbRec ) ) ) == NULL )
      moviedbError ( "out of memory" ) ;
    nratings = readRatingsDb ( ratings ) ;
  }

  if ( yropt != NONE )
  {
    if ( ( years = calloc ( MAXTITLES, sizeof ( struct titleDbRec ) ) ) == NULL )
      moviedbError ( "out of memory" ) ;
    nyears = readTitleDb ( years ) ;
  }

  lseenList ( listId, mrropt, yropt, usopt, threshold, votes, nvotes, years, nyears, ratings, nratings ) ;

  return ( 0 ) ;
}

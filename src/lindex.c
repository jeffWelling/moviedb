/*============================================================================
 *
 *  Program: lindex.c 
 *
 *  Version: 3.22
 *
 *  Purpose: index list databases
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996-2003 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 * 
 *  Options:
 *           -acr  index actors database
 *           -acs  index actress database
 *           -dir  index directors database
 *         -write  index writers database
 *          -comp  index composers database
 *          -cine  index cinematographers database
 *          -edit  index editors database
 *        -prodes  index production designers database
 *       -costdes  index costume designers database
 *         -prdcr  index producers database
 *          -misc  index miscellaneous database
 *
 *         -title  index titles database
 *
 *           -mrr  add movie ratings report info to results
 *          -smrr  add movie ratings report info & sort by ratings
 *          -vmrr  add movie ratings report info & sort by votes
 *         -match  only print title if mrr data exists
 *
 *      -vmin <n>  limit title search to movies with n+ votes
 *      -vmax <n>  limit title search to movies with at most n votes
 *       -veq <n>  limit title search to movies with n votes
 *
 *           -yru  add year of release info
 *            -yr  add year of release info & sort chronologically
 *      -yreq <y>  limit title search to movies released in year <y>
 *    -yrfrom <y>  limit title search to movies released from <y> onwards
 *      -yrto <y>  limit title search to movies released up to <y>
 *
 *   -genre <str>  limit title search by genre
 *   -keyword <str> limit title search by keyword
 *    -cert <str>  limit title search by certificate
 *    -time <str>  limit title search by running time
 *  -prodco <str>  limit title search by production company
 *   -color <str>  limit title search by color information
 *     -mix <str>  limit title search by sound mix
 *   -cntry <str>  limit title search by country of production
 *    -lang <str>  limit title search by language
 *   -sfxco <str>  limit title search by special effects company
 *    -dist <str>  limit title search by distributor
 *
 *             -m  movies only, ignore TV-series
 *             -i  case sensistive search
 *       -s <str>  match substrings
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "moviedb.h"
#include "dbutils.h"
#include "ratings.h"
#include "titleinfo.h"

#define LINDEX_USAGE1 "usage: lindex [-acr -acs -dir -write -comp -cine -edit -prodes -costdes -prdcr"
#define LINDEX_USAGE2 "               -misc] [-title] [-mrr|-smrr|-vmrr] [-match] [-s <s>] [-m -i]" 
#define LINDEX_USAGE3 "              [-yr|-yru] [-yreq <year>|-yrfrom <year> -yrto <year>] [-mix <s>]"
#define LINDEX_USAGE4 "              [-genre <s>] [-keyword <s>] [-time <s>] [-prodco <s>] [-cert <s>]"
#define LINDEX_USAGE5 "              [-cntry <s>] [-color <s>] [-lang <s>]"
#define LINDEX_USAGE6 "              [-veq <votes>|-vmin <votes>|-vmax <votes>]"

int getNextLindexName ( FILE *indexFp, FILE *keyFp, struct lindexRec *rec, struct searchConstraints *constraints )
{
  char line [ MXLINELEN ] ;
  int stopFlag = FALSE ;
  NameID nameKey ;
  long offset ;

  while ( !stopFlag )
  {
    nameKey = getName ( rec -> fp ) ;
    if ( feof ( rec -> fp ) )
      break ;
    (void) getFullOffset ( rec -> fp ) ;
    (void) fseek ( indexFp, 4 * nameKey, SEEK_SET ) ;
    offset = getFullOffset ( indexFp ) ;
    (void) fseek ( keyFp, offset, SEEK_SET ) ;
    (void) fgets ( line, MXLINELEN, keyFp ) ;
    stripSep ( line ) ;
    stopFlag = TRUE ;
    if ( constraints -> subString [ 0 ] )
      if ( ! constraints -> caseSen )
      {
        if ( caseStrStr ( line, constraints -> subString ) == NULL )
  	  stopFlag = FALSE ;
      }
      else
        if ( strstr ( line, constraints -> subString ) == NULL )
  	  stopFlag = FALSE ;
  }

  if ( stopFlag )
  {
    (void) strcpy ( rec -> name, line ) ;
    return ( TRUE ) ;
  }
  else
  {
    (void) fclose ( rec -> fp ) ;
    rec -> fp = NULL ;
    return ( FALSE ) ;
  }
}


void lindexFilmographyDatabases ( int dbCount, struct lindexRec recs [ ], struct searchConstraints *constraints )
{
  FILE *indexFp, *keyFp ;
  int i, j, activeCount, eqFlag = FALSE ;
  int active [ NO_OF_FILMOGRAPHY_LISTS ] ;
  char fn [ MAXPATHLEN ] ;

  indexFp = openFile ( NAMEIDX ) ;
  keyFp = openFile ( NAMEKEY ) ;

  for ( i = 0 ; i < dbCount ; i++ )
  {
    (void) constructFilename ( fn, filmographyDefs [ recs [ i ] . listId ] . stem, NDXEXT ) ;
    recs [ i ] . fp = openFile ( fn ) ;
  }

  if ( dbCount == 1 )
    while ( getNextLindexName ( indexFp, keyFp, &recs [ 0 ], constraints ) )
      (void) printf ( "%s\n" , recs [ 0 ] . name ) ;
  else
  {
    for ( activeCount = 0 , i = 0 ; i < dbCount ; i++ )
      if ( ( active [ i ] = getNextLindexName ( indexFp, keyFp, &recs [ i ], constraints ) ) )
        activeCount++ ;

    while ( activeCount > 0 )
    {
      for ( i = 0 ; ! active [ i ] && i < dbCount ; i++ ) ;

      for ( j = 1 ; j < dbCount ; j++ )
        if ( ( active [ j ] ) && ( eqFlag = strcmp ( recs [ i ] . name, recs [ j ] . name ) ) > 0 )
	   i = j ;
	 else if ( active [ j ] && eqFlag == 0 && i !=j )
	 {
           if ( ( active [ j ] = getNextLindexName ( indexFp, keyFp, &recs [ j ], constraints ) ) == FALSE )
	     activeCount-- ;
	   else
	     j-- ;
	 }
       (void) printf ( "%s\n" , recs [ i ] . name ) ;
       if ( ( active [ i ] = getNextLindexName ( indexFp, keyFp, &recs [ i ], constraints ) ) == FALSE )
	 activeCount-- ;
     }
   }
}


int getNextLindexTitle ( FILE *dbFp, FILE *indexFp, FILE *keyFp, struct searchConstraints *constraints, char *title, TitleID *titleKey, int *yearVal )
{
  int stopFlag = FALSE ;
  long offset ;

  while ( !stopFlag )
  {
    *titleKey = getTitle ( dbFp ) ;
    if ( feof ( dbFp ) )
      break ;
    *yearVal = getInt ( dbFp ) ;
    (void) getInt ( dbFp ) ;
    (void) getAttr ( dbFp ) ;
    (void) fseek ( indexFp, *titleKey * 4, SEEK_SET ) ;
    offset = getFullOffset ( indexFp ) ;
    (void) fseek ( keyFp, offset, SEEK_SET ) ;
    (void) fgets ( title, MXLINELEN, keyFp ) ;
    stripSep ( title ) ;
    stopFlag = TRUE ;
    if ( *yearVal < constraints -> yearFrom || *yearVal > constraints -> yearTo )
      stopFlag = FALSE ;
    if ( constraints -> moviesOnly && *title == '"' )
      stopFlag = FALSE ;
    else if ( constraints -> subString [ 0 ] )
      if ( ! constraints -> caseSen )
      {
        if ( caseStrStr ( title, constraints -> subString ) == NULL )
  	  stopFlag = FALSE ;
      }
      else
        if ( strstr ( title, constraints -> subString ) == NULL )
  	  stopFlag = FALSE ;
  }
  return ( stopFlag ) ;
}


int yearLindexSort ( struct lindexTitleRec *r1, struct lindexTitleRec *r2 )
{
  if ( r1 -> year != r2 -> year )
    return ( r1 -> year - r2 -> year ) ;
  else
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
}


int smrrLindexSort ( struct lindexTitleRec *r1, struct lindexTitleRec *r2 )
{
  float result ;

  result =  r1 -> mrr . rating - r2 -> mrr . rating ;
  if ( result == 0 )
    return ( vmrrLindexSort ( r1, r2 ) ) ;
  else if ( result > 0 )
    return ( -1 ) ;
  else
    return ( 1 ) ;
}


int vmrrLindexSort ( struct lindexTitleRec *r1, struct lindexTitleRec *r2 )
{
  if ( r1 -> mrr . votes != r2 -> mrr . votes )
    return ( r2 -> mrr . votes - r1 -> mrr . votes ) ;
  else
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
}



void lindexTitles ( int yropt, int mrropt, struct searchConstraints *constraints ) 
{
  FILE  *dbFp, *indexFp, *keyFp, *mrrFp = NULL ;
  FILE  *titleInfoDb [ NO_OF_TITLE_INFO_LISTS ] ;
  FILE  *titleInfoIdx [ NO_OF_TITLE_INFO_LISTS ] ;
  char  title [ MXLINELEN ] ;
  int   i, count = 0, quitLoop ;
  struct mrrRec mrr ;
  struct lindexTitleRec *array ;
  int year ;
  TitleID titleKey, mrrKey, titleInfoKey [ NO_OF_TITLE_INFO_LISTS ] ;
  long titleInfoOffset [ NO_OF_TITLE_INFO_LISTS ] ;

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
  {
    if ( constraints -> titleInfoString [ i ] [ 0 ] )
    {
      titleInfoDb [ i ] = openFile ( titleInfoDefs [ i ] . db ) ;
      titleInfoIdx [ i ] = openFile ( titleInfoDefs [ i ] . index ) ;
      readTitleInfoIndex ( titleInfoIdx [ i ], &titleInfoKey [ i ], &titleInfoOffset [ i ] ) ;
    }
    else
    {
      titleInfoDb [ i ] = NULL ;
      titleInfoIdx [ i ] = NULL ;
    }
  }

  dbFp = openFile ( MOVIEDB ) ;
  indexFp = openFile ( TITLEIDX ) ;
  keyFp = openFile ( TITLEKEY ) ;

  if ( mrropt != NONE )
  {
    mrrFp = openFile ( MRRDB ) ;
    (void) readMrrRec ( mrrFp, &mrrKey, &mrr ) ;
  }

  array = malloc ( sizeof (struct lindexTitleRec) * MAXTITLES ) ;

  while ( getNextLindexTitle ( dbFp, indexFp, keyFp, constraints, title, &titleKey, &year ) )
  {
    for ( quitLoop = FALSE, i = 0 ; !quitLoop && i < NO_OF_TITLE_INFO_LISTS ; i++ )
    {
      if ( constraints -> titleInfoString [ i ] [ 0 ] )
      {
        while ( titleInfoKey [ i ] < titleKey && readTitleInfoIndex ( titleInfoIdx [ i ], &titleInfoKey [ i ], &titleInfoOffset [ i ] ) ) ;
        if ( titleInfoKey [ i ] != titleKey )
          quitLoop = TRUE ;
        else
        {
          if ( checkConstraints ( titleInfoDb [ i ], titleInfoOffset [ i ], titleKey , constraints -> titleInfoString [ i ] ) == FALSE )
            quitLoop = TRUE ;
        }
      }
    }
    if ( quitLoop )
      continue ;
    if ( mrropt != NONE )
    {
      while ( mrrKey < titleKey && readMrrRec ( mrrFp, &mrrKey, &mrr ) ) ;
      if ( mrr . votes < constraints -> voteMin ) 
        continue ;
      if ( mrr . votes > constraints -> voteMax ) 
        continue ;
    }
    if ( yropt == NONE && mrropt == NONE )
      (void) printf ( "%s\n", title ) ;
    else if ( yropt == YRU && mrropt == NONE )
    {
      displayTitleYear ( title, year ) ;
      (void) printf ( "\n" ) ;
    }
    else if ( yropt == NONE && mrropt == MRR )
    {
      if ( mrrKey != titleKey )
      {
        if ( !constraints -> mrrMatch )
	  (void) printf ( "                                %s\n", title ) ;
      }
      else
        (void) printf ( "      %s%8d   %2.1f  %s\n", mrr . distribution, mrr . votes, mrr . rating, title ) ;
    }
    else if ( yropt == YRU && mrropt == MRR )
    {
      if ( mrrKey != titleKey )
      {
        if ( !constraints -> mrrMatch )
        {
	  (void) printf ( "                                " ) ;
          displayTitleYear ( title, year ) ;
          (void) printf ( "\n" ) ;
        }
      }
      else
      {
        (void) printf ( "      %s%8d   %2.1f  ", mrr . distribution, mrr . votes, mrr . rating ) ;
        displayTitleYear ( title, year ) ;
        (void) printf ( "\n" ) ;
      }
    }
    else 
    {
      array [ count ] . title = duplicateString ( title ) ;
      if ( mrrKey == titleKey )
        array [ count ] . mrr = mrr ;
      else
      {
        array [ count ] . mrr . votes = 0 ;
        array [ count ] . mrr . rating = 0 ;
      }
      array [ count ] . year = year ;
      count++ ;
    }
  }

  if ( yropt == YR || mrropt == SMRR || mrropt == VMRR )
  {
    if ( yropt == YR )
      qsort ( (void*) array, (size_t) count, sizeof ( struct lindexTitleRec ), (int (*) (const void*, const void*)) yearLindexSort ) ;
    else if ( mrropt == SMRR )
      qsort ( (void*) array, (size_t) count, sizeof ( struct lindexTitleRec ), (int (*) (const void*, const void*)) smrrLindexSort ) ;
    else
      qsort ( (void*) array, (size_t) count, sizeof ( struct lindexTitleRec ), (int (*) (const void*, const void*)) vmrrLindexSort ) ;

    for ( i = 0 ; i < count ; i++ )
    {
      if ( yropt == YR && mrropt == NONE )
      {
        displayTitleYear ( array [ i ] . title, array [ i ] . year ) ;
        (void) printf ( "\n" ) ;
      }
      else if ( yropt == YR )
      {
        if ( array [ i ] . mrr . votes == 0 )
        {
          if ( !constraints -> mrrMatch )
          {
	    (void) printf ( "                                " ) ;
            displayTitleYear ( array [ i ] . title, array [ i ] . year ) ;
            (void) printf ( "\n" ) ;
          }
        }
        else
        {
          (void) printf ( "      %s%8d   %2.1f  ", array [ i ] . mrr . distribution, array [ i ] . mrr . votes, array [ i ] . mrr . rating ) ;
          displayTitleYear ( array [ i ] . title, array [ i ] . year ) ;
          (void) printf ( "\n" ) ;
	}
      }
      else
      {
        if ( array [ i ] . mrr . votes == 0 )
        {
          if ( !constraints -> mrrMatch )
	    (void) printf ( "                                %s\n", array [ i ] . title ) ;
        }
        else
          (void) printf ( "      %s%8d   %2.1f  %s\n", array [ i ] . mrr . distribution, array [ i ] . mrr . votes, array [ i ] . mrr . rating, array [ i ] . title ) ;
      }
      free ( (void*) array [ i ] . title ) ;
    }
  }
       
  (void) fclose ( dbFp ) ;
  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
  if ( mrropt != NONE )
    (void) fclose ( mrrFp ) ;
  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
  {
    if ( constraints -> titleInfoString [ i ] [ 0 ] )
    {
      (void) fclose ( titleInfoDb [ i ] ) ;
      (void) fclose ( titleInfoIdx [ i ] ) ;
    }
  }
}


int main ( int argc, char **argv )
{
  int err = FALSE ;
  int i, j ;
  int okopt ;
  int dbcount = 0 ;
  int yearOption = NONE ;
  int mrrOpt = NONE ;
  int nmopt = FALSE ;
  int ttlopt = FALSE ;
  struct lindexRec recs [ NO_OF_FILMOGRAPHY_LISTS ] ;
  struct searchConstraints constraints ;

  logProgram ( argv [ 0 ] ) ;
  initialiseConstraints ( &constraints ) ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 1; i < argc; i++ )
    if ( strcmp ( "-i", argv[i] ) == 0 )
      constraints . caseSen = TRUE ;
    else if ( strcmp ( "-m", argv[i] ) == 0 )
      constraints . moviesOnly = TRUE ;
    else if ( strcmp ( "-s", argv[i] ) == 0 )
    {
      if ( ++i < argc )
	(void) strcpy ( constraints . subString, argv[i] ) ;
      else
	err = TRUE ;
    }
    else if ( strcmp ( "-mrr", argv[i] ) == 0 )
    {
      mrrOpt = MRR ;
      constraints . moviesOnly = TRUE ;
    }
    else if ( strcmp ( "-smrr", argv[i] ) == 0 )
    {
      mrrOpt = SMRR ;
      constraints . moviesOnly = TRUE ;
    }
    else if ( strcmp ( "-vmrr", argv[i] ) == 0 )
    {
      mrrOpt = VMRR ;
      constraints . moviesOnly = TRUE ;
    }
    else if ( strcmp ( "-match", argv[i] ) == 0 )
      constraints . mrrMatch = TRUE ;
    else if ( strcmp ( "-yr", argv[i] ) == 0 )
      yearOption = YR ;
    else if ( strcmp ( "-yru", argv[i] ) == 0 )
      yearOption = YRU ;
    else if ( strcmp ( "-title", argv[i] ) == 0 )
      ttlopt = TRUE ;
    else if ( strcmp ( "-yrto", argv[i] ) == 0 )
    {
      if ( ++i < argc )
        constraints . yearTo = atoi ( argv[i] ) ;
      else
        err = TRUE ;
    }
    else if ( strcmp ( "-yrfrom", argv[i] ) == 0 )
    {
      if ( ++i < argc )
        constraints . yearFrom = atoi ( argv[i] ) ;
      else
        err = TRUE ;
    }
    else if ( strcmp ( "-yreq", argv[i] ) == 0 )
    {
      if ( ++i < argc )
      {
        constraints . yearFrom = atoi ( argv[i] ) ;
        constraints . yearTo = constraints . yearFrom ;
      }
      else
        err = TRUE ;
    }
    else if ( strcmp ( "-vmax", argv[i] ) == 0 )
    {
      if ( mrrOpt == NONE )
        mrrOpt = MRR ;
      if ( ++i < argc )
        constraints . voteMax = atoi ( argv[i] ) ;
      else
        err = TRUE ;
    }
    else if ( strcmp ( "-vmin", argv[i] ) == 0 )
    {
      if ( mrrOpt == NONE )
        mrrOpt = MRR ;
      if ( ++i < argc )
        constraints . voteMin = atoi ( argv[i] ) ;
      else
        err = TRUE ;
      constraints . mrrMatch = TRUE ;
    }
    else if ( strcmp ( "-veq", argv[i] ) == 0 )
    {
      if ( mrrOpt == NONE )
        mrrOpt = MRR ;
      if ( ++i < argc )
      {
        constraints . voteMin = atoi ( argv[i] ) ;
        constraints . voteMax = constraints . voteMin ;
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
	  okopt = TRUE ;
	  recs [ dbcount ] . fp = NULL ;
	  recs [ dbcount ] . listId = filmographyDefs [ j ] . listid ;
	  recs [ dbcount ] . name [ 0 ] = '\0' ;
	  dbcount++ ;
	  nmopt = TRUE ;
	}
      if ( ! okopt )
      {
        okopt = FALSE ;
        for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
          if ( strcmp ( titleInfoDefs [ j ] . option, argv [ i ] ) == 0 )
          {
            okopt = TRUE ;
            if ( ++i < argc )
	      (void) strcpy ( constraints . titleInfoString [ j ], argv[i] ) ;
            else
	      err = TRUE ;
          }
      }
      if ( ! okopt )
      {
         (void) fprintf ( stderr, "lindex: unrecognised option %s\n", argv[i] ) ;
         err = TRUE ;
      }
    }

  if ( !err && !nmopt && !ttlopt )
    for ( j = 0 ; j < NO_OF_FILMOGRAPHY_LISTS ; j++ )
    {
       recs [ dbcount ] . fp = NULL ;
       recs [ dbcount ] . listId = filmographyDefs [ j ] . listid ;
       recs [ dbcount ] . name [ 0 ] = '\0' ;
       dbcount++ ;
       nmopt = TRUE ;
    }

  if ( nmopt && ttlopt )
  {
    (void) fprintf ( stderr, "lindex: can't mix title and name indexing in same command\n" ) ;
    err = TRUE ;
  }

  if ( !err && !nmopt && !ttlopt )
  {
    (void) fprintf ( stderr, "lindex: must specify at least one title or name indexing option\n" ) ;
    err = TRUE ;
  }


  if ( err )
   moviedbUsage ( LINDEX_USAGE1, LINDEX_USAGE2, LINDEX_USAGE3, LINDEX_USAGE4, LINDEX_USAGE5, LINDEX_USAGE6 ) ;

  if ( nmopt )
    lindexFilmographyDatabases ( dbcount, recs, &constraints ) ;
  else
    lindexTitles ( yearOption, mrrOpt, &constraints ) ; 
 
  return ( 0 ) ;
}

/*============================================================================
 *
 *  Program: filmography.c
 *
 *  Version: 3.3
 *
 *  Purpose: Filmography searching procedures
 *
 *  Authors:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "moviedb.h"
#include "dbutils.h"
#include "filmography.h"
#include "years.h"
#include "biographies.h"
#include "aka.h"
#include "ratings.h"
#include "lcs.h"
#include "titleinfo.h"

struct listRec *lookupFilmography ( FILE *dbFp, FILE *indexFp, struct nameSearchRec *nrec, int listId ) ;

void freeNameAkas ( struct akaNameRec *naka )
{
  struct akaNameRec *arec, *aprev ;

  aprev = NULL ;
  for ( arec = naka ; arec != NULL ; arec = arec -> next )
  {
    free ( (void*) aprev ) ;
    free ( (void*) arec -> akaName ) ;
    aprev = arec ;
  }
  free ( (void*) aprev ) ;
}


void freeNameSearchRec ( struct nameSearchRec *rec )
{
  struct akaRec *arec, *aprev ;
  int i, j ;

  free ( (void*) rec -> name ) ;
  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    if ( rec -> lists [ i ] != NULL )
    {
      for ( j = 0 ; j < rec -> lists [ i ] -> count ; j++ )
      {
        free ( (void*) rec -> lists [ i ] -> entries [ j ] . title ) ;
        free ( (void*) rec -> lists [ i ] -> entries [ j ] . attr ) ;
        free ( (void*) rec -> lists [ i ] -> entries [ j ] . cname ) ;
        free ( (void*) rec -> lists [ i ] -> entries [ j ] . mrr ) ;
        aprev = NULL ;
        for ( arec = rec -> lists [ i ] -> entries [ j ] . aka ; arec != NULL ; arec = arec -> next )
        {
 	  free ( (void*) aprev ) ;
	  free ( (void*) arec -> akaTitle ) ;
	  free ( (void*) arec -> akaAttr ) ;
          aprev = arec ;
        }
        free ( (void*) aprev ) ;
      }
      free ( (void*)  rec -> lists [ i ] ) ;
    }
  }
  freePersonRec ( rec -> biography ) ;
  freeNameAkas ( rec -> aka ) ;
}


void freeNameSearchChain ( struct nameSearchRec *chain )
{
  struct nameSearchRec *nrec, *prev = NULL ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    free ( (void*) prev ) ;
    freeNameSearchRec ( nrec ) ;
    prev = nrec ;
  }
  free ( (void*) prev ) ;
  chain = NULL ;
}


void addNameChainOpts (struct nameSearchRec *nchain, struct nameSearchOptRec options)
{
  struct nameSearchRec *nrec ;

  for ( nrec = nchain ; nrec != NULL ; nrec = nrec -> next )
  {
    nrec -> searchparams . approx = options . approx ;
    nrec -> searchparams . mrropt = options . mrropt ;
    nrec -> searchparams . yropt = options . yropt ;
    nrec -> searchparams . casesen = options . casesen ;
    nrec -> searchparams . substring = options . substring ;
    nrec -> searchparams . mvsonly = options . mvsonly ;
    nrec -> searchparams . chopt = options . chopt ;
    nrec -> searchparams . biopt = options . biopt ;
    nrec -> searchparams . akaopt = options . akaopt ;
  }
}

 
struct nameSearchRec *addNameSearchRec (struct nameSearchRec *chain, char *name, int listopt)
{
  struct nameSearchRec *head, *tail ;
  int i ;

  if ( chain == NULL )
  {
    tail = newNameSearchRec ( ) ;
    head = tail ;
  }
  else
  {
    head = chain ;
    for ( tail = chain ; tail -> next != NULL ; tail = tail -> next ) ;
    tail -> next = newNameSearchRec ( ) ;
    tail = tail -> next ;
  }

  tail -> name = duplicateString ( name ) ;
  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
    if ( filmographyOptions [ listopt ] . listflags [ i ] )
      tail -> searchFlags [ i ] = TRUE ;

  if ( listopt == GLOBAL_NAME_SEARCH )
    tail -> searchparams . searchall = TRUE ;
  else
    tail -> searchparams . searchall = FALSE ;

  return ( head ) ;
}


void swapAkaNames (struct nameSearchRec *nchain)
{
  FILE  *dbFp, *indexFp = NULL, *keyFp = NULL ;
  struct nameSearchRec *nrec ;
  char line [ MXLINELEN ] ;
  long mid, lower, upper, saveUpper, offset ;
  int found = FALSE ;
  NameID nameKey, dbKey ;

  dbFp = openFile ( NAKAIDX ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ;
  saveUpper = ftell ( dbFp ) / NAKABYTES ;

  for ( nrec = nchain ; nrec != NULL ; nrec = nrec -> next )
  {
    if ( nrec -> firstMatch >= 0 ) continue ;
    nameKey = nrec -> nameKey ;
    lower = 0 ;
    upper = saveUpper ;
    found = FALSE ;
    while ( !found && upper >= lower )
    {
      mid = ( upper + lower ) / 2 ;
      (void) fseek ( dbFp, mid * NAKABYTES, SEEK_SET ) ;
      dbKey = getName ( dbFp ) ;
      if ( nameKey == dbKey )
        found = TRUE ;
      else if ( dbKey < nameKey )
        lower = mid + 1 ;
      else
        upper = mid - 1 ;
    }
    if ( found )
    {
      if (  keyFp == NULL )
      {
        keyFp = openFile ( NAMEKEY ) ;
        indexFp = openFile ( NAMEIDX ) ;
      }
      nrec -> nameKey = getName ( dbFp ) ;
      (void) fseek ( indexFp, 4 * nrec -> nameKey, SEEK_SET ) ;
      offset = getFullOffset ( indexFp ) ;
      (void) fseek ( keyFp, offset, SEEK_SET ) ;
      (void) fgets ( line, MXLINELEN, keyFp ) ;
      (void) free ( (void*) nrec -> name ) ;
      nrec -> name = duplicateField ( line ) ;
    }
  }
  if ( keyFp != NULL )
  {
    (void) fclose ( keyFp ) ;
    (void) fclose ( indexFp ) ;
  }
  (void) fclose ( dbFp ) ;
}


void approxFilmographySearchKeyLookup (struct nameSearchRec *chain)
{
  FILE  *indexFp, *nameIndexFp, *nameKeyFp ;
  struct nameSearchRec *nrec, *tail = chain ;
  char  line [ MXLINELEN ] ;
  char  *substring ;
  char fn [ MAXPATHLEN ] ;
  NameID nameKey ;
  long offset ;
  int  listId = -1, i, casesen, matched, approx ;
  int  count = 0 ;

  casesen = chain -> searchparams . casesen ;
  approx = chain -> searchparams . approx ;
  substring = chain -> name ;
  nameIndexFp = openFile ( NAMEIDX ) ;
  nameKeyFp = openFile ( NAMEKEY ) ;

  for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ ) 
    if ( chain -> searchFlags [ listId ] )
    {
      (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, NDXEXT ) ;
      indexFp = openFile ( fn ) ;
      while ( !feof ( indexFp ) )
      {
        nameKey = getName ( indexFp ) ;
        matched = FALSE ;
        (void) getFullOffset ( indexFp ) ;
        (void) fseek ( nameIndexFp, 4 * nameKey, SEEK_SET ) ;
        offset = getFullOffset ( nameIndexFp ) ;
        (void) fseek ( nameKeyFp, offset, SEEK_SET ) ;
        (void) fgets ( line, MXLINELEN, nameKeyFp ) ;
        stripSep ( line ) ;
        if ( minimal_distance ( line, substring, casesen ) <= approx )
          matched = TRUE ;
        for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
           if ( nrec -> nameKey == nameKey )
             matched = FALSE ;

        if ( matched )
        {
          if ( count == 0 )
          {
            chain -> name = duplicateString ( line ) ;
            chain -> nameKey = nameKey ;
            count++ ;
          }
          else
          {
            tail -> next = newNameSearchRec ( ) ;
            tail = tail -> next ;
            tail -> name = duplicateString ( line ) ;
            tail -> nameKey = nameKey ;
            tail -> searchparams = chain -> searchparams ;
            tail -> next = NULL ;
            for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
              tail -> searchFlags [ i ] = chain -> searchFlags [ i ] ;
            count++ ;
          }
        }
      }
      (void) fclose ( indexFp ) ;
    }
}


void substringFilmographySearchKeyLookup (struct nameSearchRec *chain)
{
  FILE  *indexFp, *nameIndexFp, *nameKeyFp ;
  struct nameSearchRec *nrec, *tail = chain ;
  char  line [ MXLINELEN ] ;
  char  *substring ;
  char fn [ MAXPATHLEN ] ;
  NameID nameKey ;
  long offset ;
  int  listId = -1, i, casesen, matched ;
  int  count = 0 ;

  casesen = chain -> searchparams . casesen ;
  substring = chain -> name ;
  nameIndexFp = openFile ( NAMEIDX ) ;
  nameKeyFp = openFile ( NAMEKEY ) ;

  for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ ) 
    if ( chain -> searchFlags [ listId ] )
    {
      (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, NDXEXT ) ;
      indexFp = openFile ( fn ) ;
      while ( !feof ( indexFp ) )
      {
        nameKey = getName ( indexFp ) ;
        matched = FALSE ;
        (void) getFullOffset ( indexFp ) ;
        (void) fseek ( nameIndexFp, 4 * nameKey, SEEK_SET ) ;
        offset = getFullOffset ( nameIndexFp ) ;
        (void) fseek ( nameKeyFp, offset, SEEK_SET ) ;
        (void) fgets ( line, MXLINELEN, nameKeyFp ) ;
        if ( casesen ) 
        {
          if ( strstr ( line, substring ) != NULL )
            matched = TRUE ;
        }
        else if ( caseStrStr ( line, substring ) != NULL )
          matched = TRUE ;
        for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
           if ( nrec -> nameKey == nameKey )
             matched = FALSE ;

        if ( matched )
        {
          if ( count == 0 )
          {
            chain -> name = duplicateField ( line ) ;
            chain -> nameKey = nameKey ;
            count++ ;
          }
          else
          {
            tail -> next = newNameSearchRec ( ) ;
            tail = tail -> next ;
            tail -> name = duplicateField ( line ) ;
            tail -> nameKey = nameKey ;
            tail -> searchparams = chain -> searchparams ;
            for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
              tail -> searchFlags [ i ] = chain -> searchFlags [ i ] ;
            count++ ;
          }
        }
      }
      (void) fclose ( indexFp ) ;
    }
}


void straightFilmographySearchKeyLookup (struct nameSearchRec *chain)
{
  FILE  *indexFp ;
  struct nameSearchRec *nrec ;
  char line [ MXLINELEN ] ;
  long mid = -1, lower = 0, upper ;
  int compare ;
  int found = FALSE ;
  char *keyptr ;
  int skipped = 0 ;

  indexFp = openFile ( NAMEKEY ) ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    (void) fseek ( indexFp, 0, SEEK_END ) ; 
    upper = ftell ( indexFp ) ;
    found = FALSE ;
    mid = -1 ;
    lower = 0 ;
    while ( ! found && upper >= lower )
    {
       mid = ( upper + lower ) / 2 ;
       (void) fseek ( indexFp, mid, SEEK_SET ) ;
       if ( mid > 0 )
         if ( fgets ( line, MXLINELEN, indexFp ) == NULL )
           moviedbError ( "names index file corrupt" ) ;
         else
           skipped = strlen ( line ) ;
       else
         skipped = 0 ;
       if ( fgets ( line, MXLINELEN, indexFp ) == NULL )
       {
         (void) findSOL ( indexFp, mid ) ;
         (void) fgets ( line, MXLINELEN, indexFp ) ;
       }
       compare = fieldCaseCompare ( line, nrec -> name );
       if ( compare == 0 )
          found = TRUE;
       else if ( compare < 0 )
          lower = mid + skipped + strlen ( line ) - 1 ;
       else
          upper = mid - 1 ;
    }

   if ( found )
     if ( ( keyptr = strchr ( line, FSEP ) ) != NULL ) 
     {
       *keyptr++ = '\0' ;
       nrec -> nameKey = strtol ( keyptr, (char **) NULL, 16) ;
       (void) strcpy ( nrec -> name, line ) ;
     }
   else
       nrec -> nameKey = NONAME ;
  }

  (void) fclose ( indexFp ) ;
}


void filmographySearchKeyLookup (struct nameSearchRec *chain)
{
  if ( chain -> searchparams . substring )
    substringFilmographySearchKeyLookup ( chain ) ;
  else if ( chain -> searchparams . approx )
    approxFilmographySearchKeyLookup ( chain ) ;
  else
    straightFilmographySearchKeyLookup ( chain ) ;

  if ( chain -> searchparams . akaopt ) {
	/* Do preliminary lookup to determine which names are "real" */
    struct listRec *tempListRec ;
    FILE  *dbFp = NULL, *indexFp = NULL ;
    int   listId ;
    struct nameSearchRec *nrec ;
    char fn [ MAXPATHLEN ] ;

    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
    {
      for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
      {
	if ( nrec -> firstMatch >= 0 ) continue ;
	if ( nrec -> searchFlags [ listId ] )
	{
	  if ( dbFp == NULL )
	  {
	    (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, DBSEXT ) ;
	    dbFp = openFile ( fn ) ;
	    (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, NDXEXT ) ;
	    indexFp = openFile ( fn ) ;
	  }
	  tempListRec = lookupFilmography ( dbFp, indexFp, nrec, listId ) ; 
	  if  ( tempListRec != NULL ) {
	    nrec -> firstMatch = listId ;
	    free ( tempListRec ) ;
	  }
	}
      }
      if ( dbFp != NULL )
      {
	(void) fclose ( dbFp ) ;
	(void) fclose ( indexFp ) ;
	dbFp = NULL ;
      }
    }
    swapAkaNames ( chain ) ;
    for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
      nrec -> firstMatch = -1 ;
  }
}


long lookupNamesIndex ( FILE *indexFp, NameID nameKey ) 
{
  long mid = -1, lower = 0, upper ;
  NameID  indexName ;
  int found = FALSE ;

  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  upper = ftell ( indexFp ) ;
  upper = upper / 7 ;

  while ( !found && upper >= lower )
  {
     mid = ( upper + lower ) / 2 ;
     (void) fseek ( indexFp, mid * 7, SEEK_SET ) ;
     indexName = getName ( indexFp ) ;
     if ( nameKey == indexName )
       found = TRUE ;
    else if ( indexName < nameKey )
      lower = mid + 1 ;
    else
      upper = mid - 1 ;
  }
  if ( found ) 
    return ( getFullOffset ( indexFp ) ) ;
  else
    return ( -1 ) ;
}


int listRecTitleKeySort ( struct listEntry *e1, struct listEntry *e2 )
{
  if ( e1 -> titleKey == e2 -> titleKey )
    return ( 0 ) ;
  else if ( e1 -> titleKey > e2 -> titleKey ) 
    return ( 1 ) ;
  else
    return ( -1 ) ;
}


int readFilmographyToListRec ( FILE *dbFp, long offset, long nameKey, struct listRec *lrec, int listId ) 
{
  int i, len, noWithAttr, noWithoutAttr, count = 0 ;

  (void) fseek ( dbFp, offset, SEEK_SET ) ;

  (void) getName ( dbFp ) ;
  if ( feof ( dbFp ) )
    return ( FALSE ) ;
  getFilmographyCounts ( dbFp, &noWithAttr, &noWithoutAttr ) ;
  for ( i = 0 ; i < noWithAttr ; i++ )
  {
    lrec -> entries [ count ] . titleKey = getTitle ( dbFp ) ;
    lrec -> entries [ count ] . attrKey = getAttr ( dbFp ) ;
    if ( listId < NO_OF_CAST_LISTS )
    {
       len = getByte ( dbFp ) ;
       if ( len > 0 ) 
         lrec -> entries [ count ] . cname = getString ( len, dbFp ) ;
       else
         lrec -> entries [ count ] . cname =  NULL ;
       lrec -> entries [ count ] . position = getPosition ( dbFp ) ;
       lrec -> entries [ count ] . lineOrder = NOPOS ;
       lrec -> entries [ count ] . groupOrder = NOPOS ;
       lrec -> entries [ count ] . subgroupOrder = NOPOS ;
    }
    else if ( listId == WRITER_LIST_ID )
    {
       lrec -> entries [ count ] . cname =  NULL ;
       lrec -> entries [ count ] . position = NOPOS ;
       lrec -> entries [ count ] . lineOrder = getPosition ( dbFp ) ;
       lrec -> entries [ count ] . groupOrder = getPosition ( dbFp ) ;
       lrec -> entries [ count ] . subgroupOrder = getPosition ( dbFp ) ;
    }
    else
    {
      lrec -> entries [ count ] . cname =  NULL ;
      lrec -> entries [ count ] . position = NOPOS ;
      lrec -> entries [ count ] . lineOrder = NOPOS ;
      lrec -> entries [ count ] . groupOrder = NOPOS ;
      lrec -> entries [ count ] . subgroupOrder = NOPOS ;
    }
    lrec -> entries [ count ] . title =  NULL ;
    lrec -> entries [ count ] . attr =  NULL ;
    lrec -> entries [ count ] . year =  -1 ;
    lrec -> entries [ count ] . mrr =  NULL ;
    lrec -> entries [ count ] . aka =  NULL ;
    count++ ;
  }
  for ( i = 0 ; i < noWithoutAttr ; i++ )
  {
    lrec -> entries [ count ] . titleKey = getTitle ( dbFp ) ;
    if ( listId < NO_OF_CAST_LISTS )
    {
       len = getByte ( dbFp ) ;
       if ( len > 0 ) 
         lrec -> entries [ count ] . cname = getString ( len, dbFp ) ;
       else
         lrec -> entries [ count ] . cname =  NULL ;
       lrec -> entries [ count ] . position = getPosition ( dbFp ) ;
       lrec -> entries [ count ] . lineOrder = NOPOS ;
       lrec -> entries [ count ] . groupOrder = NOPOS ;
       lrec -> entries [ count ] . subgroupOrder = NOPOS ;
    }
    else if ( listId == WRITER_LIST_ID )
    {
       lrec -> entries [ count ] . cname =  NULL ;
       lrec -> entries [ count ] . position =  NOPOS ;
       lrec -> entries [ count ] . lineOrder = getPosition ( dbFp ) ;
       lrec -> entries [ count ] . groupOrder = getPosition ( dbFp ) ;
       lrec -> entries [ count ] . subgroupOrder = getPosition ( dbFp ) ;
    }
    else
    {
      lrec -> entries [ count ] . cname =  NULL ;
      lrec -> entries [ count ] . position =  NOPOS ;
      lrec -> entries [ count ] . lineOrder = NOPOS ;
      lrec -> entries [ count ] . groupOrder = NOPOS ;
      lrec -> entries [ count ] . subgroupOrder = NOPOS ;
    }
    lrec -> entries [ count ] . attrKey = NOATTR ;
    lrec -> entries [ count ] . title =  NULL ;
    lrec -> entries [ count ] . attr =  NULL ;
    lrec -> entries [ count ] . year =  -1 ;
    lrec -> entries [ count ] . mrr =  NULL ;
    lrec -> entries [ count ] . aka =  NULL ;
    count++ ;
  }
  lrec -> count = count ;
  (void) qsort ( (void*) lrec -> entries, (size_t) count, sizeof ( struct listEntry ), (int (*) (const void*, const void*)) listRecTitleKeySort ) ;
  return ( TRUE ) ;
}


struct listRec *lookupFilmography ( FILE *dbFp, FILE *indexFp, struct nameSearchRec *nrec, int listId ) 
{
  long offset ;
  struct listRec *retval ;

  if ( nrec -> nameKey != NONAME )
    if ( ( offset = lookupNamesIndex ( indexFp, nrec -> nameKey ) ) >= 0 )
    {
      retval = newListRec ( ) ;
      if ( readFilmographyToListRec ( dbFp, offset, nrec -> nameKey, retval, listId ) )
        return ( retval ) ;
    }
  return ( NULL ) ;
}


void filmographyResultsTitleKeyLookup (struct nameSearchRec *chain)
{
  FILE  *keyFp, *indexFp ;
  struct nameSearchRec *nrec ;
  struct akaRec *arec ;
  int i, listId ;

  indexFp = openFile ( TITLEIDX ) ;
  keyFp = openFile ( TITLEKEY ) ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
      if ( nrec -> lists [ listId ] != NULL )
        for ( i = 0 ; i < nrec -> lists [ listId ] -> count ; i++ )
        {
          nrec -> lists [ listId ] -> entries [ i ] . title = mapTitleKeyToText ( nrec -> lists [ listId ] -> entries [ i ] . titleKey, keyFp, indexFp ) ;

          for ( arec = nrec -> lists [ listId ] -> entries [ i ] . aka ; arec != NULL ; arec = arec -> next )
            arec -> akaTitle = mapTitleKeyToText ( arec -> akaKey, keyFp, indexFp ) ;
        }
  }
  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
}


void filmographyResultsAttrKeyLookup (struct nameSearchRec *chain)
{
  FILE  *keyFp, *indexFp ;
  struct nameSearchRec *nrec ;
  struct akaRec *arec ;
  int i, listId ;

  indexFp = openFile ( ATTRIDX ) ;
  keyFp = openFile ( ATTRKEY ) ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
      if ( nrec -> lists [ listId ] != NULL )
        for ( i = 0 ; i < nrec -> lists [ listId ] -> count ; i++ )
        {
            nrec -> lists [ listId ] -> entries [ i ] . attr = mapAttrKeyToText ( nrec -> lists [ listId ] -> entries [ i ] . attrKey, keyFp, indexFp ) ;

            for ( arec = nrec -> lists [ listId ] -> entries [ i ] . aka ; arec != NULL ; arec = arec -> next )
              arec -> akaAttr = mapAttrKeyToText ( arec -> akaAttrKey, keyFp, indexFp ) ;
        }
  }
  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
}


void nameResultsAkaKeyLookup (struct nameSearchRec *nchain)
{
  FILE  *keyFp, *indexFp ;
  struct nameSearchRec *nrec ;
  struct akaNameRec *arec ;

  indexFp = openFile ( NAMEIDX ) ;
  keyFp = openFile ( NAMEKEY ) ;

  for ( nrec = nchain ; nrec != NULL ; nrec = nrec -> next )
    for ( arec = nrec -> aka ; arec != NULL ; arec = arec -> next )
      arec -> akaName = mapNameKeyToText ( arec -> akaKey, keyFp, indexFp ) ;

  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
}


void filmographyResultsKeyLookup (struct nameSearchRec *chain)
{
   filmographyResultsTitleKeyLookup ( chain ) ;
   filmographyResultsAttrKeyLookup ( chain ) ;
   nameResultsAkaKeyLookup ( chain ) ;
} 


void processFilmographySearch (struct nameSearchRec *chain)
{
  FILE  *dbFp = NULL, *indexFp = NULL ;
  int   listId ;
  struct nameSearchRec *nrec ;
  char fn [ MAXPATHLEN ] ;

  filmographySearchKeyLookup ( chain ) ;

  for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
  {
    for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
    {
      if ( nrec -> searchFlags [ listId ] )
      {
	if ( nrec -> firstMatch >= 0 && ! ( nrec -> searchparams . searchall ) )
	  continue ;
        if ( dbFp == NULL )
        {
          (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, DBSEXT ) ;
          dbFp = openFile ( fn ) ;
          (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, NDXEXT ) ;
          indexFp = openFile ( fn ) ;
        }
        nrec -> lists [ listId ] = lookupFilmography ( dbFp, indexFp, nrec, listId ) ; 
      }
      if  ( nrec -> lists [ listId ] != NULL && nrec -> firstMatch < 0 ) 
        nrec -> firstMatch = listId ;
    }
    if ( dbFp != NULL )
    {
      (void) fclose ( dbFp ) ;
      (void) fclose ( indexFp ) ;
      dbFp = NULL ;
    }
  }
  addYearsToNameSearch ( chain ) ;
  addRatingsToNameSearch ( chain ) ;
  addAkaToNameSearch ( chain ) ;
  addBiographiesToNameSearch ( chain ) ;
  addNameAkaToNameSearch ( chain ) ;

  filmographyResultsKeyLookup ( chain ) ;
}


int smrrNameSearchSort ( struct listEntry *e1, struct listEntry *e2 )
{
  float result, r1, r2 ;

  if ( e1 -> mrr == NULL )
    r1 = 0 ;
  else
    r1 = e1 -> mrr -> rating ;

  if ( e2 -> mrr == NULL )
    r2 = 0 ;
  else
    r2 = e2 -> mrr -> rating ;

  result = r2 - r1 ;
  if ( result == 0 )
    return ( 0 ) ;
  else if ( result > 0 )
    return ( 1 ) ;
  else
    return ( -1 ) ;
} 


int vmrrNameSearchSort ( struct listEntry *e1, struct listEntry *e2 )
{
  int r1, r2 ;

  if ( e1 -> mrr == NULL )
    r1 = 0 ;
  else
    r1 = e1 -> mrr -> votes ;

  if ( e2 -> mrr == NULL )
    r2 = 0 ;
  else
    r2 = e2 -> mrr -> votes ;

  return ( r2 - r1 ) ;
}


int yearNameSearchSort ( struct listEntry *e1, struct listEntry *e2 )
{
  return ( e1 -> year - e2 -> year ) ;
}


int alphaNameSearchSort ( struct listEntry *e1, struct listEntry *e2 )
{
  if ( e1 -> title [ 0 ] == '"' )
    if ( e2 -> title [ 0 ] == '"' )
      return ( caseCompare ( e1 -> title, e2 -> title )  ) ;
    else
      return ( 1 ) ;
  else
    if ( e2 -> title [ 0 ] == '"' )
      return ( -1 ) ;
    else
      return ( caseCompare ( e1 -> title, e2 -> title )  ) ;
}


void sortListResults ( struct nameSearchRec *chain )
{
  struct nameSearchRec *nrec ;
  int i ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
    if ( nrec -> firstMatch >= 0 )
      for ( i = nrec -> firstMatch ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
        if ( nrec -> lists [ i ] != NULL && nrec -> lists [ i ] -> count > 0 )
        {
          if ( nrec -> searchparams . mrropt == SMRR )
            (void) qsort ( (void*) nrec -> lists [ i ] -> entries, (size_t) nrec -> lists [ i ] -> count, sizeof ( struct listEntry ), (int (*) (const void*, const void*)) smrrNameSearchSort ) ; 
          else if ( nrec -> searchparams . mrropt  == VMRR )
            (void) qsort ( (void*) nrec -> lists [ i ] -> entries, (size_t) nrec -> lists [ i ] -> count, sizeof ( struct listEntry ), (int (*) (const void*, const void*)) vmrrNameSearchSort ) ; 
          else if ( nrec -> searchparams . yropt == YR )
            (void) qsort ( (void*) nrec -> lists [ i ] -> entries, (size_t) nrec -> lists [ i ] -> count, sizeof ( struct listEntry ), (int (*) (const void*, const void*)) yearNameSearchSort ) ; 
          else
            (void) qsort ( (void*) nrec -> lists [ i ] -> entries, (size_t) nrec -> lists [ i ] -> count, sizeof ( struct listEntry ), (int (*) (const void*, const void*)) alphaNameSearchSort ) ; 
        }
}


struct nameSearchRec *combineListResults ( struct nameSearchRec *chain )
{
  struct nameSearchRec *nrec, *minRec, *returnrec ;
  int i, j, k, l, count = 0, minCount, found, matched, curCount ;
  TitleID minTitleKey ;

  for ( nrec = chain, minCount = 9999, minRec = NULL ; nrec != NULL ; nrec = nrec -> next )
    if ( nrec -> firstMatch >= 0 )
    {
      for ( j = nrec -> firstMatch, curCount = 0 ; j < NO_OF_FILMOGRAPHY_LISTS ; j++ )
        if ( nrec -> lists [ j ] != NULL )
          curCount += nrec -> lists [ j ] -> count ;
      if ( curCount < minCount )
      {
        minCount = curCount ;
        minRec = nrec ;
      }
    }
    else 
      return ( NULL ) ;

  if ( minCount == 9999 )
    return ( NULL ) ;

  returnrec = newNameSearchRec ( ) ;
  returnrec -> searchparams = chain -> searchparams ;
  returnrec -> lists [ 0 ] = newListRec ( ) ;
  returnrec -> lists [ 0 ] -> count = 0 ;

  for ( k = minRec -> firstMatch ; k < NO_OF_FILMOGRAPHY_LISTS ; k++ )
    if ( minRec -> lists [ k ] != NULL )
    for ( i = 0 ; i < minRec -> lists [ k ] -> count ; i++ )
    {
      matched = TRUE ;
      minTitleKey = minRec -> lists [ k ] -> entries [ i ] . titleKey;
      for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
      {
        found = FALSE ;
        for ( l = nrec -> firstMatch ; !found && l < NO_OF_FILMOGRAPHY_LISTS ; l++ )
          if ( nrec -> lists [ l ] != NULL )
            for ( j = 0 ; !found && j < nrec -> lists [ l ] -> count ; j++ ) 
              found = ( minTitleKey == nrec -> lists [ l ] -> entries [ j ] . titleKey ) ;
        if ( ! found )
          matched = FALSE ;
      }
      if ( matched )
      {
        for ( j = 0 ; j < count ; j++ )
          if ( returnrec -> lists [ 0 ] -> entries [ j ] . titleKey == minTitleKey )
            matched = FALSE ;
        if ( matched )
        {
          returnrec -> lists [ 0 ] -> entries [ count ] . title = duplicateString ( minRec -> lists [ minRec -> firstMatch ] -> entries [ i ] . title ) ;
          returnrec -> lists [ 0 ] -> entries [ count ] . attr = NULL ;
          returnrec -> lists [ 0 ] -> entries [ count ] . cname = NULL ;
          returnrec -> lists [ 0 ] -> entries [ count ] . year = minRec -> lists [ minRec -> firstMatch ] -> entries [ i ] . year ;
          returnrec -> lists [ 0 ] -> entries [ count ] . mrr = cloneMrrData ( minRec -> lists [ minRec -> firstMatch ] -> entries [ i ] . mrr ) ;
          returnrec -> lists [ 0 ] -> entries [ count ] . aka = cloneAkaData ( minRec -> lists [ minRec -> firstMatch ] -> entries [ i ] . aka ) ;
          returnrec -> lists [ 0 ] -> count++ ;
          count++ ;
        }
      }
    }
  if ( returnrec -> lists [ 0 ] -> count > 0 )
    returnrec -> firstMatch = 0 ;
  return ( returnrec ) ;
}


struct nameSearchRec *makeNameChain ( struct titleSearchRec *trec )
{
  struct nameSearchRec *rrec = NULL ;
  int i, j, base, listId ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    listId = titleOrderData [ i ] . listId ;
    if ( trec -> results [ listId ] . count > 0 )
    {
      base = trec -> results [ listId ] . base ;
      for ( j = 0 ; j < trec -> results [ listId ] . count ; j++ )
        rrec = addNameSearchRec ( rrec, trec -> entries [ base + j ] . name, listId + 1 ) ;
    }
  }
  return ( rrec ) ;
}


void constrainFilmographySearch ( struct searchConstraints *constraints, struct nameSearchRec *nchain )
{
  FILE *titleInfoDb [ NO_OF_TITLE_INFO_LISTS ] ;
  FILE *titleInfoIdx [ NO_OF_TITLE_INFO_LISTS ] ;
  struct nameSearchRec *nrec ;
  int listId, i, j, quitLoop ;
  TitleID titleKey ;
 
  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
  {
    if ( constraints -> titleInfoString [ i ] [ 0 ] )
    {
      titleInfoDb [ i ] = openFile ( titleInfoDefs [ i ] . db ) ;
      titleInfoIdx [ i ] = openFile ( titleInfoDefs [ i ] . index ) ;
    }
    else
    {
      titleInfoDb [ i ] = NULL ;
      titleInfoIdx [ i ] = NULL ;
    }
  }

  for ( nrec = nchain ; nrec != NULL ; nrec = nrec -> next )
  {
    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
      if ( nrec -> lists [ listId ] != NULL )
        for ( i = 0 ; i < nrec -> lists [ listId ] -> count ; i++ )
        {
          titleKey = nrec -> lists [ listId ] -> entries [ i ] . titleKey ;
          for ( quitLoop = FALSE, j = 0 ; !quitLoop && j < NO_OF_TITLE_INFO_LISTS ; j++ )
          {
            if ( constraints -> titleInfoString [ j ] [ 0 ] )
              if ( constrainTitleInfo ( titleInfoDb [ j ], titleInfoIdx [ j ], titleKey, constraints -> titleInfoString [ j ] ) == FALSE )
                quitLoop = TRUE ;
          }
          if ( quitLoop )
            nrec -> lists [ listId ] -> entries [ i ] . titleKey = NOTITLE ;
        }
  }

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
  {
    if ( constraints -> titleInfoString [ i ] [ 0 ] )
    {
      (void) fclose ( titleInfoDb [ i ] ) ;
      (void) fclose ( titleInfoIdx [ i ] ) ;
    }
  }
}

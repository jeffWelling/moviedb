/*============================================================================
 *
 *  Program: titlesearch.c 
 *
 *  Version: 3.22
 *
 *  Purpose: title searching procedures
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
#include "aka.h"
#include "ratings.h"
#include "plot.h"
#include "trivia.h"
#include "titleinfo.h"
#include "literature.h"
#include "business.h"
#include "laserdisc.h"
#include "movielinks.h"
#include "castcomp.h"
#include "display.h"

void freeTitleSearchRec ( struct titleSearchRec *trec ) 
{
  struct akaRec *aprev, *arec ;
  struct lineRec *ln, *prevln ;
  int i ;

  free ( (void*) trec -> title ) ;
  free ( (void*) trec -> attr ) ;
  free ( (void*) trec -> mrr ) ;
  aprev = NULL ;
  for ( arec = trec -> aka ; arec != NULL ; arec = arec -> next )
  {
     free ( (void*) aprev ) ;
     free ( (void*) arec -> akaTitle ) ;
     free ( (void*) arec -> akaAttr ) ;
     aprev = arec ;
  }
  free ( (void*) aprev ) ;
  freePlotRec ( trec -> plot ) ; 

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    freeTrivia ( trec -> trivlists [ i ] ) ; 

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    freeTitleInfo ( trec -> titleInfo [ i ] ) ; 

  for ( i = 0 ; i < trec -> nameCount ; i++ )
  {
    free ( (void*) trec -> entries [ i ] . name ) ;
    free ( (void*) trec -> entries [ i ] . attr ) ;
    free ( (void*) trec -> entries [ i ] . cname ) ;
  } 
  freeMovieLinks ( trec -> noOfLinks, trec -> links ) ;

  prevln = NULL ;
  for ( ln = trec -> literature ; ln != NULL ; ln = ln -> next )
  {
    free ( (void*) prevln ) ;
    free ( (void*) ln -> text ) ;
    prevln = ln ;
  }
  free ( (void*) prevln ) ;
  prevln = NULL ;
  for ( ln = trec -> business ; ln != NULL ; ln = ln -> next )
  {
    free ( (void*) prevln ) ;
    free ( (void*) ln -> text ) ;
    prevln = ln ;
  }
  free ( (void*) prevln ) ;

  freeLaserRec ( trec -> laserdisc ) ;
}


void freeTitleChain ( struct titleSearchRec *chain )
{
  struct titleSearchRec *trec, *prev = NULL ;

  for ( trec = chain ; trec != NULL ; trec = trec -> next )
  {
    free ( (void*) prev ) ;
    freeTitleSearchRec ( trec ) ;
    prev = trec ;
  }
  free ( (void*) prev ) ;
}



void addTitleChainOpts ( struct titleSearchRec *tchain, struct titleSearchOptRec options )
{
  struct titleSearchRec *trec ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    trec -> searchparams = options ;
}

 
void addTitleChainTrivOpts ( struct titleSearchRec *tchain, int trivopts [ ] )
{
  struct titleSearchRec *trec ;
  int i ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
      trec -> trivflags [ i ] = trivopts [ i ] ;
}


void addTitleChainTitleInfoOpts ( struct titleSearchRec *tchain, int titleInfoOpts [ ] )
{
  struct titleSearchRec *trec ;
  int i ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
      trec -> titleInfoFlags [ i ] = titleInfoOpts [ i ] ;
}


struct titleSearchRec *addTitleSearchRec ( struct titleSearchRec *tchain, char *title )
{
  struct titleSearchRec *head, *tail ;

  if ( tchain == NULL )
  {
    tail = newTitleSearchRec ( ) ;
    head = tail ;
  }
  else
  {
    head = tchain ;
    for ( tail = tchain ; tail -> next != NULL ; tail = tail -> next ) ;
    tail -> next = newTitleSearchRec ( ) ;
    tail = tail -> next ;
  }
  tail -> title = duplicateString ( title ) ;
  return ( head ) ;
}


void swapAkaTitles (struct titleSearchRec *tchain)
{
  FILE  *dbFp, *indexFp = NULL, *keyFp = NULL ;
  struct titleSearchRec *trec ;
  char line [ MXLINELEN ] ;
  long mid, lower, upper, saveUpper, offset ;
  int found = FALSE ;
  TitleID titleKey, dbKey ;

  dbFp = openFile ( AKAIDX ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ;
  saveUpper = ftell ( dbFp ) / 6 ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    titleKey = trec -> titleKey ;
    lower = 0 ;
    upper = saveUpper ;
    found = FALSE ;
    while ( !found && upper >= lower )
    {
      mid = ( upper + lower ) / 2 ;
      (void) fseek ( dbFp, mid * 6, SEEK_SET ) ;
      dbKey = getTitle ( dbFp ) ;
      if ( titleKey == dbKey )
        found = TRUE ;
      else if ( dbKey < titleKey )
        lower = mid + 1 ;
      else
        upper = mid - 1 ;
    }
    if ( found )
    {
      if (  keyFp == NULL )
      {
        keyFp = openFile ( TITLEKEY ) ;
        indexFp = openFile ( TITLEIDX ) ;
      }
      trec -> titleKey = getTitle ( dbFp ) ;
      (void) fseek ( indexFp, 4 * trec -> titleKey, SEEK_SET ) ;
      offset = getFullOffset ( indexFp ) ;
      (void) fseek ( keyFp, offset, SEEK_SET ) ;
      (void) fgets ( line, MXLINELEN, keyFp ) ;
      (void) free ( (void*) trec -> title ) ;
      trec -> title = duplicateField ( line ) ;
    }
  }
  if ( keyFp != NULL )
  {
    (void) fclose ( keyFp ) ;
    (void) fclose ( indexFp ) ;
  }
  (void) fclose ( dbFp ) ;
}


void substringTitleSearchKeyLookup (struct titleSearchRec *tchain)
{
  FILE  *titleKeyFp ;
  struct titleSearchRec *tail = tchain ;
  char  line [ MXLINELEN ] ;
  char  *substring, *ptr ;
  int  casesen, matched ;
  int  count = 0 ;

  casesen = tchain -> searchparams . casesen ;
  substring = tchain -> title ;
  titleKeyFp = openFile ( TITLEKEY ) ;

  while ( fgets ( line, MXLINELEN, titleKeyFp ) != NULL )
  {
    ptr = strchr ( line, FSEP ) ;
    *ptr = '\0' ;
    matched = FALSE ;
    if ( casesen ) 
    {
      if ( strstr ( line, substring ) != NULL )
        matched = TRUE ;
    }
    else if ( caseStrStr ( line, substring ) != NULL )
      matched = TRUE ;

    if ( matched )
    {
      if ( count == 0 )
      {
         tchain -> title = duplicateString ( line ) ;
         tchain -> titleKey = strtol ( ++ptr, (char **) NULL, 16) ;
         count++ ;
      }
      else
      {
        tail -> next = newTitleSearchRec ( ) ;
        tail = tail -> next ;
        tail -> title = duplicateString ( line ) ;
        tail -> titleKey = strtol ( ++ptr, (char **) NULL, 16 ) ;
        tail -> searchparams = tchain -> searchparams ;
        count++ ;
      }
    }
  }
  (void) fclose ( titleKeyFp ) ;
  if ( count > 1 )
  {
    addTitleChainTrivOpts ( tchain -> next, tchain -> trivflags ) ;
    addTitleChainTitleInfoOpts ( tchain -> next, tchain -> titleInfoFlags ) ;
  }
}


void yearMatchTitleSearchKeyLookup (struct titleSearchRec *tchain)
{
  FILE  *indexFp ;
  struct titleSearchRec *trec ;
  struct titleSearchRec *tail = tchain ;
  char line [ MXLINELEN ] ;
  long mid = -1, lower = 0, upper ;
  int compare, count = 0 ;
  int found = FALSE ;
  char *keyptr, *searchtitle ;
  int skipped = 0 ;

  indexFp = openFile ( TITLEKEY ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
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
           moviedbError ( "titles index file corrupt" ) ;
         else
           skipped = strlen ( line ) ;
       else
         skipped = 0 ;
       if ( fgets ( line, MXLINELEN, indexFp ) == NULL )
       {
         (void) findSOL ( indexFp, mid ) ;
         (void) fgets ( line, MXLINELEN, indexFp ) ;
       }
       compare = yearFieldCaseCompare ( line, trec -> title );
       if ( compare == 0 )
          found = TRUE;
       else if ( compare < 0 )
          lower = mid + skipped + strlen ( line ) - 1 ;
       else
          upper = mid - 1 ;
    }

   if ( found )
   {
     while ( mid > 0 && found )
     {
       (void) findSOL ( indexFp, mid ) ;
       mid = ftell ( indexFp ) - 1 ;
       (void) fgets ( line, MXLINELEN, indexFp ) ;
       found = ( yearFieldCaseCompare ( line, trec -> title ) == 0 ) ;
     }
     if ( !found )
     {
       found = TRUE ;
       (void) fgets ( line, MXLINELEN, indexFp ) ;
     }
     if ( ( keyptr = strchr ( line, FSEP ) ) != NULL ) 
     {
       *keyptr++ = '\0' ;
       searchtitle = trec -> title ;
       trec -> title = duplicateString ( line ) ;
       trec -> titleKey = strtol ( keyptr, (char **) NULL, 16) ;
       while ( found && fgets ( line, MXLINELEN, indexFp ) != NULL )
         if ( ( found = ( yearFieldCaseCompare ( line, searchtitle ) == 0 ) ) )
           if ( ( keyptr = strchr ( line, FSEP ) ) != NULL ) 
           {
             *keyptr++ = '\0' ;
             tail -> next = newTitleSearchRec ( ) ;
             tail = tail -> next ;
             tail -> title = duplicateString ( line ) ;
             tail -> titleKey = strtol ( keyptr, (char **) NULL, 16 ) ;
             tail -> searchparams = tchain -> searchparams ;
             count++ ;
           }
       free ( (void*) searchtitle ) ;
     }
   }
   else
       trec -> titleKey = NOTITLE ;
  }

  (void) fclose ( indexFp ) ;
  if ( count > 0 )
  {
    addTitleChainTrivOpts ( tchain -> next, tchain -> trivflags ) ;
    addTitleChainTitleInfoOpts ( tchain -> next, tchain -> titleInfoFlags ) ;
  }
}


void straightTitleSearchKeyLookup (struct titleSearchRec *tchain)
{
  FILE  *indexFp ;
  struct titleSearchRec *trec ;
  char line [ MXLINELEN ] ;
  long mid = -1, lower = 0, upper ;
  int compare ;
  int found = FALSE ;
  char *keyptr ;
  int skipped = 0 ;

  indexFp = openFile ( TITLEKEY ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
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
           moviedbError ( "titles index file corrupt" ) ;
         else
           skipped = strlen ( line ) ;
       else
         skipped = 0 ;
       if ( fgets ( line, MXLINELEN, indexFp ) == NULL )
       {
         (void) findSOL ( indexFp, mid ) ;
         (void) fgets ( line, MXLINELEN, indexFp ) ;
       }
       compare = fieldCaseCompare ( line, trec -> title );
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
       trec -> titleKey = strtol ( keyptr, (char **) NULL, 16) ;
       (void) strcpy ( trec -> title, line ) ;
     }
   else
       trec -> titleKey = NOTITLE ;
  }

  (void) fclose ( indexFp ) ;
}


void findTitleData ( FILE *dbFp, long offset, TitleID titleKey, int dupeCount, NameID *nameKey, AttributeID *attrKey, char **cname, int *position, int *lineOrder, int *groupOrder, int *subgroupOrder, int listId )
{
  int i, len, noWithAttr, noWithoutAttr, loopCount = 0 ; 
  TitleID dbTitleKey = NOTITLE ;
  AttributeID dbAttrKey = NOATTR ;

  *cname = NULL ;
  *attrKey = NOATTR ;
  *position = MAXPOS ;
  *lineOrder = MAXPOS ;
  *groupOrder = MAXPOS ;
  *subgroupOrder = MAXPOS ;
  (void) fseek ( dbFp, offset, SEEK_SET ) ;
  *nameKey = getName ( dbFp ) ;
  getFilmographyCounts ( dbFp, &noWithAttr, &noWithoutAttr ) ;
  for ( i = 0 ; i < noWithAttr ; i++ )
  {
    dbTitleKey = getTitle ( dbFp ) ;
    dbAttrKey = getAttr ( dbFp ) ;
    if ( dbTitleKey == titleKey )
      if ( ++loopCount > dupeCount )
        break ;
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
  }
  if ( dbTitleKey == titleKey && loopCount > dupeCount )
  {
    *attrKey = dbAttrKey ;
    if ( listId < NO_OF_CAST_LISTS ) 
    {
      len = getByte ( dbFp ) ;
      if ( len > 0 )
        *cname = getString ( len, dbFp ) ;
      if ( (*position = getPosition ( dbFp ) ) == NOPOS )
        *position = MAXPOS ;
    }
    else if ( listId == WRITER_LIST_ID ) 
    {
      if ( (*lineOrder = getPosition ( dbFp ) ) == NOPOS )
        *lineOrder = MAXPOS ;

      if ( (*groupOrder = getPosition ( dbFp ) ) == NOPOS )
        *groupOrder = MAXPOS ;

      if ( (*subgroupOrder = getPosition ( dbFp ) ) == NOPOS )
        *subgroupOrder = MAXPOS ;
    }
    return ;
  }

  if ( listId < NO_OF_CAST_LISTS ) 
  {
    for ( i = 0 ; i < noWithoutAttr ; i++ )
    {
      dbTitleKey = getTitle ( dbFp ) ;
      if ( dbTitleKey == titleKey )
        break ;
      len = getByte ( dbFp ) ;
      if ( len > 0 )
        (void) fseek ( dbFp, len, SEEK_CUR ) ;
      (void) getPosition ( dbFp ) ;
    }
    if ( dbTitleKey == titleKey )
    {
      len = getByte ( dbFp ) ;
      if ( len > 0 )
        *cname = getString ( len, dbFp ) ;
      if ( (*position = getPosition ( dbFp ) ) == NOPOS )
        *position = MAXPOS ;
    }
  }
  else if ( listId == WRITER_LIST_ID ) 
  {
    for ( i = 0 ; i < noWithoutAttr ; i++ )
    {
      dbTitleKey = getTitle ( dbFp ) ;
      if ( dbTitleKey == titleKey )
        if ( ++loopCount > dupeCount )
          break ;
      (void) getPosition ( dbFp ) ;
      (void) getPosition ( dbFp ) ;
      (void) getPosition ( dbFp ) ;
    }
    if ( dbTitleKey == titleKey )
    {
      if ( (*lineOrder = getPosition ( dbFp ) ) == NOPOS )
        *lineOrder = MAXPOS ;

      if ( (*groupOrder = getPosition ( dbFp ) ) == NOPOS )
        *groupOrder = MAXPOS ;

      if ( (*subgroupOrder = getPosition ( dbFp ) ) == NOPOS )
        *subgroupOrder = MAXPOS ;
    }
  }
}
  

int findTitleIndexBase ( FILE *indexFp, TitleID titleKey ) 
{
  long mid = -1, lower = 0, upper ;
  int found = FALSE ;
  TitleID indexKey = NOTITLE ;

  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  upper = ftell ( indexFp ) ;
  upper = upper / 7 ;

  while ( !found && upper >= lower )
  {
     mid = ( upper + lower ) / 2 ;
     (void) fseek ( indexFp, mid * 7, SEEK_SET ) ;
     indexKey = getTitle ( indexFp ) ;
     if ( titleKey == indexKey )
       found = TRUE ;
    else if ( indexKey < titleKey )
      lower = mid + 1 ;
    else
      upper = mid - 1 ;
  }
  if ( found ) 
  {
    if ( mid == 0 )
      (void) fseek ( indexFp, 0, SEEK_SET ) ;
    else  
    {
      while ( titleKey == indexKey )
      {
        if ( --mid >= 0 )
        {
          (void) fseek ( indexFp, mid * 7, SEEK_SET ) ;
          indexKey = getTitle ( indexFp ) ;
        }
        else
          break ;
      }
      if ( mid >= 0 )
        (void) getFullOffset ( indexFp ) ;
      else
        (void) fseek ( indexFp, 0, SEEK_SET ) ;
    }
    return ( TRUE ) ;
  }
  else
    return ( FALSE ) ;
}


void lookupTitles ( FILE *dbFp, FILE *indexFp, struct titleSearchRec *trec, int listId ) 
{
  TitleID titleKey ;
  AttributeID attrKey ;
  NameID nameKey ;
  long offset, prevOffset ;
  char *cname ;
  int position ;
  int lineOrder ;
  int groupOrder ;
  int subgroupOrder ;
  int dupeCount ;

  if ( trec -> titleKey != NOTITLE )
    if ( findTitleIndexBase ( indexFp, trec -> titleKey ) )
    {
      titleKey = getTitle ( indexFp ) ;
      offset = getFullOffset ( indexFp ) ;
      dupeCount = 0 ;
      while ( titleKey == trec -> titleKey )
      {
        findTitleData ( dbFp, offset, titleKey, dupeCount, &nameKey, &attrKey, &cname, &position, &lineOrder, &groupOrder, &subgroupOrder, listId ) ;
        if ( trec -> results [ listId ] . base < 0 )
          trec -> results [ listId ] . base = trec -> nameCount ;
        trec -> entries [ trec -> nameCount ] . nameKey = nameKey ;
        trec -> entries [ trec -> nameCount ] . attrKey = attrKey ;
        trec -> entries [ trec -> nameCount ] . cname = cname ;
        trec -> entries [ trec -> nameCount ] . position = position ;
        trec -> entries [ trec -> nameCount ] . lineOrder = lineOrder ;
        trec -> entries [ trec -> nameCount ] . groupOrder = groupOrder ;
        trec -> entries [ trec -> nameCount ] . subgroupOrder = subgroupOrder ;
        trec -> results [ listId ] . count++ ;
        trec -> nameCount++ ;
	if ( trec -> nameCount > MAXTITLERESULTS )
	  moviedbError ( "Too many names -- increase MAXTITLERESULTS" ) ;
        titleKey = getTitle ( indexFp ) ;
        prevOffset = offset ;
        offset = getFullOffset ( indexFp ) ;
        if ( offset == prevOffset )
          dupeCount++ ;
        else
          dupeCount = 0 ;
      }
    }
}


void titleResultsNameKeyLookup (struct titleSearchRec *tchain)
{
  FILE  *keyFp, *indexFp ;
  struct titleSearchRec *trec ;
  int i ;

  indexFp = openFile ( NAMEIDX ) ;
  keyFp = openFile ( NAMEKEY ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    for ( i = 0 ; i < trec -> nameCount ; i++ )
      trec -> entries [ i ] . name = mapNameKeyToText ( trec -> entries [ i ] . nameKey, keyFp, indexFp ) ;

  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
}


void titleResultsAttrKeyLookup (struct titleSearchRec *tchain)
{
  FILE  *keyFp, *indexFp ;
  struct titleSearchRec *trec ;
  struct titleInfoRec *tirec ;
  struct akaRec *arec ;
  int i ;

  indexFp = openFile ( ATTRIDX ) ;
  keyFp = openFile ( ATTRKEY ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    for ( i = 0 ; i < trec -> nameCount ; i++ )
      trec -> entries [ i ] . attr = mapAttrKeyToText ( trec -> entries [ i ] . attrKey, keyFp, indexFp ) ;
    trec -> attr = mapAttrKeyToText ( trec -> attrKey, keyFp, indexFp ) ;
    for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
      for ( tirec = trec -> titleInfo [ i ] ; tirec != NULL ; tirec = tirec -> next )
        tirec -> attr = mapAttrKeyToText ( tirec -> attrKey, keyFp, indexFp ) ;
    for ( arec = trec -> aka ; arec != NULL ; arec = arec -> next )
      arec -> akaAttr = mapAttrKeyToText ( arec -> akaAttrKey, keyFp, indexFp ) ;
  }
  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
}


void titleResultsTitleKeyLookup (struct titleSearchRec *tchain)
{
  FILE  *keyFp, *indexFp ;
  struct titleSearchRec *trec ;
  struct akaRec *arec ;
  int i ;

  indexFp = openFile ( TITLEIDX ) ;
  keyFp = openFile ( TITLEKEY ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    for ( arec = trec -> aka ; arec != NULL ; arec = arec -> next )
      arec -> akaTitle = mapTitleKeyToText ( arec -> akaKey, keyFp, indexFp ) ;
    for ( i = 0 ; i < trec -> noOfLinks ; i++ )
      trec -> links [ i ] . title = mapTitleKeyToText ( trec -> links [ i ] . titleKey, keyFp, indexFp ) ;
  }

  (void) fclose ( indexFp ) ;
  (void) fclose ( keyFp ) ;
}


void titleResultsKeyLookup (struct titleSearchRec *tchain)
{
   titleResultsAttrKeyLookup ( tchain ) ;
   titleResultsNameKeyLookup ( tchain ) ;
   titleResultsTitleKeyLookup ( tchain ) ;
} 


void removeDuplicateAKAs (struct titleSearchRec *tchain)
{
  struct titleSearchRec *trec, *dup, *prev ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    for ( prev = trec, dup = trec -> next ; dup != NULL ; prev = dup, dup = dup -> next )
     if ( trec -> titleKey == dup -> titleKey )
     {
       prev -> next = dup -> next ;
       freeTitleSearchRec ( dup ) ;
       free ( (void*) dup ) ;
       dup = prev ;
     }
  }
}



void titleSearchKeyLookup (struct titleSearchRec *tchain)
{
  if ( tchain -> searchparams . substring )
    substringTitleSearchKeyLookup ( tchain ) ;
  else if ( tchain -> searchparams . yearMatch )
    yearMatchTitleSearchKeyLookup ( tchain ) ;
  else
    straightTitleSearchKeyLookup ( tchain ) ;
  if ( tchain -> searchparams . akaopt )
  {
    swapAkaTitles ( tchain ) ;
    if ( tchain -> searchparams . substring ) 
      removeDuplicateAKAs ( tchain ) ;
  }
}


void processTitleSearch ( struct titleSearchRec *tchain )
{
  FILE  *dbFp = NULL, *indexFp = NULL ;
  int   listId ;
  struct titleSearchRec *trec ;
  char fn [ MAXPATHLEN ] ;

  titleSearchKeyLookup ( tchain ) ;

  for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
  {
    for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    {
        if ( dbFp == NULL )
        {
          (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, DBSEXT ) ;
          dbFp = openFile ( fn ) ;
          (void) constructFilename ( fn, filmographyDefs [ listId ] . stem, TDXEXT ) ;
          indexFp = openFile ( fn ) ;
        }
        lookupTitles ( dbFp, indexFp, trec, listId ) ; 
    }
    if ( dbFp != NULL )
    {
      (void) fclose ( dbFp ) ;
      (void) fclose ( indexFp ) ;
      dbFp = NULL ;
    }
  }

  addYearsToTitleSearch ( tchain ) ;
  addRatingsToTitleSearch ( tchain ) ;
  addAkaToTitleSearch ( tchain ) ;
  addTriviaToTitleSearch ( tchain ) ;
  addTitleInfoToTitleSearch ( tchain ) ;
  addPlotToTitleSearch ( tchain ) ;
  addLiteratureToTitleSearch ( tchain ) ;
  addBusinessToTitleSearch ( tchain ) ;
  addLaserDiscToTitleSearch ( tchain ) ;
  addCastCompleteStatusToTitleSearch ( tchain ) ;
  addCrewCompleteStatusToTitleSearch ( tchain ) ;
  addMovieLinksToTitleSearch ( tchain ) ;

  titleResultsKeyLookup ( tchain ) ;
}


struct titleSearchRec *combineTitleResults ( struct titleSearchRec *tchain )
{
  struct titleSearchRec *trec, *minRec = NULL, *returnrec ;
  int i, j, k, minCount, matched, base, count = 0 ;
  NameID nameKey ;

  for ( trec = tchain, minCount = 9999 ; trec != NULL ; trec = trec -> next )
    if ( trec -> nameCount < minCount )
    {
      minCount = trec -> nameCount ;
      minRec = trec ;
    }

  if ( minCount == 9999 || minCount == 0 )
    return ( NULL ) ;

  returnrec = newTitleSearchRec ( ) ;
  returnrec -> searchparams = tchain -> searchparams ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    if ( minRec -> results [ i ] . count > 0 )
    {
      base = minRec -> results [ i ] . base ;
      for ( j = 0 ; j < minRec -> results [ i ] . count ; j++ )
      {
        nameKey = minRec -> entries [ base + j ] . nameKey ;
        matched = TRUE ;
        for ( trec = tchain ; matched && trec != NULL ; trec = trec -> next )
        {
          matched = FALSE ;
          if ( trec -> results [ i ] . count > 0 )
            for ( k = 0 ; !matched && k < trec -> results [ i ] . count ; k++ )
              if ( nameKey == trec -> entries [ k + trec -> results [ i ] . base] . nameKey )
                matched = TRUE ;
        }
        if ( matched ) 
        {
          returnrec -> entries [ count ] . name = duplicateString ( minRec -> entries [ base + j ] . name ) ;
          returnrec -> entries [ count ] . attr = NULL ;
          returnrec -> entries [ count ] . cname = NULL ;
          if ( returnrec -> results [ i ] . base < 0 )
            returnrec -> results [ i ] . base = count ;
          returnrec -> results [ i ] . count++ ;
          returnrec -> nameCount++ ;
          count++ ;
        }
      }
    }
  }
  return ( returnrec ) ;
}


struct titleSearchRec *makeTitleChain ( struct nameSearchRec *nrec, struct titleSearchOptRec options, int trivopts  [ ], struct nameSearchOptRec noptions, int titleInfoOpts [ ] )
{
  struct titleSearchRec *rrec = NULL, *prev, *r ;
  int  i, j, found ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
    if ( nrec -> lists [ i ] != NULL )
      for ( j = 0 ; j < nrec -> lists [ i ] -> count ; j++ )
        if ( !noptions . mvsonly || nrec -> lists [ i ] -> entries [ j ] . title [ 0 ] != '"' )
        {
          for ( found = FALSE, r = rrec ; r != NULL ; r = r -> next ) 
            if ( strcmp ( r -> title, nrec -> lists [ i ] -> entries [ j ] . title ) == 0 )
              found = TRUE ;
          if ( found == FALSE )
            if ( noptions . searchall )
            {
              prev = NULL ;
              for ( found = FALSE, r = rrec ; !found && r != NULL ; prev = r, r = r -> next )
                if ( nrec -> lists [ i ] -> entries [ j ] . year < r -> year )
                  found = TRUE ;
              if ( found && prev != NULL )
              {
                prev -> next = newTitleSearchRec ( ) ;
                prev -> next -> next = r ;
                prev -> next -> title = duplicateString ( nrec -> lists [ i ] -> entries [ j ] . title ) ;
                prev -> next -> year = nrec -> lists [ i ] -> entries [ j ] . year ;
              }
              else
            {
                if ( rrec == NULL )
                {
                  rrec = newTitleSearchRec ( ) ;
                  rrec -> title = duplicateString ( nrec -> lists [ i ] -> entries [ j ] . title ) ;
                  rrec -> year = nrec -> lists [ i ] -> entries [ j ] . year ;
                }
                else
                {
                   for ( r = rrec ; r -> next != NULL ; r = r -> next ) ;
                   r -> next = newTitleSearchRec ( ) ;
                   r -> next -> title = duplicateString ( nrec -> lists [ i ] -> entries [ j ] . title ) ;
                   r -> next -> year = nrec -> lists [ i ] -> entries [ j ] . year ;
                }
              }
            }
            else
              rrec = addTitleSearchRec ( rrec, nrec -> lists [ i ] -> entries [ j ] . title ) ;
        }    
  addTitleChainOpts ( rrec, options ) ;
  addTitleChainTrivOpts ( rrec, trivopts ) ;
  addTitleChainTitleInfoOpts ( rrec, titleInfoOpts ) ;
  return ( rrec ) ;
}


struct titleSearchRec *addTitleFile ( char *fname )
{
  FILE *fp ;
  struct titleSearchRec *rrec = NULL ;
  char line [ MXLINELEN ] ;

  fp = openFile ( fname ) ;

  while ( fgets ( line, MXLINELEN, fp ) != NULL )
    {
      stripEOL ( line ) ;
      rrec = addTitleSearchRec ( rrec, line ) ;
    }

  (void) fclose ( fp ) ;
  return ( rrec ) ;
}


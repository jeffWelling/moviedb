/*============================================================================
 *
 *  Program: years.c 
 *
 *  Version: 3.4
 *
 *  Purpose: year database procedures
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1998 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 * 
 *============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "moviedb.h"
#include "dbutils.h"

void addYearsToNameSearch (struct nameSearchRec *chain)
{
  FILE  *dbFp ;
  struct nameSearchRec *nrec ;
  long mid, lower, upper, saveUpper ;
  int found = FALSE ;
  int i, listId ;
  TitleID titleKey, dbKey ;

  for ( nrec = chain ; !found && nrec != NULL ; nrec = nrec -> next )
    found = nrec -> searchparams. yropt ;

  if ( ! found )
    return ;

  dbFp = openFile ( MOVIEDB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / RECBYTES ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
      if ( nrec -> lists [ listId ] != NULL )
        for ( i = 0 ; i < nrec -> lists [ listId ] -> count ; i++ )
        {
          titleKey = nrec -> lists [ listId ] -> entries [ i ] . titleKey ;
          lower = 0 ;
          upper = saveUpper ;
          found = FALSE ;
          while ( !found && upper >= lower )
          {
             mid = ( upper + lower ) / 2 ;
             (void) fseek ( dbFp, mid * RECBYTES, SEEK_SET ) ;
             dbKey = getTitle ( dbFp ) ;
             if ( titleKey == dbKey )
               found = TRUE ;
            else if ( dbKey < titleKey )
              lower = mid + 1 ;
            else
              upper = mid - 1 ;
          }
          if ( found )
            nrec -> lists [ listId ] -> entries [ i ] . year = getInt ( dbFp ) ;
        }
  }
  (void) fclose ( dbFp ) ;
}


void addYearsToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp ;
  struct titleSearchRec *trec ;
  long mid, lower, upper, saveUpper ;
  int found = FALSE ;
  TitleID titleKey, dbKey ;

  for ( trec = tchain ; !found && trec != NULL ; trec = trec -> next )
    found = trec -> searchparams. yropt ;

  if ( ! found )
    return ;

  dbFp = openFile ( MOVIEDB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / RECBYTES ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    titleKey = trec -> titleKey ;
    lower = 0 ;
    upper = saveUpper ;
    found = FALSE ;
    while ( !found && upper >= lower )
    {
      mid = ( upper + lower ) / 2 ;
      (void) fseek ( dbFp, mid * RECBYTES, SEEK_SET ) ;
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
      trec -> year = getInt ( dbFp ) ;
      (void) getInt ( dbFp ) ;
      trec -> attrKey = getAttr ( dbFp ) ;
    }
  }
  (void) fclose ( dbFp ) ;
}


int yearLformatSort ( struct formatRec *r1, struct formatRec *r2 )
{
  if ( r1 -> year != r2 -> year )
    return ( r1 -> year - r2 -> year ) ;
  else
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
}


TitleID readTitleDb ( struct titleDbRec array [] ) 
{
  TitleID count = 0 ;
  FILE *dbFp ;

  dbFp = openFile ( MOVIEDB ) ;
  while ( TRUE )
  {
    array [ count ] . titleKey = getTitle ( dbFp ) ;
    if ( feof ( dbFp ) )
      break ;
    array [ count ] . year = getInt ( dbFp ) ;
    (void) getInt ( dbFp ) ;
    (void) getAttr ( dbFp ) ;
    count++ ;
  }
  return ( count ) ;
}

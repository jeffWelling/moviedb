/*============================================================================
 *
 *  Program: ratings.c 
 *
 *  Version: 3.18
 *
 *  Purpose: movie ratings procedures
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
 *============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include "moviedb.h"
#include "dbutils.h"

struct mrrRec *cloneMrrData ( struct mrrRec *mrec )
{
  struct mrrRec *res ;

  if ( mrec == NULL )
    return ( NULL ) ;

  res = newMrrRec ( ) ;
  *res = *mrec ;
  return ( res ) ;
}


struct mrrRec *readMrrEntry ( FILE *dbFp )
{
  struct mrrRec *retval ;
  int i ;

  retval = newMrrRec ( ) ;
  for ( i = 0 ; i < 10 ; i++ )
    retval -> distribution [ i ] = fgetc ( dbFp ) ;
  retval -> distribution [ 10 ] = '\0' ;
  retval -> votes = getInt ( dbFp ) ;
  retval -> rating = (float) getByte ( dbFp ) / 10.0 ;
  return ( retval ) ;
}

void addRatingsToNameSearch (struct nameSearchRec *chain)
{
  FILE  *dbFp ;
  struct nameSearchRec *nrec ;
  long mid, lower, upper, saveUpper ;
  int found = FALSE ;
  int i, listId, key ;
  TitleID dbKey ;

  for ( nrec = chain ; !found && nrec != NULL ; nrec = nrec -> next )
    found = nrec -> searchparams. mrropt ;

  if ( ! found )
    return ;

  dbFp = openFile ( MRRDB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / 16 ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
      if ( nrec -> lists [ listId ] != NULL )
        for ( i = 0 ; i < nrec -> lists [ listId ] -> count ; i++ )
        {
          key = nrec -> lists [ listId ] -> entries [ i ] . titleKey ;
          lower = 0 ;
          upper = saveUpper ;
          found = FALSE ;
          while ( !found && upper >= lower )
          {
             mid = ( upper + lower ) / 2 ;
             (void) fseek ( dbFp, mid * 16, SEEK_SET ) ;
             dbKey = getTitle ( dbFp ) ;
             if ( key == dbKey )
               found = TRUE ;
            else if ( dbKey < key )
              lower = mid + 1 ;
            else
              upper = mid - 1 ;
          }
          if ( found )
            nrec -> lists [ listId ] -> entries [ i ] . mrr = readMrrEntry ( dbFp ) ;
        }
  }
  (void) fclose ( dbFp ) ;
}


void addRatingsToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp ;
  struct titleSearchRec *trec ;
  long mid, lower, upper, saveUpper ;
  int found = FALSE ;
  TitleID key, dbKey ;

  for ( trec = tchain ; !found && trec != NULL ; trec = trec -> next )
    found = trec -> searchparams. mrropt ;

  if ( ! found )
    return ;

  dbFp = openFile ( MRRDB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / 16 ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    key = trec -> titleKey ;
    lower = 0 ;
    upper = saveUpper ;
    found = FALSE ;
    while ( !found && upper >= lower )
    {
      mid = ( upper + lower ) / 2 ;
      (void) fseek ( dbFp, mid * 16, SEEK_SET ) ;
      dbKey = getTitle ( dbFp ) ;
      if ( key == dbKey )
        found = TRUE ;
      else if ( dbKey < key )
        lower = mid + 1 ;
      else
        upper = mid - 1 ;
    }
    if ( found )
      trec -> mrr = readMrrEntry ( dbFp ) ;
  }
  (void) fclose ( dbFp ) ;
}


int readMrrRec ( FILE *mrrFp, TitleID *mrrKey, struct mrrRec *mrr )
{
  int  i ;

  *mrrKey = getTitle ( mrrFp ) ;
  if ( feof ( mrrFp ) )
  {
    *mrrKey = NOTITLE ;
    return ( FALSE ) ;
  }
  else
  {
    for ( i = 0 ; i < 10 ; i++ )
      mrr -> distribution [ i ] = fgetc ( mrrFp ) ;
    mrr -> distribution [ 10 ] = '\0' ;
    mrr -> votes = getInt ( mrrFp ) ;
    mrr -> rating = (float) getByte ( mrrFp ) / 10.0 ;
    return ( TRUE ) ;
  }
}


int smrrLformatSort ( struct formatRec *r1, struct formatRec *r2 )
{
  float result ;

  result = r2 -> mrr . rating - r1 -> mrr . rating ;
  if ( result == 0 )
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
  else if ( result > 0 )
    return ( 1 ) ;
  else
    return ( -1 ) ;
}


int vmrrLformatSort ( struct formatRec *r1, struct formatRec *r2 )
{
  if ( r1 -> mrr . votes != r2 -> mrr . votes )
    return ( r2 -> mrr . votes - r1 -> mrr . votes ) ;
  else
    return ( caseCompare ( r1 -> title, r2 -> title ) ) ;
}


TitleID readRatingsDb ( struct mrrDbRec array [] )
{
  TitleID count = 0, i ;
  FILE *dbFp ;

  dbFp = openFile ( MRRDB ) ;
  while ( TRUE )
  {
    array [ count ] . titleKey = getTitle ( dbFp ) ;
    if ( feof ( dbFp ) )
      break ;
    for ( i = 0 ; i < 10 ; i++ )
      array [ count ] . mrr . distribution [ i ] = fgetc ( dbFp ) ;
    array [ count ] . mrr . distribution [ 10 ] = '\0' ;
    array [ count ] . mrr . votes = getInt ( dbFp ) ;
    array [ count ] . mrr . rating = (float) getByte ( dbFp ) / 10.0 ;
    count++ ;
  }
  return ( count ) ;
}

TitleID ratingsDbSize ( void )
{
  TitleID count = 0 ;
  FILE *dbFp ;

  dbFp = openFile ( MRRDB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  count = ftell ( dbFp ) / 16 ;
  (void) fclose ( dbFp ) ;
  return ( count ) ;
}


TitleID readVotesDb ( struct voteDbRec array [] )
{
  TitleID count = 0 ;
  FILE *dbFp ;

  dbFp = openFile ( VOTEDB ) ;
  while ( TRUE )
  {
    array [ count ] . titleKey = getTitle ( dbFp ) ;
    if ( feof ( dbFp ) )
      break ;
    array [ count ] . vote = getByte ( dbFp ) ;
    count++ ;
  }
  return ( count ) ;
}



/*============================================================================ 
 * 
 *  Program: castcomp.c 
 * 
 *  Version: 3.3
 * 
 *  Purpose: cast completion procedures
 * 
 *  Author:  C J Needham <col@imdb.com>      
 * 
 *  Copyright (c) 1996-1997 The Internet Movie Database Inc.
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
#include <string.h> 
#include "moviedb.h" 
#include "dbutils.h" 

enum compCastStatus findCompletionStatus ( FILE *dbFp, TitleID titleKey )
{
  long upper, lower, mid, offset ;
  int found = FALSE ;
  TitleID indexKey ;

  (void) fseek ( dbFp, 0, SEEK_END ) ;
  upper = ftell ( dbFp ) / 4 ;
  lower = 0 ;
  found = FALSE ;
  while ( !found && upper >= lower )
  {
    mid = ( upper + lower ) / 2 ;
    (void) fseek ( dbFp, mid * 4, SEEK_SET ) ;
    indexKey = getTitle ( dbFp ) ;
    if ( titleKey == indexKey )
      found = TRUE ;
    else if ( indexKey < titleKey )
      lower = mid + 1 ;
    else
      upper = mid - 1 ;
  }
  if ( found )
    return ( getByte ( dbFp ) ) ;
  else
    return ( unknown ) ;
}


void addCastCompleteStatusToTitleSearch ( struct titleSearchRec *tchain )
{
  FILE  *dbFp ;
  struct titleSearchRec *trec ;

  dbFp = openFile ( CASTCOMDB ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    trec -> castStatus = findCompletionStatus ( dbFp, trec -> titleKey ) ;

  (void) fclose ( dbFp ) ;
}


void addCrewCompleteStatusToTitleSearch ( struct titleSearchRec *tchain )
{
  FILE  *dbFp ;
  struct titleSearchRec *trec ;

  dbFp = openFile ( CREWCOMDB ) ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    trec -> crewStatus = findCompletionStatus ( dbFp, trec -> titleKey ) ;

  (void) fclose ( dbFp ) ;
}

/*============================================================================
 *
 *  Program: business.c
 *
 *  Version: 3.3
 *
 *  Purpose: business list functions
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996-1997 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 *============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "moviedb.h"
#include "dbutils.h"
#include "literature.h"


void freeBusiness ( struct lineRec *data )
{
  struct lineRec *rec =NULL, *prev ;
 
  if ( rec != NULL )
  {
    prev = NULL ;
    for ( rec = data ; rec != NULL ; rec = rec -> next )
    {
      free ( (void*) prev ) ;
      free ( (void*) rec -> text ) ;
      prev = rec ;
    }
    free ( (void*) prev ) ;
  }
}


struct lineRec *readBusiness ( FILE *stream, long offset ) 
{
  char line [ MXLINELEN ] ;
  struct lineRec *rec = NULL, *tail = NULL ;

  (void) fseek ( stream, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, stream ) ;

  while ( fgets ( line, MXLINELEN, stream ) != NULL )
    if ( strncmp ( line, "MV:", 3 ) == 0 )
      break ;
    else 
    {
      if ( rec == NULL )
      {
        rec = newLineRec ( ) ;
        rec -> text = duplicateString ( line ) ;
        rec -> next = NULL ;
        tail = rec ;
      }
      else
      {
        tail -> next = newLineRec ( ) ;
        tail -> next -> text = duplicateString ( line ) ;
        tail -> next -> next = NULL ;
        tail = tail -> next ;
      }
    }
  return ( rec ) ;
}


void addBusinessToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp, *indexFp ;
  struct titleSearchRec *trec ;
  long mid, lower, upper, saveUpper, offset ;
  int found = FALSE ;
  TitleID titleKey, indexTitleKey ;

  for ( trec = tchain ; !found && trec != NULL ; trec = trec -> next )
    found = trec -> searchparams. litopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( BUSDB ) ;
  indexFp = openFile ( BUSIDX ) ;
  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( indexFp ) / 6 ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    if ( trec -> searchparams . litopt )
    {
      titleKey = trec -> titleKey ;
      lower = 0 ;
      upper = saveUpper ;
      found = FALSE ;
      while ( !found && upper >= lower )
      {
        mid = ( upper + lower ) / 2 ;
        (void) fseek ( indexFp, mid * 6, SEEK_SET ) ;
        indexTitleKey = getTitle ( indexFp ) ;
        if ( titleKey == indexTitleKey )
          found = TRUE ;
        else if ( indexTitleKey < titleKey )
          lower = mid + 1 ;
        else
          upper = mid - 1 ;
      }
      if ( found )
      {
        offset = getOffset ( indexFp ) ;
        trec -> business = readBusiness ( dbFp, offset ) ;
      }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( indexFp ) ;
}

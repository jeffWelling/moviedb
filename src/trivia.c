/*============================================================================ 
 * 
 *  Program: trivia.c 
 * 
 *  Version: 3.24
 * 
 *  Purpose: trivia procedures
 * 
 *  Author:  C J Needham <col@imdb.com>      
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
#include <stdlib.h> 
#include <string.h> 
#include "moviedb.h" 
#include "dbutils.h" 


void freeTrivia ( struct lineRec *rec )
{
  struct lineRec *lrec, *prevl = NULL ;

  for ( lrec = rec ; lrec != NULL ; lrec = lrec -> next )
  {
    free ( (void*) prevl ) ;
    free ( (void*) lrec -> text ) ;
    prevl = lrec ;
  }
  free ( (void*) prevl ) ;
}

 
struct lineRec *readTrivia ( FILE *dbFp, long offset )  
{ 
  char line [ MXLINELEN ] ; 
  struct lineRec *head = NULL, *tail = NULL, *lrec = NULL ; 

  (void) fseek ( dbFp, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, dbFp ) ;
 
  while ( fgets ( line, MXLINELEN, dbFp ) != NULL ) 
    if ( strncmp ( line, TRIVTITLE, 2 ) == 0 ) 
      break ; 
    else 
    { 
      lrec = newLineRec ( ) ;
      lrec -> text = duplicateString ( line ) ; 
      lrec -> next = NULL ; 
      if ( head == NULL ) 
        head = lrec ; 
      else 
        tail -> next = lrec ; 
      tail = lrec ; 
    } 
  return ( head ) ; 
} 
 
 
struct lineRec *findTitleTrivia ( FILE *dbFp, FILE *indexFp, TitleID titleKey )
{
  long upper, lower, mid, offset ;
  int found = FALSE ;
  TitleID indexKey ;

  (void) fseek ( indexFp, 0, SEEK_END ) ;
  upper = ftell ( indexFp ) / 7 ;
  lower = 0 ;
  found = FALSE ;
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
    offset = getFullOffset ( indexFp ) ;
    return ( readTrivia ( dbFp, offset ) ) ;
  }
  else
    return ( NULL ) ;
}


void addTriviaToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp = NULL, *indexFp = NULL ;
  struct titleSearchRec *trec ;
  int listId ;

  for ( listId = 0 ; listId < NO_OF_TRIV_LISTS ; listId++ )
  {
    for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    {
      if ( trec -> trivflags [ listId ] )
      {
        if ( dbFp == NULL )
        {
          dbFp = openFile ( triviaDefs [ listId ] . db ) ;
          indexFp = openFile ( triviaDefs [ listId ] . index ) ;
        }
        trec -> trivlists [ listId ] = findTitleTrivia ( dbFp, indexFp, trec -> titleKey ) ;
      }
    }
    if ( dbFp != NULL )
    {
      (void) fclose ( dbFp ) ;
      (void) fclose ( indexFp ) ;
      dbFp = NULL ;
    }
  }
}

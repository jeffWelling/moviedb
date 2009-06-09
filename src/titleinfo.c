/*============================================================================ 
 * 
 *  Program: titleinfo.c 
 * 
 *  Version: 3.1
 * 
 *  Purpose: title info procedures
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


void freeTitleInfo ( struct titleInfoRec *rec )
{
  struct titleInfoRec *lrec, *prevl = NULL ;

  for ( lrec = rec ; lrec != NULL ; lrec = lrec -> next )
  {
    free ( (void*) prevl ) ;
    free ( (void*) lrec -> text ) ;
    free ( (void*) lrec -> attr ) ;
    prevl = lrec ;
  }
  free ( (void*) prevl ) ;
}

 
struct titleInfoRec *readTitleInfo ( FILE *dbFp, long offset, TitleID searchKey )  
{ 
  char *line ; 
  int len ;
  struct titleInfoRec *head = NULL, *tail = NULL, *lrec = NULL ; 
  TitleID titleKey ;

  (void) fseek ( dbFp, offset, SEEK_SET ) ;

  titleKey = getTitle ( dbFp ) ;

  while ( feof ( dbFp ) == 0 && titleKey == searchKey ) 
  { 
    len = getByte ( dbFp ) ;
    line = getString ( len, dbFp ) ;
    lrec = newTitleInfoRec ( ) ;
    lrec -> text = line ; 
    lrec -> attrKey = getAttr ( dbFp ) ;
    lrec -> next = NULL ; 
    if ( head == NULL ) 
      head = lrec ; 
    else 
      tail -> next = lrec ; 
    tail = lrec ; 
    titleKey = getTitle ( dbFp ) ;
  } 
  return ( head ) ; 
} 
 
 
struct titleInfoRec *findTitleInfo ( FILE *dbFp, FILE *indexFp, TitleID titleKey )
{
  long upper, lower, mid, offset ;
  int found = FALSE ;
  TitleID indexKey ;

  (void) fseek ( indexFp, 0, SEEK_END ) ;
  upper = ftell ( indexFp ) / 6 ;
  lower = 0 ;
  found = FALSE ;
  while ( !found && upper >= lower )
  {
    mid = ( upper + lower ) / 2 ;
    (void) fseek ( indexFp, mid * 6, SEEK_SET ) ;
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
    offset = getOffset ( indexFp ) ;
    return ( readTitleInfo ( dbFp, offset, titleKey ) ) ;
  }
  else
    return ( NULL ) ;
}


void addTitleInfoToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp = NULL, *indexFp = NULL ;
  struct titleSearchRec *trec ;
  int listId ;

  for ( listId = 0 ; listId < NO_OF_TITLE_INFO_LISTS ; listId++ )
  {
    for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    {
      if ( trec -> titleInfoFlags [ listId ] )
      {
        if ( dbFp == NULL )
        {
          dbFp = openFile ( titleInfoDefs [ listId ] . db ) ;
          indexFp = openFile ( titleInfoDefs [ listId ] . index ) ;
        }
        trec -> titleInfo [ listId ] = findTitleInfo ( dbFp, indexFp, trec -> titleKey ) ;
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


int constrainTitleInfo ( FILE *dbFp, FILE *indexFp, TitleID titleKey, char *string )
{
  long upper, lower, mid, offset ;
  int found = FALSE, len ;
  TitleID indexKey, key ;
  char *line ;

  (void) fseek ( indexFp, 0, SEEK_END ) ;
  upper = ftell ( indexFp ) / 6 ;
  lower = 0 ;
  found = FALSE ;
  while ( !found && upper >= lower )
  {
    mid = ( upper + lower ) / 2 ;
    (void) fseek ( indexFp, mid * 6, SEEK_SET ) ;
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
    offset = getOffset ( indexFp ) ;
    (void) fseek ( dbFp, offset, SEEK_SET ) ;
    found = FALSE ;
    key = getTitle ( dbFp ) ;
    while ( feof ( dbFp ) == 0 && key == titleKey && found == FALSE ) 
    { 
       len = getByte ( dbFp ) ;
       line = getString ( len, dbFp ) ;
       (void) getAttr ( dbFp ) ;
       key = getTitle ( dbFp ) ;
       if ( caseCompare ( line, string ) == 0 )
         found = TRUE ;
       free ( (void*) line ) ;
    }
    return ( found ) ;
  }
  else
    return ( FALSE ) ;
}


int readTitleInfoIndex ( FILE *idxFp, TitleID *titleKey, long *offset )
{
  *titleKey = getTitle ( idxFp ) ;
  if ( feof ( idxFp ) )
    return ( FALSE ) ;
  else
    *offset = getOffset ( idxFp ) ;
  return ( TRUE ) ;
}


int checkConstraints ( FILE *dbFp, long offset, TitleID titleKey, char *string )
{
  static char line [ MXLINELEN ] ;
  int found = FALSE, len ;
  TitleID key ;

  (void) fseek ( dbFp, offset, SEEK_SET ) ;
  found = FALSE ;
  key = getTitle ( dbFp ) ;
  while ( feof ( dbFp ) == 0 && key == titleKey && found == FALSE ) 
  { 
     len = getByte ( dbFp ) ;
     getStringFast ( len, dbFp, line ) ;
     (void) getAttr ( dbFp ) ;
     key = getTitle ( dbFp ) ;
     if ( caseCompare ( line, string ) == 0 )
       found = TRUE ;
  }
  return ( found ) ;
}

/*============================================================================ 
 * 
 *  Program: movielink.c 
 * 
 *  Version: 3.18
 * 
 *  Purpose: movie links procedures
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
#include <string.h> 
#include "moviedb.h" 
#include "dbutils.h" 
#define my_min(a, b)  (((a) < (b)) ? (a) : (b))

struct tempMovieLinkRec
{
  struct movieLinkRec		data [ LINKCHUNKSIZE ] ;
  struct tempMovieLinkRec	*next ;
} ;

void freeMovieLinks ( int noOfEntries, struct movieLinkRec *rec )
{
  int i ;

  for ( i = 0 ; i < noOfEntries ; i++ )
    free ( (void*) rec [ i ] . title ) ;
  free ( (void*) rec ) ;
}


struct movieLinkRec *readMovieLinks ( FILE *dbFp, TitleID searchKey, int *count )  
{ 
  struct tempMovieLinkRec firstchunk ;
  struct tempMovieLinkRec *active = &firstchunk ;
  struct tempMovieLinkRec *lastactive ;
  struct movieLinkRec *retval ;
  TitleID titleKey = searchKey ;
  int activecount = 0 ;
  int activeleft ;

  active -> next = NULL ;
  *count = 0 ;
  titleKey = getTitle ( dbFp ) ;
  while ( feof ( dbFp ) == 0 && titleKey == searchKey ) 
  { 
    active -> data [ activecount ] . link = getByte ( dbFp ) ;
    active -> data [ activecount ] . titleKey = getTitle ( dbFp ) ;
    active -> data [ activecount ] . title = NULL ;
    activecount++ ;
    if ( activecount == LINKCHUNKSIZE )
    {
	*count += activecount ;
	activecount = 0 ;
	active -> next = malloc ( sizeof (struct tempMovieLinkRec) ) ;
	active = active -> next ;
	active -> next = NULL ;
    }
    titleKey = getTitle ( dbFp ) ;
  } 
  *count += activecount ;
  if ( ( retval = calloc ( *count, sizeof ( struct movieLinkRec ) ) ) != NULL )
  {
    activecount = 0 ;
    activeleft = *count ;
    active = &firstchunk ;
    while ( active )
    {
	memcpy ( retval + activecount , active,
	  my_min ( activeleft, LINKCHUNKSIZE ) * sizeof ( struct movieLinkRec ) ) ;
	activecount += LINKCHUNKSIZE ;
	activeleft -= LINKCHUNKSIZE ;
	lastactive = active ;
	active = active -> next ;
	if ( lastactive != &firstchunk )
	  free ( lastactive ) ;
    }
    return ( retval ) ;
  }
  else
  {
    *count = 0 ;
    return ( NULL ) ;
  }
} 
 
 
struct movieLinkRec *findMovieLinks ( FILE *dbFp, TitleID titleKey, int *count )
{
  long upper, lower, mid = 0 ;
  int found = FALSE ;
  TitleID dbTitleKey = NOTITLE ;

  (void) fseek ( dbFp, 0, SEEK_END ) ;
  upper = ftell ( dbFp ) / 7 ;
  lower = 0 ;
  found = FALSE ;
  while ( !found && upper >= lower )
  {
    mid = ( upper + lower ) / 2 ;
    (void) fseek ( dbFp, mid * 7, SEEK_SET ) ;
    dbTitleKey = getTitle ( dbFp ) ;
    if ( titleKey == dbTitleKey )
      found = TRUE ;
    else if ( dbTitleKey < titleKey )
      lower = mid + 1 ;
    else
      upper = mid - 1 ;
  }
  if ( found )
  {
    if ( mid == 0 )
      (void) fseek ( dbFp, 0, SEEK_SET ) ;
    else
    {
      while ( titleKey == dbTitleKey )
      {
        if ( --mid >= 0 ) 
        {
          (void) fseek ( dbFp, mid * 7, SEEK_SET ) ;
          dbTitleKey = getTitle ( dbFp ) ;
        }
        else
          break ;
      }
      if ( mid >= 0 )
      {
        (void) getByte ( dbFp ) ;
        (void) getTitle ( dbFp ) ;
      }
      else
        (void) fseek ( dbFp, 0, SEEK_SET ) ;
    }
    return ( readMovieLinks ( dbFp, titleKey, count ) ) ;
  }
  else
    return ( NULL ) ;
}


void addMovieLinksToTitleSearch ( struct titleSearchRec *tchain )
{
  FILE  *dbFp ;
  struct titleSearchRec *trec ;

  dbFp = openFile ( LINKDB ) ;
  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    if ( trec -> searchparams . linkopt )
      trec -> links = findMovieLinks ( dbFp, trec -> titleKey, &(trec -> noOfLinks) ) ;
  }
  (void) fclose ( dbFp ) ;
}

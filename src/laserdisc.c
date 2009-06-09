/*============================================================================
 *
 *  Program: laserdisc.c
 *
 *  Version: 3.3
 *
 *  Purpose: laserdisc database procedures
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

void freeLaserRec ( struct laserRec *rec )
{
  struct laserRec *lrec, *prevrec ;
  struct lineRec *ln, *prevln ;
 
  if ( rec != NULL )
  {
    prevrec = NULL ;
    for ( lrec = rec ; lrec != NULL ; lrec = lrec -> next )
    {
      free ( (void*) lrec -> LN ) ;
      free ( (void*) lrec -> LB ) ;
      free ( (void*) lrec -> CN ) ;
      free ( (void*) lrec -> LT ) ;
      prevln = NULL ;
      for ( ln = lrec -> misc ; ln != NULL ; ln = ln -> next )
      {
        free ( (void*) prevln ) ;
        free ( (void*) ln -> text ) ;
        prevln = ln ;
      }
      free ( (void*) prevln ) ;
      prevrec = lrec ;
    }
    free ( (void*) prevrec ) ;
  }
}


struct laserRec *readLaserDisc ( FILE *stream, long offset ) 
{
  char line [ MXLINELEN + 1 ] ;
  struct laserRec *rec ;
  struct lineRec *tail ;
  char *newStr ;
  char saveChar ;
  size_t saveLen ;

  line [ MXLINELEN ] = '\0' ;

  rec = newLaserRec ( ) ;
  rec -> misc = NULL ;
  rec -> next = NULL ;
  rec -> LN = NULL ;
  rec -> LB = NULL ;
  rec -> CN = NULL  ;
  rec -> LT = NULL ;

  (void) fseek ( stream, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, stream ) ;

  while ( fgets ( line, MXLINELEN, stream ) != NULL )
    if ( line [ 0 ] != '\n' )
      if ( line [ 2 ] != ':' && line [ 0 ] != '-' )
        return ( NULL ) ; 
      else
      {
        if ( line [ 3 ] ==  '\n' )
	{
	  line [ 3 ] = ' ' ;
	  line [ 4 ] = '\n' ;
	  line [ 5 ] = '\0' ;
        }
        if ( strncmp ( line, "--", 2 ) == 0 )
          break ;
        else if ( strncmp ( line, "LN", 2 ) == 0 )
          rec -> LN = duplicateString ( line + 4) ;
        else if ( strncmp ( line, "LB", 2 ) == 0 )
          rec -> LB = duplicateString ( line + 4) ;
        else if ( strncmp ( line, "CN", 2 ) == 0 )
          rec -> CN = duplicateString ( line + 4) ;
        else if ( strncmp ( line, "LT", 2 ) == 0 )
          rec -> LT = duplicateString ( line + 4) ;
        else 
        {
          if ( rec -> misc == NULL )
          {
            rec -> misc = newLineRec ( ) ;
            tail = rec -> misc ;
          }
          else
          {
            tail -> next = newLineRec ( ) ;
            tail = tail -> next ;
          }
          tail -> text = duplicateString ( line ) ;
          tail -> next = NULL ;
          while ( strlen ( line ) > MXLINELEN - 2 )
	  {
            saveLen = strlen ( tail -> text ) ;
            saveChar = tail -> text [ saveLen - 1 ] ;
	    (void) fgets ( line, MXLINELEN, stream ) ;
	    newStr = appendString ( tail -> text, line ) ;
            newStr [ saveLen - 1 ] = saveChar ;
	    tail -> text = newStr ;
	  }
        }
      }
  return ( rec ) ;
}


void addLaserDiscToTitleSearch ( struct titleSearchRec *tchain )
{
  FILE  *dbFp, *indexFp ;
  struct laserRec *tail ;
  struct titleSearchRec *trec ;
  long mid, lower, upper, saveUpper, offset ;
  int found = FALSE ;
  TitleID titleKey, indexTitleKey ;

  for ( trec = tchain ; !found && trec != NULL ; trec = trec -> next )
    found = trec -> searchparams. ldopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( LDDB ) ;
  indexFp = openFile ( LDIDX ) ;
  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( indexFp ) / 6 ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    if ( trec -> searchparams . ldopt )
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
        if ( mid == 0 )
          (void) fseek ( indexFp, 0, SEEK_SET ) ;
        else 
        {
          while ( titleKey == indexTitleKey )
          {
            if ( --mid >= 0 ) 
            {
              (void) fseek ( indexFp, mid * 6, SEEK_SET ) ;
              indexTitleKey = getTitle ( indexFp ) ;
            }
            else
              break ;
          }
          if ( mid >= 0 )
            (void) getOffset ( indexFp ) ;
          else
            (void) fseek ( indexFp, 0, SEEK_SET ) ;
        }
        indexTitleKey = getTitle ( indexFp ) ;
        while ( titleKey == indexTitleKey )
        {
          offset = getOffset ( indexFp ) ;
          if ( trec -> laserdisc == NULL )
          {
            trec -> laserdisc = readLaserDisc ( dbFp, offset ) ;
	    if ( trec -> laserdisc )
              tail = trec -> laserdisc ;
/* else should natter */
          }
          else
          {
            tail -> next = readLaserDisc ( dbFp, offset ) ;
	    if ( tail -> next )
              tail = tail -> next ;
/* else should natter */
          }
          indexTitleKey = getTitle ( indexFp ) ;
        }
      }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( indexFp ) ;
}

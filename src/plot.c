/*============================================================================
 *
 *  Program: plot.c
 *
 *  Version: 3.2
 *
 *  Purpose: plot summary prodcedures
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996 The Internet Movie Database Inc.
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


void freePlotRec ( struct plotRec *rec )
{
  struct outlineRec *oline, *prevoline ;
 
  if ( rec != NULL )
  {
    prevoline = NULL ;
    for ( oline = rec -> outline ; oline != NULL ; oline = oline -> next )
    {
      free ( (void*) prevoline ) ;
      free ( (void*) oline -> BY ) ;
      free ( (void*) oline -> PL ) ;
      prevoline = oline ;
    }
    free ( (void*) prevoline ) ;
    free ( (void*) rec -> RV ) ;
    free ( (void*) rec ) ;
  }
}


struct plotRec *readPlot ( FILE *stream, long offset ) 
{
  int state = MV_STATE ;
  char line [ MXLINELEN ] ;
  struct plotRec *rec ;
  struct outlineRec *oline, *optr = NULL ;

  rec = newPlotRec ( ) ;
  rec -> outline = NULL ;
  rec -> RV = NULL ;

  (void) fseek ( stream, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, stream ) ;

  while ( fgets ( line, MXLINELEN, stream ) != NULL )
    if ( line [ 0 ] != '\n' && line [ 0 ] != '-' )
      if ( line [ 2 ] != ':' )
        return ( NULL ) ;
      else
      {
        if ( strncmp ( line, TITLE_KEY, 2 ) == 0 )
          break ;
        else if ( strncmp ( line, PLOT_KEY, 2 ) == 0 )
        {
          if ( state != PL_STATE )
          {
             oline = newOutlineRec ( ) ;
             if ( optr == NULL )
               rec -> outline = oline ;
             else 
                optr -> next = oline ;
             optr = oline ;
             optr -> PL = duplicateString ( line + 4 ) ;
             optr -> BY = NULL ;
             optr -> next = NULL ;
             state = PL_STATE ;
          }
          else
            optr -> PL = appendString ( optr -> PL, line + 4 ) ;
        }
        else if ( strncmp ( line, BY_KEY, 2 ) == 0 )
        {
          if ( state != PL_STATE )
            return ( NULL ) ;
          optr -> BY = duplicateString ( line + 4 ) ;
          state = MV_STATE ;
        }
        else if ( strncmp ( line, REVIEW_KEY, 2 ) == 0 )
	  rec -> RV = duplicateString ( line + 4 ) ;
     }
     return ( rec ) ;
}


void addPlotToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp, *indexFp ;
  struct titleSearchRec *trec ;
  long mid, lower, upper, saveUpper, offset ;
  int found = FALSE ;
  TitleID titleKey, indexTitleKey ;

  for ( trec = tchain ; !found && trec != NULL ; trec = trec -> next )
    found = trec -> searchparams. plotopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( PLOTDB ) ;
  indexFp = openFile ( PLOTIDX ) ;
  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( indexFp ) / 7 ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    if ( trec -> searchparams . plotopt )
    {
      titleKey = trec -> titleKey ;
      lower = 0 ;
      upper = saveUpper ;
      found = FALSE ;
      while ( !found && upper >= lower )
      {
        mid = ( upper + lower ) / 2 ;
        (void) fseek ( indexFp, mid * 7, SEEK_SET ) ;
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
        offset = getFullOffset ( indexFp ) ;
        trec -> plot = readPlot ( dbFp, offset ) ;
      }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( indexFp ) ;

#ifdef INTERNAL
  dbFp = openFile ( OUTLDB ) ;
  indexFp = openFile ( OUTLIDX ) ;
  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( indexFp ) / 7 ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    if ( trec -> searchparams . plotopt )
    {
      titleKey = trec -> titleKey ;
      lower = 0 ;
      upper = saveUpper ;
      found = FALSE ;
      while ( !found && upper >= lower )
      {
        mid = ( upper + lower ) / 2 ;
        (void) fseek ( indexFp, mid * 7, SEEK_SET ) ;
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
	struct plotRec *olrec ;

        offset = getFullOffset ( indexFp ) ;
        olrec = readPlot ( dbFp, offset ) ;
	if ( trec -> plot ) {
	  olrec -> outline -> next = trec -> plot -> outline ;
	  trec -> plot -> outline = olrec -> outline ;
	  free ( olrec ) ;
	}
	else
	  trec -> plot = olrec ;
      }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( indexFp ) ;
#endif
}

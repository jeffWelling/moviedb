/*============================================================================
 *
 *  Program: biographies.c
 *
 *  Version: 3.4
 *
 *  Purpose: biography database procedures
 *
 *  Author:  C J Needham <col@imdb.com>      
 *
 *  Copyright (c) 1996-1998 The Internet Movie Database Inc.
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

void freePersonRec ( struct personRec *rec )
{
  struct bioRec *brec, *prevrec ;
  struct bioMiscRec *bmrec, *prevbmrec ;
  struct lineRec *ln, *prevln ;
 
  if ( rec != NULL )
  {
    prevrec = NULL ;
    for ( brec = rec -> biography ; brec != NULL ; brec = brec -> next )
    {
      free ( (void*) prevrec ) ;
      free ( (void*) brec -> BY ) ;
      free ( (void*) brec -> biog ) ;
      prevrec = brec ;
    }
    free ( (void*) prevrec ) ;

    prevbmrec = NULL ;
    for ( bmrec = rec -> misc ; bmrec != NULL ; bmrec = bmrec -> next )
    {
      free ( (void*) prevbmrec ) ;
      prevln = NULL ;
      for ( ln = bmrec -> list ; ln != NULL ; ln = ln -> next )
      {
        free ( (void*) prevln ) ;
        free ( (void*) ln -> text ) ;
        prevln = ln ;
      }
      free ( (void*) prevln ) ;
      prevbmrec = bmrec ;
    }
    free ( (void*) prevbmrec ) ;

    free ( (void*) rec -> RN ) ;
    free ( (void*) rec -> DB ) ;
    free ( (void*) rec -> DD ) ;
  
    rec -> biography = NULL ;
    rec -> RN = NULL ;
    rec -> DB = NULL ;
    rec -> DD = NULL ;
    rec -> misc = NULL ;
  }
}


struct personRec *readBiography ( FILE *stream, long offset ) 
{
  int state = PERSON_STATE ;
  char line [ MXLINELEN ] ;
  struct personRec *rec ;
  struct bioRec *brec, *bptr = NULL ;
  struct bioMiscRec *mrec, *mptr = NULL ;
  struct lineRec *ln, *lptr = NULL ;
  char lastmisc [ 2 ] = "  " ;

  rec = newPersonRec ( ) ;
  rec -> biography = NULL ;
  rec -> RN = NULL ;
  rec -> DB = NULL ;
  rec -> DD = NULL ;
  rec -> misc = NULL ;

  (void) fseek ( stream, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, stream ) ;

  while ( fgets ( line, MXLINELEN, stream ) != NULL )
    if ( line [ 0 ] != '\n' && line [ 0 ] != '-' )
      if ( line [ 2 ] != ':' )
        return ( NULL ) ; 
      else
      {
        if ( line [ 3 ] ==  '\n' )
	{
	  line [ 3 ] = ' ' ;
	  line [ 4 ] = '\n' ;
	  line [ 5 ] = '\0' ;
        }
        if ( strncmp ( line, NAME_KEY, 2 ) == 0 )
          break ;
        else if ( strncmp ( line, REALNAME_KEY, 2 ) == 0 )
          rec -> RN = duplicateString ( line + 4) ;
        else if ( strncmp ( line, BIRTHDATE_KEY, 2 ) == 0 )
          rec -> DB = duplicateString ( line + 4) ;
        else if ( strncmp ( line, DEATHDATE_KEY, 2 ) == 0 )
          rec -> DD = duplicateString ( line + 4) ;
        else if ( strncmp ( line, BIO_KEY, 2 ) == 0 )
        {
          if ( state != BIO_STATE )
          {
             brec = newBioRec ( ) ;
             if ( bptr == NULL )
               rec -> biography = brec ;
             else 
                bptr -> next = brec ;
             bptr = brec ;
             bptr -> biog = duplicateString ( line + 4 ) ;
             bptr -> BY = NULL ;
             bptr -> next = NULL ;
             state = BIO_STATE ;
          }
          else
            bptr -> biog = appendString ( bptr -> biog, line + 4 ) ;
        }
        else if ( strncmp ( line, AUTHOR_KEY, 2 ) == 0 )
        {
          if ( state != BIO_STATE )
            return ( NULL ) ;
          bptr -> BY = duplicateString ( line + 4) ;
          state = PERSON_STATE ;
        }
        else
        {
          if ( strncmp ( line, lastmisc, 2 ) != 0 )
          {
            (void) strncpy ( lastmisc, line, 2 ) ;
            mrec = newBioMiscRec ( ) ;
            if ( mptr == NULL )
              rec -> misc = mrec ;
            else 
               mptr -> next = mrec ;
            mptr = mrec ;
            ln = newLineRec ( ) ;
            ln -> text = duplicateString ( line + 4) ;
	    ln -> next = NULL ;
            mptr -> list = ln ;
            mptr -> next = NULL ;
            lptr = ln ;
            (void) strncpy ( mptr -> label, line, 2 ) ;
          }
          else
          {
            ln = newLineRec ( ) ;
            ln -> text = duplicateString ( line + 4) ;
	    ln -> next = NULL ;
            lptr -> next = ln ;
            lptr = ln ;
          }

        }
     }
  return ( rec ) ;
}


void addBiographiesToNameSearch (struct nameSearchRec *chain)
{
  FILE  *dbFp, *indexFp ;
  struct nameSearchRec *nrec ;
  long mid, lower, upper, saveUpper ;
  int found = FALSE ;
  NameID nameKey, indexNameKey ;
  long offset ;

  for ( nrec = chain ; !found && nrec != NULL ; nrec = nrec -> next )
    found = nrec -> searchparams. biopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( BIODB ) ;
  indexFp = openFile ( BIOIDX ) ;
  (void) fseek ( indexFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( indexFp ) / 7 ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
    if ( nrec -> searchparams . biopt )
    {
      nameKey = nrec -> nameKey ;
      lower = 0 ;
      upper = saveUpper ;
      found = FALSE ;
      while ( !found && upper >= lower )
      {
        mid = ( upper + lower ) / 2 ;
        (void) fseek ( indexFp, mid * 7, SEEK_SET ) ;
        indexNameKey = getName ( indexFp ) ;
        if ( nameKey == indexNameKey )
          found = TRUE ;
        else if ( indexNameKey < nameKey )
          lower = mid + 1 ;
        else
          upper = mid - 1 ;
      }
      if ( found )
      {
         offset = getFullOffset ( indexFp ) ;
         nrec -> biography = readBiography ( dbFp, offset ) ;
      }
  }
  (void) fclose ( dbFp ) ;
  (void) fclose ( indexFp ) ;
}

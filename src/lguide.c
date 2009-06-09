/*============================================================================
 *
 *  Program: lguide.c 
 *
 *  Version: 3.7
 *
 *  Purpose: search list databases for a set of titles
 *
 *  Author:  C J Needham <col@imdb.com>     
 *
 *  Copyright (c) 1996-1999 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 * 
 *  Options:
 *    -f <file>   specify file of title
 *   
 *    -yr         display year of release
 *    -mrr        display movie ratings report info
 *    -aka        search alternative title database
 *    -pl         add plot description
 *    -bus        add business info
 *    -lit        add literature info
 *    -chr        add character names
 *    -trivia     add any trivia, crazy credits, goofs and quotes
 *
 *    -full       short hand for -yr -mrr -pl -chr -aka -trivia
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "moviedb.h"
#include "dbutils.h"
#include "titlesearch.h"
#include "display.h"

#define LGUIDE_USAGE1 "usage: lguide -f <file> [-aka -mrr -yr -pl -chr -trivia] [-full]"

void reportUnmatches ( struct titleSearchRec *tchain ) 
{
  struct titleSearchRec *trec ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    if ( trec -> titleKey == NOTITLE )
      fprintf ( stderr, "No data for: %s\n", trec -> title ) ;
}


int main ( int argc, char **argv )
{
  char  fname [ MAXPATHLEN ] = "\0" ;
  int   err = FALSE ;
  int   i, j ;
  int   trivopts [ NO_OF_TRIV_LISTS ] ;
  int   titleInfoOpts [ NO_OF_TITLE_INFO_LISTS ] ;
  struct titleSearchRec *tchain = NULL ;
  struct titleSearchOptRec options = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;

  logProgram ( argv [ 0 ] ) ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    trivopts [ i ] = FALSE ;
  for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
    titleInfoOpts [ j ] = FALSE ;
   
  for ( i = 1; i < argc; i++ )
    if ( strcmp ( "-f", argv[i] ) == 0 )
      if ( ++i < argc )
        (void) strcpy ( fname, argv [ i ] ) ;
      else
        err = TRUE ;
    else if ( strcmp ( "-pl", argv[i] ) == 0 )
      options . plotopt = TRUE ;
    else if ( strcmp ( "-bus", argv[i] ) == 0 )
      options . businessopt = TRUE ;
    else if ( strcmp ( "-ld", argv[i] ) == 0 )
      options . ldopt = TRUE ;
    else if ( strcmp ( "-mrr", argv[i] ) == 0 )
      options . mrropt = TRUE ;
    else if ( strcmp ( "-yr", argv[i] ) == 0 )
      options . yropt = TRUE ;
    else if ( strcmp ( "-aka", argv[i] ) == 0 )
      options . akaopt = TRUE ;
    else if ( strcmp ( "-chr", argv[i] ) == 0 )
      options . chopt = TRUE ;
    else if ( strcmp ( "-trivia", argv[i] ) == 0 )
    {
      for ( j = 0 ; j < NO_OF_TRIV_LISTS ; j++ )
        trivopts [ j ] = TRUE ;
      for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
        titleInfoOpts [ j ] = TRUE ;
    }
    else if ( strcmp ( "-full", argv[i] ) == 0 )
    {
      for ( j = 0 ; j < NO_OF_TRIV_LISTS ; j++ )
        trivopts [ j ] = TRUE ;
      for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
        titleInfoOpts [ j ] = TRUE ;
      options . akaopt = TRUE ;
      options . mrropt = TRUE ;
      options . yropt = TRUE ;
      options . chopt = TRUE ;
      options . plotopt = TRUE ;
      options . businessopt = TRUE ;
      options . ldopt = TRUE ;
      options . litopt = TRUE ;
      options . linkopt = TRUE ;
    }
    else
    {
      (void) fprintf ( stderr, "lguide: unrecognised option %s\n", argv[i] ) ;
      err = TRUE ;
    }

  if ( fname [ 0 ] == '\0' )
  {
    (void) fprintf ( stderr, "lguide: no input file specified\n" ) ;
    err = TRUE ;
  }
  else
    if ( !err )
      tchain = addTitleFile ( fname ) ;
 
  if ( !err && tchain == NULL )
  {
    (void) fprintf ( stderr, "lguide: no titles found\n" ) ;
    err = TRUE ;
  }

  if ( err )
    moviedbUsage ( LGUIDE_USAGE1, NULL, NULL, NULL, NULL, NULL ) ;

  addTitleChainOpts ( tchain, options ) ;
  addTitleChainTrivOpts ( tchain, trivopts ) ;
  addTitleChainTitleInfoOpts ( tchain, titleInfoOpts ) ;
  processTitleSearch ( tchain ) ;
  reportUnmatches ( tchain ) ;
  displayTitleSearchResults ( tchain, TRUE ) ; 
  freeTitleChain ( tchain ) ;
  return ( 0 ) ;
}

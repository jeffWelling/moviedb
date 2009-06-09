/*============================================================================
 *
 *  Program: title.c 
 *
 *  Version: 3.7
 *
 *  Purpose: search databases for titles
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
 *    -t <title>  search for details on title
 *    -s          treat title as substring and return all matches
 *    -i          case-sensitive search for -s
 *   
 *    -yr         display year of release
 *    -mrr        display movie ratings report info
 *    -aka        search alternative title database
 *    -pl         add plot description
 *    -lit        add literature 
 *    -bus        add business 
 *    -ld         add laserdisc
 *    -trivia     add any trivia, crazy credits, quotes and goofs
 *
 *    -full       short hand for -yr -mrr -pl -trivia -lit -bus -ld
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

#define TITLE_USAGE1 "usage: title [-t <title>]+ [-i -s]"
#define TITLE_USAGE2 "             [-aka -mrr -yr -pl -trivia -lit -bus -ld] [-full]"

int main ( int argc, char **argv )
{
  int   err = FALSE ;
  int   i, j ;
  int   fullopt = FALSE ;
  int   tidy = FALSE ;
  int   trivopts [ NO_OF_TRIV_LISTS ] ;
  int   titleInfoOpts [ NO_OF_TITLE_INFO_LISTS ] ;
  struct titleSearchRec *combined = NULL, *tchain = NULL ;
  struct titleSearchOptRec options = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;

  logProgram ( argv [ 0 ] ) ;
  options . yearMatch = TRUE ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    trivopts [ i ] = FALSE ;

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    titleInfoOpts [ i ] = FALSE ;

  for ( i = 1; i < argc; i++ )
    if ( strcmp ( "-t", argv[i] ) == 0 )
      if ( ++i < argc )
        tchain = addTitleSearchRec ( tchain, argv [ i ] ) ;
      else
        err = TRUE ;
    else if ( strcmp ( "-i", argv [ i ] ) == 0 )
      options . casesen = TRUE ;
    else if ( strcmp ( "-pl", argv [ i ] ) == 0 )
    {
      options . plotopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-bus", argv [ i ] ) == 0 )
    {
      options . businessopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-ld", argv [ i ] ) == 0 )
    {
      options . ldopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-nold", argv [ i ] ) == 0 )
    {
      options . ldopt = FALSE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-lit", argv [ i ] ) == 0 )
    {
      options . litopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-ym", argv [ i ] ) == 0 )
    {
      options . yearMatch = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-exact", argv [ i ] ) == 0 )
      options . yearMatch = FALSE ;
    else if ( strcmp ( "-s", argv [ i ] ) == 0 )
    {
      options . substring = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-mrr", argv [ i ] ) == 0 )
    {
      options . mrropt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-yr", argv [ i ] ) == 0 )
    {
      options . yropt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-trivia", argv [ i ] ) == 0 )
    {
      for ( j = 0 ; j < NO_OF_TRIV_LISTS ; j++ )
        trivopts [ j ] = TRUE ;
      for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
        titleInfoOpts [ j ] = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-aka", argv [ i ] ) == 0 )
    {
      options . akaopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-link", argv [ i ] ) == 0 )
    {
      options . linkopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-full", argv [ i ] ) == 0 )
    {
      for ( j = 0 ; j < NO_OF_TRIV_LISTS ; j++ )
        trivopts [ j ] = TRUE ;
      for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
        titleInfoOpts [ j ] = TRUE ;
      options . mrropt = TRUE ;
      options . yropt = TRUE ;
      options . plotopt = TRUE ;
      options . businessopt = TRUE ;
      options . ldopt = TRUE ;
      options . litopt = TRUE ;
      options . akaopt = TRUE ;
      options . linkopt = TRUE ;
      tidy = TRUE ;
      fullopt = TRUE ;
    }
    else
    {
      (void) fprintf ( stderr, "title: unrecognised option %s\n", argv[i] ) ;
      err = TRUE ;
    }

  if ( err || tchain == NULL )
    moviedbUsage ( TITLE_USAGE1, TITLE_USAGE2, NULL, NULL, NULL, NULL ) ;

  if ( tchain -> next != NULL )
  {
    if ( options . substring )
      moviedbError ("title: -s not available with more than one search" ) ;
    else if ( fullopt )
      moviedbError ( "title: -full not available with more than one search" ) ;
    else if ( options . mrropt )
      moviedbError ( "title: -mrr not available with more than one search" ) ;
    else if ( options . yropt )
      moviedbError ( "title: -yr not available with more than one search" ) ;
    else if ( options . plotopt )
      moviedbError ( "title: -pl not available with more than one search" ) ;
    else if ( options . businessopt )
      moviedbError ( "title: -bus not available with more than one search" ) ;
    else if ( options . ldopt )
      moviedbError ( "title: -ld not available with more than one search" ) ;
    else if ( options . litopt )
      moviedbError ( "title: -lit not available with more than one search" ) ;
    else if ( options . akaopt )
      moviedbError( "title: -aka not available with more than one search" ) ;
    else if ( trivopts [ 0 ] )
      moviedbError( "title: -trivia not available with more than one search" ) ;
    else if ( titleInfoOpts [ 0 ] )
      moviedbError( "title: -info not available with more than one search" ) ;
  }

  if ( tchain != NULL && tchain -> next != NULL )
  {
    addTitleChainOpts ( tchain, options ) ;
    addTitleChainTrivOpts ( tchain, trivopts ) ;
    addTitleChainTitleInfoOpts ( tchain, titleInfoOpts ) ;
    processTitleSearch ( tchain ) ;
    combined = combineTitleResults ( tchain ) ; 
    displayTitleSearchResults ( combined, FALSE ) ; 
    freeTitleChain ( combined ) ; 
  }
  else 
  { 
    addTitleChainOpts ( tchain, options ) ;
    addTitleChainTrivOpts ( tchain, trivopts ) ;
    addTitleChainTitleInfoOpts ( tchain, titleInfoOpts ) ;
    processTitleSearch ( tchain ) ;
    displayTitleMatches ( tchain ) ;
    displayTitleSearchResults ( tchain, tidy ) ; 
  } 

  freeTitleChain ( tchain ) ;

  return ( 0 ) ;
}

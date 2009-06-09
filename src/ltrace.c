/*============================================================================
 *
 *  Program: ltrace.c 
 *
 *  Version: 3.7
 *
 *  Purpose: search list databases for full details of a persons movies
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
 *    -ar <name>    search actors database
 *    -as <name>    search actress database 
 *    -a <name>     search both cast databases
 *    -d <name>     search directors database
 *    -w <name>     search writers database
 *    -c <name>     search composers database
 *    -ph <name>    search cinematographers database
 *    -ed <name>    search editors database
 *    -pd <name>    search production designers database
 *    -cd <name>    search costume designers database
 *    -pr <name>    search producers database
 *    -misc <name>  search miscellaneous database
 *    -name <name>  search all databases
 *
 *    -mrr        add movie ratings report info to results
 *    -yr         add year of release info & sort chronologically
 *    -chr        add character name info (if available)
 *    -aka        add aka info
 *    -trivia     add trivia, crazy credits, goofs and quotes info
 *    -pl         add plot summaries
 *    -full       combines all options in this group
 *
 *    -m          movies only, ignore TV-series
 *
 *============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "log.h"
#include "moviedb.h"
#include "dbutils.h"
#include "filmography.h"
#include "titlesearch.h"
#include "display.h"

#define LTRACE_USAGE1 "usage: ltrace -cast|-acr|-acs|-dir|-write|-comp|-cine|-edit|-prodes|-costdes|"
#define LTRACE_USAGE2 "              -prdcr|-misc|-name <name>] [-mrr -aka -pl -chr -yr -trivia]"
#define LTRACE_USAGE3 "              [-full] [-m]"

int main ( int argc, char **argv )
{
  struct nameSearchRec  *nrec = NULL ;
  struct titleSearchRec *tchain = NULL ;
  struct nameSearchOptRec  noptions = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
  struct titleSearchOptRec  toptions = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
  int    trivopts [ NO_OF_TRIV_LISTS ] ;
  int    titleInfoOpts [ NO_OF_TITLE_INFO_LISTS ] ;
  int    i, j, okopt, err = FALSE ;

  logProgram ( argv [ 0 ] ) ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    trivopts [ i ] = FALSE ;

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    titleInfoOpts [ i ] = FALSE ;

  for ( i = 1; i < argc; i++ )
    if ( strcmp ( "-m", argv[i] ) == 0 )
      noptions . mvsonly = TRUE ;
    else if ( strcmp ( "-yr", argv[i] ) == 0 )
    {
      toptions . yropt = YR ;
      noptions . yropt = YR ;
    }
    else if ( strcmp ( "-aka", argv[i] ) == 0 )
    {
      toptions . akaopt = TRUE ;
      noptions . akaopt = TRUE ;
    }
    else if ( strcmp ( "-chr", argv[i] ) == 0 )
    {
      toptions . chopt = TRUE ;
      noptions . chopt = TRUE ;
    }
    else if ( strcmp ( "-mrr", argv[i] ) == 0 )
      toptions . mrropt = MRR ;
    else if ( strcmp ( "-pl", argv[i] ) == 0 )
      toptions . plotopt = TRUE ;
    else if ( strcmp ( "-bus", argv[i] ) == 0 )
      toptions . businessopt = TRUE ;
    else if ( strcmp ( "-ld", argv[i] ) == 0 )
      toptions . ldopt = TRUE ;
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
      toptions . yropt = TRUE ;
      toptions . chopt = TRUE ;
      toptions . mrropt = TRUE ;
      toptions . akaopt = TRUE ;
      toptions . plotopt = TRUE ;
      toptions . businessopt = TRUE ;
      toptions . ldopt = TRUE ;
      toptions . litopt = TRUE ;
      toptions . linkopt = TRUE ;
      noptions . yropt = YR ;
      noptions . akaopt = TRUE ;
      noptions . chopt = TRUE ;
      noptions . biopt = TRUE ;
    }
    else
    {
      okopt = FALSE ;
      for ( j = 0 ; j < NO_OF_SEARCH_OPTS ; j++ )
	if ( strcmp ( filmographyOptions [ j ] . option, argv [ i ] ) == 0 || strcmp ( filmographyOptions [ j ] . oldOption, argv [ i ] ) == 0 )
        {
	  okopt = TRUE ;
          if ( ++i < argc )
          {
            nrec = addNameSearchRec ( nrec, argv [ i ], j ) ;
            if ( j == GLOBAL_NAME_SEARCH )
              noptions . searchall = TRUE ;
          }
          else
          {
            err = TRUE ;
	    break ;
          }
	}
      if ( ! okopt )
      {
         (void) fprintf ( stderr, "ltrace: unrecognised option %s\n", argv[i] ) ;
         err = TRUE ;
      }
    }

  if ( err || nrec == NULL )
    moviedbUsage ( LTRACE_USAGE1, LTRACE_USAGE2, LTRACE_USAGE3, NULL, NULL, NULL ) ;

  if ( nrec -> next != NULL )
    moviedbError ( "ltrace: only one name allowed" ) ;

  addNameChainOpts ( nrec, noptions ) ;
  processFilmographySearch ( nrec ) ;
  sortListResults ( nrec ) ;
  tchain = makeTitleChain ( nrec, toptions, trivopts, noptions, titleInfoOpts ) ;
  if ( tchain != NULL )
  {
    processTitleSearch ( tchain ) ;
    displayNameSearchResults ( nrec, TRUE ) ;
    displayTitleSearchResults ( tchain, TRUE ) ;
    freeTitleChain ( tchain ) ; 
  }
  freeNameSearchChain ( nrec ) ; 

  return ( 0 ) ;
}

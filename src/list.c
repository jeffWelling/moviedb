/*============================================================================
 *
 *  Program: list.c 
 *
 *  Version: 3.7
 *
 *  Purpose: search databases for filmographies
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
 *    -cast <name>  search both cast databases
 *     -acr <name>  search actors databases
 *     -acs <name>  search actress databases
 *     -dir <name>  search directors database
 *   -write <name>  search writers database
 *    -comp <name>  search composers database
 *    -cine <name>  search cinematographers database
 *    -edit <name>  search editors database
 *  -prodes <name>  search production designers database
 * -costdes <name>  search costume designers database
 *   -prdcr <name>  search producers database
 *    -misc <name>  search miscellaneous database
 *    -name <name>  search all databases
 *
 *    -mrr        add movie ratings report info to results
 *    -smrr       add movie ratings report info & sort by ratings
 *    -vmrr       add movie ratings report info & sort by votes
 *
 *    -yru        add year of release info
 *    -yr         add year of release info & sort chronologically
 *
 *    -chr        add character name info (if available)
 *    -bio        add data from biographies database
 *    -aka        add aka data for each title
 *    -full       shorthand for -bio -yr -chr -aka
 *
 *    -m          movies only, ignore TV-series
 *    -s          treat the name search as a substring and return all matches
 *    -i          case sensistive search for substring
 *
 *    -1..-9      use approximate matching, max difference in range 1 to 9
 *
 *   -genre <str>  limit search by genre
 *   -keyword <str> limit search by keyword
 *    -cert <str>  limit search by certificate
 *    -time <str>  limit search by running time
 *  -prodco <str>  limit search by production company
 *   -color <str>  limit search by color information
 *     -mix <str>  limit search by sound mix
 *   -cntry <str>  limit search by country of production
 *     -rel <str>  limit search by release date
 *     -loc <str>  limit search by location
 *    -tech <str>  limit search by technical info
 *    -lang <str>  limit search by language info
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
#include "display.h"

#define LIST_USAGE1 "usage: list [-cast|-acr|-acs|-dir|-write|-comp|-cine|-edit|-prodes|-costdes|"
#define LIST_USAGE2 "            -prdcr|-misc|-name <name>]+ [-full] [-mrr|-smrr|-vmrr] [-yr|-yru]"
#define LIST_USAGE3 "            [-chr] [-bio] [-aka] [-m -s -i] [-s|-1|-2|-3|-4|-5|-6|-7|-8|-9]"
#define LIST_USAGE4 "            [-genre <s>] [-keyword <s>] [-time <s>] [-prodco <s>] [-cert <s>]"
#define LIST_USAGE5 "            [-mix <str>] [-cntry <s>] [-loc <s>] [-rel <s>]"
#define LIST_USAGE6 "            [-tech <s>] [-lang <s>]"

int main ( int argc, char **argv )
{
  struct nameSearchRec *combined = NULL, *nchain = NULL ;
  struct nameSearchOptRec options = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
  struct searchConstraints constraints ;
  int    err = FALSE ;
  int    i, j, okopt ;
  int    tidy = FALSE ;
  int    fullopt = FALSE ;

  logProgram ( argv [ 0 ] ) ;
  initialiseConstraints ( &constraints ) ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 1 ; i < argc ; i++ )
    if ( strcmp ( "-i", argv [ i ] ) == 0 )
      options . casesen = TRUE ;
    else if ( strcmp ( "-m", argv [ i ] ) == 0 )
      options . mvsonly = TRUE ;
    else if ( strcmp ( "-aka", argv [ i ] ) == 0 )
      options . akaopt = TRUE ;
    else if ( strcmp ( "-mrr", argv [ i ] ) == 0 )
      options . mrropt = MRR ;
    else if ( strcmp ( "-smrr", argv [ i ] ) == 0 )
      options . mrropt = SMRR ;
    else if ( strcmp ( "-vmrr", argv [ i ] ) == 0 )
      options . mrropt = VMRR ;
    else if ( strcmp ( "-yr", argv [ i ] ) == 0 )
      options . yropt = YR ;
    else if ( strcmp ( "-yru", argv [ i ] ) == 0 )
      options . yropt = YRU ;
    else if ( strcmp ( "-chr", argv [ i ] ) == 0 )
      options . chopt = TRUE ;
    else if ( strcmp ( "-s", argv [ i ] ) == 0 )
    {
      options . substring = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-bio", argv [ i ] ) == 0 )
    {
      options . biopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( strcmp ( "-full", argv [ i ] ) == 0 )
    {
      options . yropt = YR ;
      options . biopt = TRUE ;
      options . chopt = TRUE ;
      options . akaopt = TRUE ;
      fullopt = TRUE ;
      tidy = TRUE ;
    }
    else if ( ( strlen ( argv [ i ] ) == 2 ) && ( argv [ i ] [ 1 ] >= '1' ) && ( argv [ i ] [ 1 ] <= '9' ) )
      options . approx = argv [ i ] [ 1 ] - '0' ;
    else
    {
      okopt = FALSE ;
      for ( j = 0 ; j < NO_OF_SEARCH_OPTS ; j++ )
	if ( strcmp ( filmographyOptions [ j ] . option, argv [ i ] ) == 0 || strcmp ( filmographyOptions [ j ] . oldOption, argv [ i ] ) == 0 )
        {
	  okopt = TRUE ;
          if ( ++i < argc )
          {
            nchain = addNameSearchRec ( nchain, argv [ i ], j ) ;
            if ( j == GLOBAL_NAME_SEARCH )
              tidy = TRUE ;
          }
          else
          {
            err = TRUE ;
	    break ;
          }
	}
      if ( ! okopt )
      {
        for ( j = 0 ; j < NO_OF_TITLE_INFO_LISTS ; j++ )
          if ( strcmp ( titleInfoDefs [ j ] . option, argv [ i ] ) == 0 )
          {
            okopt = TRUE ;
            if ( ++i < argc )
              (void) strcpy ( constraints . titleInfoString [ j ], argv[i] ) ;
            else
              err = TRUE ;
          }
      }
      if ( ! okopt )
      {
         (void) fprintf ( stderr, "list: unrecognised option %s\n", argv[i] ) ;
         err = TRUE ;
      }
    }

  if ( err || nchain == NULL )
    moviedbUsage ( LIST_USAGE1, LIST_USAGE2, LIST_USAGE3, LIST_USAGE4, LIST_USAGE5, LIST_USAGE6 ) ;

  if ( nchain -> next != NULL )
  {
    if ( options . substring )
      moviedbError ( "list: -s not available with more than one name search" ) ;
    else if ( fullopt )
      moviedbError ( "list: -full not available with more than one name search" ) ;
    else if ( options . biopt )
      moviedbError ( "list: -bio not available with more than one name search" ) ;
    else if ( options . chopt )
      moviedbError ( "list: -chr not available with more than one name search" ) ;
  }

  if ( options . approx && options . substring )
    moviedbError ( "list: -s not available with approximate matching" ) ;

  if ( nchain != NULL && nchain -> next != NULL )
  {
    addNameChainOpts ( nchain, options ) ;
    processFilmographySearch ( nchain ) ;
    constrainFilmographySearch ( &constraints, nchain ) ;
    combined = combineListResults ( nchain ) ;
    if ( combined != NULL )
    {
      sortListResults ( combined ) ;
      displayNameSearchResults ( combined, FALSE ) ;
      freeNameSearchChain ( combined ) ; 
    }
  }
  else
  { 
    addNameChainOpts ( nchain, options ) ;
    processFilmographySearch ( nchain ) ;
    constrainFilmographySearch ( &constraints, nchain ) ;
    sortListResults ( nchain ) ;
    displayNameSearchResults ( nchain, tidy ) ;
  } 

  freeNameSearchChain ( nchain ) ; 

  return ( 0 ) ;
}

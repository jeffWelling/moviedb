/*============================================================================
 *
 *  Program: mkballot.c 
 *
 *  Version: 3.7
 *
 *  Purpose: generate movie ratings ballots 
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
 *       -a <name>  search both cast databases
 *      -ar <name>  search actors databases
 *      -as <name>  search actress databases
 *       -d <name>  search directors database
 *       -w <name>  search writers database
 *       -c <name>  search composers database
 *      -ph <name>  search cinematographers database
 *      -ed <name>  search editors database
 *      -pd <name>  search production designers database
 *      -cd <name>  search costume designers database
 *      -pr <name>  search producers database
 *    -misc <name>  search miscellaneous database
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

#define MKBALLOT_USAGE1 "usage: mkballot -cast|-acr|-acs|-dir|-write|-comp|-cine|-edit|-prodes|-costdes|"
#define MKBALLOT_USAGE2 "                -prdcr|-misc <name>"

int main ( int argc, char **argv )
{
  struct nameSearchRec *nchain = NULL ;
  struct nameSearchOptRec options = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
  int    err = FALSE ;
  int    i, j, okopt ;

  logProgram ( argv [ 0 ] ) ;

  if ( argc == 1 )
    err = TRUE ;

  for ( i = 1 ; i < argc ; i++ )
  {
    okopt = FALSE ;
    for ( j = 0 ; j < NO_OF_SEARCH_OPTS - 1 ; j++ )
      if ( strcmp ( filmographyOptions [ j ] . option, argv [ i ] ) == 0 )
      {
        okopt = TRUE ;
        if ( ++i < argc )
          nchain = addNameSearchRec ( nchain, argv [ i ], j ) ;
        else
        {
          err = TRUE ;
          break ;
        }
      }
    if ( ! okopt )
    {
       (void) fprintf ( stderr, "mkballot: unrecognised option %s\n", argv[i] ) ;
       err = TRUE ;
    }
  }

  if ( err || nchain == NULL )
    moviedbUsage ( MKBALLOT_USAGE1, MKBALLOT_USAGE2, NULL, NULL, NULL, NULL ) ;

  if ( nchain -> next != NULL )
    moviedbError ( "mkballot: only one name allowed" ) ;

  addNameChainOpts ( nchain, options ) ;
  processFilmographySearch ( nchain ) ;
  sortListResults ( nchain ) ;
  displayNameSearchResultsAsBallot ( nchain ) ;
  freeNameSearchChain ( nchain ) ; 
  return ( 0 ) ;
}

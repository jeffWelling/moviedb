/*============================================================================ 
 * 
 *  Program: listall.c  
 * 
 *  Version: 3.7
 * 
 *  Purpose: search list databases 
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
 *    -aka        check for alternative title 
 *    
 *    -yr         display year of release 
 *    -yru        display year of release, unsorted 
 * 
 *    -mrr        display movie ratings report info 
 *    -smrr       display movie ratings report info, sorted by ratings 
 *    -vmrr       display movie ratings report info, sorted by votes 
 * 
 *    -chr        add character names 
 *    -bio        add biography data
 *    -full       combination of -yr -aka -chr -bio
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
#include "filmography.h" 
#include "display.h" 
 
#define LISTALL_USAGE1 "usage: listall -t <title> [-m] [-mrr|-smrr|-vmrr] [-yr|-yru]" 
#define LISTALL_USAGE2 "               [-aka] [-bio] [-chr] [-full]" 
 
int main ( int argc, char **argv ) 
{ 
  struct titleSearchRec *trec = NULL ; 
  struct titleSearchOptRec toptions = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
  struct nameSearchRec  *nchain = NULL ; 
  struct nameSearchOptRec noptions = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 } ;
  int   err = FALSE ;
  int   i, j ;
  int   trivopts [ NO_OF_TRIV_LISTS ] ;
 
  logProgram ( argv [ 0 ] ) ; 
  toptions . yearMatch = TRUE ;

  if ( argc == 1 ) 
    err = TRUE ; 

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    trivopts [ i ] = FALSE ;
 
  for ( i = 1 ; i < argc; i++ ) 
    if ( strcmp ( "-t", argv[i] ) == 0 ) 
      if ( ++i < argc ) 
        trec = addTitleSearchRec ( trec, argv [ i ] ) ; 
      else 
        err = TRUE ; 
    else if ( strcmp ( "-m", argv [ i ] ) == 0 ) 
      noptions . mvsonly = TRUE ; 
    else if ( strcmp ( "-aka", argv [ i ] ) == 0 ) 
    {
      noptions . akaopt = TRUE ; 
      toptions . akaopt = TRUE ; 
    }
    else if ( strcmp ( "-exact", argv [ i ] ) == 0 )
      toptions . yearMatch = FALSE ;
    else if ( strcmp ( "-mrr", argv [ i ] ) == 0 ) 
      noptions . mrropt = MRR ; 
    else if ( strcmp ( "-smrr", argv [ i ] ) == 0 ) 
      noptions . mrropt = SMRR ; 
    else if ( strcmp ( "-vmrr", argv [ i ] ) == 0 ) 
      noptions . mrropt = VMRR ; 
    else if ( strcmp ( "-yr", argv [ i ] ) == 0 ) 
      noptions . yropt = YR ; 
    else if ( strcmp ( "-yru", argv [ i ] ) == 0 ) 
      noptions . yropt = YRU ; 
    else if ( strcmp ( "-chr", argv [ i ] ) == 0 ) 
      noptions . chopt = TRUE ; 
    else if ( strcmp ( "-full", argv [ i ] ) == 0 ) 
    {
      noptions . biopt = TRUE ;
      noptions . chopt = TRUE ;
      noptions . akaopt = TRUE ;
      noptions . yropt = YR ; 
      toptions . akaopt = TRUE ;
      toptions . yropt = TRUE ;
      toptions . mrropt = TRUE ;
      toptions . plotopt = TRUE ;
      toptions . businessopt = TRUE ;
      toptions . ldopt = TRUE ;
      toptions . chopt = TRUE ;
      toptions . litopt = TRUE ;
      toptions . linkopt = TRUE ;
      for ( j = 0 ; j < NO_OF_TRIV_LISTS ; j++ )
        trivopts [ j ] = TRUE ;
    }
    else
    { 
       (void) fprintf ( stderr, "listall: unrecognised option %s\n", argv[i] ) ; 
       err = TRUE ; 
    } 
 
  if ( err || trec == NULL ) 
    moviedbUsage ( LISTALL_USAGE1, LISTALL_USAGE2, NULL, NULL, NULL, NULL ) ;
 
  if ( trec -> next != NULL ) 
    moviedbError ( "listall: only one title allowed" ) ; 
 
  addTitleChainOpts ( trec, toptions ) ;
  addTitleChainTrivOpts ( trec, trivopts ) ;
  processTitleSearch ( trec ) ;
  if ( trec -> next != NULL )
    displayListallTitleMatches ( trec ) ;
  else
  {
    nchain = makeNameChain ( trec ) ; 
    if ( nchain != NULL )
    {
      addNameChainOpts ( nchain, noptions ) ;
      processFilmographySearch ( nchain ) ;
      sortListResults ( nchain ) ;
      displayTitleSearchResults ( trec, TRUE ) ;
      displayNameSearchResults ( nchain, TRUE ) ;
      freeNameSearchChain ( nchain ) ; 
    }
  }
  freeTitleChain ( trec ) ; 
  return ( 0 ) ; 
} 

/*============================================================================
 *
 *  Program: aka.c 
 *
 *  Version: 3.3
 *
 *  Purpose: procedures to support the aka database
 *
 *  Author:  C J Needham <col@imdb.com>
 *
 *  Copyright (c) 1996 The Internet Movie Database Inc
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
#include "moviedb.h"
#include "dbutils.h"

struct akaRec *cloneAkaData ( struct akaRec *aka )
{
  struct akaRec *arec, *tail = NULL, *akadata, *res = NULL ;

  if ( aka == NULL )
    return ( NULL ) ;

  for ( arec = aka ; arec != NULL ; arec = arec -> next )
  {
    akadata = newAkaRec ( ) ;
    if ( res == NULL )
      res = akadata ;
    else
      tail -> next = akadata ;
    tail = akadata ;
    akadata -> akaTitle = duplicateString ( arec -> akaTitle ) ;
    akadata -> akaAttr = duplicateString ( arec -> akaAttr ) ;
    akadata -> next = NULL ;
  }
  return ( res ) ;
}


struct akaRec *readAkaSearchResults ( FILE *dbFp, TitleID titleKey )
{
  struct akaRec *head = NULL, *tail = NULL ;
  TitleID dbKey = titleKey ;

  dbKey = getTitle ( dbFp ) ;
  while ( titleKey == dbKey && !feof ( dbFp ) ) 
  {
    if ( head == NULL )
    {
      head = newAkaRec ( ) ;
      tail = head ;
    }
    else
    {
      tail -> next = newAkaRec ( ) ;
      tail = tail -> next ;
    }
    tail -> akaKey = getTitle ( dbFp ) ;
    tail -> akaAttrKey = getAttr ( dbFp ) ;
    tail -> akaTitle = NULL ;
    tail -> akaAttr = NULL ;
    tail -> next = NULL ;
    dbKey = getTitle ( dbFp ) ;
  }
  return ( head ) ;
}


void addAkaToNameSearch (struct nameSearchRec *chain)
{
  FILE  *dbFp ;
  struct nameSearchRec *nrec ;
  long mid = 0, lower, upper, saveUpper ;
  int found = FALSE ;
  int i, listId ;
  TitleID titleKey, dbKey = NOTITLE ;

  for ( nrec = chain ; !found && nrec != NULL ; nrec = nrec -> next )
    found = nrec -> searchparams. akaopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( AKADB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / AKABYTES ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    for ( listId = 0 ; listId < NO_OF_FILMOGRAPHY_LISTS ; listId++ )
      if ( nrec -> lists [ listId ] != NULL )
        for ( i = 0 ; i < nrec -> lists [ listId ] -> count ; i++ )
        {
          titleKey = nrec -> lists [ listId ] -> entries [ i ] . titleKey ;
          lower = 0 ;
          upper = saveUpper ;
          found = FALSE ;
          while ( !found && upper >= lower )
          {
             mid = ( upper + lower ) / 2 ;
             (void) fseek ( dbFp, mid * AKABYTES, SEEK_SET ) ;
             dbKey = getTitle ( dbFp ) ;
             if ( titleKey == dbKey )
               found = TRUE ;
            else if ( dbKey < titleKey )
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
              while ( titleKey == dbKey )
              {
                if ( --mid >= 0 ) 
                {
                  (void) fseek ( dbFp, mid * AKABYTES, SEEK_SET ) ;
                  dbKey = getTitle ( dbFp ) ;
                }
                else
                  break ;
              }
              if ( mid >= 0 )
              {
                (void) getTitle ( dbFp ) ;
                (void) getAttr ( dbFp ) ;
              }
              else
                (void) fseek ( dbFp, 0, SEEK_SET ) ;
            }
            nrec -> lists [ listId ] -> entries [ i ] . aka = readAkaSearchResults ( dbFp, titleKey ) ;
          }
        }
  }
  (void) fclose ( dbFp ) ;
}


void addAkaToTitleSearch (struct titleSearchRec *tchain)
{
  FILE  *dbFp ;
  struct titleSearchRec *trec ;
  long mid = 0, lower, upper, saveUpper ;
  int found = FALSE ;
  TitleID titleKey, dbKey = NOTITLE ;

  for ( trec = tchain ; !found && trec != NULL ; trec = trec -> next )
    found = trec -> searchparams. akaopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( AKADB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / AKABYTES ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
  {
    titleKey = trec -> titleKey ;
    lower = 0 ;
    upper = saveUpper ;
    found = FALSE ;
    while ( !found && upper >= lower )
    {
      mid = ( upper + lower ) / 2 ;
      (void) fseek ( dbFp, mid * AKABYTES, SEEK_SET ) ;
      dbKey = getTitle ( dbFp ) ;
      if ( titleKey == dbKey )
        found = TRUE ;
      else if ( dbKey < titleKey )
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
        while ( titleKey == dbKey )
        {
          if ( --mid >= 0 ) 
          {
            (void) fseek ( dbFp, mid * AKABYTES, SEEK_SET ) ;
            dbKey = getTitle ( dbFp ) ;
          }
          else
            break ;
        }
        if ( mid >= 0 )
        {
          (void) getTitle ( dbFp ) ;
          (void) getAttr ( dbFp ) ;
        }
        else
          (void) fseek ( dbFp, 0, SEEK_SET ) ;
      }
      trec -> aka = readAkaSearchResults ( dbFp, titleKey ) ;
    }
  }
  (void) fclose ( dbFp ) ;
}


struct akaNameRec *readAkaNameSearchResults ( FILE *dbFp, NameID nameKey )
{
  struct akaNameRec *head = NULL, *tail = NULL ;
  NameID dbKey = nameKey ;

  dbKey = getName ( dbFp ) ;
  while ( nameKey == dbKey && !feof ( dbFp ) ) 
  {
    if ( head == NULL )
    {
      head = newAkaNameRec ( ) ;
      tail = head ;
    }
    else
    {
      tail -> next = newAkaNameRec ( ) ;
      tail = tail -> next ;
    }
    tail -> akaKey = getName ( dbFp ) ;
    tail -> akaName = NULL ;
    tail -> next = NULL ;
    dbKey = getName ( dbFp ) ;
  }
  return ( head ) ;
}


void addNameAkaToNameSearch (struct nameSearchRec *nchain)
{
  FILE  *dbFp ;
  struct nameSearchRec *nrec ;
  long mid = 0, lower, upper, saveUpper ;
  int found = FALSE ;
  NameID nameKey, dbKey = NONAME ;

  for ( nrec = nchain ; !found && nrec != NULL ; nrec = nrec -> next )
    found = nrec -> searchparams. akaopt ;

  if ( ! found )
    return ;

  dbFp = openFile ( NAKADB ) ;
  (void) fseek ( dbFp, 0, SEEK_END ) ; 
  saveUpper = ftell ( dbFp ) / NAKABYTES ;

  for ( nrec = nchain ; nrec != NULL ; nrec = nrec -> next )
  {
    nameKey = nrec -> nameKey ;
    lower = 0 ;
    upper = saveUpper ;
    found = FALSE ;
    while ( !found && upper >= lower )
    {
      mid = ( upper + lower ) / 2 ;
      (void) fseek ( dbFp, mid * NAKABYTES, SEEK_SET ) ;
      dbKey = getName ( dbFp ) ;
      if ( nameKey == dbKey )
        found = TRUE ;
      else if ( dbKey < nameKey )
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
        while ( nameKey == dbKey )
        {
          if ( --mid >= 0 ) 
          {
            (void) fseek ( dbFp, mid * NAKABYTES, SEEK_SET ) ;
            dbKey = getName ( dbFp ) ;
          }
          else
            break ;
        }
        if ( mid >= 0 )
          (void) getName ( dbFp ) ;
        else
          (void) fseek ( dbFp, 0, SEEK_SET ) ;
      }
      nrec -> aka = readAkaNameSearchResults ( dbFp, nameKey ) ;
    }
  }
  (void) fclose ( dbFp ) ;
}

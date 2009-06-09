/*============================================================================ 
 * 
 *  Program: display.c 
 * 
 *  Version: 3.23
 * 
 *  Purpose: display procedures
 * 
 *  Author:  C J Needham <col@imdb.com>      
 * 
 *  Copyright (c) 1990-2004 The Internet Movie Database Inc.
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
#include <ctype.h> 
#include "moviedb.h" 
#include "dbutils.h" 

void displayTitleMatches ( struct titleSearchRec *tchain )
{
  struct titleSearchRec *trec ;

  if ( tchain -> next != NULL ) 
  {
    (void) printf ( "------------------------------------------------------------------------------\n" ) ;
    (void) printf ( "Titles Matched:\n\n" ) ;
    for ( trec = tchain ; trec != NULL ; trec = trec -> next ) 
      (void) printf ( "  %s\n", trec -> title ) ;
    (void) printf ( "\n" ) ;
  }
}


void displayListallTitleMatches ( struct titleSearchRec *tchain )
{
  struct titleSearchRec *trec ;

  if ( tchain -> next != NULL ) 
  {
    (void) printf ( "------------------------------------------------------------------------------\n" ) ;
    (void) printf ( "Titles Matched:\n\n" ) ;
    for ( trec = tchain ; trec != NULL ; trec = trec -> next ) 
      (void) printf ( "  %s\n", trec -> title ) ;
    (void) printf ( "\nPlease choose one of the above titles and try again\n" ) ;
    (void) printf ( "------------------------------------------------------------------------------\n" ) ;
  }
}


void displayNameURL ( char *name ) 
{
   char url [ MXLINELEN ] ;
   char *ptr ;
   
   (void) strcpy ( url, "http://www.imdb.com/Name?" ) ;
   for ( ptr = url + strlen(url) ; *name != 0 ; ptr++, name++ )
   {
     if ( isalnum ( *name ) )
       *ptr = *name ;
     else
     {
       (void) sprintf ( ptr, "%%%2X", *name & 255 ) ;
       ptr = ptr + 2 ;
     }
   }
   *ptr = '\0' ;
   (void) printf ( "URL:\n  %s\n\n", url ) ;
}


void displayBioMiscRec ( struct bioMiscRec *rec )
{
  struct lineRec *line ;

  if ( rec -> list != NULL )
  {
    if ( rec -> label [ 0 ] == 'N' && rec -> label [ 1 ] == 'K' )
      (void) printf ( "Nickname:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'T' && rec -> label [ 1 ] == 'R' )
      (void) printf ( "Trivia:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'B' && rec -> label [ 1 ] == 'O' )
      (void) printf ( "Books:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'O' && rec -> label [ 1 ] == 'W' )
      (void) printf ( "Other Works:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'S' && rec -> label [ 1 ] == 'P' )
      (void) printf ( "Married:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'A' && rec -> label [ 1 ] == 'G' )
      (void) printf ( "Agent's Address:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'Q' && rec -> label [ 1 ] == 'U' )
      (void) printf ( "Quotes:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'P' && rec -> label [ 1 ] == 'B' )
      (void) printf ( "Place of Birth:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'H' && rec -> label [ 1 ] == 'T' )
      (void) printf ( "Height:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'B' && rec -> label [ 1 ] == 'T' )
      (void) printf ( "Biographical Movies:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'P' && rec -> label [ 1 ] == 'I' )
      (void) printf ( "Portrayed in:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'G' && rec -> label [ 1 ] == 'A' )
      (void) printf ( "Guest Appearances:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'W' && rec -> label [ 1 ] == 'N' )
      (void) printf ( "Where Now:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'T' && rec -> label [ 1 ] == 'M' )
      (void) printf ( "Trademark:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'I' && rec -> label [ 1 ] == 'T' )
      (void) printf ( "Interviews:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'A' && rec -> label [ 1 ] == 'T' )
      (void) printf ( "Articles:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'S' && rec -> label [ 1 ] == 'A' )
      (void) printf ( "Salary History:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'C' && rec -> label [ 1 ] == 'V' )
      (void) printf ( "Magazine Covers:\n\n" ) ;
    else if ( rec -> label [ 0 ] == 'P' && rec -> label [ 1 ] == 'T' )
      (void) printf ( "Pictorials:\n\n" ) ;
    else
      (void) printf ( "%c%c:\n\n", rec -> label [ 0 ], rec -> label [ 1 ] ) ;
  }

  for ( line = rec -> list ; line != NULL ; line = line -> next )
    (void) printf ( "  %s", line -> text ) ;
  (void) printf ( "\n" ) ;

}


void displayBioRec ( struct bioRec *rec )
{
  char *justified ;

  if ( rec -> biog != NULL )
  {
    (void) printf ( "Biography:\n\n" ) ;

    justified = txt_f_justify ( 2, 70, rec -> biog ) ;
    if ( justified != NULL )
    {
      (void) printf ( "%s", justified ) ;
      free ( (void*) justified ) ;
    }
    if ( rec -> BY != NULL )
      (void) printf ( "Biography by:\n\n  %s\n", rec -> BY ) ;
  }
}


void displayBiography ( struct personRec *rec, char *name, int biopt, struct akaNameRec *aka )
{
  struct bioRec *brec ;
  struct bioMiscRec *miscptr ;
  struct akaNameRec *arec ;

  (void) printf ( "\n------------------------------------------------------------------------------\n" ) ;
  if ( name != NULL )
  {
    (void) printf ( "Name:\n\n  %s\n\n", name ) ;
    displayNameURL ( name ) ;
  }

  if (  aka != NULL )
  {
     (void) printf ( "Aka Names:\n\n" ) ;
     for ( arec = aka ; arec != NULL ; arec = arec -> next )
       (void) printf ( "  %s\n", arec -> akaName ) ;
     (void) printf ( "\n" ) ;
  }

  if ( rec != NULL )
  {
    if ( rec -> RN != NULL )
      (void) printf ( "Birth Name:\n\n  %s\n", rec -> RN ) ;

    if ( rec -> DB != NULL )
      (void) printf ( "Birth Date:\n\n  %s\n", rec -> DB ) ;

    if ( rec -> DD != NULL )
      (void) printf ( "Death Date:\n\n  %s\n", rec -> DD ) ;

    for ( brec = rec -> biography ; brec != NULL ; brec = brec -> next )
      displayBioRec ( brec ) ;

    for ( miscptr = rec -> misc ; miscptr != NULL ; miscptr = miscptr -> next )
      displayBioMiscRec ( miscptr ) ;
  }
}


void displayMovieLinks ( int noOfEntries, struct movieLinkRec *links ) 
{
  int i ;

  if ( noOfEntries > 0 )
  {
    (void) printf ( "Movie Links:\n" ) ;
    for ( i = 0 ; i < noOfEntries ; i++ )
      (void) printf ( "  (%s %s)\n", movieLinkDefs [ links [ i ] . link ] . string, links [ i ] . title ) ;
    (void) printf ( "\n" ) ;
  }
}


void displayOutline ( struct outlineRec *rec )
{
  char *justified ;

  if ( rec -> PL != NULL )
  {
    (void) printf ( "Summary:\n" ) ;
    justified = txt_f_justify ( 2, 70, rec -> PL ) ;
    if ( justified != NULL )
    {
      (void) printf ( "%s", justified ) ;
      free ( (void*) justified ) ;
    }
    if ( rec -> BY != NULL )
      (void) printf ( "Summary by:\n  %s\n", rec -> BY ) ;
  }
}


void displayPlot ( struct plotRec *rec )
{
  struct outlineRec *oline ;

  if ( rec != NULL )
  {
    for ( oline = rec -> outline ; oline != NULL ; oline = oline -> next )
      displayOutline ( oline ) ;

    if ( rec -> RV != NULL )
      (void) printf ( "Reviews (rec.arts.movies.reviews index):\n  %s\n", rec -> RV ) ;
  }
}


void displayLiterature ( struct lineRec *rec )
{
  struct lineRec *current ;
  char tag [ 5 ] ;

  if ( rec != NULL )
  {
    (void) printf ( "Literature:" ) ;
    tag [ 0 ] = '\0' ;
    for ( current = rec ; current != NULL ; current = current -> next )
    {
      if ( strncmp ( tag, current -> text, 4 ) != 0 && *(current -> text) != '\n' )
      {
        (void) strncpy ( tag, current -> text, 4 ) ;
        if ( strncmp ( current -> text, "SCRP", 4 ) == 0 )
          (void) printf ( "  Script:\n" ) ;
        else if ( strncmp ( current -> text, "NOVL", 4 ) == 0 )
          (void) printf ( "  Novel:\n" ) ;
        else if ( strncmp ( current -> text, "ADPT", 4 ) == 0 )
          (void) printf ( "  Adaptation:\n" ) ;
        else if ( strncmp ( current -> text, "BOOK", 4 ) == 0 )
          (void) printf ( "  Books:\n" ) ;
        else if ( strncmp ( current -> text, "PROT", 4 ) == 0 )
          (void) printf ( "  Production Protocol:\n" ) ;
        else if ( strncmp ( current -> text, "IVIW", 4 ) == 0 )
          (void) printf ( "  Interviews:\n" ) ;
        else if ( strncmp ( current -> text, "CRIT", 4 ) == 0 )
          (void) printf ( "  Critics:\n" ) ;
        else if ( strncmp ( current -> text, "ESSY", 4 ) == 0 )
          (void) printf ( "  Essays:\n" ) ;
        else if ( strncmp ( current -> text, "OTHR", 4 ) == 0 )
          (void) printf ( "  Other:\n" ) ;
        else
          (void) printf ( "  Unknown:\n" ) ;
      }
      if ( strlen ( current -> text ) < 6 )
        (void) printf ( "\n" ) ;
      else
        (void) printf ( "    %s", current -> text + 6 ) ;
    }
    (void) printf ( "\n" ) ;
  }
}


void displayBusiness ( struct lineRec *rec )
{
  struct lineRec *current ;
  char tag [ 3 ] ;

  if ( rec != NULL )
  {
    (void) printf ( "Business Info:" ) ;
    tag [ 0 ] = '\0' ;
    for ( current = rec ; current != NULL ; current = current -> next )
    {
      if ( strncmp ( tag, current -> text, 2 ) != 0 && *(current -> text) != '\n' )
      {
        (void) strncpy ( tag, current -> text, 2 ) ;
        if ( strncmp ( current -> text, "BT", 2 ) == 0 )
          (void) printf ( "  Budget:\n" ) ;
        else if ( strncmp ( current -> text, "GR", 2 ) == 0 )
          (void) printf ( "  Box Office Gross:\n" ) ;
        else if ( strncmp ( current -> text, "WG", 2 ) == 0 )
          (void) printf ( "  Weekend Box Office Gross:\n" ) ;
        else if ( strncmp ( current -> text, "OW", 2 ) == 0 )
          (void) printf ( "  Opening Weekend Box Office:\n" ) ;
        else if ( strncmp ( current -> text, "RT", 2 ) == 0 )
          (void) printf ( "  Rentals:\n" ) ;
        else if ( strncmp ( current -> text, "AD", 2 ) == 0 )
          (void) printf ( "  Admissions:\n" ) ;
        else if ( strncmp ( current -> text, "PD", 2 ) == 0 )
          (void) printf ( "  Production Dates:\n" ) ;
        else if ( strncmp ( current -> text, "SD", 2 ) == 0 )
          (void) printf ( "  Shooting Dates:\n" ) ;
        else if ( strncmp ( current -> text, "ST", 2 ) == 0 )
          (void) printf ( "  Studio:\n" ) ;
        else if ( strncmp ( current -> text, "CP", 2 ) == 0 )
          (void) printf ( "  Copyright Holder:\n" ) ;
        else
          (void) printf ( "  Unknown:\n" ) ;
      }
      if ( strlen ( current -> text ) < 4 )
        (void) printf ( "\n" ) ;
      else
        (void) printf ( "    %s", current -> text + 4 ) ;
    }
    (void) printf ( "\n" ) ;
  }
}


void displayLaserDisc ( struct laserRec *rec )
{
  struct laserRec *lrec ;
  struct lineRec *current ;
  char tag [ 5 ] ;

  if ( rec != NULL )
  {
    (void) printf ( "LaserDisc:\n" ) ;
    for ( lrec = rec ; lrec != NULL ; lrec = lrec -> next )
    {
      if ( lrec -> LN != NULL )
        (void) printf ( "  IMDb LaserDisc #:\n    %s", lrec -> LN ) ;
      if ( lrec -> LB != NULL )
        (void) printf ( "  Label:\n    %s", lrec -> LB ) ;
      if ( lrec -> CN != NULL )
        (void) printf ( "  Catalogue:\n    %s", lrec -> CN ) ;
      if ( lrec -> LT != NULL )
        (void) printf ( "  LaserDisc Title:\n    %s", lrec -> LT ) ;
      tag [ 0 ] = '\0' ;
      for ( current = lrec -> misc; current != NULL ; current = current -> next )
      {
        if ( strncmp ( tag, current -> text, 2 ) != 0 && *(current -> text) != '\n' )
        {
          (void) strncpy ( tag, current -> text, 2 ) ;
          if ( strncmp ( current -> text, "OT", 2 ) == 0 )
            (void) printf ( "  Featured Title(s):\n" ) ;
          else if ( strncmp ( current -> text, "PC", 2 ) == 0 )
            (void) printf ( "  Production Country:\n" ) ;
          else if ( strncmp ( current -> text, "YR", 2 ) == 0 )
            (void) printf ( "  Year:\n" ) ;
          else if ( strncmp ( current -> text, "CF", 2 ) == 0 )
            (void) printf ( "  Certification:\n" ) ;
          else if ( strncmp ( current -> text, "CA", 2 ) == 0 )
            (void) printf ( "  Category:\n" ) ;
          else if ( strncmp ( current -> text, "GR", 2 ) == 0 )
            (void) printf ( "  Group:\n" ) ;
          else if ( strncmp ( current -> text, "LA", 2 ) == 0 )
            (void) printf ( "  Language:\n" ) ;
          else if ( strncmp ( current -> text, "SU", 2 ) == 0 )
            (void) printf ( "  Subtitles:\n" ) ;
          else if ( strncmp ( current -> text, "LE", 2 ) == 0 )
            (void) printf ( "  Length:\n" ) ;
          else if ( strncmp ( current -> text, "RD", 2 ) == 0 )
            (void) printf ( "  Release Date:\n" ) ;
          else if ( strncmp ( current -> text, "ST", 2 ) == 0 )
            (void) printf ( "  Status:\n" ) ;
          else if ( strncmp ( current -> text, "PR", 2 ) == 0 )
            (void) printf ( "  Official Retail Price:\n" ) ;
          else if ( strncmp ( current -> text, "VS", 2 ) == 0 )
            (void) printf ( "  Video Standard:\n" ) ;
          else if ( strncmp ( current -> text, "CO", 2 ) == 0 )
            (void) printf ( "  Color Information:\n" ) ;
          else if ( strncmp ( current -> text, "SE", 2 ) == 0 )
            (void) printf ( "  Sound Encoding:\n" ) ;
          else if ( strncmp ( current -> text, "DS", 2 ) == 0 )
            (void) printf ( "  Digital Sound:\n" ) ;
          else if ( strncmp ( current -> text, "AL", 2 ) == 0 )
            (void) printf ( "  Analog Left:\n" ) ;
          else if ( strncmp ( current -> text, "AR", 2 ) == 0 )
            (void) printf ( "  Analog Right:\n" ) ;
          else if ( strncmp ( current -> text, "MF", 2 ) == 0 )
            (void) printf ( "  Master Format:\n" ) ;
          else if ( strncmp ( current -> text, "PP", 2 ) == 0 )
            (void) printf ( "  Pressing Plant:\n" ) ;
          else if ( strncmp ( current -> text, "SZ", 2 ) == 0 )
            (void) printf ( "  Disc Size:\n" ) ;
          else if ( strncmp ( current -> text, "SI", 2 ) == 0 )
            (void) printf ( "  Number of Sides:\n" ) ;
          else if ( strncmp ( current -> text, "DF", 2 ) == 0 )
            (void) printf ( "  Disc Format:\n" ) ;
          else if ( strncmp ( current -> text, "PF", 2 ) == 0 )
            (void) printf ( "  Picture Format:\n" ) ;
          else if ( strncmp ( current -> text, "AS", 2 ) == 0 )
            (void) printf ( "  Aspect Ratio:\n" ) ;
          else if ( strncmp ( current -> text, "CC", 2 ) == 0 )
            (void) printf ( "  Close Captions/Teletext/LD+G:\n" ) ;
          else if ( strncmp ( current -> text, "CS", 2 ) == 0 )
            (void) printf ( "  Number of Chapter Stops:\n" ) ;
          else if ( strncmp ( current -> text, "QP", 2 ) == 0 )
            (void) printf ( "  Quality Program:\n" ) ;
          else if ( strncmp ( current -> text, "TX", 2 ) == 0 )
            (void) printf ( "  THX:\n" ) ;
          else if ( strncmp ( current -> text, "IN", 2 ) == 0 )
            (void) printf ( "  Additional Information:\n" ) ;
          else if ( strncmp ( current -> text, "SL", 2 ) == 0 )
            (void) printf ( "  Supplements:\n" ) ;
          else if ( strncmp ( current -> text, "RV", 2 ) == 0 )
            (void) printf ( "  Review:\n" ) ;
          else if ( strncmp ( current -> text, "RC", 2 ) == 0 )
            (void) printf ( "  Release Country:\n" ) ;
          else
            (void) printf ( "  Unknown:\n" ) ;
        }
        if ( strlen ( current -> text ) < 4 )
          (void) printf ( "\n" ) ;
        else
          (void) printf ( "    %s", current -> text + 4 ) ;
      }
      (void) printf ( "\n\n" ) ;
    }
    (void) printf ( "\n" ) ;
  }
}


void displayAttributes ( char *attr, char *cname, int chopt )
{
  if ( attr == NULL )
    if ( cname == NULL || chopt == FALSE )
      (void) printf ( "\n" ) ;
    else
      (void) printf ( "  [%s]\n", cname ) ;
  else
    if ( cname == NULL || chopt == FALSE )
      (void) printf ( " %s\n", attr ) ;
    else
      (void) printf ( " %s  [%s]\n", attr, cname ) ;
}


void displayAkaTitles ( struct akaRec *arec, int tidy, int mrropt )
{
  struct akaRec *aka ;

  for ( aka = arec ; aka != NULL ; aka = aka -> next )
  {
    if ( mrropt )
      (void) printf ( "                              " ) ;
    if ( tidy )
      if ( aka -> akaAttr != NULL )
        (void) printf ( "    (aka %s) %s\n", aka -> akaTitle, aka -> akaAttr ) ;
      else
        (void) printf ( "    (aka %s)\n", aka -> akaTitle ) ;
    else
      if ( aka -> akaAttr != NULL )
        (void) printf ( "    (aka %s) %s\n", aka -> akaTitle, aka -> akaAttr ) ;
      else
        (void) printf ( "    (aka %s)\n", aka -> akaTitle ) ;
  }
}


void displayTitleAttrPairs ( struct listEntry lrec [ ], int count, int tidy, int mrropt, int mvsonly, int chopt )
{
   int i ;

   if ( count == 0 )
     return ;

   for ( i = 0 ; i < count ; i++ )
   {
     if ( lrec [ i ] . titleKey != NOTITLE )
       if ( !mvsonly || lrec [ i ] . title [ 0 ] != '"' )
       {
         if ( mrropt )
           if ( lrec [ i ] . mrr != NULL && lrec [ i ] . mrr -> votes > 0 )
             (void) printf ( "      %s%8d   %2.1f  ", lrec [ i ] . mrr -> distribution, lrec [ i ] . mrr -> votes, lrec [ i ] . mrr -> rating ) ;
           else
             (void) printf ( "                                " ) ;
         else
           if ( tidy )
             (void) printf ( "  " ) ;
  
         if ( lrec [ i ] . year > 0 )
           displayTitleYear ( lrec [ i ] . title, lrec [ i ] . year ) ;
         else
           (void) printf ( "%s", lrec [ i ] . title ) ;
  
         displayAttributes ( lrec [ i ] . attr, lrec [ i ] . cname, chopt ) ;
         displayAkaTitles ( lrec [ i ] . aka, tidy, mrropt ) ; 
       }
   }
}


void displayNameSearchResults ( struct nameSearchRec *chain, int tidy )
{
  int i ;
  struct nameSearchRec *nrec ;

  for ( nrec = chain ; nrec != NULL ; nrec = nrec -> next )
  {
    if ( nrec -> firstMatch >= 0 )
      if ( tidy )
      {
        displayBiography ( nrec -> biography, nrec -> name, nrec -> searchparams . biopt, nrec -> aka ) ;
        for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
          if ( nrec -> lists [ i ] != NULL && nrec -> lists [ i ] -> count > 0 )
          {
            (void) printf ( "Filmography as %s:\n\n", filmographyDefs [ i ] . filmography ) ;
            displayTitleAttrPairs ( nrec -> lists [ i ] -> entries, nrec -> lists [ i ] -> count, tidy, nrec -> searchparams . mrropt, nrec -> searchparams . mvsonly, nrec -> searchparams . chopt ) ;
            (void) printf ( "\n" ) ;
          }
        (void) printf ( "\n           Copyright 1990-2004 The Internet Movie Database Inc.\n" ) ;
        (void) printf ( "      Support the IMDb by visiting our web site: http://www.imdb.com/\n\n" ) ;
      }
      else
        displayTitleAttrPairs ( nrec -> lists [ nrec -> firstMatch ] -> entries, nrec -> lists [ nrec -> firstMatch ] -> count, tidy, nrec -> searchparams . mrropt, nrec -> searchparams . mvsonly, nrec -> searchparams . chopt ) ;
  }
}


void displayNameSearchResultsAsBallot ( struct nameSearchRec *nrec )
{
  int i, flag ;
  char ch ;
  char *ptr ;

  if ( nrec != NULL && nrec -> firstMatch >= 0 )
    for ( i = 0 ; i < nrec -> lists [ nrec -> firstMatch ] -> count ; i++ )
      if ( nrec -> lists [ nrec -> firstMatch ] -> entries [ i ] . title [ 0 ] != '"' ) 
      {
        (void) printf ( "vote     " ) ;
        ptr = nrec -> lists [ nrec -> firstMatch ] -> entries [ i ] . title ;
        flag = TRUE ;
        while ( ( ch = *ptr++) && flag )
          if ( ch == ' ' && *ptr == '(' && *(ptr+1) != '1' )
            flag = FALSE ;
          else
            if ( isupper ( ch ) )
              (void) printf ( "%c", tolower ( ch ) ) ;
            else
              (void) printf ( "%c", ch ) ;

        (void) printf ( "\n" ) ;
      }
}


void displayRawTitleAttrPairs ( struct listEntry lrec [ ], int count, char *name )
{
   int i ;

   if ( count == 0 )
     return ;

   for ( i = 0 ; i < count ; i++ )
     (void) printf ( "%s|%s|%s|%s\n", name, lrec [ i ] . title, lrec [ i ] . attr, lrec [ i ] . cname ) ;
}


void displayTrivia ( struct lineRec *triv, int listId ) 
{ 
  struct lineRec *line ; 
 
  if ( triv != NULL ) 
  {
    (void) printf ( "%s:", triviaDefs [ listId ] . desc ) ; 
 
    for ( line = triv ; line != NULL ; line = line -> next ) 
      if ( *(line -> text) == '-' )
        (void) printf ( "\n%s", line -> text ) ; 
      else
        (void) printf ( "%s", line -> text ) ; 
  }
} 
 

void displayTagLines ( struct lineRec *triv ) 
{ 
  struct lineRec *line ; 
  char *tag, *justified ;
 
  if ( triv != NULL ) 
  {
    (void) printf ( "Tag Line:\n" ) ; 

    stripEOL ( triv -> text ) ; 
    do 
    {
      tag = duplicateString ( triv -> text + 1 ) ;
      for ( line = triv -> next ; line != NULL ; line = line -> next ) 
      {
        stripEOL ( line -> text ) ; 
        if ( *(line -> text) != '\t' )
          tag = appendTag ( tag, line -> text ) ;
        else
          break ;
      }
      justified = txt_f_justify ( 2, 70, tag ) ;
      if ( justified != NULL )
      {
        (void) printf ( "%s\n", justified ) ;
        free ( (void*) justified ) ;
      }
      free ( (void*) tag ) ;
      triv = line ;
    } while ( triv != NULL ) ;
    (void) printf ( "\n" ) ;
  }
} 


void displayTitleInfo ( struct titleInfoRec *titleInfo, int listId ) 
{ 
  struct titleInfoRec *line ; 
  int column = 0, indent = 0, firstFlag = TRUE, i, attrLen ;
  char currentTag [ 5 ] ;
  char *ptr ;
 
  if ( titleInfo != NULL ) 
  {
    if ( titleInfoDefs [ listId ] . displayFormat == stdDisplay )
    {
      (void) printf ( "%s:\n", titleInfoDefs [ listId ] . tag ) ; 
      for ( line = titleInfo ; line != NULL ; line = line -> next ) 
      {
        (void) printf ( "  %s", line -> text ) ; 
        if ( line -> attr == NULL )
          (void) printf ( "\n" ) ; 
        else
          (void) printf ( " %s\n", line -> attr ) ; 
      }
      (void) printf ( "\n" ) ; 
    }
    else if ( titleInfoDefs [ listId ] . displayFormat == mergeDisplay )
    {
      (void) printf ( "%s:\n", titleInfoDefs [ listId ] . tag ) ; 
      for ( line = titleInfo ; line != NULL ; line = line -> next ) 
      {
        if ( firstFlag )
          (void) printf ( "  %s", line -> text ) ; 
        else
          (void) printf ( " / %s", line -> text ) ; 
        if ( line -> attr != NULL )
          (void) printf ( " %s", line -> attr ) ; 
        firstFlag = FALSE ;
      }
      (void) printf ( "\n\n" ) ; 
    }
    else if ( titleInfoDefs [ listId ] . displayFormat == techDisplay )
    {
      (void) printf ( "%s:", titleInfoDefs [ listId ] . tag ) ; 
      currentTag [ 0 ] = '\0' ;
      for ( line = titleInfo ; line != NULL ; line = line -> next ) 
        if ( ( ptr = strchr ( line -> text, ':' ) ) != NULL )
        {
          *ptr++ = '\0' ;
          if ( strcmp ( currentTag, line -> text ) != 0 )
          {
            (void) printf ( "\n" ) ;
            (void) strcpy ( currentTag, line -> text ) ;
            if ( strcmp ( line -> text, "MET" ) == 0 )
            {
              (void) printf ( "  Film Length: %s", ptr ) ;
              column = 14 ;
              indent = 14 ;
            }
            else if ( strcmp ( line -> text, "OFM" ) == 0 )
            {
              (void) printf ( "  Negative Format: %s", ptr ) ;
              column = 19 ;
              indent = 19 ;
            }
            else if ( strcmp ( line -> text, "PFM" ) == 0 )
            {
              (void) printf ( "  Print Format: %s", ptr ) ;
              column = 16 ;
              indent = 16 ;
            }
            else if ( strcmp ( line -> text, "RAT" ) == 0 )
            {
              (void) printf ( "  Aspect Ratio: %s", ptr ) ;
              column = 16 ;
              indent = 16 ;
            }
            else if ( strcmp ( line -> text, "PCS" ) == 0 )
            {
              (void) printf ( "  Process: %s", ptr ) ;
              column = 11 ;
              indent = 11 ;
            }
            else if ( strcmp ( line -> text, "LAB" ) == 0 )
            {
              (void) printf ( "  Laboratory: %s", ptr ) ;
              column = 14 ;
              indent = 14 ;
            }
            else if ( strcmp ( line -> text, "CAM" ) == 0 )
            {
              (void) printf ( "  Camera: %s", ptr ) ;
              column = 10 ;
              indent = 10 ;
            }
            else
            {
              (void) printf ( "  %s:%s", line -> text, ptr ) ;
              column = strlen ( line -> text ) + 2 ;
              indent = column ;
            }
            if ( line -> attr != NULL )
            {
              (void) printf ( " %s", line -> attr ) ; 
              column += strlen ( line -> attr ) + 1 ;
            }
          }
          else
          {
            (void) printf ( " ; " ) ;
            if ( line -> attr != NULL )
              attrLen = strlen ( line -> attr ) ;
            else
              attrLen = 0 ;
            if ( column + attrLen + strlen ( ptr ) > 68 )
            {
              (void) printf ( "\n" ) ;
              for ( i = 0 ; i < indent ; i++ )
                (void) printf ( " " ) ;
              column = indent ;
            }
            (void) printf ( "%s", ptr ) ;
            column += strlen ( ptr ) + 1 ;
            if ( line -> attr != NULL )
            {
              (void) printf ( " %s", line -> attr ) ; 
              column += strlen ( line -> attr ) + 1 ;
            }
          }
        }
      (void) printf ( "\n\n" ) ; 
    }
  }
} 
 

void displayCompactTitleInfo ( struct titleInfoRec *titleInfo, int listId ) 
{ 
  struct titleInfoRec *line ; 
  int lineLen = 0, textLen ;
 
  if ( titleInfo != NULL ) 
  {
    (void) printf ( "%s:\n", titleInfoDefs [ listId ] . desc ) ; 
 
    for ( line = titleInfo ; line != NULL ; line = line -> next ) 
    {
      textLen = strlen ( line -> text ) ;
      if ( line -> attr != NULL )
        textLen = textLen + strlen ( line -> attr ) + 1 ;
      if ( lineLen + textLen > 72 )
      {
        (void) printf ( "\n" ) ; 
        lineLen = textLen ;
      }
      else
        lineLen = lineLen + textLen + 2 ;
      (void) printf ( "  %s", line -> text ) ; 
      if ( line -> attr != NULL )
        (void) printf ( " %s", line -> attr ) ; 
    }
    if ( lineLen == 0 )
      (void) printf ( "\n" ) ; 
    else
      (void) printf ( "\n\n" ) ; 
  }
} 
 
 
void displayTidyFilmographyListData ( struct titleListEntry *lrec ) 
{ 
  char  tmp [ MXLINELEN ] ;
  char  fmtline [ MXLINELEN ] ;
  char  *comma, *bracket ;
  int   nmlen, k ;

  (void) strcpy ( tmp, lrec -> name ) ;
  if ( ( comma = strchr ( tmp, ',' ) ) != NULL )
  {
     if ( *(comma + strlen ( comma ) - 1) == ')' &&
          ( bracket = strstr ( comma, " (" ) ) != NULL )
     {
       *comma = '\0' ;
       *bracket = '\0' ;
       if ( lrec -> attr != NULL )
         (void) sprintf ( fmtline, "  %s %s %s %s", comma + 2, tmp, bracket + 1, lrec -> attr ) ;
       else
         (void) sprintf ( fmtline, "  %s %s %s", comma + 2, tmp, bracket + 1 ) ;
     }
     else
     {
       *comma = '\0' ;
       if ( lrec -> attr != NULL )
         (void) sprintf ( fmtline, "  %s %s %s", comma + 2, tmp, lrec -> attr ) ;
       else
         (void) sprintf ( fmtline, "  %s %s", comma + 2, tmp ) ;
     }
  }
  else
    if ( lrec -> attr != NULL )
      (void) sprintf ( fmtline, "  %s %s", lrec -> name, lrec -> attr ) ;
    else
      (void) sprintf ( fmtline, "  %s", lrec -> name ) ;

  if ( lrec -> cname == NULL )
    (void) printf ( "%s\n", fmtline ) ;
  else
  {
    (void) printf ( "%s .", fmtline ) ;
    nmlen = strlen ( fmtline ) ;
    k = nmlen ;
    do
    {
      (void) printf ( "." ) ;
      k++ ;
    } while ( k < 35 ) ;
    (void) printf ( " %s\n" , lrec -> cname ) ;
  }
}


void displayWritersTidyFilmographyListData ( struct titleListEntry *lrec ) 
{ 
  char  tmp [ MXLINELEN ] ;
  char  fmtline [ MXLINELEN ] ;
  char  *comma, *bracket ;
  int   nmlen, k ;

  (void) strcpy ( tmp, lrec -> name ) ;
  if ( ( comma = strchr ( tmp, ',' ) ) != NULL )
  {
     if ( *(comma + strlen ( comma ) - 1) == ')' &&
          ( bracket = strstr ( comma, " (" ) ) != NULL )
     {
       *comma = '\0' ;
       *bracket = '\0' ;
       if ( lrec -> attr != NULL )
         (void) sprintf ( fmtline, "%s %s %s %s", comma + 2, tmp, bracket + 1, lrec -> attr ) ;
       else
         (void) sprintf ( fmtline, "%s %s %s", comma + 2, tmp, bracket + 1 ) ;
     }
     else
     {
       *comma = '\0' ;
       if ( lrec -> attr != NULL )
         (void) sprintf ( fmtline, "%s %s %s", comma + 2, tmp, lrec -> attr ) ;
       else
         (void) sprintf ( fmtline, "%s %s", comma + 2, tmp ) ;
     }
  }
  else
    if ( lrec -> attr != NULL )
      (void) sprintf ( fmtline, "%s %s", lrec -> name, lrec -> attr ) ;
    else
      (void) sprintf ( fmtline, "%s", lrec -> name ) ;

  (void) printf ( "  %s", fmtline ) ;
}


int titleResultSort ( struct titleListEntry *e1, struct titleListEntry *e2 )
{
  if ( e1 -> lineOrder != e2 -> lineOrder )
    return ( e1 -> lineOrder - e2 -> lineOrder ) ;
  else if ( e1 -> groupOrder != e2 -> groupOrder )
    return ( e1 -> groupOrder - e2 -> groupOrder ) ;
  else if ( e1 -> subgroupOrder != e2 -> subgroupOrder )
    return ( e1 -> subgroupOrder - e2 -> subgroupOrder ) ;

  if ( e1 -> cname != NULL )
    if ( e2 -> cname != NULL )
      return ( strcmp ( e1 -> name, e2 -> name ) ) ;
    else
      return ( -1 ) ;
  if ( e2 -> cname != NULL )
    return ( 1 ) ;
  else
    return ( strcmp ( e1 -> name, e2 -> name ) ) ;
}


int castTitleResultSort ( struct titleListEntry *e1, struct titleListEntry *e2 )
{
  if ( e1 -> position != e2 -> position )
    return ( e1 -> position - e2 -> position ) ;
  else
    return ( strcmp ( e1 -> name, e2 -> name ) ) ;
}


void displayFilmographyListData ( struct titleSearchRec *trec )
{
  int i, j, base, count, listId, castOrdered, writersOrdered ;
  int linePrev, groupPrev, subgroupPrev ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS - 3 ; i++ )
  {
    listId = titleOrderData [ i ] . listId ;
    base = trec -> results [ listId ] . base ;
    count = trec -> results [ listId ] . count ;
    if ( count > 0 )
    {
      if ( count == 1 )
        (void) printf ( "%s", titleOrderData [ i ] . description ) ;
      else
      {
        (void) printf ( "%s", titleOrderData [ i ] . pluralDesc ) ;
        (void) qsort ( (void*) &(trec -> entries [ base ]), (size_t) count, sizeof ( struct titleListEntry ), (int (*) (const void*, const void*)) titleResultSort ) ;
      }

      if ( listId == WRITER_LIST_ID )
      {
        if ( trec -> entries [ base ] . lineOrder == 1 )
        {
          (void) printf ( " (credits order)\n" ) ;
          writersOrdered = TRUE ;
        }
        else
        {
          (void) printf ( " (alphabetical order)\n" ) ;
          writersOrdered = FALSE ;
        }
        for ( j = 0 ; j < count ; j++ )
        {
          if ( j == 0 )
            displayWritersTidyFilmographyListData ( &(trec -> entries [ base + j ]) ) ;
          else 
          {
            if ( linePrev != trec -> entries [ base + j ] . lineOrder && trec -> entries [ base + j ] . lineOrder != MAXPOS )
              (void) printf ( "\n\n" ) ;
            else if ( groupPrev == trec -> entries [ base + j ] . groupOrder && trec -> entries [ base + j ] . groupOrder != MAXPOS )
              (void) printf ( "  &\n" ) ;
            else if ( trec -> entries [ base + j ] . subgroupOrder != MAXPOS )
              (void) printf ( "  and\n" ) ;
            else
            {
              if ( writersOrdered )
              {
                (void) printf ( "\n\n" ) ;
                writersOrdered = FALSE ;
              }
              else
                (void) printf ( "\n" ) ;
            }
            displayWritersTidyFilmographyListData ( &(trec -> entries [ base + j ]) ) ;
          }
          linePrev = trec -> entries [ base + j ] . lineOrder ;
          groupPrev = trec -> entries [ base + j ] . groupOrder ;
          subgroupPrev = trec -> entries [ base + j ] . subgroupOrder ;
        }
        (void) printf ( "\n" ) ;
      }
      else
      {
        (void) printf ( "\n" ) ;
        for ( j = 0 ; j < count ; j++ )
          displayTidyFilmographyListData ( &(trec -> entries [ base + j ]) ) ;
      }
      (void) printf ( "\n" ) ;
    }
  }
  if ( trec -> results [ 0 ] . base >= 0 )
    base = trec -> results [ 0 ] . base ;
  else
    base = trec -> results [ 1 ] . base ;
  count = trec -> results [ 0 ] . count + trec -> results [ 1 ] . count ;
  if ( count > 0 )
  {
    (void) qsort ( (void*) &(trec -> entries [ base ]), (size_t) count, sizeof ( struct titleListEntry ), (int (*) (const void*, const void*)) castTitleResultSort ) ;
    if ( trec -> entries [ base ] . position == 1 )
    {
      castOrdered = TRUE ;
      if ( trec -> castStatus == completed )
        (void) printf ( "Complete Cast: (credits order)\n" ) ;
      else if ( trec -> castStatus == verified )
        (void) printf ( "Verified Complete Cast: (credits order)\n" ) ;
      else
        (void) printf ( "Cast: (credits order)\n" ) ;
    }
    else
    {
      castOrdered = FALSE ;
      if ( trec -> castStatus == completed )
        (void) printf ( "Complete Cast: (alphabetical order)\n" ) ;
      else if ( trec -> castStatus == verified )
        (void) printf ( "Verified Complete Cast: (alphabetical order)\n" ) ;
      else
        (void) printf ( "Cast: (alphabetical order)\n" ) ;
    }
    for ( j = 0 ; j < count ; j++ )
    {
      if ( castOrdered && trec -> entries [ base + j ] . position == MAXPOS )
      {
         castOrdered = FALSE ;
         (void) printf ( "\nRemainder of Cast: (alphabetical order)\n" ) ;
      }
      displayTidyFilmographyListData ( &(trec -> entries [ base + j ]) ) ;
    }
    (void) printf ( "\n" ) ;
  }
  base = trec -> results [ MISCSRCH ] . base ;
  count = trec -> results [ MISCSRCH ] . count ;
  if ( count > 0 )
  {
    if ( count == 1 )
      (void) printf ( "%s", titleOrderData [ NO_OF_FILMOGRAPHY_LISTS - 3 ] . description ) ;
    else
    {
      (void) printf ( "%s", titleOrderData [ NO_OF_FILMOGRAPHY_LISTS - 3 ] . pluralDesc ) ;
      (void) qsort ( (void*) &(trec -> entries [ base ]), (size_t) count, sizeof ( struct titleListEntry ), (int (*) (const void*, const void*)) titleResultSort ) ;
    }
    (void) printf ( "\n" ) ;
    for ( j = 0 ; j < count ; j++ )
      displayTidyFilmographyListData ( &(trec -> entries [ base + j ]) ) ;
    (void) printf ( "\n" ) ;
  }
  if ( trec -> crewStatus == completed )
    (void) printf ( "Crew believed to be complete.\n\n" ) ;
  else if ( trec -> crewStatus == verified )
    (void) printf ( "Crew verified as complete.\n\n" ) ;
}


void displayListEntry ( struct titleListEntry *lrec, char *tag )
{
  if ( lrec -> attr == NULL )
    if ( tag != NULL )
      if ( lrec -> cname != NULL )
        (void) printf ( "%s %s  [%s]\n", lrec -> name, tag, lrec -> cname ) ;
      else
        (void) printf ( "%s %s\n", lrec -> name, tag ) ;
    else
      if ( lrec -> cname != NULL )
        (void) printf ( "%s  [%s]\n", lrec -> name, lrec -> cname ) ;
      else
        (void) printf ( "%s\n", lrec -> name ) ;
  else
    if ( tag != NULL )
      if ( lrec -> cname != NULL )
        (void) printf ( "%s %s %s  [%s]\n", lrec -> name, tag, lrec -> attr, lrec ->cname ) ;
      else
        (void) printf ( "%s %s %s\n", lrec -> name, tag, lrec -> attr ) ;
    else
      if ( lrec -> cname != NULL )
        (void) printf ( "%s %s  [%s]\n", lrec -> name, lrec -> attr, lrec -> cname ) ;
      else
        (void) printf ( "%s %s\n", lrec -> name, lrec -> attr ) ;
}


int validOutput ( struct titleSearchRec *trec )
{
  int i ;

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    if ( trec -> titleInfo [ i ] != NULL )
      return ( TRUE ) ;

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
    if ( trec -> trivlists [ i ] != NULL )
      return ( TRUE ) ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
    if ( trec -> results [ i ] . count > 0 )
      return ( TRUE ) ;

  if ( trec -> plot != NULL )
    return TRUE ;

  if ( trec -> aka != NULL )
    return TRUE ;

  if ( trec -> mrr != NULL )
    return TRUE ;

  if ( trec -> year > 0 )
    return TRUE ;

  return FALSE ;
}


void displayBriefTitleSearch ( struct titleSearchRec *trec )
{
  int i, j, base, count, listId ;

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS - 2 ; i++ )
  {
    listId = titleOrderData [ i ] . listId ;
    base = trec -> results [ listId ] . base ;
    count = trec -> results [ listId ] . count ;
    if ( count > 0 )
    {
      if ( count != 1 )
        (void) qsort ( (void*) &(trec -> entries [ base ]), (size_t) count, sizeof ( struct titleListEntry ), (int (*) (const void*, const void*)) titleResultSort ) ;

      for ( j = 0 ; j < count ; j++ )
        displayListEntry ( &(trec -> entries [ base + j ]), titleOrderData [ i ] . tag ) ;
    }
  }
  if ( trec -> results [ 0 ] . base >= 0 )
    base = trec -> results [ 0 ] . base ;
  else
    base = trec -> results [ 1 ] . base ;
  count = trec -> results [ 0 ] . count + trec -> results [ 1 ] . count ;
  if ( count > 0 )
  {
    (void) qsort ( (void*) &(trec -> entries [ base ]), (size_t) count, sizeof ( struct titleListEntry ), (int (*) (const void*, const void*)) titleResultSort ) ;
    for ( j = 0 ; j < count ; j++ )
      displayListEntry ( &(trec -> entries [ base + j ]), titleOrderData [ NO_OF_FILMOGRAPHY_LISTS - 1 ] . tag ) ;
  }
} 


void displayTitleURL ( char *title ) 
{
   char url [ MXLINELEN ] ;
   char *ptr ;
   
   (void) strcpy ( url, "http://www.imdb.com/Title?" ) ;
   for ( ptr = url + strlen(url) ; *title != 0 ; ptr++, title++ )
   {
     if ( isalnum ( *title ) )
       *ptr = *title ;
     else
     {
       (void) sprintf ( ptr, "%%%2X", *title & 255 ) ;
       ptr = ptr + 2 ;
     }
   }
   *ptr = '\0' ;
   (void) printf ( "URL:\n  %s\n\n", url ) ;
}


void displayTitleSearchRec ( struct titleSearchRec *trec, int tidy )
{
  struct akaRec     *arec ;
  int i ;

  if ( ! validOutput ( trec ) )
    return ;

  if ( tidy )
  {
    (void) printf ( "------------------------------------------------------------------------------\nTitle:\n  " ) ;

    if ( trec -> mrr != NULL && trec -> mrr -> votes > 0 )
      (void) printf ( "      %s%8d   %2.1f  ", trec -> mrr -> distribution, 
		 trec -> mrr -> votes, trec -> mrr -> rating ) ;
    else
      (void) printf ( "                                " ) ;

    if ( trec -> year != 0 )
      displayTitleYear ( trec -> title, trec -> year ) ;
    else
      (void) printf ( "%s", trec -> title ) ;
    if ( trec -> attr == NULL ) 
      (void) printf ( "\n\n" ) ;
    else
      (void) printf ( " %s\n\n", trec -> attr ) ;

    displayTitleURL ( trec -> title ) ;

    if ( trec -> aka != NULL )
    {
      (void) printf ( "Aka Titles:\n" ) ;
      for ( arec = trec -> aka ; arec != NULL ; arec = arec -> next )
        if ( arec -> akaAttr != NULL )
          (void) printf ( "  %s %s\n", arec -> akaTitle, arec -> akaAttr ) ;
        else
	  (void) printf ( "  %s\n", arec -> akaTitle ) ;
      (void) printf ( "\n" ) ;
    }

    if ( trec -> trivlists [ NO_OF_TRIV_LISTS - 1 ] != NULL )
      displayTagLines ( trec -> trivlists [ NO_OF_TRIV_LISTS - 1 ] ) ; 

    for ( i = 2 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
      if ( trec -> titleInfo [ i ] != NULL )
        displayTitleInfo ( trec -> titleInfo [ i ], i ) ; 

    if ( trec -> searchparams . plotopt )
      displayPlot ( trec -> plot ) ; 

/* Genres */
    if ( trec -> titleInfo [ 0 ] != NULL )
      displayCompactTitleInfo ( trec -> titleInfo [ 0 ], 0 ) ; 

/* Keywords */
    if ( trec -> titleInfo [ 1 ] != NULL )
      displayCompactTitleInfo ( trec -> titleInfo [ 1 ], 1 ) ;

    displayFilmographyListData ( trec ) ;

    if ( trec -> searchparams . linkopt )
      displayMovieLinks ( trec -> noOfLinks, trec -> links ) ; 

    for ( i = 0 ; i < NO_OF_TRIV_LISTS - 1 ; i++ )
      if ( trec -> trivlists [ i ] != NULL )
        displayTrivia ( trec -> trivlists [ i ], i ) ; 

    if ( trec -> searchparams . litopt )
      displayLiterature ( trec -> literature ) ; 

    if ( trec -> searchparams . businessopt )
      displayBusiness ( trec -> business ) ; 

    if ( trec -> searchparams . ldopt )
      displayLaserDisc ( trec -> laserdisc ) ; 

    (void) printf ( "\n           Copyright 1990-2004 The Internet Movie Database Inc.\n" ) ;
    (void) printf ( "      Support the IMDb by visiting our web site: http://www.imdb.com/\n\n" ) ;
  }
  else
    displayBriefTitleSearch ( trec ) ; 
}


void displayTitleSearchResults ( struct titleSearchRec *tchain, int tidy )
{
  struct titleSearchRec *trec ;

  for ( trec = tchain ; trec != NULL ; trec = trec -> next )
    displayTitleSearchRec ( trec, tidy ) ; 
}


/*============================================================================
 *
 *  Program: dbutils.c
 *
 *  Version: 3.23
 *
 *  Program: general database functions
 *
 *  Author:  C J Needham <col@imdb.com>      
 *
 *  Copyright (c) 1996-2004 The Internet Movie Database Inc.
 *
 *  OpenFile function by: Timo Lamminjoki <lamminjo@pcu.helsinki.fi>
 *
 *  Copyright (c) Timo Lamminjoki 1993
 *
 *  Text justification functions by: Mark Harding <mah@imdb.com>      
 *
 *  Copyright (c) 1996 The Internet Movie Database Inc.
 *
 *  Permission is granted by the copyright holder to distribute this program
 *  is source form only, providing this notice remains intact, and no fee
 *  of any kind is charged. This software is provided as is and there are no 
 *  warranties, expressed or implied.
 *
 *============================================================================
 */

#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <unistd.h>
#include "moviedb.h"
#include "dbutils.h"

struct titleOrderRec titleOrderData [ NO_OF_FILMOGRAPHY_LISTS ] =
   { { PRDCRSRCH, "(pr)",   "Producer:", "Producers:" },
     { DIRSRCH,   "(d)",    "Director:", "Directors:" },
     { WRITESRCH, "(w)",    "Writer:", "Writers:" },
     { COMPSRCH,  "(m)",    "Composer:", "Composers:" },
     { CINESRCH,  "(ph)",   "Cinematographer:", "Cinematographers:" },
     { EDITSRCH,  "(ed)",   "Editor:", "Editors:" },
     { PDESSRCH,  "(pd)",   "Production Designer:", "Production Designers:" },
     { CDESSRCH,  "(cd)",   "Costume Designer:", "Costume Designers:" },
#ifdef INTERNAL
     { CASSRCH,   "(cas)",  "Casting:", "Casting:" },
     { ARTSRCH,   "(art)",  "Art Director:", "Art Directors:" },
     { SETSRCH,   "(set)",  "Set Decorator:", "Set Decorators:" },
     { STUSRCH,   "(stu)",  "Stunts:", "Stunts:" },
     { ASDSRCH,   "(ad)",   "Assistant Director:", "Assistant Directors:" },
     { PMGSRCH,   "(pm)",   "Production Manager:", "Production Managers:" },
     { SOUSRCH,   "(snd)",  "Sound:", "Sound:" },
     { SPESRCH,   "(sfx)",  "Special Effects:", "Special Effects:" },
     { MAKSRCH,   "(mu)",   "Make-Up:", "Make-Up:" },
     { ARDSRCH,   "(ard)",  "Art Department:", "Art Department:" },
     { VFXSRCH,   "(vfx)",  "Visual Effects:", "Visual Effects:" },
#endif
     { MISCSRCH,  "(misc)", "Miscellaneous:", "Miscellaneous:" },
     { ACRSRCH,   NULL,     "Cast:", "Cast:" },
     { ACSSRCH,   NULL,     "Cast:", "Cast:" }
   } ;


struct filmographyOptRec filmographyOptions [ NO_OF_SEARCH_OPTS ] =
{
  { "-cast",      "-a", { 1,1,0,0,0,0,0,0,0,0,0 } } ,
  { "-acr",      "-ar", { 1,0,0,0,0,0,0,0,0,0,0 } } ,
  { "-acs",      "-as", { 0,1,0,0,0,0,0,0,0,0,0 } } ,
  { "-dir",       "-d", { 0,0,1,0,0,0,0,0,0,0,0 } } ,
  { "-write",     "-w", { 0,0,0,1,0,0,0,0,0,0,0 } } ,
  { "-comp",      "-c", { 0,0,0,0,1,0,0,0,0,0,0 } } ,
  { "-cine",     "-ph", { 0,0,0,0,0,1,0,0,0,0,0 } } ,
  { "-edit",     "-ed", { 0,0,0,0,0,0,1,0,0,0,0 } } ,
  { "-prodes",   "-pd", { 0,0,0,0,0,0,0,1,0,0,0 } } ,
  { "-costdes",  "-cd", { 0,0,0,0,0,0,0,0,1,0,0 } } ,
  { "-prdcr",    "-pr", { 0,0,0,0,0,0,0,0,0,1,0 } } ,
#ifdef INTERNAL
  { "-castdir","-cdir", { 0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0 } } ,
  { "-art",     "-art", { 0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0 } } ,
  { "-set",     "-set", { 0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0 } } ,
  { "-stunt",    "-st", { 0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0 } } ,
  { "-asstdir",  "-ad", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0 } } ,
  { "-prodman",  "-pm", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0 } } ,
  { "-snddept", "-snd", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0 } } ,
  { "-sfxdept", "-sfx", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0 } } ,
  { "-makeup",   "-mu", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0 } } ,
  { "-artdept", "-ard", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0 } } ,
  { "-vfxdept", "-vfx", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0 } } ,
  { "-misc",   "-misc", { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1 } } ,
  { "-name",   "-name", { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1 } } 
#else
  { "-misc",   "-misc", { 0,0,0,0,0,0,0,0,0,0,1 } } ,
  { "-name",   "-name", { 1,1,1,1,1,1,1,1,1,1,1 } } 
#endif
} ;



struct filmographyDefRec filmographyDefs [ NO_OF_FILMOGRAPHY_LISTS ] = 
  { { "-acr", ACRSTEM, ACRLIST, "Actor", ACRSRCH, "Acting", "Actors" } ,
    { "-acs", ACSSTEM, ACSLIST, "Actress", ACSSRCH, "Acting", "Actresses" } ,
    { "-dir", DIRSTEM, DIRLIST, "Director", DIRSRCH, "Directing", "Directors" } ,
    { "-write", WRITESTEM, WRITELIST, "Writer", WRITESRCH, "Writing", "Writers" } ,
    { "-comp", COMPSTEM, COMPLIST, "Composer", COMPSRCH, "Score", "Composers" } ,
    { "-cine", CINESTEM, CINELIST, "Cinematographer", CINESRCH, "Cinematography", "Cinematographers" } ,
    { "-edit", EDITSTEM, EDITLIST, "Editor", EDITSRCH,  "Editing", "Editors" } ,
    { "-prodes", PDESSTEM, PDESLIST, "Production Designer", PDESSRCH, "Production Design", "Production Designers" } ,
    { "-costdes", CDESSTEM, CDESLIST, "Costume Designer",  CDESSRCH, "Costume Design", "Costume Designers" } ,
    { "-prdcr", PRDCRSTEM, PRDCRLIST, "Producer", PRDCRSRCH, "Produced by", "Producers" },
#ifdef INTERNAL
    { "-castdir", CASSTEM, CASLIST, "Casting Director", CASSRCH, "Casting by", "Casting Directors" },
    { "-art", ARTSTEM, ARTLIST, "Art Director", ARTSRCH, "Art Direction", "Art Directors" },
    { "-set", SETSTEM, SETLIST, "Set Decorator", SETSRCH, "Set Decoration", "Set Decorators" },
    { "-stunt", STUSTEM, STULIST, "Stunt Person", STUSRCH, "Stunts", "Stunt People" },
    { "-asstdir", ASDSTEM, ASDLIST, "Assistant Director", ASDSRCH, "Assistant Drirecting", "Assistant Directors" },
    { "-prodman", PMGSTEM, PMGLIST, "Production Manager", PMGSRCH, "Production Managing", "Production Managers" },
    { "-snddept", SOUSTEM, SOULIST, "Member of Sound Department", SOUSRCH, "Sound Department", "Sound Department" },
    { "-sfxdept", SPESTEM, SPELIST, "Member of Special Effects Department", SPESRCH, "Special Effects Department", "Special Effects Department" },
    { "-makeup", MAKSTEM, MAKLIST, "Member of Make-Up Department", MAKSRCH, "Make-Up Department", "Make-Up Department" },
    { "-artdept", ARDSTEM, ARDLIST, "Member of Art Department", ARDSRCH, "Art Department", "Art Department" },
    { "-vfxdept", VFXSTEM, VFXLIST, "Member of Visual Effects Department", VFXSRCH, "Visual Effects Department", "Visual Effects Department" },
#endif
    { "-misc", MISCSTEM, MISCLIST, "Miscellaneous", MISCSRCH, "Miscellaneous", "Miscellaneous" } 
  } ;

struct triviaDefRec triviaDefs [ NO_OF_TRIV_LISTS ] = 
  { { "-triv", TRIVIA, TRIVIADB, TRIVIDX, "Trivia", "FILM TRIVIA\n" },
    { "-crazy", CRAZYLIST, CRAZYDB, CRAZYIDX, "Crazy Credits", "CRAZY CREDITS\n" },
    { "-goof", GOOFLIST, GOOFDB, GOOFIDX, "Goofs", "GOOFS LIST\n" },
    { "-quote", QUOTELIST, QUOTEDB, QUOTEIDX, "Quotes", "QUOTES LIST\n" },
    { "-strack", STRAKLIST, STRAKDB, STRAKIDX, "Soundtrack", "SOUNDTRACKS LIST\n" },
    { "-vers", VERSLIST, VERSDB, VERSIDX, "Alternate Versions", "ALTERNATE VERSIONS LIST\n" },
    { "-tag", TAGLIST, TAGDB, TAGIDX, "Tag Lines", "TAG LINES LIST\n" }
  } ;

struct titleInfoDefRec titleInfoDefs [ NO_OF_TITLE_INFO_LISTS ] = 
  { { "-genre", GENRELIST, GENREDB, GENREIDX, "Genres", "Genres", "8: THE GENRES LIST\n", stdDisplay },
    { "-keyword", KYWDLIST, KYWDDB, KYWDIDX, "Keywords", "Keywords", "8: THE KEYWORDS LIST\n", stdDisplay },
    { "-prodco", PRODCOLIST, PRODCODB, PRODCOIDX, "Production Company", "Production Company", "PRODUCTION COMPANIES LIST\n", stdDisplay },
    { "-dist", DISTLIST, DISTDB, DISTIDX, "Distributor", "Distributor", "DISTRIBUTORS LIST\n", stdDisplay },
    { "-cntry", CNTRYLIST, CNTRYDB, CNTRYIDX, "Country", "Country of Production", "COUNTRIES LIST\n", mergeDisplay },
    { "-cert", CERTLIST, CERTDB, CERTIDX, "Certificates", "Certificates", "CERTIFICATES LIST\n", stdDisplay },
    { "-time", TIMESLIST, TIMESDB, TIMESIDX, "Running Times", "Running Time", "RUNNING TIMES LIST\n", stdDisplay },
    { "-color", COLORLIST, COLORDB, COLORIDX, "Color Information", "Filmed In", "COLOR INFO LIST\n", stdDisplay },
    { "-mix", MIXLIST, MIXDB, MIXIDX, "Sound Mix", "Sound Mix", "SOUND-MIX LIST\n", stdDisplay },
    { "-tech", TECHLIST, TECHDB, TECHIDX, "Technical", "Technical Info", "TECHNICAL LIST\n", techDisplay },
    { "-rel", RELLIST, RELDB, RELIDX, "Release Dates", "Release Date", "RELEASE DATES LIST\n", stdDisplay },
    { "-loc", LOCLIST, LOCDB, LOCIDX, "Locations", "Locations", "LOCATIONS LIST\n", stdDisplay },
    { "-lang", LANGLIST, LANGDB, LANGIDX, "Language", "Language", "LANGUAGE LIST\n", stdDisplay },
    { "-sfxco", SFXCOLIST, SFXCODB, SFXCOIDX, "Special Effects Company", "Special Effects Company", "SFXCO COMPANIES LIST\n", stdDisplay },
#ifdef INTERNAL
    { "-miscco", MSCCOLIST, MSCCODB, MSCCOIDX, "Miscellaneous Company", "Miscellaneous Company", "MISCELLANEOUS COMPANY LIST\n", stdDisplay }
#endif
  } ;

struct movieLinksDefRec movieLinkDefs [ NO_OF_LINK_TYPES ] =
  { 
    { followsLink,      "follows", 7 },
    { followedByLink,   "followed by", 11 },
    { remakeOfLink,     "remake of", 9 },
    { remadeAsLink,     "remade as", 9 },
    { referencesLink,   "references", 10 },
    { referencedInLink, "referenced in", 13 },
    { spoofsLink,       "spoofs", 6 },
    { spoofedInLink,    "spoofed in", 10 },
    { featuresLink,     "features", 8 },
    { featuredInLink,   "featured in", 11 },
    { spinOffFromLink,  "spin off from", 13 },
    { spinOffLink,      "spin off", 8 },
    { versionOfLink,    "version of", 10 },
    { similarToLink,    "similar to", 10 },
    { editedIntoLink,   "edited into", 11 },
    { editedFromLink,   "edited from", 11 },
    { alternateLanguageVersionOfLink,  "alternate language version of", 29 },
    { unknownLink,      "unknown link", 12 }
  } ;

char* duplicateString ( const char *str )
{
  char* p ;

  if ( ( p = malloc ( strlen ( str ) + 1 ) ) != NULL )
  {
     (void) strcpy ( p, str ) ;
     return ( p ) ;
  }

  moviedbError ( "error: out of memory" ) ;
  return ( NULL ) ;
}


char *appendString ( char *original, char *extension )
{
  int newLen, oriLen ;
  char *newStr ;

  oriLen = strlen ( original ) ;
  newLen = oriLen + strlen ( extension ) + 2 ;
  newStr = realloc ( original, newLen ) ;
  if ( *extension == ' ' ) 
    (void) strcpy ( newStr + oriLen, extension ) ;
  else if ( *extension == '\t' ) 
  {
    *(newStr + oriLen - 1) = ' ' ;
    (void) strcpy ( newStr + oriLen, extension + 1 ) ;
  }
  else if ( *extension == '\n' ) 
  {
    *(newStr + oriLen) = '\n' ;
    *(newStr + oriLen + 1) = '\0' ;
  }
  else
  {
    *(newStr + oriLen - 1) = ' ' ;
    (void) strcpy ( newStr + oriLen, extension ) ;
  }
  return ( newStr ) ;
}


char *appendTag ( char *original, char *extension )
{
  int newLen, oriLen ;
  char *newStr ;

  oriLen = strlen ( original ) ;
  newLen = oriLen + strlen ( extension ) + 2 ;
  newStr = realloc ( original, newLen ) ;
  strcat ( newStr, " " ) ;
  strcat ( newStr, extension ) ;
  return ( newStr ) ;
}


char* duplicateField ( const char *str )
{
  char* p ;


  if ( ( p = strchr ( str, FSEP ) ) == NULL )
    return ( NULL ) ;
  else
  {
    *p = '\0' ;
    if ( ( p = malloc ( strlen ( str ) + 1 ) ) != NULL )
    {
       (void) strcpy ( p, str ) ;
       return ( p ) ;
    }
  }
  moviedbError ( "error: out of memory" ) ;
  return ( NULL ) ;
}


char *constructFilename (char *filename, const char *base, const char *extension)
{
  (void) strcpy ( filename, base ) ;
  (void) strcat ( filename, "." ) ;
  (void) strcat ( filename, extension ) ;
  return ( filename ) ;
}


int isReadable ( const char *fname )
{ 
  struct stat s ;
  char buf [ MAXPATHLEN ];

  if ( stat ( fname, &s ) == 0 )
    return ( TRUE ) ;

  (void) sprintf ( buf, "%s%s", fname, ZDBSEXT ) ;
  if ( stat ( buf, &s ) == 0 )
    return ( TRUE ) ;
    
  (void) sprintf ( buf, "%s%s", fname, ZLISTEXT ) ;
  if ( stat ( buf, &s ) == 0 )
    return ( TRUE ) ;
    
  return ( FALSE ) ;
}


int caseCompare ( unsigned char *s1, unsigned char *s2 )
{
  for ( ; upperCase (*s1) == upperCase (*s2) ; s1++, s2++ )
    if ( *s1 == '\0' )
      return ( 0 ) ;
  return ( upperCase (*s1) - upperCase (*s2) ) ;
}


void putPosition ( int i, FILE *stream)
{
  (void) fputc ( i & 255, stream ) ;
}


void putByte ( int i, FILE *stream)
{
  (void) fputc ( i & 255, stream ) ;
}


void putTitle ( TitleID titleKey, FILE *stream)
{
  (void) fputc ( titleKey & 255, stream ) ;
  (void) fputc ( ( titleKey >> 8 ) & 255, stream ) ;
  (void) fputc ( ( titleKey >> 16 ) & 255, stream ) ;
}


void putAttr ( AttributeID attrKey, FILE *stream)
{
  (void) fputc ( attrKey & 255, stream ) ;
  (void) fputc ( ( attrKey >> 8 ) & 255, stream ) ;
  (void) fputc ( ( attrKey >> 16 ) & 255, stream ) ;
}


void putInt (int i, FILE *stream)
{
  (void) fputc ( i & 255, stream ) ;
  (void) fputc ( ( i >> 8 ) & 255, stream ) ;
}


void putOffset ( long offset, FILE *stream )
{
  (void) fputc ( offset & 255, stream ) ;
  (void) fputc ( ( offset >> 8 ) & 255, stream ) ;
  (void) fputc ( ( offset >> 16 ) & 255, stream ) ;
}


void putFullOffset ( long offset, FILE *stream )
{
  (void) fputc ( offset & 255, stream ) ;
  (void) fputc ( ( offset >> 8 ) & 255, stream ) ;
  (void) fputc ( ( offset >> 16 ) & 255, stream ) ;
  (void) fputc ( ( offset >> 24 ) & 255, stream ) ;
}


void putName ( NameID nameKey, FILE *stream)
{
  (void) fputc ( nameKey & 255, stream ) ;
  (void) fputc ( ( nameKey >> 8 ) & 255, stream ) ;
  (void) fputc ( ( nameKey >> 16 ) & 255, stream ) ;
}


void putString ( char *str, FILE *stream)
{
  while ( *str ) 
    (void) fputc ( *str++, stream ) ;
}


void putFilmographyCounts ( int noWithAttr, int noWithoutAttr, FILE *stream )
{
  long i ;

  i = ( ( noWithAttr & 4095 ) << 12 ) | ( noWithoutAttr & 4095 ) ;
  (void) fputc ( i & 255, stream ) ;
  (void) fputc ( ( i >> 8 ) & 255, stream ) ;
  (void) fputc ( ( i >> 16 ) & 255, stream ) ;
}


NameID getName (FILE *stream)
{
  NameID  nameKey ;
 
  nameKey = fgetc ( stream )  & 255 ;
  nameKey |= fgetc ( stream ) << 8 ;
  nameKey |= fgetc ( stream ) << 16 ; 
  return ( nameKey ) ;
}


long getOffset (FILE *stream)
{
  long  offset ;
 
  offset = fgetc ( stream )  & 255 ;
  offset |= fgetc ( stream ) << 8 ;
  offset |= fgetc ( stream ) << 16 ; 
  return ( offset ) ;
}


long getFullOffset (FILE *stream)
{
  long  offset ;
 
  offset = fgetc ( stream )  & 255 ;
  offset |= fgetc ( stream ) << 8 ;
  offset |= fgetc ( stream ) << 16 ; 
  offset |= fgetc ( stream ) << 24 ; 
  return ( offset ) ;
}


int getByte (FILE *stream)
{
  return ( fgetc ( stream )  & 255 ) ;
}


int getPosition (FILE *stream)
{
  return ( fgetc ( stream )  & 255 ) ;
}


TitleID getTitle (FILE *stream)
{
  TitleID  titleKey ;
 
  titleKey = fgetc ( stream )  & 255 ;
  titleKey |= fgetc ( stream ) << 8 ; 
  titleKey |= fgetc ( stream ) << 16 ; 
  return ( titleKey ) ;
}


AttributeID getAttr (FILE *stream)
{
  AttributeID  attrKey ;
 
  attrKey = fgetc ( stream )  & 255 ;
  attrKey |= fgetc ( stream ) << 8 ; 
  attrKey |= fgetc ( stream ) << 16 ; 
  return ( attrKey ) ;
}


int getInt (FILE *stream)
{
  int  i ;
 
  i = fgetc ( stream )  & 255 ;
  i |= fgetc ( stream ) << 8 ; 
  return ( i ) ;
}


void getFilmographyCounts ( FILE *stream, int *noWithAttr, int*noWithoutAttr )
{
  long i ;

  i = fgetc ( stream )  & 255 ;
  i |= fgetc ( stream ) << 8 ;
  i |= fgetc ( stream ) << 16 ; 
  *noWithAttr = ( i >> 12 )  & 4095 ;
  *noWithoutAttr = i & 4095 ;
}


char *getString (int len, FILE *stream)
{
  char line [ MXLINELEN ] ;
  int i ;

  for ( i = 0 ; i < len ; i++ )
    line [ i ] = fgetc ( stream ) ;
  line [ i ] = '\0' ;

  return ( duplicateString ( line ) ) ;
}


void getStringFast ( int len, FILE *stream, char *line )
{
  int i ;

  for ( i = 0 ; i < len ; i++ )
    line [ i ] = fgetc ( stream ) ;
  line [ i ] = '\0' ;
}


void stripEOL (char *str)
{
  char *p ;

  if ( ( p = strchr ( str, '\n' ) ) != NULL )
    *p = '\0' ;
}


void stripSep (char *str)
{
  char *p ;

  if ( ( p = strchr ( str, FSEP ) ) != NULL )
    *p = '\0' ;
}


void moviedbWriteError ( const char *filename )
{
  (void) fprintf ( stderr, "error writing %s\n", filename ) ;
  exit ( -1 ) ;
}


void moviedbError ( const char *message )
{
  (void) fprintf ( stderr, "%s\n", message ) ;
  exit ( -1 ) ;
}


void moviedbUsage ( const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6 )
{
  if ( s1 != NULL )
    (void) fprintf ( stderr, "%s\n", s1 ) ;
  if ( s2 != NULL )
    (void) fprintf ( stderr, "%s\n", s2 ) ;
  if ( s3 != NULL )
    (void) fprintf ( stderr, "%s\n", s3 ) ;
  if ( s4 != NULL )
    (void) fprintf ( stderr, "%s\n", s4 ) ;
  if ( s5 != NULL )
    (void) fprintf ( stderr, "%s\n", s5 ) ;
  if ( s6 != NULL )
    (void) fprintf ( stderr, "%s\n", s6 ) ;
  exit ( -1 ) ;
}


void upperCaseString (char *str)
{
  while ( *str )
  {
    *str = upperCase ( *str ) ;
    str++ ;
  }
}


FILE *writeFile ( const char *path )
{
  FILE *retval ;

  if ( ( retval = fopen ( path, "w" ) ) == NULL )
    moviedbWriteError ( path ) ;

  return ( retval ) ;
}


FILE *openFile ( const char *path )
{
   FILE *ret;
#ifdef COMPRESS
   pid_t pid;
   int tmpfd;
   struct stat s ;
   char buf [ MAXPATHLEN ] ;
   char *args [ MAXZCATARGS ] ;
   char argstring [ MAXPATHLEN + MXLINELEN ] ;
   char *ptr ;
   int i ;
   char tmpfname [ ] = "mkdbXXXXXX" ;
#endif

   if ( ( ret = fopen ( path, "r" ) ) == NULL )
   {
#ifndef COMPRESS
      (void) fprintf ( stderr, "Can't open %s\n", path );
      exit ( -1 );
#else
      (void) sprintf ( buf, "%s%s", path, ZDBSEXT ) ;
      if ( stat ( buf, &s ) != 0 )
      {
        (void) sprintf ( buf, "%s%s", path, ZLISTEXT ) ;
        if ( stat ( buf, &s ) != 0 )
        {
	   (void) fprintf ( stderr, "Can't find file: %s\n", path );
	   exit ( -1 );
        }
      }
      tmpfd = mkstemp ( tmpfname ) ;
      unlink ( tmpfname ) ;

      if ( tmpfd == -1 )
      {
	 (void) fprintf ( stderr, "Can't open workfile\n" );
	 exit ( -1 );
      }
      if ( ( pid = fork () ) == -1 )
      {
	 (void) fprintf ( stderr, "Can't fork subprocess\n" );
	 exit ( -1 );
      }
      if ( pid ) /* parent */
      {
	 /* wait for child to die (and hope there were no others).
	    Should probably use waitpid. */
	 while ( wait ( NULL ) != -1 ) ;
	 lseek ( tmpfd, 0, SEEK_SET ); /* rewind uncompressed file */
	 if ( ( ret = fdopen ( tmpfd, "r+" ) ) == NULL )
	 {
	    (void) fprintf ( stderr, "Can't open workfile\n" );
	    exit ( -1 );
	 }
	 return ret;
      }
      else /* child */
      {
	 (void) dup2 ( tmpfd, 1 ); /* redirect standard output to temp file */
	 (void) close ( tmpfd ); /* close unnecessary file descriptor */
	 /* name of compressed file to buf */
         (void) sprintf ( buf, "%s%s", path, ZLISTEXT ) ;
         if ( stat ( buf, &s ) == 0 )
         {
           (void) sprintf ( argstring, "%s %s %s%s", ZLISTCAT, ZLISTCATOPTS, path, ZLISTEXT ) ;
           args [ 0 ] = argstring ;
           ptr = strtok ( argstring, " " ) ;
           for ( i = 1 ; ptr != NULL ; i++ )
           {
             ptr = strtok ( NULL, " " ) ;
             args [ i ] = ptr ;
           }
	   execvp ( args [ 0 ], args );
	   (void) fprintf ( stderr, "execvp failed\n" );
	   exit ( -1 );
         }
         else
         {
           (void) sprintf ( argstring, "%s %s %s%s", ZDBSCAT, ZDBSCATOPTS, path, ZDBSEXT ) ;
           args [ 0 ] = argstring ;
           ptr = strtok ( argstring, " " ) ;
           for ( i = 1 ; ptr != NULL ; i++ )
           {
             ptr = strtok ( NULL, " " ) ;
             args [ i ] = ptr ;
           }
	   execvp ( args [ 0 ], args );
	   (void) fprintf ( stderr, "execvp failed\n" );
	   exit ( -1 );
         }
      }
#endif /* COMPRESS */
   }
   else
     return ret ;
   return ( NULL ) ;
}


FILE *copyFile ( const char *fromName ) 
{
  int toFd;
  FILE *toFp, *fromFp ;
  char tmpfname [ ] = "mkdbXXXXXX" ;
  int c ;

  fromFp = openFile ( fromName ) ;
  if ( ( toFd = mkstemp ( tmpfname ) ) == -1 )
    moviedbWriteError ( tmpfname ) ;
  unlink ( tmpfname ) ;
  toFp = fdopen ( toFd, "w+" ) ;

  while ( ( c = fgetc ( fromFp ) ) != EOF )
    fputc ( c, toFp ) ;
  (void) fclose ( fromFp ) ;
  
  rewind ( toFp ) ;
  return ( toFp ) ;
}


long findSOL ( FILE *stream, long pos )
{
   int ch;

   if ( pos > 0 )
   {
      do {
         pos--;
         (void) fseek ( stream, pos, SEEK_SET ) ;
         ch = fgetc ( stream ) ;
      } while ( (ch != '\n') && (ch != EOF) && (pos > 0) ) ;
      if ( pos != 0 )
          pos++ ;
      else
           (void) fseek ( stream, pos, SEEK_SET ) ;
   }
   else
   {
       pos = 0 ;
       (void) fseek ( stream, pos, SEEK_SET ) ;
   }
   return pos ;
}


int fieldCaseCompare ( unsigned char *s1, unsigned char *s2 )
{
  if ( s2 == NULL )
    return ( 1 ) ;

  while ( upperCase  ( *s1 ) == upperCase  ( *s2 ) )
  {
    if ( *(s1+1) == FSEP && *(s2+1) == '\0' )
      return ( 0 ) ;
    s1++; s2++ ;
  }

  if ( *s1 == FSEP )
    return ( -1 ) ;
  else if ( *s2 == '\0' )
    return ( 1 ) ;
  else
    return ( upperCase ( *s1 ) - upperCase  ( *s2 ) ) ;
}


int yearFieldCaseCompare ( unsigned char *s1, unsigned char *s2 )
{
  if ( s2 == NULL )
    return ( 1 ) ;

  while ( upperCase  ( *s1 ) == upperCase  ( *s2 ) )
  {
    if ( *(s1+1) == FSEP && *(s2+1) == '\0' )
      return ( 0 ) ;
    s1++; s2++ ;
    if ( *s2 == '\0' && *s1 == ' ' && *(s1+1) == '(' )
      return ( 0 ) ;
  }

  if ( *s1 == FSEP )
    return ( -1 ) ;
  else if ( *s2 == '\0' )
    return ( 1 ) ;
  else
    return ( upperCase ( *s1 ) - upperCase  ( *s2 ) ) ;
}


struct titleSearchRec *newTitleSearchRec ( void )
{
  struct titleSearchRec *rrec ;
  int i ;

  if ( ( rrec = (struct titleSearchRec *) malloc ( sizeof ( struct titleSearchRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;

  rrec -> title = NULL ;
  rrec -> titleKey = NOTITLE ;
  rrec -> nameCount = 0 ;
  rrec -> year = 0 ;
  rrec -> attrKey = NOATTR ;
  rrec -> castStatus = unknown ;
  rrec -> crewStatus = unknown ;
  rrec -> attr = NULL ;
  rrec -> mrr = NULL ;
  rrec -> aka = NULL ;
  rrec -> plot = NULL ;
  rrec -> literature = NULL ;
  rrec -> business = NULL ;
  rrec -> laserdisc = NULL ;
  rrec -> noOfLinks = 0 ;
  rrec -> links = NULL ;
  rrec -> next = NULL ;
    
  for ( i = 0 ; i < MAXTITLERESULTS ; i++ )
  {
    rrec -> entries [ i ] . nameKey = NONAME ;
    rrec -> entries [ i ] . attrKey = NOATTR ;
    rrec -> entries [ i ] . name = NULL ;
    rrec -> entries [ i ] . attr = NULL ;
    rrec -> entries [ i ] . cname = NULL ;
    rrec -> entries [ i ] . lineOrder = NOPOS ;
    rrec -> entries [ i ] . groupOrder = NOPOS ;
    rrec -> entries [ i ] . subgroupOrder = NOPOS ;
  }

  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    rrec -> results [ i ] . count = 0 ;
    rrec -> results [ i ] . base = -1 ;
  }

  for ( i = 0 ; i < NO_OF_TRIV_LISTS ; i++ )
  {
    rrec -> trivflags [ i ] = FALSE ;
    rrec -> trivlists [ i ] = NULL ;
  }

  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
  {
    rrec -> titleInfoFlags [ i ] = FALSE ;
    rrec -> titleInfo [ i ] = NULL ;
  }

  return ( rrec ) ;
}


struct nameSearchRec *newNameSearchRec ( void )
{
  struct nameSearchRec *rrec ;
  int i ;

  if ( ( rrec = (struct nameSearchRec *) malloc ( sizeof ( struct nameSearchRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;

  rrec -> name = NULL ;
  rrec -> nameKey = NONAME ;
  rrec -> firstMatch = -1 ;
  rrec -> biography = NULL ;
  rrec -> aka = NULL ;
  rrec -> next = NULL ;
  for ( i = 0 ; i < NO_OF_FILMOGRAPHY_LISTS ; i++ )
  {
    rrec -> lists [ i ] = NULL ;
    rrec -> searchFlags [ i ] = FALSE ;
  }
  return ( rrec ) ;
}


struct plotRec *newPlotRec ( void )
{
  struct plotRec *retval ;

  if ( ( retval = (struct plotRec *) malloc ( sizeof ( struct plotRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct outlineRec *newOutlineRec ( void )
{
  struct outlineRec *retval ;

  if ( ( retval = (struct outlineRec *) malloc ( sizeof ( struct outlineRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct mrrRec *newMrrRec ( void )
{
  struct mrrRec *retval ;

  if ( ( retval = (struct mrrRec *) malloc ( sizeof ( struct mrrRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct listRec *newListRec ( void )
{
  struct listRec *retval ;

  if ( ( retval = (struct listRec *) malloc ( sizeof ( struct listRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct akaRec *newAkaRec ( void )
{
  struct akaRec *retval ;

  if ( ( retval = (struct akaRec *) malloc ( sizeof ( struct akaRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct akaNameRec *newAkaNameRec ( void )
{
  struct akaNameRec *retval ;

  if ( ( retval = (struct akaNameRec *) malloc ( sizeof ( struct akaNameRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct personRec *newPersonRec ( void )
{
  struct personRec *retval ;

  if ( ( retval = (struct personRec *) malloc ( sizeof ( struct personRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct lineRec *newLineRec ( void )
{
  struct lineRec *retval ;

  if ( ( retval = (struct lineRec *) malloc ( sizeof ( struct lineRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct bioRec *newBioRec ( void )
{
  struct bioRec *retval ;

  if ( ( retval = (struct bioRec *) malloc ( sizeof ( struct bioRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct bioMiscRec *newBioMiscRec ( void )
{
  struct bioMiscRec *retval ;

  if ( ( retval = (struct bioMiscRec *) malloc ( sizeof ( struct bioMiscRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}


struct laserRec *newLaserRec ( void )
{
  struct laserRec *retval ;

  if ( ( retval = (struct laserRec *) malloc ( sizeof ( struct laserRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;
    
  return ( retval ) ;
}



struct titleInfoRec *newTitleInfoRec ( void )
{
  struct titleInfoRec *retval ;

  if ( ( retval = (struct titleInfoRec *) malloc ( sizeof ( struct titleInfoRec ) ) ) == NULL )
    moviedbError ( "error: out of memory" ) ;

  retval -> text = NULL ;
  retval -> attr = NULL ;
    
  return ( retval ) ;
}


void displayTitleYear ( char *title, int year ) 
{
   char *bptr ;

   if ( ( bptr = strchr ( title, '(' ) ) != NULL )
   {
     if ( *(bptr+1) == '1' && *(bptr+2) == '9' )
       (void) printf ( "%s", title ) ;
      else if ( *(bptr+1) == '2' && *(bptr+2) == '0' )
        (void) printf ( "%s", title ) ;
      else if ( *(bptr+1) == '1' && *(bptr+2) == '8' )
        (void) printf ( "%s", title ) ;
     else 
     {
       *bptr = '\0' ;
       if ( year > 0 )
         (void) printf ( "%s(%4d) (%s", title, year, bptr + 1) ;
       else
         (void) printf ( "%s(\?\?\?\?) (%s", title, bptr + 1 ) ;
       *bptr = '(' ;
     }
   }
   else
     if ( year > 0 )
       (void) printf ( "%s (%4d)", title, year ) ;
     else
       (void) printf ( "%s (\?\?\?\?)", title ) ;
}


char *caseStrStr ( unsigned char *line, unsigned char *str ) 
{
  unsigned char upperLine [ MXLINELEN ] ;
  unsigned char upperStr [ MXLINELEN ] ;
  unsigned char *from, *to ;

  for ( from = line, to = upperLine ; *from ; from++, to++ )
    if ( islower ( *from ) )
      *to = toupper ( *from ) ;
    else
      *to = *from ;
  *to = '\0' ;

  for ( from = str, to = upperStr ; *from ; from++, to++ )
    if ( islower ( *from ) )
      *to = toupper ( *from ) ;
    else
      *to = *from ;
  *to = '\0' ;

  return ( strstr ( upperLine, upperStr ) ) ;
}


char *mapNameKeyToText ( NameID nameKey, FILE *nameKeyFp, FILE *nameIndexFp )
{
  char line [ MXLINELEN ] ;
  long offset ;

  (void) fseek ( nameIndexFp, 4 * nameKey, SEEK_SET ) ;
  offset = getFullOffset ( nameIndexFp ) ;
  (void) fseek ( nameKeyFp, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, nameKeyFp ) ;
  return ( duplicateField ( line ) ) ;
}


char *mapTitleKeyToText ( TitleID titleKey, FILE *titleKeyFp, FILE *titleIndexFp )
{
  char line [ MXLINELEN ] ;
  long offset ;

  (void) fseek ( titleIndexFp, 4 * titleKey, SEEK_SET ) ;
  offset = getFullOffset ( titleIndexFp ) ;
  (void) fseek ( titleKeyFp, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, titleKeyFp ) ;
  return ( duplicateField ( line ) ) ;
}


char *mapAttrKeyToText ( AttributeID attrKey, FILE *attrKeyFp, FILE *attrIndexFp )
{
  char line [ MXLINELEN ] ;
  long offset ;

  if ( attrKey != NOATTR )
  {
    (void) fseek ( attrIndexFp, 4 * attrKey, SEEK_SET ) ;
    offset = getFullOffset ( attrIndexFp ) ;
    (void) fseek ( attrKeyFp, offset, SEEK_SET ) ;
    (void) fgets ( line, MXLINELEN, attrKeyFp ) ;
    return ( duplicateField ( line ) ) ;
  }
  else
    return ( NULL ) ;
}


char *mapfastNameKeyToText ( NameID nameKey, FILE *nameKeyFp, FILE *nameIndexFp )
{
  static char line [ MXLINELEN ] ;
  long offset ;
  char *p;

  (void) fseek ( nameIndexFp, 4 * nameKey, SEEK_SET ) ;
  offset = getFullOffset ( nameIndexFp ) ;
  (void) fseek ( nameKeyFp, offset, SEEK_SET ) ;
  (void) fgets ( line, MXLINELEN, nameKeyFp ) ;
  if ( (p = strchr ( line, FSEP ) ) == NULL )
    return ( NULL ) ;
  *p = '\0' ;
  return ( line ) ;
}


char *mapfastAttrKeyToText ( AttributeID attrKey, FILE *attrKeyFp, FILE *attrIndexFp )
{
  static char line [ MXLINELEN ] ;
  long offset ;
  char *p ;

  if ( attrKey != NOATTR )
  {
    (void) fseek ( attrIndexFp, 4 * attrKey, SEEK_SET ) ;
    offset = getFullOffset ( attrIndexFp ) ;
    (void) fseek ( attrKeyFp, offset, SEEK_SET ) ;
    (void) fgets ( line, MXLINELEN, attrKeyFp ) ;
    if ( (p = strchr ( line, FSEP ) ) == NULL )
      return ( NULL ) ;
    *p = '\0' ;
    return ( line ) ;
  }
  else
    return ( NULL ) ;
}


void displayFormatPlain ( struct formatRec listData [], int count )
{
  int i, len ;

  if ( count < 1 )
    return ;

  (void) printf ( "%s", listData [ 0 ] . name ) ;
  len = strlen ( listData [ 0 ] . name ) ;
  if ( len >= 16 )
    (void) printf ( "\t" ) ;
  else if ( len >= 8 )
    (void) printf ( "\t\t" ) ;
  else
    (void) printf ( "\t\t\t" ) ;
    
  (void) printf ( "%s", listData [ 0 ] . title ) ;
  if ( listData [ 0 ] . attr != NULL )
    (void) printf ( " %s\n", listData [ 0 ] . attr ) ;
  else
    (void) printf ( "\n" ) ;

  for ( i = 1 ; i < count ; i++ )
  {
    (void) printf ( "\t\t\t" ) ;
    (void) printf ( "%s", listData [ i ] . title ) ;
    if ( listData [ i ] . attr != NULL )
      (void) printf ( " %s\n", listData [ i ] . attr ) ;
    else
      (void) printf ( "\n" ) ;
  }

  (void) printf ( "\n" ) ;
}


void freeFormatData ( struct formatRec listData [], int count )
{
  int i ;

  for ( i = 0 ; i < count ; i++ )
  {
    free ( (void*) listData [ i ] . name ) ;
    free ( (void*) listData [ i ] . title ) ;
    free ( (void*) listData [ i ] . attr ) ;
  }
}


void initialiseConstraints ( struct searchConstraints *sc ) 
{
  int i ;

  sc -> caseSen = FALSE ;
  sc -> moviesOnly = FALSE ;
  sc -> mrrMatch = FALSE ;
  sc -> yearFrom = 0 ;
  sc -> yearTo = 9999 ;
  sc -> voteMin = 0 ;
  sc -> voteMax = 99999 ;
  sc -> subString [ 0 ] = '\0' ;
  for ( i = 0 ; i < NO_OF_TITLE_INFO_LISTS ; i++ )
    sc -> titleInfoString [ i ] [ 0 ] = '\0' ;
}


/*------------------------------------------------------------------------------
 *  FUNCTION NAME:    txt_f_justify
 *
 *  PARAMETERS:
 *     int   left   - Left margin for output
 *     int   right  - Right margin for output
 *     char *text   - Text to be formatted
 *
 *  RETURNS:
 *     char *       - Pointer to \0 terminated buffer containing justified text
 *     NULL         - Failure.
 *
 *  GLOBALS USED:
 *     None.
 *
 *  FUNCTIONS CALLED:
 *
 *  DESCRIPTION:
 *     This function takes the _text_ and fully justifies it between the
 *     _left_ and _right_ margins.  Carriage returns within _text_ are seen
 *     as paragraph markers.  
 *
 *     Once the text has been justified, it is returned to the calling
 *     routine.  It is the responsibility of the caller to free the text
 *     after use.
 *
 *------------------------------------------------------------------------------*/
char *
txt_f_justify(int left, int right, char *text)
{
    int   width = 0;                        /*  Width of justification space  */
    int   output_len = 0;                   /*  Length of output buffer       */
    char *output = NULL;                    /*  Pointer to output buffer      */
    char *word = NULL;                      /*  Pointer to next input word    */
    char  line[80] = "";                    /*  Buffer for formatted data     */
    int   line_len = 0;                     /*  Length of format buffer       */
    char *txt_f_justify_line();             /*  Function to justify one line  */
    
    /*  Calculate width of justfified line  */

    width = right - left;

    /*  
     *  Calculate current length of text and add extra space for formatting, be
     *  over generous so that space only allocated once.
     */

    output_len = strlen(text) * 2;
   
    /*  Create the output buffer  */

    output = (char *)malloc(sizeof(char) * output_len);
    if (output == NULL)
    {
        return NULL;                        /*  Creation failed so fail func. */
    }

    *output = '\0';                         /*  Initialise the output buffer  */

    word = text;

    /*  Ok, take each word in input and add to line buffer ready for format   */

    while (*word != '\0')
    {
        char inword[80];                              /*  Next word in input  */
        char *chr = inword;                           /*  Position in word    */
        int word_len = 0;                             /*  Length of word      */

        /*  Get next word  */

        while ((*word != ' ') && (*word != '\n') && (*word != '\0'))
        {
            *chr = *word;
            chr++, word++, word_len++;
        }

        *chr = '\0';                                  /*  Terminate word      */

        if (word_len == 0)
        {
            /*  Either '\n' or '\0' or ' '  */

            switch (*word)
            {
                case ' '  : word++;                   /*  Skip spaces         */
                            break;

                /*  Paragraph mark add line to output and a blank line  */

                case '\n' : strcat(output, 
                                   txt_f_justify_line(left, right, line, 0)); 
                            strcat(output, "\n");
                            chr = inword;
                            word_len = 0;
                            word++;
                            *line = '\0';
                            line_len = 0;
                            break;
 
                /*  End of input - just quit  */

                case '\0' : break;
            }
        }
        else
        {
            /*  If space in output buffer, add to format line  */

            if ((word_len + line_len + 1) <= width)
            {
                strcat(line, inword);
                strcat(line, " ");
                line_len += word_len + 1;
            }
            else
            {
                /*  Format current line and then put word at start of buf  */

                strcat(output, txt_f_justify_line(left, right, line, 1)); 
                strcpy(line, inword);
                strcat(line, " ");
                line_len = word_len + 1;
            }
        }
    }

    /*  Tack on last line of text  */

    if (strlen(line) > 0)
    {
        strcat(output, txt_f_justify_line(left, right, line, 0));
    }

    return output;
}   

/*------------------------------------------------------------------------------
 *  FUNCTION NAME:     justify_line
 *
 *  PARAMETERS:
 *     int   left    - left margin
 *     int   right   - right margin
 *     char *text    - text
 *     int   justify - Whether to justify or just indent
 *
 *  RETURNS:
 *     char *        - pointer to formatted text
 *
 *  GLOBALS USED:
 *     None.
 *
 *  FUNCTIONS CALLED:
 *
 *  DESCRIPTION:
 *     This function formats one line of text between the specified margins.
 *     If _justify_ is 1 then justification occurs, otherwise the text is
 *     merely indented to the left margin.
 *
 *     Left/Right padding alternates on subsequent lines to improve 
 *     readability.
 *
 *------------------------------------------------------------------------------*/
char *
txt_f_justify_line(int left, int right, char *line, int justify)
{
    static char output[80];
    static int fill = 0;                 /*  Left fill or right fill          */
    char *temp = duplicateString(line);

    memset(output, 0, sizeof(char) * 80);
    memset(output, ' ', left);
    
    if (justify == 1)
    {
        int len = strlen(temp);
        int spaces = 0;
        char *chr = temp;                /*  Position in input line           */
        char *out = output + left;       /*  Position on output temp          */
        int num_spc = 0;                 /*  Number of spaces in line         */
        int spc[40];                     /*  Number of spaces at each point   */
        int spc_entry = 0;               /*  Current entry in space table     */
        
        while(temp[len - 1] == ' ')
        {
            temp[len - 1] = '\0';
            len--;
        }

        /*  If filling from right, reset pointers  */

        if (fill == 0)
        {
            chr = temp + len - 1;
            out = output + right - 1;
        }
  
        /*  Calculate number of spaces in line  */

        {
            char *space;
 
            for (space = temp; *space != '\0'; space++)
                if (*space == ' ') 
                    num_spc++;
        }

        /*  Calculate number of padding spaces to add  */

        spaces = right - left - len;

        /*  Fill in spaces table  */

        {
            int entry = 0;
            int spc_cnt = spaces;
 
            for (entry = 0; entry < num_spc; entry++)
                spc[entry] = 0;

            /*  Add in extra spaces  */

            entry = 0;

            while(spc_cnt != 0)
            {
                spc[entry]++;
                if ( num_spc > 0 )
                  entry = (entry + 1) % num_spc;
                spc_cnt--;
            }
        }

        /*  Copy line to output, justifying as we go  */

        while ((fill == 1) ? (*chr != '\0') : (chr >= temp))
        {
            *out = *chr;

            if ((*chr == ' ') && (spaces > 0))
            {
                 int n;

                 /*  Add in the required number of spaces  */

                 for (n = 0; n < spc[spc_entry]; n++)
                 {
                     (fill == 1) ? out++ : out--;

                    *out = ' ';
                 }
 
                 spc_entry++;   /*  Move to next space entry              */
                 spaces--;      /*  Number of fillers required, one less  */
            }

            (fill == 1) ? (chr++, out++) : (chr--, out--);
         }
         strcat(output, "\n");
    }
    else
    {
        strcat(output, temp);
        strcat(output, "\n");
    }

    fill = abs(fill - 1);        /*  Toggle left/right padding  */

    free(temp);

    return output;
}   

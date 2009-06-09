#define upperCase(ch) (islower (ch) ? toupper (ch) : ch)

extern struct titleOrderRec titleOrderData [ NO_OF_FILMOGRAPHY_LISTS ] ;
extern struct filmographyOptRec filmographyOptions [ NO_OF_SEARCH_OPTS ] ;
extern struct filmographyDefRec filmographyDefs [ NO_OF_FILMOGRAPHY_LISTS ] ;
extern struct triviaDefRec triviaDefs [ NO_OF_TRIV_LISTS ] ;
extern struct titleInfoDefRec titleInfoDefs [ NO_OF_TITLE_INFO_LISTS ] ;
extern struct movieLinksDefRec movieLinkDefs [ NO_OF_LINK_TYPES ] ;

char *duplicateString ( const char *str ) ;
char *appendString ( char *original, char *extension ) ;
char *appendTag ( char *original, char *extension ) ;
char* duplicateField ( const char *str ) ;
char *constructFilename ( char *filename, const char *base, const char *extension ) ;
int isReadable ( const char *fname ) ;
int caseCompare ( unsigned char *s1, unsigned char *s2 ) ;

void putByte ( int i, FILE *stream ) ;
void putPosition ( int i, FILE *stream ) ;
void putTitle ( TitleID titleKey, FILE *stream ) ;
void putAttr ( AttributeID attrKey, FILE *stream ) ;
void putInt ( int i, FILE *stream ) ;
void putOffset ( long offset, FILE *stream ) ;
void putFullOffset ( long offset, FILE *stream ) ;
void putName ( NameID nameKey, FILE *stream ) ;
void putString ( char *str, FILE *stream ) ;
void putFilmographyCounts ( int noWithAttr, int noWithoutAttr, FILE *stream ) ;

NameID getName ( FILE *stream ) ;
long getOffset ( FILE *stream ) ;
long getFullOffset ( FILE *stream ) ;
int getByte ( FILE *stream ) ;
int getPosition ( FILE *stream ) ;
TitleID getTitle (FILE *stream) ;
AttributeID getAttr (FILE *stream) ;
int getInt ( FILE *stream ) ;
void getFilmographyCounts ( FILE *stream, int *noWithAttr, int*noWithoutAttr ) ;
char *getString ( int len, FILE *stream ) ;
void getStringFast ( int len, FILE *stream, char *line ) ;

void stripEOL ( char *str ) ;
void stripSep ( char *str ) ;

void moviedbError ( const char *message ) ;
void moviedbWriteError ( const char *filename ) ;
void moviedbUsage ( const char *s1, const char *s2, const char *s3, const char *s4, const char *s5, const char *s6 ) ;

void upperCaseString ( char *str ) ;

FILE *openFile ( const char *path ) ;
FILE *writeFile ( const char *path ) ;
FILE *copyFile ( const char *fromName ) ;

long findSOL ( FILE *stream, long pos ) ;
int fieldCaseCompare ( unsigned char *s1, unsigned char *s2 ) ;
int yearFieldCaseCompare ( unsigned char *s1, unsigned char *s2 ) ;

struct titleSearchRec *newTitleSearchRec ( void ) ;
struct nameSearchRec *newNameSearchRec ( void ) ;
struct plotRec *newPlotRec ( void ) ;
struct outlineRec *newOutlineRec ( void ) ;
struct mrrRec *newMrrRec ( void ) ;
struct listRec *newListRec ( void ) ;
struct akaRec *newAkaRec ( void ) ;
struct akaNameRec *newAkaNameRec ( void ) ;
struct personRec *newPersonRec ( void ) ;
struct lineRec *newLineRec ( void ) ;
struct bioRec *newBioRec ( void ) ;
struct bioMiscRec *newBioMiscRec ( void ) ;
struct laserRec *newLaserRec ( void ) ;
struct titleInfoRec *newTitleInfoRec ( void ) ;

void displayTitleYear ( char *title, int year ) ;

char *caseStrStr ( unsigned char *line, unsigned char *str ) ;

char *mapNameKeyToText ( NameID nameKey, FILE *nameKeyFp, FILE *nameIndexFp ) ;
char *mapTitleKeyToText ( TitleID titleKey, FILE *titleKeyFp, FILE *titleIndexFp ) ;
char *mapAttrKeyToText ( AttributeID attrKey, FILE *attrKeyFp, FILE *attrIndexFp ) ;
char *mapfastNameKeyToText ( NameID nameKey, FILE *nameKeyFp, FILE *nameIndexFp ) ;
char *mapfastAttrKeyToText ( AttributeID attrKey, FILE *attrKeyFp, FILE *attrIndexFp ) ;

void displayFormatPlain ( struct formatRec listData [], int count ) ;
void freeFormatData ( struct formatRec listData [], int count ) ;

void initialiseConstraints ( struct searchConstraints *sc ) ;

char *txt_f_justify(int left, int right, char *text) ;

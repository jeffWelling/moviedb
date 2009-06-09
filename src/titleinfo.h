void freeTitleInfo ( struct titleInfoRec *rec ) ;
void addTitleInfoToTitleSearch ( struct titleSearchRec *tchain ) ;
int constrainTitleInfo ( FILE *dbFp, FILE *indexFp, TitleID titleKey, char *string ) ;
int readTitleInfoIndex ( FILE *idxFp, TitleID *titleKey, long *offset ) ;
int checkConstraints ( FILE *dbFp, long offset, TitleID titleKey, char *string ) ;

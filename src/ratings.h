struct mrrRec *cloneMrrData ( struct mrrRec *mrec ) ;
void addRatingsToNameSearch (struct nameSearchRec *chain) ;
void addRatingsToTitleSearch (struct titleSearchRec *tchain) ;
int readMrrRec ( FILE *mrrFp, TitleID *mrrKey, struct mrrRec *mrr ) ;
int smrrLformatSort ( struct formatRec *r1, struct formatRec *r2 ) ;
int vmrrLformatSort ( struct formatRec *r1, struct formatRec *r2 ) ;
TitleID readRatingsDb ( struct mrrDbRec array [] ) ;
TitleID ratingsDbSize ( void ) ;
TitleID readVotesDb ( struct voteDbRec array [] ) ;

void freeTitleChain ( struct titleSearchRec *chain ) ;
void addTitleChainOpts ( struct titleSearchRec *tchain, struct titleSearchOptRec options ) ;
void addTitleChainTrivOpts ( struct titleSearchRec *tchain, int trivopts [ ] ) ;
void addTitleChainTitleInfoOpts ( struct titleSearchRec *tchain, int titleInfoOpts [ ] ) ;
struct titleSearchRec *addTitleSearchRec ( struct titleSearchRec *tchain, char *title ) ;
void processTitleSearch ( struct titleSearchRec *tchain ) ;
struct titleSearchRec *combineTitleResults ( struct titleSearchRec *tchain ) ;
struct titleSearchRec *makeTitleChain ( struct nameSearchRec *nrec, struct titleSearchOptRec options, int trivopts  [ ], struct nameSearchOptRec noptions, int titleInfoOpts [ ] ) ;
struct titleSearchRec *addTitleFile ( char *fname ) ;

void freeNameSearchChain ( struct nameSearchRec *chain ) ;
void addNameChainOpts (struct nameSearchRec *nchain, struct nameSearchOptRec options) ;
struct nameSearchRec *addNameSearchRec (struct nameSearchRec *chain, char *name, int listopt) ;
void processFilmographySearch (struct nameSearchRec *chain) ;
void sortListResults ( struct nameSearchRec *chain ) ;
struct nameSearchRec *combineListResults ( struct nameSearchRec *chain ) ;
struct nameSearchRec *makeNameChain ( struct titleSearchRec *trec ) ;
void constrainFilmographySearch ( struct searchConstraints *constraints, struct nameSearchRec *nchain ) ;

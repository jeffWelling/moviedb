DBS = actors.data actresses.data aka-names.data aka-titles.data \
      biographies.data certificates.data cinematographers.data composers.data \
      costume-designers.data crazy-credits.data directors.data editors.data \
      genres.data keywords.data goofs.data miscellaneous.data movies.data \
      plot.data producers.data production-companies.data \
      production-designers.data \
      quotes.data ratings.data running-times.data soundtracks.data \
      trivia.data votes.data writers.data sound-mix.data color-info.data \
      countries.data release-dates.data locations.data literature.data \
      technical.data movie-links.data taglines.data complete-cast.data \
      alternate-versions.data language.data special-effects-companies.data \
      laserdisc.data business.data distributors.data complete-crew.data


NAMES = actors.names actresses.names cinematographers.names composers.names \
        costume-designers.names directors.names editors.names \
        miscellaneous.names producers.names production-designers.names \
        writers.names

TITLES = actors.titles actresses.titles cinematographers.titles \
         composers.titles costume-designers.titles directors.titles \
         editors.titles miscellaneous.titles producers.titles \
         production-designers.titles writers.titles

IDX = aka-names.index aka-titles.index attributes.index biographies.index \
      certificates.index crazy-credits.index genres.index keywords.index \
      goofs.index \
      names.index plot.index production-companies.index quotes.index \
      running-times.index soundtracks.index titles.index trivia.index \
      sound-mix.index color-info.index countries.index release-dates.index \
      locations.index literature.index technical.index taglines.index \
      alternate-versions.index language.index special-effects-companies.index \
      laserdisc.index business.index distributors.index

KEY = attributes.key names.key titles.key key.counts

# The following are manual expansion of DBS 
# as some make's don't support $(DBS:.dbs=.dbs$(ZEXTENSION)).

ZDBS = actors.data$(ZDBSEXT) actresses.data$(ZDBSEXT) \
       aka-names.data$(ZDBSEXT) aka-titles.data$(ZDBSEXT) \
       biographies.data$(ZDBSEXT) certificates.data$(ZDBSEXT) \
       cinematographers.data$(ZDBSEXT) composers.data$(ZDBSEXT) \
       costume-designers.data$(ZDBSEXT) crazy-credits.data$(ZDBSEXT) \
       directors.data$(ZDBSEXT) editors.data$(ZDBSEXT) genres.data$(ZDBSEXT) \
       keywords.data$(ZDBSEXT) \
       goofs.data$(ZDBSEXT) miscellaneous.data$(ZDBSEXT) \
       movies.data$(ZDBSEXT) plot.data$(ZDBSEXT) producers.data$(ZDBSEXT) \
       production-companies.data$(ZDBSEXT) \
       special-effects-companies.data$(ZDBSEXT) \
       production-designers.data$(ZDBSEXT) quotes.data$(ZDBSEXT) \
       ratings.data$(ZDBSEXT) running-times.data$(ZDBSEXT) \
       soundtracks.data$(ZDBSEXT) trivia.data$(ZDBSEXT) votes.data$(ZDBSEXT) \
       writers.data$(ZDBSEXT) sound-mix.data$(ZDBSEXT) \
       color-info.data$(ZDBSEXT) countries.data$(ZDBSEXT) \
       release-dates.data$(ZDBSEXT) locations.data$(ZDBSEXT) \
       literature.data$(ZDBSEXT) technical.data$(ZDBSEXT) \
       movie-links.data$(ZDBSEXT) taglines.data$(ZDBSEXT) \
       complete-cast.data$(ZDBSEXT) alternate-versions$(ZDBSEXT) \
       language.data$(ZDBSEXT) laserdisc.data$(ZDBSEXT) \
       business.data$(ZDBSEXT) distributors.data$(ZDBSEXT) \
       complete-crew.data$(ZDBSEXT)

ZNAMES = actors.names$(ZDBSEXT) actresses.names$(ZDBSEXT) \
         cinematographers.names$(ZDBSEXT) composers.names$(ZDBSEXT) \
         costume-designers.names$(ZDBSEXT) directors.names$(ZDBSEXT) \
         editors.names$(ZDBSEXT) miscellaneous.names$(ZDBSEXT) \
         producers.names$(ZDBSEXT) production-designers.names$(ZDBSEXT) \
         writers.names$(ZDBSEXT)

ZTITLES = actors.titles$(ZDBSEXT) actresses.titles$(ZDBSEXT) \
          cinematographers.titles$(ZDBSEXT) composers.titles$(ZDBSEXT) \
          costume-designers.titles$(ZDBSEXT) directors.titles$(ZDBSEXT) \
          editors.titles$(ZDBSEXT) miscellaneous.titles$(ZDBSEXT) \
          producers.titles$(ZDBSEXT) production-designers.titles$(ZDBSEXT) \
          writers.titles$(ZDBSEXT)

ZIDX = aka-names.index$(ZDBSEXT) aka-titles.index$(ZDBSEXT) \
       attributes.index$(ZDBSEXT) biographies.index$(ZDBSEXT) \
       certificates.index$(ZDBSEXT) crazy-credits.index$(ZDBSEXT) \
       genres.index$(ZDBSEXT) keywords.index$(ZDBSEXT) goofs.index$(ZDBSEXT) \
       names.index$(ZDBSEXT) \
       production-companies.index$(ZDBSEXT) plot.index$(ZDBSEXT) \
       quotes.index$(ZDBSEXT)  running-times.index$(ZDBSEXT) \
       soundtracks.index$(ZDBSEXT) titles.index$(ZDBSEXT) \
       trivia.index$(ZDBSEXT) sound-mix.index$(ZDBSEXT) \
       color-info.index$(ZDBSEXT) countries.index$(ZDBSEXT) \
       release-dates.index$(ZDBSEXT) locations.index$(ZDBSEXT) \
       literature.index$(ZDBSEXT) technical.index$(ZDBSEXT) \
       taglines.index$(ZDBSEXT) language.index$(ZDBSEXT) \
       alternate-versions.index$(ZDBSEXT) \
       special-effects-companies.index$(ZDBSEXT) \
       laserdisc.index$(ZDBSEXT) business.index$(ZDBSEXT) \
       distributors.index$(ZDBSEXT)

ZKEY = attributes.key$(ZDBSEXT) names.key$(ZDBSEXT) titles.key$(ZDBSEXT) \
        key.counts$(ZDBSEXT)

all :
	@echo "I don't think you've read the install doc..."

# ----------------------------------------------------------------

# do-movies must be first
DOALL = do-movies do-actors do-actress do-cine do-composers \
        do-directs do-writers do-editors do-prodes do-costdes do-prdcr \
        do-misc do-aka do-naka do-plot do-bio do-crazy do-goofs do-quotes \
        do-stracks do-trivia do-ratings do-votes do-times do-certs do-genres \
        do-keywords \
        do-prodco do-color do-mix do-cntry do-rel do-loc do-lit do-tech \
        do-link do-tag do-castcom do-crewcom do-vers do-lang do-sfxco \
        do-bus do-ld do-dist

databases : $(DOALL) touch-dbs recompress

touch-dbs :
	$(ETCDIR)mkdb $(MKDBOPT) -create

do-movies :
	@$(ETCDIR)make-base movies $(RAWDIR)movies.list "$(ETCDIR)mkdb $(MKDBOPT) -movie" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)movies.list $(RAWDIR)movies.list$(ZLISTEXT) 

do-actors :
	@$(ETCDIR)make-base actors $(RAWDIR)actors.list "$(ETCDIR)mkdb $(MKDBOPT) -acr" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)actors.list $(RAWDIR)actors.list$(ZLISTEXT) 

do-actress :
	@$(ETCDIR)make-base actresses $(RAWDIR)actresses.list "$(ETCDIR)mkdb $(MKDBOPT) -acs" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)actresses.list $(RAWDIR)actresses.list$(ZLISTEXT) 

do-directs :
	@$(ETCDIR)make-base directors $(RAWDIR)directors.list "$(ETCDIR)mkdb $(MKDBOPT) -dir" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)directors.list $(RAWDIR)directors.list$(ZLISTEXT) 

do-writers :
	@$(ETCDIR)make-base writers $(RAWDIR)writers.list "$(ETCDIR)mkdb $(MKDBOPT) -write" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)writers.list $(RAWDIR)writers.list$(ZLISTEXT) 

do-composers :
	@$(ETCDIR)make-base composers $(RAWDIR)composers.list "$(ETCDIR)mkdb $(MKDBOPT) -comp" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)composers.list $(RAWDIR)composers.list$(ZLISTEXT) 

do-cine :
	@$(ETCDIR)make-base cinematographers $(RAWDIR)cinematographers.list "$(ETCDIR)mkdb $(MKDBOPT) -cine" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)cinematographers.list $(RAWDIR)cinematographers.list$(ZLISTEXT) 

do-prdcr :
	@$(ETCDIR)make-base producers $(RAWDIR)producers.list "$(ETCDIR)mkdb $(MKDBOPT) -prdcr" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)producers.list $(RAWDIR)producers.list$(ZLISTEXT) 

do-misc :
	@$(ETCDIR)make-base miscellaneous $(RAWDIR)miscellaneous.list "$(ETCDIR)mkdb $(MKDBOPT) -misc" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)miscellaneous.list $(RAWDIR)miscellaneous.list$(ZLISTEXT) 

do-aka :
	@$(ETCDIR)make-base aka-titles $(RAWDIR)aka-titles.list "$(ETCDIR)mkdb $(MKDBOPT) -aka" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)aka-titles.list $(RAWDIR)aka-titles.list$(ZLISTEXT) 

do-naka :
	@$(ETCDIR)make-base aka-names $(RAWDIR)aka-names.list "$(ETCDIR)mkdb $(MKDBOPT) -naka" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)aka-names.list $(RAWDIR)aka-names.list$(ZLISTEXT) 

do-ratings :
	@$(ETCDIR)make-base ratings $(RAWDIR)ratings.list "$(ETCDIR)mkdb $(MKDBOPT) -mrr" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)ratings.list $(RAWDIR)ratings.list$(ZLISTEXT) 

do-votes :
	@[ -f $(RAWDIR)votelog -o -f "$(RAWDIR)votelog$(ZLISTEXT)" ] || touch $(RAWDIR)votelog
	@$(ETCDIR)make-base votes $(RAWDIR)votelog "$(ETCDIR)mkdb $(MKDBOPT) -votes" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"

do-trivia :
	@$(ETCDIR)make-base trivia $(RAWDIR)trivia.list "$(ETCDIR)mkdb $(MKDBOPT) -triv" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)trivia.list $(RAWDIR)trivia.list$(ZLISTEXT) 

do-vers :
	@$(ETCDIR)make-base alternate-versions $(RAWDIR)alternate-versions.list "$(ETCDIR)mkdb $(MKDBOPT) -vers" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)alternate-versions.list $(RAWDIR)alternate-versions.list$(ZLISTEXT) 

do-plot :
	@$(ETCDIR)make-base plot $(RAWDIR)plot.list "$(ETCDIR)mkdb $(MKDBOPT) -plot" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)plot.list $(RAWDIR)plot.list$(ZLISTEXT) 

do-bio :
	@$(ETCDIR)make-base biographies $(RAWDIR)biographies.list "$(ETCDIR)mkdb $(MKDBOPT) -bio" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)biographies.list $(RAWDIR)biographies.list$(ZLISTEXT) 

do-crazy :
	@$(ETCDIR)make-base crazy-credits $(RAWDIR)crazy-credits.list "$(ETCDIR)mkdb $(MKDBOPT) -crazy" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)crazy-credits.list $(RAWDIR)crazy-credits.list$(ZLISTEXT) 

do-editors :
	@$(ETCDIR)make-base editors $(RAWDIR)editors.list "$(ETCDIR)mkdb $(MKDBOPT) -edit" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)editors.list $(RAWDIR)editors.list$(ZLISTEXT) 

do-prodes :
	@$(ETCDIR)make-base production-designers $(RAWDIR)production-designers.list "$(ETCDIR)mkdb $(MKDBOPT) -prodes" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)production-designers.list $(RAWDIR)production-designers.list$(ZLISTEXT) 

do-costdes :
	@$(ETCDIR)make-base costume-designers $(RAWDIR)costume-designers.list "$(ETCDIR)mkdb $(MKDBOPT) -costdes" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)costume-designers.list $(RAWDIR)costume-designers.list$(ZLISTEXT) 

do-goofs :
	@$(ETCDIR)make-base goofs $(RAWDIR)goofs.list "$(ETCDIR)mkdb $(MKDBOPT) -goof" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)goofs.list $(RAWDIR)goofs.list$(ZLISTEXT) 

do-quotes :
	@$(ETCDIR)make-base quotes $(RAWDIR)quotes.list "$(ETCDIR)mkdb $(MKDBOPT) -quote" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)quotes.list $(RAWDIR)quotes.list$(ZLISTEXT) 

do-stracks :
	@$(ETCDIR)make-base soundtracks $(RAWDIR)soundtracks.list "$(ETCDIR)mkdb $(MKDBOPT) -strack" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)soundtracks.list $(RAWDIR)soundtracks.list$(ZLISTEXT) 

do-times :
	@$(ETCDIR)make-base running-times $(RAWDIR)running-times.list "$(ETCDIR)mkdb $(MKDBOPT) -time" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)running-times.list $(RAWDIR)running-times.list$(ZLISTEXT) 

do-certs :
	@$(ETCDIR)make-base certificates $(RAWDIR)certificates.list "$(ETCDIR)mkdb $(MKDBOPT) -cert" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)certificates.list $(RAWDIR)certificates.list$(ZLISTEXT) 

do-genres :
	@$(ETCDIR)make-base genres $(RAWDIR)genres.list "$(ETCDIR)mkdb $(MKDBOPT) -genre" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)genres.list $(RAWDIR)genres.list$(ZLISTEXT) 

do-keywords :
	@$(ETCDIR)make-base keywords $(RAWDIR)keywords.list "$(ETCDIR)mkdb $(MKDBOPT) -keyword" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)keywords.list $(RAWDIR)keywords.list$(ZLISTEXT) 

do-prodco :
	@$(ETCDIR)make-base production-companies $(RAWDIR)production-companies.list "$(ETCDIR)mkdb $(MKDBOPT) -prodco" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)production-companies.list $(RAWDIR)production-companies.list$(ZLISTEXT) 

do-dist :
	@$(ETCDIR)make-base distributors $(RAWDIR)distributors.list "$(ETCDIR)mkdb $(MKDBOPT) -dist" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)distributors.list $(RAWDIR)distributors.list$(ZLISTEXT) 

do-sfxco :
	@$(ETCDIR)make-base special-effects-companies $(RAWDIR)special-effects-companies.list "$(ETCDIR)mkdb $(MKDBOPT) -sfxco" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)special-effects-companies.list $(RAWDIR)special-effects-companies.list$(ZLISTEXT) 

do-color :
	@$(ETCDIR)make-base color-info $(RAWDIR)color-info.list "$(ETCDIR)mkdb $(MKDBOPT) -color" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)color-info.list $(RAWDIR)color-info.list$(ZLISTEXT) 

do-mix :
	@$(ETCDIR)make-base sound-mix $(RAWDIR)sound-mix.list "$(ETCDIR)mkdb $(MKDBOPT) -mix" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)sound-mix.list $(RAWDIR)sound-mix.list$(ZLISTEXT) 

do-cntry :
	@$(ETCDIR)make-base countries $(RAWDIR)countries.list "$(ETCDIR)mkdb $(MKDBOPT) -cntry" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)countries.list $(RAWDIR)countries.list$(ZLISTEXT) 

do-rel :
	@$(ETCDIR)make-base release-dates $(RAWDIR)release-dates.list "$(ETCDIR)mkdb $(MKDBOPT) -rel" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)release-dates.list $(RAWDIR)release-dates.list$(ZLISTEXT) 

do-loc :
	@$(ETCDIR)make-base locations $(RAWDIR)locations.list "$(ETCDIR)mkdb $(MKDBOPT) -loc" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)locations.list $(RAWDIR)locations.list$(ZLISTEXT) 

do-lit :
	@$(ETCDIR)make-base literature $(RAWDIR)literature.list "$(ETCDIR)mkdb $(MKDBOPT) -lit" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)literature.list $(RAWDIR)literature.list$(ZLISTEXT) 

do-tech :
	@$(ETCDIR)make-base technical $(RAWDIR)technical.list "$(ETCDIR)mkdb $(MKDBOPT) -tech" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)technical.list $(RAWDIR)technical.list$(ZLISTEXT) 

do-lang :
	@$(ETCDIR)make-base language $(RAWDIR)language.list "$(ETCDIR)mkdb $(MKDBOPT) -lang" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)language.list $(RAWDIR)language.list$(ZLISTEXT) 

do-link :
	@$(ETCDIR)make-base movie-links $(RAWDIR)movie-links.list "$(ETCDIR)mkdb $(MKDBOPT) -link" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)movie-links.list $(RAWDIR)movie-links.list$(ZLISTEXT) 

do-tag :
	@$(ETCDIR)make-base taglines $(RAWDIR)taglines.list "$(ETCDIR)mkdb $(MKDBOPT) -tag" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)taglines.list $(RAWDIR)taglines.list$(ZLISTEXT) 

do-castcom :
	@$(ETCDIR)make-base complete-cast $(RAWDIR)complete-cast.list "$(ETCDIR)mkdb $(MKDBOPT) -castcom" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)complete-cast.list $(RAWDIR)complete-cast.list$(ZLISTEXT) 

do-crewcom :
	@$(ETCDIR)make-base complete-crew $(RAWDIR)complete-crew.list "$(ETCDIR)mkdb $(MKDBOPT) -crewcom" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)complete-crew.list $(RAWDIR)complete-crew.list$(ZLISTEXT) 

do-ld  :
	@$(ETCDIR)make-base laserdisc $(RAWDIR)laserdisc.list "$(ETCDIR)mkdb $(MKDBOPT) -ld" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)laserdisc.list $(RAWDIR)laserdisc.list$(ZLISTEXT) 

do-bus :
	@$(ETCDIR)make-base business $(RAWDIR)business.list "$(ETCDIR)mkdb $(MKDBOPT) -bus" "$(ZDBSEXT)" "$(ZLISTEXT)" "$(MAKE)"
	$(KEEPLIST) rm -f $(RAWDIR)business.list $(RAWDIR)business.list$(ZLISTEXT) 


# ----------------------------------------------------------------

cleandbs :
	rm -f $(DBS) $(ZDBS) $(NAMES) $(ZNAMES) $(TITLES) $(ZTITLES)
	rm -f $(IDX) $(ZIDX) $(KEY) $(ZKEY)

# ----------------------------------------------------------------

compress :
	-[ -s actors.data ] && $(ZDBSCOMPRESS) actors.data
	-[ -s actresses.data ] && $(ZDBSCOMPRESS) actresses.data
	-[ -s aka-names.data ] && $(ZDBSCOMPRESS) aka-names.data
	-[ -s aka-titles.data ] && $(ZDBSCOMPRESS) aka-titles.data
	-[ -s attributes.key ] && $(ZDBSCOMPRESS) attributes.key
	-[ -s biographies.data ] && $(ZDBSCOMPRESS) biographies.data
	-[ -s business.data ] && $(ZDBSCOMPRESS) business.data	
	-[ -s complete-cast.data ]  && $(ZDBSCOMPRESS) complete-cast.data
	-[ -s complete-crew.data ]  && $(ZDBSCOMPRESS) complete-crew.data
	-[ -s certificates.data ]  && $(ZDBSCOMPRESS) certificates.data
	-[ -s cinematographers.data ]  && $(ZDBSCOMPRESS) cinematographers.data
	-[ -s color-info.data ] && $(ZDBSCOMPRESS) color-info.data
	-[ -s composers.data ] && $(ZDBSCOMPRESS) composers.data
	-[ -s costume-designers.data ] && $(ZDBSCOMPRESS) costume-designers.data	
	-[ -s countries.data ] && $(ZDBSCOMPRESS) countries.data	
	-[ -s crazy-credits.data ] && $(ZDBSCOMPRESS) crazy-credits.data
	-[ -s directors.data ] && $(ZDBSCOMPRESS) directors.data
	-[ -s distributors.data ] && $(ZDBSCOMPRESS) distributors.data	
	-[ -s editors.data ] && $(ZDBSCOMPRESS) editors.data	
	-[ -s genres.data ] && $(ZDBSCOMPRESS) genres.data	
	-[ -s keywords.data ] && $(ZDBSCOMPRESS) keywords.data	
	-[ -s goofs.data ] && $(ZDBSCOMPRESS) goofs.data	
	-[ -s laserdisc.data ] && $(ZDBSCOMPRESS) laserdisc.data	
	-[ -s literature.data ] && $(ZDBSCOMPRESS) literature.data	
	-[ -s locations.data ] && $(ZDBSCOMPRESS) locations.data	
	-[ -s miscellaneous.data ] && $(ZDBSCOMPRESS) miscellaneous.data	
	-[ -s movie-links.data ] && $(ZDBSCOMPRESS) movie-links.data	
	-[ -s movies.data ] && $(ZDBSCOMPRESS) movies.data	
	-[ -s names.key ] && $(ZDBSCOMPRESS) names.key
	-[ -s plot.data ] && $(ZDBSCOMPRESS) plot.data
	-[ -s producers.data ] && $(ZDBSCOMPRESS) producers.data	
	-[ -s production-companies.data ] && $(ZDBSCOMPRESS) production-companies.data	
	-[ -s production-designers.data ] && $(ZDBSCOMPRESS) production-designers.data	
	-[ -s quotes.data ] && $(ZDBSCOMPRESS) quotes.data	
	-[ -s ratings.data ] && $(ZDBSCOMPRESS) ratings.data
	-[ -s release-dates.data ] && $(ZDBSCOMPRESS) release-dates.data
	-[ -s running-times.data ] && $(ZDBSCOMPRESS) running-times.data
	-[ -s sound-mix.data ] && $(ZDBSCOMPRESS) sound-mix.data	
	-[ -s soundtracks.data ] && $(ZDBSCOMPRESS) soundtracks.data	
	-[ -s special-effects-companies.data ] && $(ZDBSCOMPRESS) special-effects-companies.data	
	-[ -s taglines.data ] && $(ZDBSCOMPRESS) taglines.data	
	-[ -s technical.data ] && $(ZDBSCOMPRESS) technical.data
	-[ -s titles.key ] && $(ZDBSCOMPRESS) titles.key
	-[ -s trivia.data ] && $(ZDBSCOMPRESS) trivia.data
	-[ -s alternate-versions.data ] && $(ZDBSCOMPRESS) alternate-versions.data
	-[ -s votes.data ] && $(ZDBSCOMPRESS) votes.data	
	-[ -s writers.data ] && $(ZDBSCOMPRESS) writers.data

maxcompress : compress
	-[ -s actors.names ] && $(ZDBSCOMPRESS) actors.names
	-[ -s actors.titles ] && $(ZDBSCOMPRESS) actors.titles
	-[ -s actresses.names ] && $(ZDBSCOMPRESS) actresses.names
	-[ -s actresses.titles ] && $(ZDBSCOMPRESS) actresses.titles
	-[ -s aka-names.index ] && $(ZDBSCOMPRESS) aka-names.index
	-[ -s aka-titles.index ] && $(ZDBSCOMPRESS) aka-titles.index
	-[ -s attributes.index ] && $(ZDBSCOMPRESS) attributes.index
	-[ -s biographies.index ] && $(ZDBSCOMPRESS) biographies.index
	-[ -s business.index ] && $(ZDBSCOMPRESS) business.index	
	-[ -s certificates.index ]  && $(ZDBSCOMPRESS) certificates.index
	-[ -s cinematographers.names ] && $(ZDBSCOMPRESS) cinematographers.names
	-[ -s cinematographers.titles ] && $(ZDBSCOMPRESS) cinematographers.titles
	-[ -s color-info.index ] && $(ZDBSCOMPRESS) color-info.index
	-[ -s composers.names ] && $(ZDBSCOMPRESS) composers.names
	-[ -s composers.titles ] && $(ZDBSCOMPRESS) composers.titles
	-[ -s costume-designers.names ] && $(ZDBSCOMPRESS) costume-designers.names
	-[ -s costume-designers.titles ] && $(ZDBSCOMPRESS) costume-designers.titles
	-[ -s countries.index ] && $(ZDBSCOMPRESS) countries.index
	-[ -s crazy-credits.index ] && $(ZDBSCOMPRESS) crazy-credits.index
	-[ -s directors.names ] && $(ZDBSCOMPRESS) directors.names
	-[ -s directors.titles ] && $(ZDBSCOMPRESS) directors.titles
	-[ -s distributors.index ] && $(ZDBSCOMPRESS) distributors.index
	-[ -s editors.names ] && $(ZDBSCOMPRESS) editors.names
	-[ -s editors.titles ] && $(ZDBSCOMPRESS) editors.titles
	-[ -s genres.index ] && $(ZDBSCOMPRESS) genres.index
	-[ -s keywords.index ] && $(ZDBSCOMPRESS) keywords.index
	-[ -s goofs.index ] && $(ZDBSCOMPRESS) goofs.index
	-[ -s laserdisc.index ] && $(ZDBSCOMPRESS) laserdisc.index	
	-[ -s literature.index ] && $(ZDBSCOMPRESS) literature.index
	-[ -s locations.index ] && $(ZDBSCOMPRESS) locations.index
	-[ -s miscellaneous.names ] && $(ZDBSCOMPRESS) miscellaneous.names
	-[ -s miscellaneous.titles ] && $(ZDBSCOMPRESS) miscellaneous.titles
	-[ -s names.index ] && $(ZDBSCOMPRESS) names.index
	-[ -s plot.index ] && $(ZDBSCOMPRESS) plot.index
	-[ -s producers.names ] && $(ZDBSCOMPRESS) producers.names
	-[ -s producers.titles ] && $(ZDBSCOMPRESS) producers.titles
	-[ -s production-companies.index ] && $(ZDBSCOMPRESS) production-companies.index
	-[ -s production-designers.names ] && $(ZDBSCOMPRESS) production-designers.names
	-[ -s production-designers.titles ] && $(ZDBSCOMPRESS) production-designers.titles
	-[ -s quotes.index ] && $(ZDBSCOMPRESS) quotes.index
	-[ -s release-dates.index ] && $(ZDBSCOMPRESS) release-dates.index
	-[ -s running-times.index ] && $(ZDBSCOMPRESS) running-times.index
	-[ -s sound-mix.index ] && $(ZDBSCOMPRESS) sound-mix.index
	-[ -s soundtracks.index ] && $(ZDBSCOMPRESS) soundtracks.index
	-[ -s special-effects-companies.index ] && $(ZDBSCOMPRESS) special-effects-companies.index
	-[ -s taglines.index ] && $(ZDBSCOMPRESS) taglines.index
	-[ -s technical.index ] && $(ZDBSCOMPRESS) technical.index
	-[ -s language.index ] && $(ZDBSCOMPRESS) language.index
	-[ -s titles.index ] && $(ZDBSCOMPRESS) titles.index
	-[ -s trivia.index ] && $(ZDBSCOMPRESS) trivia.index
	-[ -s writers.names ] && $(ZDBSCOMPRESS) writers.names
	-[ -s writers.titles ] && $(ZDBSCOMPRESS) writers.titles

recompress :
	@$(ETCDIR)recompress actors.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress actors.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress actors.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress actresses.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress actresses.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress actresses.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress aka-names.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress aka-names.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress aka-titles.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress aka-titles.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress attributes.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress attributes.key $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress biographies.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress biographies.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress business.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress business.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress certificates.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress certificates.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress cinematographers.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress cinematographers.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress cinematographers.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress color-info.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress color-info.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress complete-cast.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress complete-crew.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress composers.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress composers.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress composers.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress costume-designers.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress costume-designers.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress costume-designers.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress countries.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress countries.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress crazy-credits.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress crazy-credits.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress directors.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress directors.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress directors.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress distributors.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress distributors.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress editors.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress editors.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress editors.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress genres.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress genres.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress keywords.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress keywords.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress goofs.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress goofs.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress laserdisc.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress laserdisc.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress literature.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress literature.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress locations.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress locations.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress miscellaneous.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress miscellaneous.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress miscellaneous.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress movie-links.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress movies.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress names.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress names.key $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress plot.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress plot.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress producers.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress producers.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress producers.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress production-companies.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress production-companies.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress production-designers.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress production-designers.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress production-designers.titles $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress quotes.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress quotes.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress ratings.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress release-dates.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress release-dates.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress running-times.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress running-times.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress sound-mix.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress sound-mix.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress soundtracks.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress soundtracks.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress special-effects-companies.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress special-effects-companies.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress taglines.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress taglines.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress technical.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress technical.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress language.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress language.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress titles.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress titles.key $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress alternate-versions.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress alternate-versions.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress trivia.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress trivia.index $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress writers.data $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress writers.names $(ZDBSCOMPRESS) $(ZDBSEXT)
	@$(ETCDIR)recompress writers.titles $(ZDBSCOMPRESS) $(ZDBSEXT)

# Local variables:
# indent-tabs-mode: t
# fill-column: 500
# End:

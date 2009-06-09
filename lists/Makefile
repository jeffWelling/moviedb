ORIGLISTS = actors.list actresses.list aka-titles.list biographies.list \
            cinematographers.list composers.list costume-designers.list \
            crazy-credits.list directors.list editors.list goofs.list \
            movies.list plot.list producers.list production-designers.list \
            quotes.list ratings.list soundtracks.list trivia.list votelog \
            writers.list running-times.list miscellaneous.list \
            aka-names.list certificates.list genres.list keywords.list \
            production-companies.list sound-mix.list color-info.list \
            countries.list release-dates.list locations.list literature.list \
            technical.list movie-links.list taglines.list complete-cast.list \
            language.list alternate-versions.list \
            special-effects-companies.list business.list laserdisc.list \
            distributors.list complete-crew.list

# The following is a manual expansion of ORIGLISTS 
# as some make's don't support $(ORIGLISTS:.list=.list$(ZLISTEXT)).

ZORIGLISTS = actors.list$(ZLISTEXT) actresses.list$(ZLISTEXT) \
             aka-titles.list$(ZLISTEXT) biographies.list$(ZLISTEXT) \
             cinematographers.list$(ZLISTEXT) composers.list$(ZLISTEXT) \
             costume-designers.list$(ZLISTEXT) crazy-credits.list$(ZLISTEXT) \
             directors.list$(ZLISTEXT) editors.list$(ZLISTEXT) \
             goofs.list$(ZLISTEXT) movies.list$(ZLISTEXT) \
             plot.list$(ZLISTEXT) producers.list$(ZLISTEXT) \
             production-designers.list$(ZLISTEXT) quotes.list$(ZLISTEXT) \
             ratings.list$(ZLISTEXT) soundtracks.list$(ZLISTEXT) \
             trivia.list$(ZLISTEXT) votelog$(ZLISTEXT) \
             writers.list$(ZLISTEXT) running-times.list$(ZLISTEXT) \
             miscellaneous.list$(ZLISTEXT) aka-names.list$(ZLISTEXT) \
             certificates.list$(ZLISTEXT) genres.list$(ZLISTEXT) \
             keywords.list$(ZLISTEXT) \
             production-companies.list$(ZLISTEXT) sound-mix.list$(ZLISTEXT) \
             color-info.list$(ZLISTEXT) countries.list$(ZLISTEXT) \
             release-dates.list$(ZLISTEXT) locations.list$(ZLISTEXT) \
             literature.list$(ZLISTEXT) technical.list$(ZLISTEXT) \
             movie-links.list$(ZLISTEXT) taglines.list$(ZLISTEXT) \
             complete-cast.list$(ZLISTEXT) language.list$(ZLISTEXT) \
             alternate-versions.list$(ZLISTEXT) \
             special-effects-companies.list$(ZLISTEXT) \
             business.list$(ZLISTEXT) laserdisc.list$(ZLISTEXT) \
             distributors.list$(ZLISTEXT) complete-crew.list$(ZLISTEXT)

all :
	@echo "I don't think you've read the install doc..."


# ----------------------------------------------------------------

do-lfetch : sanity-check-lfetch
	$(ETCDIR)/lfetch $(LFETCHOPT) -ext $(ZLISTEXT)

sanity-check-lfetch :
	@if echo "X$(LFETCHOPT)" | grep -v user >/dev/null; then \
	    echo ''; \
	    echo 'YOU MUST SET LFETCHOPT BEFORE USING lfetch!'; \
	    echo '(for instance -user foo@bar.edu)'; \
	    echo ''; \
	    false; \
	else \
	    true; \
	fi

reset-ftp :
	rm -f filesizes.old
	mv filesizes filesizes.old

# ----------------------------------------------------------------

cleanlists :
	rm -f $(ORIGLISTS) $(ZORIGLISTS)

# ----------------------------------------------------------------

autouncompress :
	-[ -s actors.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) actors.list$(ZLISTEXT) > actors.list && rm -f actors.list$(ZLISTEXT)
	-[ -s actresses.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) actresses.list$(ZLISTEXT) > actresses.list && rm -f actresses.list$(ZLISTEXT)
	-[ -s aka-names.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) aka-names.list$(ZLISTEXT) > aka-names.list && rm -f aka-names.list$(ZLISTEXT)
	-[ -s aka-titles.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) aka-titles.list$(ZLISTEXT) > aka-titles.list && rm -f aka-titles.list$(ZLISTEXT)
	-[ -s biographies.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) biographies.list$(ZLISTEXT) > biographies.list && rm -f biographies.list$(ZLISTEXT)
	-[ -s certificates.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) certificates.list$(ZLISTEXT) > certificates.list && rm -f certificates.list$(ZLISTEXT)
	-[ -s cinematographers.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) cinematographers.list$(ZLISTEXT) > cinematographers.list && rm -f cinematographers.list$(ZLISTEXT)
	-[ -s color-info.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) color-info.list$(ZLISTEXT) > color-info.list && rm -f color-info.list$(ZLISTEXT)
	-[ -s complete-cast.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) complete-cast.list$(ZLISTEXT) > complete-cast.list && rm -f complete-cast.list$(ZLISTEXT)
	-[ -s complete-crew.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) complete-crew.list$(ZLISTEXT) > complete-crew.list && rm -f complete-crew.list$(ZLISTEXT)
	-[ -s composers.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) composers.list$(ZLISTEXT) > composers.list && rm -f composers.list$(ZLISTEXT)
	-[ -s costume-designers.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) costume-designers.list$(ZLISTEXT) > costume-designers.list && rm -f costume-designers.list$(ZLISTEXT)
	-[ -s countries.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) countries.list$(ZLISTEXT) > countries.list && rm -f countries.list$(ZLISTEXT)
	-[ -s crazy-credits.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) crazy-credits.list$(ZLISTEXT) > crazy-credits.list && rm -f crazy-credits.list$(ZLISTEXT)
	-[ -s business.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) business.list$(ZLISTEXT) > business.list && rm -f business.list$(ZLISTEXT)
	-[ -s directors.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) directors.list$(ZLISTEXT) > directors.list && rm -f directors.list$(ZLISTEXT)
	-[ -s distributors.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) distributors.list$(ZLISTEXT) > distributors.list && rm -f distributors.list$(ZLISTEXT)
	-[ -s editors.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) editors.list$(ZLISTEXT) > editors.list && rm -f editors.list$(ZLISTEXT)
	-[ -s genres.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) genres.list$(ZLISTEXT) > genres.list && rm -f genres.list$(ZLISTEXT)
	-[ -s keywords.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) keywords.list$(ZLISTEXT) > keywords.list && rm -f keywords.list$(ZLISTEXT)
	-[ -s goofs.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) goofs.list$(ZLISTEXT) > goofs.list && rm -f goofs.list$(ZLISTEXT)
	-[ -s alternate-versions.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) alternate-versions.list$(ZLISTEXT) > alternate-versions.list && rm -f alternate-versions.list$(ZLISTEXT)
	-[ -s language.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) language.list$(ZLISTEXT) > language.list && rm -f language.list$(ZLISTEXT)
	-[ -s laserdisc.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) laserdisc.list$(ZLISTEXT) > laserdisc.list && rm -f laserdisc.list$(ZLISTEXT)
	-[ -s literature.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) literature.list$(ZLISTEXT) > literature.list && rm -f literature.list$(ZLISTEXT)
	-[ -s locations.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) locations.list$(ZLISTEXT) > locations.list && rm -f locations.list$(ZLISTEXT)
	-[ -s miscellaneous.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) miscellaneous.list$(ZLISTEXT) > miscellaneous.list && rm -f miscellaneous.list$(ZLISTEXT)
	-[ -s movies.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) movies.list$(ZLISTEXT) > movies.list && rm -f movies.list$(ZLISTEXT)
	-[ -s plot.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) plot.list$(ZLISTEXT) > plot.list && rm -f plot.list$(ZLISTEXT)
	-[ -s producers.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) producers.list$(ZLISTEXT) > producers.list && rm -f producers.list$(ZLISTEXT)
	-[ -s production-designers.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) production-designers.list$(ZLISTEXT) > production-designers.list && rm -f production-designers.list$(ZLISTEXT)
	-[ -s production-companies.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) production-companies.list$(ZLISTEXT) > production-companies.list && rm -f production-companies.list$(ZLISTEXT)
	-[ -s quotes.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) quotes.list$(ZLISTEXT) > quotes.list && rm -f quotes.list$(ZLISTEXT)
	-[ -s release-dates.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) release-dates.list$(ZLISTEXT) > release-dates.list && rm -f release-dates.list$(ZLISTEXT)
	-[ -s running-times.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) running-times.list$(ZLISTEXT) > running-times.list && rm -f running-times.list$(ZLISTEXT)
	-[ -s ratings.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) ratings.list$(ZLISTEXT) > ratings.list && rm -f ratings.list$(ZLISTEXT)
	-[ -s sound-mix.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) sound-mix.list$(ZLISTEXT) > sound-mix.list && rm -f sound-mix.list$(ZLISTEXT)
	-[ -s soundtracks.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) soundtracks.list$(ZLISTEXT) > soundtracks.list && rm -f soundtracks.list$(ZLISTEXT)
	-[ -s special-effects-companies.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) special-effects-companies.list$(ZLISTEXT) > special-effects-companies.list && rm -f special-effects-companies.list$(ZLISTEXT)
	-[ -s taglines.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) taglines.list$(ZLISTEXT) > taglines.list && rm -f taglines.list$(ZLISTEXT)
	-[ -s technical.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) technical.list$(ZLISTEXT) > technical.list && rm -f technical.list$(ZLISTEXT)
	-[ -s trivia.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) trivia.list$(ZLISTEXT) > trivia.list && rm -f trivia.list$(ZLISTEXT)
	-[ -s writers.list$(ZLISTEXT) ] && $(ZLISTCAT) $(ZLISTCATOPTS) writers.list$(ZLISTEXT) > writers.list && rm -f writers.list$(ZLISTEXT)

# ----------------------------------------------------------------



# Local variables:
# indent-tabs-mode: t
# fill-column: 500
# End:

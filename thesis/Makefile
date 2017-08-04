# $Id: Makefile,v 1.6 1999/12/17 16:48:09 matt Exp $

thesis.dvi::
	latex thesis.tex

thesis.ps: thesis.dvi
	dvips thesis.dvi

thesis.pdf: thesis.ps
	ps2pdf thesis.ps

html:
	latex2html -t "Searchable Full Text Mailing List Archive" -address "matt@osdigger.com" -local_icons -show_section_numbers -split +2 -dir /home/osdigger/www/osdigger/thesis thesis.tex

clean:
	rm -f *.aux *.toc thesis.ps thesis.dvi thesis.pdf *.log *.bbl *.blg

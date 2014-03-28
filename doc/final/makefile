TEX    = pdflatex --shell-escape
BIBTEX = bibtex
PLOT   = gnuplot -e 'set term latex'

PAPER  = final
BIBS   = project.bib
FIGDIR = fig
GPLOTS = samquan

GFILES = $(foreach p, $(GPLOTS), $(FIGDIR)/$(p).tex)

all : pdf

.PHONY : pdf
pdf : $(PAPER).pdf

$(PAPER).pdf : $(PAPER).blg $(GFILES)
	$(TEX) $(PAPER)
	$(TEX) $(PAPER)

$(PAPER).blg : $(BIBS) $(PAPER).aux
	$(BIBTEX) $(PAPER)

$(PAPER).aux : $(PAPER).tex
	$(TEX) $(PAPER)

$(GFILES) : $(FIGDIR)/%.tex : $(FIGDIR)/%.gp
	$(PLOT) $< > $@

.PHONY : clean
clean :
	rm -f *.bbl *.blg *.log *.aux *.toc *.lof \
		$(FIGDIR)/*.tex  $(FIGDIR)/*.pdf_tex $(FIGDIR)/*.pdf


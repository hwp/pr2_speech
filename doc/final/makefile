TEX    = latexmk -pdf -pdflatex="pdflatex -interaction=batchmode --shell-escape" -use-make
PLOT   = gnuplot -e 'set term latex'

PAPER  = final
BIBS   = project.bib
FIGDIR = fig
PLOTS  = samquan

PLOTP  = $(foreach p, $(PLOTS), $(FIGDIR)/$(p).tex)

$(FIGDIR)/%.tex : $(FIGDIR)/%.gp
	$(PLOT) $< > $@

all : $(PAPER).pdf

$(PAPER).pdf : $(PAPER).tex $(PLOTP)
	$(TEX) $(PAPER)

.PHONY : clean
clean :
	rm -f *.bbl *.blg *.log *.aux *.toc *.lof \
		$(FIGDIR)/*.tex  $(FIGDIR)/*.pdf_tex $(FIGDIR)/*.pdf


TEX    = latexmk -pdf -pdflatex="pdflatex -interaction=batchmode --shell-escape" -use-make
PLOT   = gnuplot -e 'set term latex'

PAPER  = final
BIBS   = project.bib mybib.bib
LISTS  = nav_commands.lm
FIGDIR = fig
PLOTS  = samquan

PLOTP  = $(foreach p, $(PLOTS), $(FIGDIR)/$(p).tex)

$(FIGDIR)/%.tex : $(FIGDIR)/%.gp
	$(PLOT) $< > $@

.PHONY : pdf 
pdf : $(PAPER).pdf

$(PAPER).pdf : $(PAPER).tex $(PLOTP) $(BIBS) $(LISTS)
	$(TEX) $(PAPER)

.PHONY : clean
clean :
	rm -f *.bbl *.blg *.log *.aux *.toc *.lof \
		$(FIGDIR)/*.tex  $(FIGDIR)/*.pdf_tex $(FIGDIR)/*.pdf


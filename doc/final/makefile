TEX    = latexmk -pdf -pdflatex="pdflatex -interaction=batchmode --shell-escape" 
PLOT   = gnuplot -e 'set term svg'

PAPER  = final
BIBS   = project.bib mybib.bib
LISTS  = nav_commands.lm
FIGDIR = fig
PLOTS  = window 

PLOTP  = $(foreach p, $(PLOTS), $(FIGDIR)/$(p).svg)

$(FIGDIR)/%.svg : $(FIGDIR)/%.gp
	$(PLOT) $< > $@

.PHONY : pdf 
pdf : $(PAPER).pdf

$(PAPER).pdf : $(PAPER).tex $(PLOTP) $(BIBS) $(LISTS) $(PLOTPsamquan)
	$(TEX) $(PAPER)

.PHONY : clean
clean :
	rm -f *.bbl *.blg *.log *.aux *.toc *.lof *.fls \
		$(FIGDIR)/*.tex  $(FIGDIR)/*.pdf_tex $(FIGDIR)/*.pdf


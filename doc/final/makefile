TEX    = latexmk -pdf -pdflatex="pdflatex -interaction=nonstopmode --shell-escape" 
PLOT   = gnuplot -e 'set term svg'

PAPER  = final
BIBS   = project.bib mybib.bib
LISTS  = nav_commands.lm
FIGDIR = fig
PLOTS  = window spec64 spec256 spec1024

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


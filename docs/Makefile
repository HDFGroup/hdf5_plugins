master_src := techrequirements.tex
document := $(master_src:.tex=.pdf)
auxfile := $(master_src:.tex=.aux)
bibliography := references.bib

all: $(document)

$(document): $(master_src) $(bibliography)
	pdflatex $(master_src)
	bibtex $(auxfile)
	pdflatex $(master_src)
	pdflatex $(master_src)

.PHONY: clean all

clean:
	rm -rf $(document)
	rm -rf $(auxfile)


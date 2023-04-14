#!/bin/bash
# Bash script to zip the whole project in order to make it deriverable
# please make sure zip and texlive are installed


# make sure git is up to date
git pull
git checkout main

# compile the report (and save it to root folder)
cd report
cp report.tex tmp.tex
pdflatex tmp.tex -output-directory .. -halt-on-error
rm tmp.tex
cd ..
mv tmp.pdf report.pdf
rm tmp.*


# zip it (excluding useless stuff)
make clean
tar -rvf ../ejercicio_evaluable1.tar -X exclude_file .

# cleanup
git reset --hard HEAD

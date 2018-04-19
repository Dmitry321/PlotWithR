#-------------------------------------------------
#
# Project created by QtCreator 2017-11-11T22:35:43
#
#-------------------------------------------------

QT       += core gui
QT       += svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PlotWithR
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \


FORMS    += mainwindow.ui
## comment this out if you need a different version of R,
## and set set R_HOME accordingly as an environment variable
R_HOME = 		$$system(R RHOME)
message("R_HOME is" $$R_HOME)

## include headers and libraries for R
RCPPFLAGS = 		$$system($$R_HOME/bin/R CMD config --cppflags)
RLDFLAGS = 		$$system($$R_HOME/bin/R CMD config --ldflags)
RBLAS = 		$$system($$R_HOME/bin/R CMD config BLAS_LIBS)
RLAPACK = 		$$system($$R_HOME/bin/R CMD config LAPACK_LIBS)

## if you need to set an rpath to R itself, also uncomment
RRPATH =		-Wl,-rpath,$$R_HOME/lib

## include headers and libraries for Rcpp interface classes
## note that RCPPLIBS will be empty with Rcpp (>= 0.11.0) and can be omitted
RCPPINCL = 		$$system($$R_HOME/bin/Rscript -e \"Rcpp:::CxxFlags\(\)\")
message("RCPPINCL is" $$RCPPINCL)
RCPPLIBS = 		$$system($$R_HOME/bin/Rscript -e \"Rcpp:::LdFlags\(\)\")
message("RCPPLIBS is" $$RCPPLIBS)

## for some reason when building with Qt we get this each time
##   /usr/local/lib/R/site-library/Rcpp/include/Rcpp/module/Module_generated_ctor_signature.h:25: warning: unused parameter ‘classname
## so we turn unused parameter warnings off
## no longer needed with Rcpp 0.9.3 or later
#RCPPWARNING =		-Wno-unused-parameter

## include headers and libraries for RInside embedding classes
RINSIDEINCL = 		$$system($$R_HOME/bin/Rscript -e \"RInside:::CxxFlags\(\)\")
RINSIDELIBS = 		$$system($$R_HOME/bin/Rscript -e \"RInside:::LdFlags\(\)\")

## compiler etc settings used in default make rules
QMAKE_CXXFLAGS +=	$$RCPPWARNING $$RCPPFLAGS $$RCPPINCL $$RINSIDEINCL
QMAKE_LIBS +=           $$RLDFLAGS $$RBLAS $$RLAPACK $$RINSIDELIBS $$RCPPLIBS

## addition clean targets
QMAKE_CLEAN +=		maiwindow Makefile

RESOURCES += \
    dsmfun.qrc

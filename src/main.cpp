/* 
 * File:   main.cpp
 * Author: yzf
 *
 * Created on July 10, 2013, 2:17 PM
 */

#include <cstdlib>
#include <cstdio>
#include <assert.h>
#include "Vocabulary.h"
#include "structs.h"
#include "ClakeCompletion.h"
#include "SATSolver.h"
#include <set>
#include <iostream>
#include <unistd.h>


using namespace std;

FILE* fout;
extern FILE* yyin;
extern vector<Rule> G_NLP;
extern int yyparse();

void io(const char* iPathName, const char* oPathName) {
    yyin = fopen (iPathName, "r");
    fout = fopen (oPathName, "w+");

    if (! yyin) {
        printf("IO Error: cannot open the input file.\n" );
        assert(0);
    }
    if (! fout) {
        printf("IO Error: cannot open the output file.\n");
        assert(0);
    }
}

int main(int argc, char** argv) {
    
    if(argc < 3) {
        io("res/input/sample.in","res/output/sample.out");
    }
    else {
        io(argv[1], argv[2]);
    }
    
    yyparse();
    fclose(yyin);
    
    for(int i = 0; i < G_NLP.size(); i++) {
        G_NLP.at(i).output(stdout);
    }
//    Vocabulary::instance().dumpVocabulary(stdout);
    ClakeCompletion::instance().setDlp(G_NLP);
    vector<_formula*> completion = ClakeCompletion::instance().convert();
    vector< set<int> > satin = Utils::convertToSATInput(completion);
    
    SATSolver sats(satin, Vocabulary::instance().apSize());
    sats.invokeSAT();
    sats.outputResult();
//    
//    Formula f = Formula(gformula, true);
//    PriCircTranslator* pct = new PriCircTranslator();
//    Formulas* fmls = pct->convert(f);
//    SMTranslator::instance().init(*fmls);
//    SMTranslator::instance().convert();
//    SMTranslator::instance().outputHengZhangFormulas(stdout);
//    SMTranslator::instance().outputCabalarFormulas(stdout);
//    SMTranslator::instance().outputFinalResult(fout);
//    SMTranslator::instance().destroy();
//    delete fmls;
//    delete pct;
//
//    fclose(fout);
//    Vocabulary::instance().dumpVocabulary(stdout);
    
    return 0;
}


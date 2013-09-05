/* 
 * File:   SATSolver.cpp
 * Author: sharpen
 * 
 * Created on September 4, 2013, 9:04 PM
 */

#include "SATSolver.h"

SATSolver::SATSolver(vector< set<int> > cnf, int num_lits) {
    num_lits_in_NLP = num_lits;
    
    while(num_lits > sat.nVars) {
        sat.newVar();
    }
    
    for(vector< set<int> >::iterator it = cnf.begin(); it != cnf.end(); it++) {
        int var;
        vec<Lit> lits;
        
        for(set<int>::iterator l_it = it->begin(); l_it != it->end(); l_it++) {
            var = abs(*l_it) - 1;
            lits.push((*l_it > 0) ? Lit(var) : ~Lit(var));
        }
        
        if(lits.size() == 1) {
            if(!sat.addUnit(lits[0])) {
                printf("addUnit failed");
            }
        }
        else {
            if(!sat.addClause(lits)) {
                printf("addClause failed");
            }
        }
    }
}

SATSolver::~SATSolver() {
}

void SATSolver::invokeSAT() {
    while(isExistModel());
}

bool SATSolver::isExistModel() {
    bool res;
    set<int> model_set;
    vec<Lit> new_clause;
    
    res = sat.solve();
    
    if(res) {
        for(int i = 0; i < num_lits_in_NLP; i++) {
            if(sat.model[i]) {
                model_set.insert(i + 1);
                new_clause.push(~Lit(i));
            }
            else {
                new_clause.push(Lit(i));
            }
        }
        sat.addClause(new_clause);
        
        return true;
    }
    else {
        return false;
    }
}
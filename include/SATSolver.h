/* 
 * File:   SATSolver.h
 * Author: sharpen
 *
 * Created on September 4, 2013, 9:04 PM
 */

#ifndef SATSOLVER_H
#define	SATSOLVER_H

#include <vector>
#include "Solver.h"

using namespace std;

class SATSolver {
public:
    SATSolver(vector<_formula*> CNFNlp);
    SATSolver(const SATSolver& orig);
    ~SATSolver();
private:
    vector<int> cnf_set;
    int num_lits_in_NLP;
};

#endif	/* SATSOLVER_H */


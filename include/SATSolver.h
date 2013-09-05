/* 
 * File:   SATSolver.h
 * Author: sharpen
 *
 * Created on September 4, 2013, 9:04 PM
 */

#ifndef SATSOLVER_H
#define	SATSOLVER_H

#include <vector>

using namespace std;

class SATSolver {
public:
    SATSolver(vector<_formula*> CNFNlp);
    SATSolver(const SATSolver& orig);
    ~SATSolver();
private:
    
};

#endif	/* SATSOLVER_H */


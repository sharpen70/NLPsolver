/* 
 * File:   DependenceGraph.h
 * Author: sharpen
 *
 * Created on September 3, 2013, 11:34 PM
 */

#ifndef DEPENDENCEGRAPH_H
#define	DEPENDENCEGRAPH_H

#include <vector>
#include <map>
#include "Rule.h"

class DependenceGraph {
public:
    DependenceGraph(vector<Rule> _dlp);
    DependenceGraph(const DependenceGraph& orig);
    ~DependenceGraph();
    void DFSFindLoops();        //find loop
    void BFSFindLoops();        //null
    
    void test();                //nonsense, just for test
    
    vector<_formula*> computeLoopFormulas();
    vector<_formula*> getExtendSupportRulesWithSize(int k);
    vector<int> getESRSizes();
    
private:
    vector<Rule> nlp;
    map<int, vector<int> > dpdGraph;
    vector< vector<int> > loops;
    vector< vector<Rule> > extendSupportRulesForLoops;
    map<int, vector<_formula*> > extendSupportRulesWithSize;   //map[k] = esr (size(esr) = k)
    int visitedNodes[MAX_ATOM_NUMBER];
    
    void dfsFind(int cur_node, int des_node, vector<int> loop_atoms);
    void findESRules();
};

#endif	/* DEPENDENCEGRAPH_H */

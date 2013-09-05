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
    void DFSFindLoops();
    void BFSFindLoops();
private:
    vector<Rule> nlp;
    map<int, vector<int> > dpdGraph;
    vector< vector<int> > loops;
    vector< vector<Rule> > extendSupportRulesForLoops;
    int visitedNodes[MAX_ATOM_NUMBER];
    
    void dfsFind(int cur_node, int des_node);
    void findESRules();
};

#endif	/* DEPENDENCEGRAPH_H */


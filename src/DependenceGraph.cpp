/* 
 * File:   DependenceGraph.cpp
 * Author: sharpen
 * 
 * Created on September 3, 2013, 11:34 PM
 */

#include "DependenceGraph.h"
#include "Utils.h"
#include <cstring>

DependenceGraph::DependenceGraph(vector<Rule> _nlp) :
                nlp(_nlp) {
    for(int i = 0; i < _nlp.size(); i++) {
        Rule r = _nlp.at(i);
        
        if(r.head > 0 && r.positive_literals.size() > 0) {
            for(int p = 0; p < r.positive_literals.size(); p++) {
                dpdGraph[r.head].push_back(r.positive_literals.at(p));
            }
        }
    }
    
    loops.clear();
    memset(visitedNodes, 0, sizeof(int) * MAX_ATOM_NUMBER);
}

DependenceGraph::DependenceGraph(const DependenceGraph& orig) {
    dpdGraph = orig.dpdGraph;
    loops = orig.loops;
}

DependenceGraph::~DependenceGraph() {
    dpdGraph.clear();
    loops.clear();
}

void DependenceGraph::DFSFindLoops() {
    for(map<int, vector<int> >::iterator it = dpdGraph.begin(); it != dpdGraph.end();
            it++) {
        if(visitedNodes[it->first] == 0) {
            visitedNodes[it->first] = 1;
            
            vector<int> lats;
            lats.push_back(it->first);
            dfsFind(it->first, it->first, lats);
        }
    }
}

void DependenceGraph::dfsFind(int cur_node, int des_node, vector<int> loop_atoms) {
    vector<int> edges = dpdGraph[cur_node];
    
    for(int i = 0; i < edges.size(); i++) {
        visitedNodes[edges.at(i)] = 1;

        if(edges.at(i) == des_node) {
            loops.push_back(loop_atoms);
        }
        else {
            vector<int> tloop = Utils::divideListAt(edges.at(i), loop_atoms);
            
            if(tloop.size() == 0) {
                loop_atoms.push_back(edges.at(i));
                dfsFind(edges.at(i), des_node, loop_atoms);
            }
            else {
                loops.push_back(tloop);
            }
        }
    }
}

void DependenceGraph::findESRules() {
    for(int i = 0; i < loops.size(); i++) {
        vector<Rule> esr;
        for(vector<Rule>::iterator r = nlp.begin(); r != nlp.end();) {
            if(!(Utils::crossList(loops.at(i), r->positive_literals)
                    || Utils::crossList(loops.at(i), r->negative_literals))) {
                esr.push_back(*r);
                r = nlp.erase(r);
            }
            else r++;
        }
        extendSupportRulesForLoops.push_back(esr);
    }
}



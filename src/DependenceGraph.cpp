/* 
 * File:   DependenceGraph.cpp
 * Author: sharpen
 * 
 * Created on September 3, 2013, 11:34 PM
 */

#include "DependenceGraph.h"
#include "Utils.h"
#include <cstring>
#include <assert.h>
#include <vector>
#include "Rule.h"
#include <map>
#include "structs.h"
#include "Vocabulary.h"
#include "CNFUtils.h"
#include <string.h>
#include <set>
#include <algorithm>
#include <functional>


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
    vector<Rule> _nlp;
    for(vector<Rule>::iterator it = nlp.begin(); it != nlp.end(); it++) {
        _nlp.push_back(*it);
    }
    
    for(int i = 0; i < loops.size(); i++) {
        vector<Rule> esr;
        for(vector<Rule>::iterator r = _nlp.begin(); r != _nlp.end();) {
//            if(!(Utils::crossList(loops.at(i), r->positive_literals)
//                    || Utils::crossList(loops.at(i), r->negative_literals))) {
            if(Utils::inList(r->head, loops.at(i)) && 
                    (!(Utils::crossList(r->positive_literals, loops.at(i))) && 
                    !(Utils::crossList(r->negative_literals, loops.at(i))))) {
                esr.push_back(*r);
                r = _nlp.erase(r);               
            }
            else r++;
        }
        extendSupportRulesForLoops.push_back(esr);
    }
}


void DependenceGraph::test() {   
    printf("\nThe dependence graph :\n");
    printf("\nThe nlp : %d\n", nlp.size());
    for(vector<Rule>::iterator it = nlp.begin(); it != nlp.end(); it++) {
        (*it).output(stdout);
    }
      
    printf("\nThe dpdGraph :\n");
    for(map<int, vector<int> >::iterator mit  = dpdGraph.begin(); 
            mit != dpdGraph.end(); mit++) {
        printf("The head : %d,   The body : ", mit->first);
        for(vector<int>::iterator bodyit = mit->second.begin();
                bodyit != mit->second.end(); bodyit++) {
            printf("%d, ", *bodyit);
        }
        printf("\n");
    }
    
    DFSFindLoops();
    printf("\nThe loops :\n");
    int i = 1;
    for(vector< vector<int> >::iterator it = loops.begin(); 
            it != loops.end(); it++) {
        printf("L%d : ", i++);
        for(vector<int>::iterator lit = (*it).begin(); 
                lit != (*it).end(); lit++) {
            printf("%d, ", *lit);
        }
        printf("\n");
    }
    
    printf("\nNLP size %d\n", nlp.size());
    
    findESRules();
    int j = 1;
    printf("\nThe extendSupportRulesForLoops :\n");
    for(vector< vector<Rule> >::iterator eit = extendSupportRulesForLoops.begin();
            eit != extendSupportRulesForLoops.end(); eit++) {
        printf("\nExtendSupport for loop %d :\n", j++);
        for(vector<Rule>::iterator rit = (*eit).begin(); rit != (*eit).end(); rit++) {
            (*rit).output(stdout);
        }
        printf("\n");
    }
    printf("\nNLP size %d\n", nlp.size());
    
    
    //-----------------LOOP FORMULA-----------------------------
    vector<_formula*> lf = computeLoopFormulas();
    printf("\nThe loop formulas :\n");
    for(vector<_formula*>::iterator it = lf.begin(); it != lf.end(); it++) {
        Utils::formulaOutput(stdout, *it);
        printf("\n");
    }
    
   printf("\nLF to sat input :\n");
    //convert to CNF for the input
    vector<_formula*> input;
    for(vector<_formula*>::iterator it = lf.begin(); it != lf.end(); it++) {
        vector<_formula*> cnflf = CNFUtils::convertCNF(*it);
        input = Utils::joinFormulas(input, cnflf);
    }
    printf("\ndddd\n");
    for(vector<_formula*>::iterator it = input.begin(); it != input.end(); it++) {
        Utils::formulaOutput(stdout, *it);
        printf("\n");
    }
    
    vector< set<int> > satInput = Utils::convertToSATInput(input);
    for(vector< set<int> >::iterator siit = satInput.begin(); 
            siit != satInput.end(); siit++) {
        for(set<int>::iterator sit = (*siit).begin(); 
                sit!= (*siit).end(); sit++) {
            printf("%d ", *sit);
        }
        printf("\n");
    }
    
    //extendSupportWithSize
    printf("\nThe Extend support with size : total %d\n", extendSupportRulesWithSize.size());
    for(map<int, vector<_formula*> >::iterator it = extendSupportRulesWithSize.begin();
            it != extendSupportRulesWithSize.end(); it++) {
        printf("size %d :\n", it->first);
        for(vector<_formula*>::iterator vit = it->second.begin();
                vit != it->second.end(); vit++) {
            Utils::formulaOutput(stdout, *vit);
            printf("\n");
        }
    }
}


vector<_formula*> DependenceGraph::computeLoopFormulas() {
    vector<_formula*> loopformulas;
    set<int> k;
    
    DFSFindLoops();
    findESRules();
    
    //loop formula head
    vector<_formula*> head;
    printf("The loops size : %d\n", loops.size());
    for(vector< vector<int> >::iterator it = loops.begin(); 
            it != loops.end(); it++) {
        //loop formula head
        _formula* _head = Utils::compositeToAtom(*((*it).begin()));
        for(vector<int>::iterator hit = (*it).begin()+1;
                hit != (*it).end(); hit++) {        
            _head = Utils::compositeByConnective(DISJ, _head
                    , Utils::compositeToAtom(*hit));
        }
        head.push_back(Utils::copyFormula(_head));
        Utils::deleteFormula(_head);
    }
        
    //loop formula body
    vector<_formula*> body;
    for(vector< vector<Rule> >::iterator bit = extendSupportRulesForLoops.begin();
                bit != extendSupportRulesForLoops.end(); bit++) {  
        k.insert((*bit).size());
        
        _formula* _body = Utils::convertRuleBodyToFormula(*((*bit).begin()));
        for(vector<Rule>::iterator rit = (*bit).begin()+1; rit != (*bit).end(); rit++) {       
            _body = Utils::compositeByConnective(DISJ, _body, 
                                        Utils::convertRuleBodyToFormula(*rit));         
        }       
        body.push_back(Utils::copyFormula(_body));       
        Utils::deleteFormula(_body);
        
    }
    
    //loop formula
    for(int i = 0; i < head.size(); i++) {
        _formula* nhead = Utils::compositeByConnective(NEGA, head.at(i), NULL);
        _formula* lf = Utils::compositeByConnective(DISJ, nhead, body.at(i));
        loopformulas.push_back(Utils::copyFormula(lf));
        Utils::deleteFormula(lf);
    }
   
    //Set to the map with index k
    for(set<int>::iterator it = k.begin(); it != k.end(); it++) {
        vector<_formula*> hold;
        for(int i = 0; i < loopformulas.size(); i++) {
            if(*it == extendSupportRulesForLoops.at(i).size())
                hold.push_back(Utils::copyFormula(loopformulas.at(i)));
        }
        extendSupportRulesWithSize[*it] = hold;
    }
    
    return loopformulas;
}


vector<_formula*> DependenceGraph::getExtendSupportRulesWithSize(int k) {
    return this->extendSupportRulesWithSize[k];
}

vector<int> DependenceGraph::getESRSizes() {
    vector<int> result;
    
    for(map<int, vector<_formula*> >::iterator it = extendSupportRulesWithSize.begin();
            it != extendSupportRulesWithSize.end(); it++) {
        result.push_back(it->first);
    }
    
    sort(result.begin(), result.end());    //default comp is operator < 
    
    return result;
}
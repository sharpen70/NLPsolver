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
    map< pair<int, int>, bool> graph;
    set<int> nodeSet;
     
    for(vector<Rule>::iterator it = _nlp.begin(); it != _nlp.end(); it++) {        
        if(it->head > 0 && it->positive_literals.size() > 0) {
            nodeSet.insert(it->head);
            for(vector<int>::iterator p_it = it->positive_literals.begin(); p_it != it->positive_literals.end(); p_it++) {
                dpdGraph[it->head].insert(*p_it);
                nodeSet.insert(*p_it);
            }
        }
    }
    
    FILE* out = fopen("res/output/sample.out", "w");
    
    edgePointer = 0;
    nodeNumber = *(--nodeSet.end());
    edgeNumber = 0;
    
    fprintf(out, "%d ", nodeNumber);
  
    
    for(map<int, set<int> >::iterator it = dpdGraph.begin(); it != dpdGraph.end(); it++) {
        edgeNumber += it->second.size();
    }
    
    fprintf(out, "%d\n", edgeNumber);
    
    heads = new int[nodeNumber + 1];
    memset(heads, -1, sizeof(int) * (nodeNumber + 1));

    edges = new Edge[edgeNumber];
    memset(edges, -1, sizeof(Edge) * edgeNumber);
    
    for(map<int, set<int> >::iterator it = dpdGraph.begin(); it != dpdGraph.end(); it++) {
        for(set<int>::iterator p_it = it->second.begin(); p_it != it->second.end(); p_it++) {
            pair<int, int> temp = make_pair(it->first, *p_it);
            if(graph[temp] != true) {
                graph[temp] = true;
                addEdge(it->first, *p_it);
                fprintf(out, "%d %d\n", it->first, *p_it);
            }           
        }
    }
                   
    
    loops.clear();
    memset(visitedNodes, 0, sizeof(int) * MAX_ATOM_NUMBER);
}

DependenceGraph::~DependenceGraph() {
    dpdGraph.clear();
    loops.clear();
    
    nodeNumber = edgeNumber = edgePointer = 0;

    if (heads) {
        delete[] heads;
        heads = NULL;
    }

    if (edges) {
        delete[] edges;
        edges = NULL;
    }
}

void DependenceGraph::addEdge(int x, int y) {
    edges[edgePointer].x = x;
    edges[edgePointer].y = y;
    edges[edgePointer].next = heads[x];
    heads[x] = edgePointer ++;
}

void DependenceGraph::dfs(int depth, int x, Info &info) {
    int *path = info.path;
    bool *vis = info.vis;
    
    if (x < info.startPoint) return ;
    
    path[depth] = x;

    if (depth > 0 && x == info.startPoint) {
        Hash hash(path, depth);
        if (!info.m[hash]) {
            info.m[hash] = true;
            set<int> loop;
            for (int i = 0; i < depth; i ++) {
                loop.insert(path[i]);
            }
            Hash lhash(loop);
            if(!loopHash[lhash]) {
                loopHash[lhash] = true;
                loops.push_back(loop);
            }
        }
    }

    for (int i = heads[x]; i != -1; i = edges[i].next) {
        if (!vis[i]) {
            vis[i] = true;
            dfs(depth + 1, edges[i].y, info);
            vis[i] = false;
        }
    }
}

void DependenceGraph::find() {
    Info info;

    info.path = new int[edgeNumber];
    memset(info.path, -1, sizeof(int) * edgeNumber);

    info.vis = new bool[edgeNumber];
    memset(info.vis, false, sizeof(bool) * edgeNumber);

    for (int i = 1; i <= nodeNumber; i ++) {
        info.startPoint = i;
        dfs(0, info.startPoint, info);
    }
}

//void DependenceGraph::DFSFindLoops() {
//    for(map<int, vector<int> >::iterator it = dpdGraph.begin(); it != dpdGraph.end();
//            it++) {
//        if(visitedNodes[it->first] == 0) {
//            visitedNodes[it->first] = 1;
//            
//            vector<int> lats;
//            lats.push_back(it->first);
//            dfsFind(it->first, lats);
//        }
//    }
//}
//
//void DependenceGraph::dfsFind(int cur_node, vector<int> loop_atoms) {
//    vector<int> edges = dpdGraph[cur_node];
//    
//    for(int i = 0; i < edges.size(); i++) {
//        visitedNodes[edges.at(i)] = 1;
//        
//        vector<int> tloop = Utils::divideListAt(edges.at(i), loop_atoms);
//
//        if(tloop.size() == 0) {
//            vector<int> tloop_atoms = loop_atoms;
//            tloop_atoms.push_back(edges.at(i));
//            dfsFind(edges.at(i), tloop_atoms);
//        }
//        else {
//            if(addedNodes[edges.at(i)] != 1) {
//                loops.push_back(tloop);
//            }
//        }
//    }
//    addedNodes[cur_node] = 1;
//}

void DependenceGraph::findESRules() {
    vector<Rule> _nlp;
    for(vector<Rule>::iterator it = nlp.begin(); it != nlp.end(); it++) {
        _nlp.push_back(*it);
    }
    
    for(vector< set<int> >::iterator it = loops.begin(); it != loops.end(); it++) {
        vector<Rule> esr;
        for(vector<Rule>::iterator r = _nlp.begin(); r != _nlp.end(); r++) {
//            if(!(Utils::crossList(loops.at(i), r->positive_literals)
//                    || Utils::crossList(loops.at(i), r->negative_literals))) {
            if(r->body_length == 0) {
                continue;
            }
            if(Utils::inList(r->head, *it) && 
                    (!(Utils::crossList(r->positive_literals, *it)) && 
                    !(Utils::crossList(r->negative_literals, *it)))) {
                esr.push_back(*r);
               // r = _nlp.erase(r);               
            }
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
    for(map<int, set<int> >::iterator mit  = dpdGraph.begin(); 
            mit != dpdGraph.end(); mit++) {
        printf("The head : %s,   The body : ", Vocabulary::instance().getAtom(mit->first));
        for(set<int>::iterator bodyit = mit->second.begin();
                bodyit != mit->second.end(); bodyit++) {
            printf("%s, ", Vocabulary::instance().getAtom(*bodyit));
        }
        printf("\n");
    }
    
    find();
    printf("\nThe loops :\n");
    int i = 1;
    for(vector< set<int> >::iterator it = loops.begin(); 
            it != loops.end(); it++) {
        printf("L%d : ", i++);
        for(set<int>::iterator lit = (*it).begin(); 
                lit != (*it).end(); lit++) {
            printf("%s, ", Vocabulary::instance().getAtom(*lit));
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
    
    //DFSFindLoops();
    findESRules();
    
    //loop formula head
    vector<_formula*> head;
    printf("The loops size : %d\n", loops.size());
    for(vector< set<int> >::iterator it = loops.begin(); 
            it != loops.end(); it++) {
        //loop formula head
        _formula* _head = NULL;
        for(set<int>::iterator hit = it->begin();
                hit != it->end(); hit++) {
            if(_head == NULL) _head = Utils::compositeToAtom(*(it->begin()));
            else _head = Utils::compositeByConnective(CONJ, _head
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
        
        if(bit->size() == 0) {
            _formula* _body = Utils::compositeToAtom(-1);
            body.push_back(_body);
        }
        else {
            _formula* _body = Utils::convertRuleBodyToFormula(*((*bit).begin()));
            for(vector<Rule>::iterator rit = (*bit).begin()+1; rit != (*bit).end(); rit++) {       
                _body = Utils::compositeByConnective(DISJ, _body, 
                                            Utils::convertRuleBodyToFormula(*rit));         
            }       
            body.push_back(Utils::copyFormula(_body));       
            Utils::deleteFormula(_body);
        }
    }
    
    //loop formula
    for(int i = 0; i < head.size(); i++) {
        _formula* lf;
        
        _formula* nhead = Utils::compositeByConnective(NEGA, head.at(i), NULL);
        if(body.at(i)->formula_type == ATOM && body.at(i)->predicate_id == -1) {
            lf = nhead;           
        }
        else {
            lf = Utils::compositeByConnective(DISJ, nhead, body.at(i));
        }
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
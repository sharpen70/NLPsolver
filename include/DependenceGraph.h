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
#include <set>
#include "Rule.h"

struct Edge {
    int x, y;
    int next;
};

struct Hash {
    int sum, squareSum;

    friend bool operator <(const Hash &a, const Hash &b) {
        if (a.sum != b.sum) {
            return a.sum < b.sum;
        }
        else {
            return a.squareSum < b.squareSum;
        }
    }

    Hash() {
        sum = squareSum = 0;
    }

    Hash(int *array, int size) {
        sum = getSum(array, size);
        squareSum = getSquareSum(array, size);
    }

    int getSum(int *array, int size) {
        int res = 0;
        for (int i = 0; i < size; i ++) {
            res += array[i];
        }
        return res;
    }

    int getSquareSum(int *array, int size) {
        int res = 0;
        for (int i = 0; i < size; i ++) {
            res += array[i] * array[i];
        }
        return res;
    }
};

struct Info {
    int startPoint;
    int *path;
    bool *vis;
    map<Hash, bool> m;

    Info():startPoint(0), path(NULL), vis(NULL) {
        m.clear();
    };
    ~Info() {
        startPoint = 0;
        m.clear();
        if (path) {
            delete[] path; path = NULL;
        }
        if (vis) {
            delete[] vis; vis = NULL;
        }
    }
};

class DependenceGraph {
public:
    DependenceGraph(vector<Rule> _dlp);
    DependenceGraph(const DependenceGraph& orig);
    ~DependenceGraph();
    void DFSFindLoops();        //find loop
    void BFSFindLoops();        //null
    void find();
    void test();                //nonsense, just for test
    
    vector<_formula*> computeLoopFormulas();
    vector<_formula*> getExtendSupportRulesWithSize(int k);
    vector<int> getESRSizes();
    
    vector< set<int> > loops;
    
    
private:
    vector<Rule> nlp;
    map<int, set<int> > dpdGraph;
    
    vector< vector<Rule> > extendSupportRulesForLoops;
    map<int, vector<_formula*> > extendSupportRulesWithSize;   //map[k] = esr (size(esr) = k)
    int visitedNodes[MAX_ATOM_NUMBER];
    int addedNodes[MAX_ATOM_NUMBER];
    
    //void dfsFind(int cur_node, vector<int> loop_atoms);
    void findESRules();
    void addEdge(int x, int y);
    void dfs(int depth, int x, Info &info);

    int nodeNumber, edgeNumber;
    int edgePointer;
    int *heads;
    Edge *edges;
};

#endif	/* DEPENDENCEGRAPH_H */

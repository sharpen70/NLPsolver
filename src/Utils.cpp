#include "Utils.h"
#include <assert.h>
#include <cstdlib>
#include <cstring>

/**
 * 使用联接词(非、析取、合取、蕴含)生成公式
 * @param _formulaType 联接词
 * @param _subformulaL 左公式
 * @param _subformulaR 右公式
 * @return 新公式
 */
_formula* Utils::compositeByConnective(FORMULA_TYPE _formulaType, 
                    _formula* _subformulaL, _formula* _subformulaR) {
    assert(_formulaType == NEGA || _formulaType == DISJ ||
		   _formulaType == CONJ || _formulaType == IMPL);

    _formula* fml = (_formula*)malloc(sizeof(_formula));
    assert(fml);

    fml->formula_type = _formulaType;
    fml->subformula_l = _subformulaL;
    fml->subformula_r = _subformulaR;
    return fml;
}
/**
 * 生成atom
 * @param _formulaType
 * @param _predicateId
 * @param _parameters
 * @return 
 */
_formula* Utils::compositeToAtom(int _atom_id) {
    _formula* fml = (_formula*)malloc(sizeof(_formula));
    assert(fml);

    fml->formula_type = ATOM;
    fml->predicate_id = _atom_id;
    return fml;
}

bool Utils::inList(int tag, vector<int> list) {
    for(int i = 0; i < list.size(); i++) {
        if(tag == list.at(i)) return true;       
    }
    
    return false;
}

vector<int> Utils::divideListAt(int tag, vector<int> list) {
    vector<int> result;
    bool find = false;
    
    for(int i = 0; i < list.size(); i++) {
        if(tag == list.at(i)) find = true;
        
        if(find) {
            result.push_back(list.at(i));
        }
    }
    
    return result;
}

bool Utils::crossList(vector<int> l1, vector<int> l2) {
    for(int i = 0; i < l1.size(); i++) {
        if(inList(l1.at(i), l2)) return true;
    }
    
    return false;
}

void Utils::joinFormulas(vector<_formula*> des_list, vector<_formula*> join_list) {
    for(int i = 0; i < join_list.size(); i++) {
        des_list.push_back(join_list.at(i));
    }
}
/* 
 * File:   CNFUtils.cpp
 * Author: sharpen
 * 
 * Created on September 3, 2013, 9:21 PM
 */

#include "CNFUtils.h"
#include "Utils.h"
#include "NNFUtils.h"

vector<_formula*> CNFUtils::convertCNF(_formula*& fml) {
    NNFUtils::convertToNegativeNormalForm(fml);
    convertToConjuntiveNormalForm(fml);
    
    vector<_formula*> result;
    divideCNFFormula(fml, result);
    
    return result;
}

_formula* CNFUtils::convertToConjuntiveNormalForm(_formula*& fml) {
    if(fml->formula_type == DISJ) {
        convertToConjuntiveNormalForm(fml->subformula_l);
        convertToConjuntiveNormalForm(fml->subformula_r);
        
        _formula* subfor_l = fml->subformula_l;
        _formula* subfor_r = fml->subformula_r;
        
        if(subfor_l->formula_type == CONJ || subfor_r->formula_type == CONJ) {
            if(subfor_l->formula_type == CONJ && subfor_r->formula_type == CONJ)
            {
                _formula* f1 = Utils::compositeByConnective(DISJ, subfor_l->subformula_l, 
                        subfor_r->subformula_l);
                _formula* f2 = Utils::compositeByConnective(DISJ, subfor_l->subformula_r,
                        Utils::copyFormula(subfor_r->subformula_l));
                _formula* f3 = Utils::compositeByConnective(DISJ, Utils::copyFormula(subfor_l->subformula_l),
                        subfor_r->subformula_r);
                _formula* f4 = Utils::compositeByConnective(DISJ, Utils::copyFormula(subfor_l->subformula_r),
                        Utils::copyFormula(subfor_r->subformula_r));
                
                _formula* f12 = Utils::compositeByConnective(CONJ, f1, f2);
                _formula* f34 = Utils::compositeByConnective(CONJ, f3, f4);
                
                fml = Utils::compositeByConnective(CONJ, f12, f34);
            }
            else {
                if(subfor_r->formula_type == CONJ) {
                    fml->subformula_l = subfor_r;
                    fml->subformula_r = subfor_l;
                    subfor_l = fml->subformula_l;
                    subfor_r = fml->subformula_r;
                }
                _formula* f1 = Utils::compositeByConnective(DISJ, subfor_l->subformula_l, subfor_r);
                _formula* f2 = Utils::compositeByConnective(DISJ, subfor_l->subformula_r, Utils::copyFormula(subfor_r));
                
                fml = Utils::compositeByConnective(CONJ, f1, f2);
            }
            convertToConjuntiveNormalForm(fml->subformula_l);
            convertToConjuntiveNormalForm(fml->subformula_r);
        }
    }
    else if(fml->formula_type == CONJ || fml->formula_type == IMPL) {
        convertToConjuntiveNormalForm(fml->subformula_l);
        convertToConjuntiveNormalForm(fml->subformula_r);
    }
    else if(fml->formula_type == NEGA) {
        convertToConjuntiveNormalForm(fml->subformula_l);
    }
    else if(fml->formula_type == ATOM) {
        
    }
    
    return fml;
}

void CNFUtils::divideCNFFormula(_formula* fml, vector<_formula*>& division) {
    if(fml->formula_type == CONJ) {
        divideCNFFormula(fml->subformula_l, division);
        divideCNFFormula(fml->subformula_r, division);
    }
    else {
        division.push_back(fml);
    }
}


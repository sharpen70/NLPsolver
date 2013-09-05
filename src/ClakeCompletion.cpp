#include "ClakeCompletion.h"
#include "CNFUtils.h"
#include "NNFUtils.h"

ClakeCompletion::ClakeCompletion() {
    no_ipf_atoms.clear();
    constrants.clear();
    ipf_atoms_rules.clear();   
}

ClakeCompletion::~ClakeCompletion() {
    no_ipf_atoms.clear();
    constrants.clear();
    ipf_atoms_rules.clear();  
}

ClakeCompletion& ClakeCompletion::instance() {
    static ClakeCompletion theInstance;
    return theInstance;
}

vector<_formula*> ClakeCompletion::convert() {
    vector<_formula*> completion;
    
    for(map<int, vector<Rule> >::iterator it = ipf_atoms_rules.begin(); it != 
            ipf_atoms_rules.end(); it++) {
        vector<Rule> ipf_rules = it->second;
        _formula* tl = Utils::compositeToAtom(it->first);
        _formula* tr;
        
        for(vector<Rule>::iterator it = ipf_rules.begin(); it != ipf_rules.end();
                it++) {
            _formula* fr = Utils::convertRuleBodyToFormula(*it);
            if(fr == NULL) break;
            if(tr == NULL) tr = fr;
            else tr = Utils::compositeByConnective(DISJ, tr, fr);
        }
        
        if(tr != NULL) {
            _formula* impl_l = Utils::compositeByConnective(IMPL, tl, tr);
            _formula* impl_r = Utils::compositeByConnective(IMPL, tr, tl);       

            Utils::joinFormulas(completion, CNFUtils::convertCNF(impl_l));
            Utils::joinFormulas(completion, CNFUtils::convertCNF(impl_r));
        }
        else {
            completion.push_back(tl);
        }
    }
    
    for(vector<int>::iterator it = no_ipf_atoms.begin(); it != no_ipf_atoms.end;
            it++) {
        _formula* nega_atom = Utils::compositeByConnective(NEGA, Utils::compositeToAtom(
                *it));
        
        completion.push_back(nega_atom);
    }
    
    for(vector<Rule>::iterator it = constrants.begin(); it != constrants.end(); 
            it++) {
        _formula* fc = Utils::convertRuleBodyToFormula(*it);     
        
        fc = Utils::compositeByConnective(NEGA, fc);
        Utils::joinFormulas(completion, NNFUtils::convertToNegativeNormalForm(fc));
    }
    
    return completion;
}

void ClakeCompletion::setDlp(const vector<Rule> nlp) {
    for(vector<Rule>::iterator it = nlp.begin(); it != nlp.end(); it++) {
        int a = it->head;
        
        if(a > 0) {
            ipf_atoms_rules[a].push_back(*it);
        }
        else {
            constrants.push_back(*it);
        }
    }
    
    for(int i = 1; i <= Vocabulary::instance().apSize(); i++) {
        bool in = false;
        
        for(map<int, vector<Rule> >::iterator it = ipf_atoms_rules.begin(); it != ipf_atoms_rules.end(); it++) {
            if(it->first == i) {
                in = true;
            }
        }
        
        if(!in) {
            no_ipf_atoms.push_back(i);
        }
    } 
}

void ClakeCompletion::test() {
    printf("\nno_ipf_atoms:");
    for(int i = 0; i < no_ipf_atoms.size(); i++) {
        printf("%s ", Vocabulary::instance().getAtom(no_ipf_atoms.at(i)));
    }
    printf("\nipf_atoms_rules:\n");
    for(map<int, vector<Rule> >::iterator it = ipf_atoms_rules.begin(); it != ipf_atoms_rules.end(); it++) {
        vector<Rule> r = it->second;
        for(int i = 0; i < r.size(); i++) {
            r.at(i).output(stdout);
        }
    }
    printf("\nconstrants\n");
    for(int i = 0; i < constrants.size(); i++) {
        constrants.at(i).output(stdout);
    }
}
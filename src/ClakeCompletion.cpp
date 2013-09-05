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
        vector<Rule> body = it->second;
        _formula* tl = Utils::compositeToAtom(it->first);
        _formula* tr;
        
        for(int i = 0; i < body.size(); i++) {
            Rule rule = body.at(i);
            for(int p = 0; p < rule.positive_literals.size(); p++) {
                int id = rule.positive_literals.at(p);
                if(tr == NULL) tr = Utils::compositeToAtom(id);
                else {
                    tr = Utils::compositeByConnective(CONJ, tr, Utils::compositeToAtom(id));
                }
            }
            for(int p = 0; p < rule.negative_literals.size(); p++) {
                int id = rule.negative_literals.at(p);
                if(tr == NULL) tr = Utils::compositeToAtom(id);
                else {
                    _formula* nega = Utils::compositeByConnective(NEGA, Utils::compositeToAtom(id));
                    tr = Utils::compositeByConnective(CONJ, tr, nega);
                }
            }
        }
        
        _formula* impl_l = Utils::compositeByConnective(IMPL, tl, tr);
        _formula* impl_r = Utils::compositeByConnective(IMPL, tr, tl);
        
        
        Utils::joinFormulas(completion, CNFUtils::convertCNF(impl_l));
        Utils::joinFormulas(completion, CNFUtils::convertCNF(impl_r));
    }
    
    for(int i = 0; i < no_ipf_atoms.size(); i++) {
        _formula* nega_atom = Utils::compositeByConnective(NEGA, Utils::compositeToAtom(
                no_ipf_atoms.at(i)));
        
        completion.push_back(nega_atom);
    }
    
    for(int i = 0; i < constrants.size(); i++) {
        Rule rule = constrants.at(i);
        _formula* tr;
        
        for(int p = 0; p < rule.positive_literals.size(); p++) {
            int id = rule.positive_literals.at(p);
            if(tr == NULL) tr = Utils::compositeToAtom(id);
            else {
                tr = Utils::compositeByConnective(CONJ, tr, Utils::compositeToAtom(id));
            }
        }
        for(int p = 0; p < rule.negative_literals.size(); p++) {
            int id = rule.negative_literals.at(p);
            if(tr == NULL) tr = Utils::compositeToAtom(id);
            else {
                _formula* nega = Utils::compositeByConnective(NEGA, Utils::compositeToAtom(id));
                tr = Utils::compositeByConnective(CONJ, tr, nega);
            }
        }
        
        tr = Utils::compositeByConnective(NEGA, tr);
        Utils::joinFormulas(completion, NNFUtils::convertToNegativeNormalForm(tr));
    }
    
    return completion;
}

void ClakeCompletion::setDlp(const vector<Rule> nlp) {
    for(int i = 0; i < nlp.size(); i++) {
        int a = nlp.at(i).head;
        
        if(a > 0) {
            ipf_atoms_rules[a].push_back(nlp.at(i));
        }
        else {
            constrants.push_back(nlp.at(i));
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
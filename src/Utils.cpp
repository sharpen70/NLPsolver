 #include "Utils.h"
#include "Rule.h"
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

_formula* Utils::copyFormula(const _formula* _fml) {
    if (_fml == NULL) {
        return NULL;
    }
	
    _formula* newFormula = (_formula*)malloc(sizeof(_formula));
    assert (newFormula);

    memcpy(newFormula, _fml, sizeof(_formula));
    switch (_fml->formula_type)
    {
    case ATOM:
        newFormula->predicate_id = _fml->predicate_id;
        break;
    case CONJ:
    case DISJ:
    case IMPL:
        assert(_fml->subformula_r);
        newFormula->subformula_r = copyFormula( _fml->subformula_r);
    case NEGA:
    case UNIV:
    case EXIS:
        assert(_fml->subformula_l);
        newFormula->subformula_l = copyFormula(_fml->subformula_l);
        break;
    default:
        assert (0);
    }

    return newFormula;
}

void Utils::deleteFormula(_formula* _fml) {
    assert(_fml);

    switch (_fml->formula_type)
    {
    case ATOM:
        break;
    case CONJ:
    case DISJ:
    case IMPL:
        assert(_fml->subformula_r);
        deleteFormula(_fml->subformula_r);
    case NEGA:
    case UNIV:
    case EXIS:
        assert(_fml->subformula_l);
        deleteFormula(_fml->subformula_l);
        break;
    default:
        assert ( 0 );
    }

    free(_fml);
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

vector<_formula*> Utils::joinFormulas(vector<_formula*> des_list, vector<_formula*> join_list) {
    for(int i = 0; i < join_list.size(); i++) {
        des_list.push_back(join_list.at(i));
    }
    
    return des_list;
}

vector< set<int> > Utils::convertToSATInput(vector<_formula*> cnfDlp) {
    vector< set<int> > res;
    for(vector<_formula*>::iterator it = cnfDlp.begin(); it != cnfDlp.end(); it++) {
        set<int> lits;
        convertCNFformulaToLits(*it, lits);
        res.push_back(lits);
        
        deleteFormula(*it);
    }
    return res;
}

void Utils::convertCNFformulaToLits(_formula* rule, set<int>& lits) {
    if(rule->formula_type == ATOM) {
        lits.insert(rule->predicate_id);
    }
    else if(rule->formula_type == NEGA) {
        lits.insert(-1 * (rule->subformula_l->predicate_id));
    }
    else {
        convertCNFformulaToLits(rule->subformula_l, lits);
        convertCNFformulaToLits(rule->subformula_r, lits);
    }
}

_formula* Utils::convertRuleBodyToFormula(const Rule& rule) {
    _formula* fml = NULL;
    
    Rule _rule(rule);
    
    if(rule.body_length == 0) return NULL;
    
    for(vector<int>::iterator it = _rule.positive_literals.begin(); it != 
            _rule.positive_literals.end(); it++) {
        if(fml == NULL) fml = Utils::compositeToAtom(*it);
        else {
            fml = Utils::compositeByConnective(CONJ, fml, Utils::compositeToAtom(*it));
        }
    }
    for(vector<int>::iterator it = _rule.negative_literals.begin(); it !=
            _rule.negative_literals.end(); it++) {
        _formula* nega = Utils::compositeByConnective(NEGA, Utils::compositeToAtom(*it));
        if(fml == NULL) fml = nega;
        else {
            fml = Utils::compositeByConnective(CONJ, fml, nega);
        }
    }
    
    return fml;
}

vector<int> Utils::readClaspAnswers(const char* AnswerSet_list) {
    FILE* asl = fopen(AnswerSet_list, "r");
    vector<int> numberOfAnswerSet;
    int max_line = 1000;
    
    while(!feof(asl)) {
        char as[max_line];
        fgets(as, max_line, asl);
        fgetc(asl);
        FILE* fas = fopen(as, "r");
        char match[] = "models";
        int index = 0;
        
        while(!feof(fas)) {
            
            char c = fgetc(fas);

            if(c == match[index]) {
                index++;
            }
            else {
                index = 0;
            }
            if(match[index] == '\0') break;
        }
        
        int number = 0;
        char digit = fgetc(fas);

        while(!isdigit(digit)) {
            digit = fgetc(fas);
        }

        while(isdigit(digit)) {
            number = 10 * number + (digit - '0');
            digit = fgetc(fas);
        }
        numberOfAnswerSet.push_back(number);
        
        
        fclose(fas);
    }
    fclose(asl);
    
    return numberOfAnswerSet;
}
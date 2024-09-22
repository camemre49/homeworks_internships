#include "components.hpp"

using namespace std;

/* operator<< overloads */
ostream& operator<<(ostream& os, Alphabet& a){
    auto it = a.symbols.begin();
    bool first = true;
    while (it != a.symbols.end()){
        if (first) { os << *it; first = false; }
        else { os << " " << *it; }
        it++;
    }
    return os;
}
ostream& operator<<(ostream& os, const Rule& r){
    os << r.initial_state << " " << r.symbol << " " << r.final_state;
    return os;
}
ostream& operator<<(ostream& os, TransitionTable& t){
    for (Rule& rule : t.rules)
        os << rule << endl;
    return os;
}
ostream& operator<<(ostream& os, const ComputationBranch& c){
    size_t i;
    for(i=0; i<c.config_history.size()-1; i++){
        string symbol = c.config_history[i].second=="" ? "e" : c.config_history[i].second;
        os << "(" << c.config_history[i].first << ", " << symbol << ") :- ";
    }
    string symbol = c.config_history[i].second=="" ? "e" : c.config_history[i].second;
    os << "(" << c.config_history[i].first << ", " << symbol << ")";
    return os;
}


/* Alphabet */
const set<char> Alphabet::get_symbols() const { return this->symbols; }

bool Alphabet::is_valid(const string& input) const{
    for (int i=0; i<input.length(); i++){
        if (this->symbols.find(input[i]) == this->symbols.end())
            return false;
    }
    return true;
}

Alphabet& Alphabet::operator+=(const Alphabet& other){
    for (char symbol : other.symbols)
        this->symbols.insert(symbol);
    return *this;
}

/* Rule */
Rule::Rule(const string& init_s, char symbol, const string& final_s) : initial_state(init_s), symbol(symbol), final_state(final_s) {}

bool Rule::applies_to(const string& c, char s) const{
    if (this->initial_state == c && this->symbol == s)
        return true;
    return false;
}

void Rule::update_state_name(const string& old_name, const string& new_name){
    if (this->initial_state == old_name) this->initial_state = new_name;
    if (this->final_state == old_name) this->final_state = new_name;
}

const string Rule::get_final_state() const { return this->final_state; }


/* TransitionTable */
void TransitionTable::add_rule(const string& initial_state, char symbol, const string& final_state){
    this->rules.push_back(Rule(initial_state, symbol, final_state));
}

void TransitionTable::update_state_name(const string& old_name, const string& new_name){
    for (auto it=this->rules.begin(); it != this->rules.end(); it++){
        it->update_state_name(old_name, new_name);
    }
}

// side note: as of C++23 operator[] can be overloaded to accept multiple inputs as well
const set<string> TransitionTable::operator()(const string& curr_state, char symbol) const{
    set<string> next_states;
    for (Rule rule : rules){
        if (rule.applies_to(curr_state, symbol))
            next_states.insert(rule.get_final_state());
    }
    return next_states;
}

TransitionTable& TransitionTable::operator+=(const TransitionTable& other){
    for (Rule rule : other.rules)
        this->rules.push_back(rule);
    return *this;
}


/* ComputationBranch */
void ComputationBranch::push_config(const string& state, const string& input){
    this->config_history.push_back(make_pair(state, input));
}

const pair<string,string> ComputationBranch::get_last_config() const{
    return this->config_history.back();
}
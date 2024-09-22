#include "components.hpp"

/* operator<< overloads */
std::ostream& operator<<(std::ostream& os, Alphabet& a){
    for (char symbol : a.get_symbols()) {
        os << symbol << " ";
    }
    return os;
}
std::ostream& operator<<(std::ostream& os, const Rule& r){
    os << r.initial_state << " " << r.symbol << " " << r.final_state << "\n";
    return os;
}
std::ostream& operator<<(std::ostream& os, TransitionTable& t){
    for (const Rule& rule : t.rules) {
        os << rule;
    }
    return os;
}
std::ostream& operator<<(std::ostream& os, const ComputationBranch& c){
    for (size_t i = 0; i < c.config_history.size(); ++i) {
        const auto& config = c.config_history[i];
        os << "(" << config.first << ", ";
        if (config.second.empty()) {
            os << "e)";
        } else {
            os << config.second << ")";
        }
        if (i != c.config_history.size() - 1) {
            os << " :- ";
        }
    }
    os << std::endl;
    return os;
}


/* Alphabet */
bool Alphabet::is_valid(const std::string& input) const{
    for (char c : input) {
        if (symbols.find(c) == symbols.end()) {
            return false;
        }
    }
    return true;
}

const std::set<char> Alphabet::get_symbols() const {
    return symbols;
}

Alphabet& Alphabet::operator+=(const Alphabet& other){
    for (char symbol : other.symbols) {
        symbols.insert(symbol);
    }
    return *this;
}


/* Rule */
Rule::Rule(const std::string& init_s, char symbol, const std::string& final_s)
        : initial_state(init_s), symbol(symbol), final_state(final_s) {
}

const std::string Rule::get_final_state() const {
    return final_state;
}

void Rule::update_state_name(const std::string& old_name, const std::string& new_name){
    if (initial_state == old_name) {
        initial_state = new_name;
    }
    if (final_state == old_name) {
        final_state = new_name;
    }
}

bool Rule::applies_to(const std::string& c, char s) const{
    return (initial_state == c && this->symbol == s);
}


/* TransitionTable */
void TransitionTable::add_rule(const std::string& initial_state, char symbol, const std::string& final_state){
    rules.emplace_back(initial_state, symbol, final_state);
}

void TransitionTable::update_state_name(const std::string& old_name, const std::string& new_name){
    for (Rule& rule : rules) {
        rule.update_state_name(old_name, new_name);
    }
}

const std::set<std::string> TransitionTable::operator()(const std::string& curr_state, char symbol) const{
    std::set<std::string> next_states;
    for (const Rule& rule : rules) {
        if (rule.applies_to(curr_state, symbol)) {
            next_states.insert(rule.get_final_state());
        }
    }
    return next_states;
}

TransitionTable& TransitionTable::operator+=(const TransitionTable& other){
    rules.insert(rules.end(), other.rules.begin(), other.rules.end());
    return *this;
}


/* ComputationBranch */
void ComputationBranch::push_config(const std::string& state, const std::string& input){
    config_history.push_back(std::make_pair(state, input));
}

const std::pair<std::string,std::string> ComputationBranch::get_last_config() const{
    if (!config_history.empty()) {
        return config_history.back();
    }
    return std::make_pair("","");
}
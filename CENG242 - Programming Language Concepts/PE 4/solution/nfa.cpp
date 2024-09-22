#include "nfa.hpp"

// private:
bool NFA::has_state(const std::string &state) const {
    return all_states.find(state) != all_states.end();
}

bool NFA::is_final_state(const std::string &state) const {
    return final_states.find(state) != final_states.end();
}

// public:
bool NFA::is_DFA() const {
    for (const auto &state: all_states) {
        for (char symbol: alphabet.get_symbols()) {
            if (transitions(state, symbol).size() != 1) {
                return false;
            }
        }
        if (!transitions(state, 'e').empty()) {
            return false;
        }
    }

    return true;
}

void NFA::update_state_name(const std::string &old_name, const std::string &new_name) {

    std::set<std::string> updated_states;
    for (const auto &state : all_states) {
        if (state == old_name) {
            updated_states.insert(new_name);
        } else {
            updated_states.insert(state);
        }
    }
    all_states = updated_states;


    std::set<std::string> updated_final_states;
    for (const auto &state : final_states) {
        if (state == old_name) {
            updated_final_states.insert(new_name);
        } else {
            updated_final_states.insert(state);
        }
    }
    final_states = updated_final_states;

    if(starting_state == old_name) {
        starting_state = new_name;
    }
}

struct Computation {
    ComputationBranch computationBranch;
    std::string left_string;

    Computation(const ComputationBranch &c, const std::string &s)
        : computationBranch(c), left_string(s){}
};

bool NFA::process(std::string input) {
    if (!alphabet.is_valid(input)) {
        std::cout << "Invalid string" << std::endl;
        return false;
    }

    std::queue<Computation> Q;
    Q.push(Computation(ComputationBranch(starting_state, input), input));

    while (!Q.empty()) {
        Computation computation = Q.front();
        Q.pop();

        if (is_final_state(computation.computationBranch.get_last_config().first) && computation.left_string.empty()) {
            std::cout << computation.computationBranch;
            std::cout << "Accept" << std::endl;
            return true;
        }


        if(!computation.left_string.empty()) {
            std::set<std::string> next_states = transitions(computation.computationBranch.get_last_config().first,
                                                            computation.left_string.front());

            for (const auto& state : next_states) {
                std::string left  = computation.left_string.substr(1, computation.left_string.size());
                ComputationBranch new_computation_branch = computation.computationBranch;
                new_computation_branch.push_config(state, left);
                Q.push(Computation(new_computation_branch, left));
            }

        }


        std::set<std::string> empty_states = transitions(computation.computationBranch.get_last_config().first,
                                                        'e');
        for (const auto& state : empty_states) {
            ComputationBranch new_computation_branch = computation.computationBranch;
            new_computation_branch.push_config(state, input);
            Q.push(Computation(new_computation_branch, input));
        }

        if(Q.empty()) {
            std::cout << computation.computationBranch;
            std::cout << "Reject" << std::endl;
        }
    }

    return false;
}

NFA NFA::operator!() const {
    if(!this->is_DFA()) {
        std::cout << "Not a DFA" << std::endl;
        return *this;
    }

    NFA complement;
    complement.alphabet = this->alphabet;
    complement.all_states = this->all_states;
    complement.starting_state = this->starting_state;
    complement.transitions = this->transitions;

    std::set<std::string> complement_final_states;
    for (const auto& state : this->all_states) {
        if (!this->is_final_state(state)) {
            complement_final_states.insert(state);
        }
    }

    complement.final_states = complement_final_states;
    return complement;
}

std::string addq(std::string  str);
NFA NFA::operator+(const NFA &other) const {
    NFA result;
    NFA other_copy = other;

    result.alphabet = this->alphabet;
    result.alphabet += other.alphabet;

    result.all_states = this->all_states;
    result.transitions = this->transitions;

    for (const std::string& state: other.all_states) {
        if(result.all_states.find(state) != result.all_states.end()){
            std::string changed_name = addq(state);
            while(result.all_states.find(changed_name) != result.all_states.end() || other_copy.all_states.find(changed_name) != other_copy.all_states.end()) {
                changed_name = addq(changed_name);
            }
            // Also updated the final and start state names
            other_copy.update_state_name(state, changed_name);
            other_copy.transitions.update_state_name(state, changed_name);
        }
    }
    result.transitions += other_copy.transitions;

    result.all_states.insert(other_copy.all_states.begin(), other_copy.all_states.end());

    result.final_states = this->final_states;
    result.final_states.insert(other_copy.final_states.begin(), other_copy.final_states.end());

    std::string init_state = "s";
    while(result.has_state(init_state)) {
        init_state += "s";
    }
    result.all_states.insert(init_state);
    result.starting_state = init_state;

    result.transitions.add_rule(init_state, 'e', other_copy.starting_state);
    result.transitions.add_rule(init_state, 'e', this->starting_state);
    return result;
}

std::string addq(std::string str) {
    return "q" + str;
}

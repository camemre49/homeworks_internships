#include "nfa.hpp"

using namespace std;

bool NFA::has_state(const string& state) const{
    return this->all_states.find(state) != this->all_states.end();
}

bool NFA::is_final_state(const string& state) const{
    return this->final_states.find(state) != this->final_states.end();
}

void NFA::update_state_name(const string& old_name, const string& new_name){
    if (starting_state == old_name) starting_state = new_name;
    if ( this->is_final_state(old_name) ){
        final_states.erase(old_name);
        final_states.insert(new_name);
    }
    all_states.erase(old_name);
    all_states.insert(new_name);
    transitions.update_state_name(old_name, new_name);
}

bool NFA::process(string input){
    // check if input is valid
    if (!this->alphabet.is_valid(input)){
        cout << "Invalid string" << endl;
        return false;
    }
    queue<ComputationBranch> computation_queue;
    ComputationBranch init_branch(this->starting_state, input);
    computation_queue.push(init_branch);

    ComputationBranch curr_branch;
    set<string> next_states;
    while(!computation_queue.empty()){
        // continue from a computation branch
        curr_branch = move(computation_queue.front());
        computation_queue.pop();

        pair<string,string> curr_config = curr_branch.get_last_config();
        string curr_state  = curr_config.first;
        string remaining_input = curr_config.second;

        // check whether this is an accepting configuration
        if (remaining_input == "" && this->is_final_state(curr_state) ){
            cout << curr_branch << endl;
            cout << "Accept" << endl;
            return true;
        }

        // check for empty transitions first
        next_states = this->transitions(curr_state, 'e');
        for (string next_state : next_states){
            ComputationBranch new_comp(curr_branch);
            new_comp.push_config(next_state, remaining_input);
            computation_queue.push(new_comp);
        }

        // check for symbol transitions now
        next_states = this->transitions(curr_state, remaining_input[0]);
        for (string next_state : next_states){
            ComputationBranch new_comp(curr_branch);
            new_comp.push_config(next_state, remaining_input.substr(1));
            computation_queue.push(new_comp);
        }
    }
    cout << curr_branch << endl;
    cout << "Reject" << endl;
    return false;
}

bool NFA::is_DFA() const{
    for (string state : this->all_states){
        set<string> next_states = this->transitions(state, 'e');
        if (next_states.size() != 0)
            return false;
    }
    for (string state : this->all_states){
        for ( char symbol : this->alphabet.get_symbols() ) {
            set<string> next_states = this->transitions(state, symbol);
            if (next_states.size() != 1)
                return false;
        }
    }
    return true;
}
    
NFA NFA::operator!(void) const{
    if (!this->is_DFA()){
        cout << "Not a DFA" << endl;
        return *this;
    }
    NFA M(*this);
    set<string> complemented;
    for (string state : this->all_states)
        if (!this->is_final_state(state))
            complemented.insert(state);
    M.final_states = complemented;
    return M;
}

NFA NFA::operator+(const NFA& other) const{
    NFA M(other);
    // update the state names shared by both machines
    string new_state_name;
    for (const string& state : other.all_states){
        new_state_name = state;
        while (this->has_state(new_state_name))
            new_state_name = "q" + new_state_name;
        M.update_state_name(state, new_state_name);
    }
    // transfer states from this machine to the new one
    for (const string& state : this->all_states)
        M.all_states.insert(state);
    for (const string& state : this->final_states)
        M.final_states.insert(state);

    // transfer transitions from this machine to the new one
    M.transitions += this->transitions;
    M.alphabet += this-> alphabet;

    // get an unused state name for the new starting state
    new_state_name = "s";
    while ( M.has_state(new_state_name) )
        new_state_name = "s" + new_state_name;
    M.all_states.insert(new_state_name);
    
    string other_starting_state = M.starting_state;
    M.starting_state = new_state_name;
    M.transitions.add_rule(M.starting_state, 'e', other_starting_state);
    M.transitions.add_rule(M.starting_state, 'e', this->starting_state);

    return M;
}
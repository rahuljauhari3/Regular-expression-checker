// Rahul Jauhari 2020A7PS0106H
#include <iostream>
#include <vector>
#include <set>
#include <stack>
#include <fstream>
using namespace std;

class State {
public:
    int charac,aa=0;          
    State* next1, *next2;
    State(int ch): charac(ch), next1(NULL), next2(NULL) {}
};

class StateNFA {
public:
    State* head;
    State** next;
    StateNFA(State* state): head(state) {}
};

State* buildNFA(string reg) {
    stack<StateNFA> stackNFA;
    for(int i = 0; i < reg.size(); ++i) {
        if(reg[i] == 20) {
            StateNFA nfa2 = stackNFA.top();
            stackNFA.pop();
            StateNFA nfa1 = stackNFA.top();
            stackNFA.pop();
            nfa1.next[0] = nfa2.head;
            nfa1.next = nfa2.next;
            stackNFA.push(nfa1);
        } else if(reg[i] == '*') {
            StateNFA nfa = stackNFA.top();
            stackNFA.pop();
            State* temp = new State(256);
            temp->next1 = nfa.head;
            nfa.next[0] = temp;
            nfa.next = &temp->next2;
            nfa.head = temp;
            stackNFA.push(nfa);
        } else if(reg[i] == '+') {
            StateNFA nfa = stackNFA.top();
            stackNFA.pop();
            State* temp = new State(256);
            temp->next1 = nfa.head;
            nfa.next[0] = temp;
            nfa.next = &temp->next2;
            stackNFA.push(nfa);
        } else if(reg[i] == '|') {
            StateNFA nfa2 = stackNFA.top();
            stackNFA.pop();
            StateNFA nfa1 = stackNFA.top();
            stackNFA.pop();
            State* temp1 = new State(256);
            temp1->next1 = nfa1.head;
            temp1->next2 = nfa2.head;
            State* temp2 = new State(256);
            nfa1.next[0] = temp2;
            nfa2.next[0] = temp2;
            StateNFA nfa = StateNFA(temp1);
            nfa.next = &temp2->next1;
            stackNFA.push(nfa);
        } else {
            State* temp = new State(reg[i]);
            StateNFA fragTemp = StateNFA(temp);
            fragTemp.next = &temp->next1;
            stackNFA.push(fragTemp);
        }
    }
    StateNFA nfa = stackNFA.top();
    State* final = new State(257);
    StateNFA fragTemp = StateNFA(final);
    nfa.next[0] = fragTemp.head;
    nfa.next = &final->next1;

    return nfa.head;
}

vector<State*> curr_list, next_list;
int left_ptr = 0, right_ptr = 0, max_right = left_ptr-1;
set<State*> doneStates;
void matcherRecursive(State* cur) {
    if(cur == NULL) return;
    if(doneStates.count(cur))  return;
    doneStates.insert(cur);
    
    if(cur->charac == 256) {
        matcherRecursive(cur->next1);
        matcherRecursive(cur->next2);
    } else if(cur->charac == 257) {
        max_right = right_ptr;
    } else {
        next_list.push_back(cur);
    }
}

int matcher(State* headState, string str, int start_i) {
    string result;
    curr_list.clear();
    next_list.clear();
    matcherRecursive(headState);
    left_ptr = start_i;
    max_right = left_ptr - 1;
    right_ptr = max_right + 1;
    curr_list = next_list;
    while(right_ptr < str.size()) {
        doneStates.clear();
        next_list.clear();
        for(int i = 0; i < curr_list.size(); ++i) {
            if(curr_list[i] == NULL) continue;
            if(curr_list[i]->charac == str[right_ptr]) {
                next_list.push_back(curr_list[i]->next1);
                if(next_list.back()->charac == 256) {
                    next_list.pop_back();
                    matcherRecursive(curr_list[i]->next1);
                } else if(next_list.back()->charac == 257) {
                    max_right = right_ptr;
                    next_list.pop_back();
                }
            }
        }
        curr_list = next_list;
        right_ptr++;
    }
    return max_right;
}

string inpost(string regex) {
    string result;
    int atoms_count = 0, union_count = 0;
    stack<pair<int, int> > p;
    for(int i = 0; i < regex.size(); ++i) {
        switch(regex[i]) {
            case '(':
                if(atoms_count > 1) {
                    atoms_count--;
                    result += 20;
                }
                p.push({atoms_count, union_count});
                atoms_count = 0;
                union_count = 0;
                break;
            case ')': 
            {
                while(--atoms_count > 0) 
                    result += 20;
                for(; union_count > 0; union_count--) 
                    result += '|';
                pair<int, int> temp;
                temp = p.top();
                p.pop();
                atoms_count = temp.first;
                union_count = temp.second;
                atoms_count++;
            }   break;
            case '|':
                while(--atoms_count > 0)
                    result += 20;
                union_count++;
                break;
            case '*':
            case '+':
                result += regex[i];
                break;
            default:
                if(atoms_count > 1) {
                    atoms_count--;
                    result += 20;
                }
                result += regex[i];
                atoms_count++;
                break;
        }
    }
    while(--atoms_count > 0) 
        result += 20;
    while(union_count-- > 0)
        result += '|';

    return result;
}

int main() {
    ifstream myfile("input.txt");
    if(!myfile.is_open()) {
        cout << "Error: Unable to open input file.\n";
        return 1;
    } 

    int reg_n;
    myfile >> reg_n;
    string line[reg_n];
    string str;
    for(int i = 0; i < reg_n; ++i)  myfile >> line[i];
    myfile >> str;

    State* heads[reg_n];
    for(int i = 0; i < reg_n; ++i) {
        string postRegex = inpost(line[i]);
        heads[i] = buildNFA(postRegex);
    }

    string ans;
    int str_ptr = 0;
    while(str_ptr < str.size()) {
        int best = -1, regex_i = -1, temp;
        for(int i = 0; i < reg_n; ++i) {
            int temp = matcher(heads[i], str, str_ptr);
            if(temp > best) {
                best = temp;
                regex_i = i;
            }
        }
        if(best < str_ptr) {
            ans += "@";
            ans += str[str_ptr];
            str_ptr++;
        } else {
            ans += "$";
            ans += to_string(regex_i+1);
            str_ptr = best+1;
        }
    }
    ans += "#";
    
    ofstream outfile;
    outfile.open("output.txt");
    outfile << ans;
    outfile.close();

    return 0;
}
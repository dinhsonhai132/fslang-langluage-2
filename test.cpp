#include <iostream>
#include <string>
#include <vector>
using namespace std;

enum VerType {
    INT, PLUS, MINUS, TIME, DIV, NONE, MEMORY, WHILE_LOOP, 
    FOR_LOOP, AND, OR, NOT, TRUE, FALSE, PRINT, INPUT, STRING, 
    TEMPORARY_MEMORY, FUNCTION, IF, THEN, ELSE, BIGGER, SMALLER
};

struct datatype {
    VerType type;
    int value;
    string name;
};

struct store_var {
    string name;
    int val;
};

vector<store_var> variables;
vector<vector<store_var>> memory;

class lexer {
private:
    string input;
    size_t pos;
    char cur;
    vector<datatype> tokens;
public:
    lexer(string input) : input(input), pos(0) {}

    void advance() {
        if (pos < input.size()) {
            pos++;
            cur = input[pos];
        } else {
            cur = '\0';
        }
    }

    void advance_to(int a) {
        if (pos < input.size()) {
            pos += a;
            cur = input[pos];
        } else {
            cur = '\0';
        }
    }

    vector<datatype> token() {

        while (pos < input.size()) {
            cur = input[pos];
            if (cur == '+') {
                tokens.push_back({PLUS, 0, ""});
                advance();

            } else if (cur == '-') {
                tokens.push_back({MINUS, 0, ""});
                advance();

            } else if (cur == '*') {
                tokens.push_back({TIME, 0, ""});
                advance();

            } else if (cur == '/') {
                tokens.push_back({DIV, 0, ""});
                advance();

            } else if (isdigit(cur)) {
                int num = 0;
                while (isdigit(input[pos]) && pos < input.size()) {
                    num = num * 10 + (input[pos] - '0');
                    advance();
                }
                tokens.push_back({INT, num, ""});

            } else if (cur == ';') {
                tokens.push_back({NONE, 0, ""});
                advance();

            } else if (cur == 'L' && input.substr(pos, 3) == "LET") {
                int val = 0;
                string name = "";
                advance_to(3);

                while (isspace(input[pos])) {
                    advance();
                }
                
                while (isalpha(input[pos]) && pos < input.size()) {
                    name += input[pos];
                    advance();
                }

                if (input[pos] == '=') {
                    advance();
                    while (isdigit(input[pos]) && pos < input.size()) {
                        val = val * 10 + (input[pos] - '0');
                        advance();
                    }
                }

                variables.push_back({name, val});
            } else if (isalpha(cur)) {
                string name = "";
                while (isalpha(cur)) {
                    name += input[pos];
                    advance();
                }
                tokens.push_back({TEMPORARY_MEMORY, 0, name});
            } else if (isspace(cur)) {
                advance();
            } else if (cur == 'P' && input.substr(pos, 5) == "PRINT") {
                tokens.push_back({PRINT, 0, ""});
                advance_to(5);
            } else if (cur == '<') {
                tokens.push_back({SMALLER, 0, ""});
                advance();
            } else if (cur == '>') {
                tokens.push_back({BIGGER, 0, ""});
                advance();
            } else {
                advance();
            }
        }
        return tokens;
    }
};


class parser {
private:
    size_t tok_idx;
    datatype cur_idx;
    vector<datatype> tokenize;
    string var;
    int val;
public:
    parser(vector<datatype> tokenize) : tokenize(tokenize), tok_idx(0) {}

    datatype get_next_tok() {
        if (tok_idx < tokenize.size()) {
            return tokenize[tok_idx++];
        }
        return {NONE, 0, ""};
    }
    
    int compair() {
        datatype left = get_next_tok();
        if (get_next_tok().type == BIGGER) {
            datatype right = get_next_tok();
            if (left.value > right.value) {
                return 1;
            } else {
                return 0;
            }
            tok_idx = 0;
        } else if (get_next_tok().type == SMALLER) {
            datatype right = get_next_tok();
            if (left.value < right.value) {
                return 1;
            } else {
                return 0;
            }
            tok_idx = 0;
        }
    }

    int factor() {
        cur_idx = get_next_tok();
        if (cur_idx.type == INT) {
            return cur_idx.value;
        } else if (cur_idx.type == TEMPORARY_MEMORY) {
            string var_name = cur_idx.name;
            for (auto &variable : variables) {
                if (variable.name == var_name) {
                    return variable.val;
                }
            }
        } else if (cur_idx.type == BIGGER || cur_idx.type == SMALLER) {
            tok_idx--;
            
        }
        return 0;
    }

    int term() {
        int result = factor();
        while (true) {
            cur_idx = get_next_tok();
            if (cur_idx.type == DIV) {
                int divisor = factor();
                if (divisor == 0) {
                    cerr << "Error: Division by zero" << endl;
                    return 0;
                }
                result /= divisor;
            } else if (cur_idx.type == TIME) {
                result *= factor();
            } else {
                tok_idx--;
                break;
            }
        }
        return result;
    }

    int expr() {
        int result = term();
        while (true) {
            cur_idx = get_next_tok();
            if (cur_idx.type == PLUS) {
                result += term();
            } else if (cur_idx.type == MINUS) {
                result -= term();
            } else {
                tok_idx--;
                break;
            }
        }
        return result;
    }

    void print_func() {
        
    } 

    void print_var() {
        for (auto &variable: variables) {
            cout << variable.name << "=" << variable.val << endl;
        }
    }
};

void info() {

}

void run() {
    cout << "Mercury [Version 0.0.2] \n(c) (this is test version) All rights reserved.\n type 'help?' for help" << endl;
    while (true) {
        string input;
        cout << ">>> ";
        getline(cin, input);
        if (input.empty()) {
            cout << endl;
            continue;
        }

        lexer lex(input);
        vector<datatype> tok = lex.token();
        parser par(tok);

        if (input == "help?") {
            cout << "Visit https://dinhsonhai132.github.io/fslang.github.io/fslang.html for more info" << endl;
        } else if (input == "var") {
            par.print_var();
        } else if (input == "exit") {
            cout << "Goodbye :)" << endl;
            break;  
        } else if (input == "info") {
            info();
        }
        else {
            cout << par.expr() << endl;
        }
    }
}

int main() {
    run();
    system("pause");
}

//
//  main.cpp
//  Second
//
//  Created by Александр Марков on 14/09/2017.
//  Copyright © 2017. All rights reserved.
//

#include <iostream>
#include <stack>
#include <string>


int get_closing_bracket(char bracket) {
    int ans_bracket = 0;
    if (bracket == ')') {
        ans_bracket = -1;
    }
    if (bracket == ']') {
        ans_bracket = -2;
    }
    if (bracket == '}') {
        ans_bracket = -3;
    }
    return ans_bracket;
}

void push_bracket(char bracket, std::stack<int>* stack) {
    if (bracket == '(') {
        stack->push(1);
    }
    if (bracket == '[') {
        stack->push(2);
    }
    if (bracket == '{') {
        stack->push(3);
    }
}

int main(int argc, const char * argv[]) {
    std::stack<int> checker;
    std::string brackets;
    size_t max_correct_substring = 0;
    std::string open_brackets = "([{";
    std::string close_brackets = ")]}";
    
    std::cin >> brackets;
    
    for (char bracket : brackets) {
        if (bracket == '(' || bracket == '[' || bracket == '{') {
            push_bracket(bracket, &checker);
            ++max_correct_substring;
        } else  if (checker.size() > 0) {
            int fit_bracket = get_closing_bracket(bracket);
            int last_bracket = checker.top();
            checker.pop();
            if (fit_bracket + last_bracket == 0) {
                ++max_correct_substring;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    if (max_correct_substring == brackets.length() && checker.empty()) {
        std::cout << "CORRECT";
    } else {
        std::cout << max_correct_substring;
    }
    return 0;
}

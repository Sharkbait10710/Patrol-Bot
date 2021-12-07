#include <iostream>
#include <bits/stdc++.h>
using namespace std;

int main() {
    //cout << "Press crtl+alt+n to run C++ programs quickly\n";
    string line = "((((((((((((";
      
    // Vector of string to save tokens
    vector <string> tokens;
      
    // stringstream class check1
    stringstream check1(line);
      
    string intermediate;
      
    // Tokenizing w.r.t. space ' '
    while(getline(check1, intermediate, ' '))
    {
        tokens.push_back(intermediate);
    }
      
    // Printing the token vector
    for(int i = 0; i < tokens.size(); i++)
        cout << tokens[i] << '\n';
    cout << "Number of parentheses is: " << line.size();
    return 0;
}
//
// Created by haley on 10/6/25.
//

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

 // Just takes in a file, puts the whole thing into a stringstream and then stores the program into an attribute
#ifndef VERTEXSHADERHANDLE_H
#define VERTEXSHADERHANDLE_H

class shaderHandle {
        ifstream file;
        string line;
    public:
    string program;
    shaderHandle() {

    }
    string load(string filename) {
        std::stringstream buffer;
        file.open(filename);
        if (!file.is_open()) {
            cerr << "Failed to open file: " << filename << endl;
        }
        buffer << file.rdbuf();
        program = buffer.str();

    }
    void parse() {
        while (getline(file, line)) {
            for (char &c : line) {
                program.push_back(c);
            }

            }
        }
    };


#endif //VERTEXSHADERHANDLE_H

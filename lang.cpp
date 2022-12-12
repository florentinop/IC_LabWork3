#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <cmath>
#include <vector>
#include <set>

using namespace std;

int main(int argc, char* argv[]) {
    int k = 1;
    int alpha = 0;
    if (argc < 2) {
        cerr << "Usage: lang <model file> <test file> [-k <k>] [-a <alpha>]" << endl;
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-k") {
            k = stoi(argv[i + 1]);
            if (k <= 0) {
                cerr << "k must be an integer greater than 0" << endl;
                return 2;
            }
            break;
        }
    }
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-a") {
            alpha = stoi(argv[i + 1]);
            if (alpha < 0) {
                cerr << "alpha must be an integer greater or equal to 0" << endl;
                return 3;
            }
            break;
        }
    }
    
    unordered_map<string, unordered_map<unsigned char, float>> ProbTable;
    ifstream readStream;
    readStream.open(argv[1]);
    char c;
    string kChars;
    set<unsigned char> alphabet;

    while (readStream >> noskipws >> c) {
        // ignore any non-ASCII character
        
        while(c != '\n'){
            
                
                

        }
    }


   return 0;
}


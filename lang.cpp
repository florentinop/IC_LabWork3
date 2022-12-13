#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <cmath>
#include <vector>
#include <set>

using namespace std;

int main(int argc, char *argv[])
{
    int k = 1;
    int alpha = 0;
    if (argc < 2)
    {
        cerr << "Usage: lang <model file> <test file> [-k <k>] [-a <alpha>]" << endl;
        return 1;
    }
    for (int i = 1; i < argc; i++)
    {
        if (string(argv[i]) == "-k")
        {
            k = stoi(argv[i + 1]);
            if (k <= 0)
            {
                cerr << "k must be an integer greater than 0" << endl;
                return 2;
            }
            break;
        }
    }
    for (int i = 1; i < argc; i++)
    {
        if (string(argv[i]) == "-a")
        {
            alpha = stoi(argv[i + 1]);
            if (alpha < 0)
            {
                cerr << "alpha must be an integer greater or equal to 0" << endl;
                return 3;
            }
            break;
        }
    }

    unordered_map<string, unordered_map<unsigned char, float>> ProbTable;
    ifstream model;
    model.open(argv[1]);
    string kChars;
    string line;
    set<unsigned char> alphabet;

    // for each line
    while (getline(model, line))
    {
        kChars = "";
        // extract context
        int i;
        for (i = 0; i < k; i++)
        {
            kChars.push_back(line[i]);
        }
        i += 4;

        while (i++ < (int)line.size())
        {
            //get following char
            char c = line[i];
            string prob;
            i += 3;

            // get probability
            while (line[i] != ')')
            {
                prob.push_back(line[i++]);
            }
            ProbTable[kChars].insert({c, stof(prob)});
            i++;
        }
    }
    model.close();

    // //print ProbTable
    // for (const auto &x : ProbTable)
    // {
    //     cout << x.first << " -> ";
    //     for (auto y : x.second)
    //     {
    //         cout << "(" << y.first << ", " << y.second << ") ";
    //     }
    //     cout << endl;
    // }
    
    //compute estimated number of bits required to compress test file
    ifstream test;
    char c;
    kChars.clear();
    float req_bits = 0.0;
    test.open(argv[2]);
    //initialize window
    for (int i = 0; i < k; i++){
        kChars.push_back('A');
    }

    while (test >> noskipws >> c) {
        // ignore any non-ASCII character
        if (c < 0 || c=='\n') {
            continue;
        }

        if(ProbTable[kChars].find(c) == ProbTable[kChars].end())
            cout << "comb not found. Do something about it..." << endl;
        else{
            req_bits -= log(ProbTable[kChars].find(c)->second);
            cout << kChars << c << " -> " << ProbTable[kChars].find(c)->second << endl;
        }
        
        // shift window
        kChars.erase(0,1);
        kChars.push_back(c);
        
    }

    //print results
    cout << "Estimated number of bits required to compress " << argv[2] << ": " << req_bits << endl;

    test.close();
    
    

    return 0;
}

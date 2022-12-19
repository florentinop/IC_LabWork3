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
    set<unsigned char> new_fcm_alphabet;

    // for each line
    while (getline(model, line))
    {
        kChars = "";
        // extract context
        int i;
        for (i = 0; i < k; i++)
        {
            kChars.push_back(line[i]);
            alphabet.insert(line[i]);
            new_fcm_alphabet.insert(line[i]);
        }
        i += 4;

        while (i++ < (int)line.size())
        {
            //get following char
            char c = line[i];
            alphabet.insert(c);
            new_fcm_alphabet.insert(c);
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

    //initialize new characters map
    unordered_map<string, unordered_map<unsigned char, float>> new_fcm;

    //initialize window
    for (int i = 0; i < k; i++){
        kChars.push_back('A');
    }
    new_fcm_alphabet.insert('A');

    while (test >> noskipws >> c) {
        // ignore any non-ASCII character
        if (c < 0 || c=='\n') {
            continue;
        }

        if(ProbTable.find(kChars) == ProbTable.end()){
            cout << "line not found. Adding context to new table..." << endl; 
            for (size_t i = 0; i < kChars.size(); i++)
            {
                new_fcm_alphabet.insert(kChars[i]);
            }
              
            if (new_fcm.find(kChars) == new_fcm.end()) {
                new_fcm[kChars] = {{c, 0}};
                new_fcm_alphabet.insert(c);
            // check if char not in frequency table
            } else if (new_fcm[kChars].find(c) == new_fcm[kChars].end()) {
                new_fcm[kChars].insert({c, 0});
                new_fcm_alphabet.insert(c);
            }
            new_fcm[kChars][c]++;  // increment frequency of character 
        }
        else if(ProbTable[kChars].find(c) == ProbTable[kChars].end()){
            int totalC = 0; 
            for (auto y: ProbTable[kChars]) {
                totalC += y.second;
            }   
            cout << "comb not found... " << "Prob= " << (float)(alpha) / (totalC + alpha*alphabet.size()) << endl;
            req_bits -= log((float)(alpha) / (totalC + alpha*ProbTable.size()));
        }
        else{
            int totalC = 0; 
            for (auto y: ProbTable[kChars]) {
                totalC += y.second;
            }    
              
            req_bits -= log((float)(ProbTable[kChars].find(c)->second + alpha) / (totalC + alpha*ProbTable.size()));
            cout << kChars << c << " -> " << ProbTable[kChars].find(c)->second << ", prob: " << (ProbTable[kChars].find(c)->second + alpha) / (totalC + alpha*alphabet.size()) << endl;
        }
        
        // shift window
        kChars.erase(0,1);
        kChars.push_back(c);
        
    }

    //Add the missing context probabilities to the formula
    for (const auto &x : new_fcm)
    {
        cout << x.first << " -> ";
        for (auto y : x.second)
        {
            int totalC = 0; 
            for (auto y: ProbTable[kChars]) {
                totalC += y.second;
            }    
            req_bits -= log((float)(y.second + alpha) / (totalC + alpha*new_fcm.size()));
            cout << "(" << y.first << ", " << y.second << ", Prob: " << (float)(y.second + alpha) / (totalC + alpha*new_fcm.size()) << ") " << endl;
        }
        
    }
    cout << new_fcm_alphabet.size() << endl;
    //print results
    cout << "Estimated number of bits required to compress " << argv[2] << ": " << req_bits << endl;

    test.close();
    
    

    return 0;
}

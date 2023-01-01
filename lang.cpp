#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <cmath>
#include <vector>
#include <set>
#include <sstream>
#include <codecvt>

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

    const clock_t begin = clock();
    unordered_map<wstring, unordered_map<wchar_t, float>> ProbTable;
    wifstream wif(argv[1]);
    wif.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wstringstream wss;
    wss << wif.rdbuf();
    wstring model = wss.str();
    wstring kChars;
    wchar_t c;
    set<wchar_t> alphabet;
    set<wchar_t> new_fcm_alphabet;

    // for each line
    while (!model.empty())
    {
        kChars.clear();
        // extract context
        for (int i = 0; i < k; i++)
        {
            kChars.push_back(model[0]);
            alphabet.insert(model[0]);
            new_fcm_alphabet.insert(model[0]);
            model = model.substr(1, model.size());
        }
        
        model = model.substr(3, model.size());
    
        while (model[1] != '\n')
        {
            //get following char
            model = model.substr(2, model.size());
            c = model[0];
            
            alphabet.insert(c);
            new_fcm_alphabet.insert(c);
            wstring prob;
            model = model.substr(3, model.size());

            // get probability
            while (model[0] != '>')
            {
                prob.push_back(model[0]);
                model = model.substr(1, model.size());
            }
            
            ProbTable[kChars].insert({c, stof(prob)});
        }
        model = model.substr(2, model.size());
        
        // exit(0);
    }
    wif.close();

    // //print ProbTable
    // for (const auto &x : ProbTable)
    // {
    //     wcout << x.first << " -> ";
    //     for (auto y : x.second)
    //     {
    //         wcout << "(" << y.first << ", " << y.second << ") ";
    //     }
    //     wcout << endl;
    // }
    
    //compute estimated number of bits required to compress test file
    wifstream wif2(argv[2]);
    wif2.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wstringstream wss2;
    wss2 << wif2.rdbuf();
    wstring test = wss2.str();
    
    kChars.clear();
    float req_bits = 0.0;

    //initialize new characters map
    unordered_map<wstring, unordered_map<wchar_t, float>> new_fcm;

    //initialize window
    for (int i = 0; i < k; i++){
        kChars.push_back(L'A');
    }
    new_fcm_alphabet.insert(L'A');

    while (!test.empty()) {
        
        c = test[0];
        test = test.substr(1, test.size());
        // ignore \n
        if (c=='\n') {
            continue;
        }

        if(ProbTable.find(kChars) == ProbTable.end()){
            // wcout << "line not found. Adding context to new table..." << endl; 
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
            // wcout << "comb not found... " << "Prob= " << (float)(alpha) / (totalC + alpha*alphabet.size()) << endl;
            req_bits -= log((float)(alpha) / (totalC + alpha*ProbTable.size()));
        }
        else{
            int totalC = 0; 
            for (auto y: ProbTable[kChars]) {
                totalC += y.second;
            }    
              
            req_bits -= log((float)(ProbTable[kChars].find(c)->second + alpha) / (totalC + alpha*ProbTable.size()));
            // wcout << kChars << c << " -> " << ProbTable[kChars].find(c)->second << ", prob: " << (ProbTable[kChars].find(c)->second + alpha) / (totalC + alpha*alphabet.size()) << endl;
        }
        
        // shift window
        kChars.erase(0,1);
        kChars.push_back(c);
        
    }

    //Add the missing context probabilities to the formula
    for (const auto &x : new_fcm)
    {
        // cout << x.first << " -> ";
        for (auto y : x.second)
        {
            int totalC = 0; 
            for (auto y: ProbTable[kChars]) {
                totalC += y.second;
            }    
            req_bits -= log2((float)(y.second + alpha) / (totalC + alpha*new_fcm.size()));
            // cout << "(" << y.first << ", " << y.second << ", Prob: " << (float)(y.second + alpha) / (totalC + alpha*new_fcm.size()) << ") " << endl;
        }
        
    }

    if(req_bits < 0){
        req_bits = 1;
    }

    // cout << new_fcm_alphabet.size() << endl;
    //print results
    cout << "Estimated number of bits required to compress " << argv[2] << ": " << req_bits << endl;

    wif2.close();
    
    cout << "Elapsed time: " << float(clock() - begin) / CLOCKS_PER_SEC << " seconds" << endl;
    

    return 0;
}

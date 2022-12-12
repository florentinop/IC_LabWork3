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
        cerr << "Usage: fcm <text> [-k <k>] [-a <alpha>]" << endl;
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
    unordered_map<string, unordered_map<unsigned char, unsigned int>> frequencyTable;
    ifstream readStream;
    readStream.open(argv[1]);
    char c;
    string kChars, initialKChars;
    set<unsigned char> alphabet;


    while (readStream >> noskipws >> c && (int)kChars.size() < k) {
        // ignore any non-ASCII character
        if (c < 0) {
            continue;
        }
        kChars += c;
    }
    initialKChars = kChars;
    readStream.seekg(k);  // needed to not skip the k+1 character
    while (readStream >> noskipws >> c) {
        // ignore any non-ASCII character
        if (c < 0) {
            continue;
        }
        // check if key not in frequency table
        if (frequencyTable.find(kChars) == frequencyTable.end()) {
            frequencyTable[kChars] = {{c, 0}};
            alphabet.insert(c);
        // check if char not in frequency table
        } else if (frequencyTable[kChars].find(c) == frequencyTable[kChars].end()) {
            frequencyTable[kChars].insert({c, 0});
            // add read character to alphabet set
            alphabet.insert(c);
        }
        frequencyTable[kChars][c]++;  // increment frequency of character
        kChars += c;
        kChars = kChars.substr(1, k);
    }
    // count occurrence of all characters
    unordered_map<unsigned char, unsigned int> charFrequency;
    
    unsigned int charCount = 0;
    for (auto x: initialKChars) {
        if (charFrequency.find(x) == charFrequency.end()) {
            charFrequency[x] = 0;
        }
        charFrequency[x]++;
        charCount++;
    }
    for (const auto& x: frequencyTable) {
        for (auto y: x.second) {
            if (charFrequency.find(y.first) == charFrequency.end()) {
                charFrequency[y.first] = 0;
            }
            charFrequency[y.first] += y.second;
            charCount += y.second;
        }
    }
    // calculate text entropy
    float entropy = 0.0;
    for (auto x: charFrequency) {
        auto p = (float) x.second / (float) charCount;
        entropy -= p * log2(p);
    }
    cout << "Entropy: " << entropy << endl;

    //derive probability table and save it to a file somehow
    map<char, float> probabilities;
    for (auto x: charFrequency) {
        auto p = (float) x.second / (float) charCount;
        probabilities[x.first]=p;
    }

   // print frequency table
   for (const auto& x: frequencyTable) {
       cout << x.first << " -> ";
       for (auto y: x.second) {
           cout << "(" << y.first << ", " << y.second << ") ";
        //    cout << (int)y.first;
       }
       cout << endl;
   }

    // get total characters per row
    vector <unsigned int> totalC(frequencyTable.size());
    int i=0;
    for (const auto& x: frequencyTable) {
        for (auto y: x.second) {
            totalC[i] += y.second;
        }
        i++;
    }


    // store probability table
    i=0;
    ofstream out_file;
    out_file.open ("model_" + (string)argv[1]);
    for (const auto& x: frequencyTable) {
        out_file << x.first << " -> ";
        for (auto y: x.second) {
            out_file << "(" << y.first << ", " << (float)(y.second + alpha) / (float)(totalC[i] + alpha*alphabet.size())  << ")";
        }
        i++;
        out_file << endl;
    }
    out_file << alphabet.size();
    out_file.close();

   return 0;
}


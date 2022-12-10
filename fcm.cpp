#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <fstream>
#include <cmath>

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
    unordered_map<string, unordered_map<char, unsigned int>> frequencyTable;
    ifstream readStream;
    readStream.open(argv[1]);
    char c;
    string kChars, initialKChars;
    while (readStream >> noskipws >> c && (int)kChars.size() < k) {
        // ignore any non-ASCII character
        if (c =='\n') {
            continue;
        }
        kChars += c;
    }
    initialKChars = kChars;
    readStream.seekg(k);  // needed to not skip the k+1 character
    while (readStream >> noskipws >> c) {
        // ignore any non-ASCII character
        if (c =='\n') {
            continue;
        }
        // check if key not in frequency table
        if (frequencyTable.find(kChars) == frequencyTable.end()) {
            frequencyTable[kChars] = {{c, 0}};
        // check if char not in frequency table
        } else if (frequencyTable[kChars].find(c) == frequencyTable[kChars].end()) {
            frequencyTable[kChars].insert({c, 0});
        }
        frequencyTable[kChars][c]++;  // increment frequency of character
        kChars += c;
        kChars = kChars.substr(1, k);
    }
    // count occurrence of all characters
    unordered_map<char, unsigned int> charFrequency;
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
       }
       cout << endl;
   }

   // store probability table
   ofstream out_file;
   out_file.open ("model_" + (string)argv[1]);
   for (const auto& x: probabilities) {
       out_file << x.first << "\t" << x.second << endl;
   }

   return 0;
}


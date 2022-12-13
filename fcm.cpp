#include <iostream>
#include <string>
#include <unordered_map>
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
    unordered_map<char, unsigned int> charFrequency;
    unordered_map<string, unsigned int> numberOfSuffixes;
    unordered_map<string, unordered_map<char, unsigned int>> frequencyTable;
    ifstream readStream(argv[1], ifstream::binary);
    string kChars;
    char c;
    int totalChars = 0;
    if (readStream.is_open()) {
        while (readStream.good() && (int) kChars.size() < k) {
            readStream >> noskipws >> c;
            // count occurrence of all characters
            if (charFrequency.find(c) == charFrequency.end()) {
                charFrequency[c] = 0;
            }
            charFrequency[c]++;
            totalChars++;
            kChars += c;
        }
    }
    readStream.seekg(k);  // needed to not skip the k+1 character
    if (readStream.is_open()) {
        while (readStream.good()) {
            readStream >> noskipws >> c;
            // check if key not in frequency table
            if (frequencyTable.find(kChars) == frequencyTable.end()) {
                frequencyTable[kChars] = {{c, 0}};
                // check if char not in frequency table
            } else if (frequencyTable[kChars].find(c) == frequencyTable[kChars].end()) {
                frequencyTable[kChars].insert({c, 0});
            }
            frequencyTable[kChars][c]++;  // increment frequency of character
            // increase number of suffixes of kChars
            if (numberOfSuffixes.find(kChars) == numberOfSuffixes.end()) {
                numberOfSuffixes[kChars] = 0;
            }
            numberOfSuffixes[kChars]++;
            // count occurrence of all characters
            if (charFrequency.find(c) == charFrequency.end()) {
                charFrequency[c] = 0;
            }
            charFrequency[c]++;
            totalChars++;
            // update kChars
            kChars += c;
            kChars = kChars.substr(1, k);
        }
    }
    // calculate text entropy
    float entropy = 0.0;
    for (auto x: charFrequency) {
        auto p = (float) x.second / (float) totalChars;
        entropy -= p * log2(p);
    }
    cout << "Entropy: " << entropy << endl;
    size_t alphabetSize = charFrequency.size();
    ofstream writeStream;
    writeStream.open("model_" + (string)argv[1], ios::out | ios::binary);

    for (const auto& x: frequencyTable) {
        writeStream << x.first << " -> ";
        for (auto y: x.second) {
            writeStream << "(" << y.first << ", " << (float) (y.second + alpha) / (float) (numberOfSuffixes[x.first] + alpha * alphabetSize) << ") ";
        }
        writeStream << endl;
    }
   return 0;
}

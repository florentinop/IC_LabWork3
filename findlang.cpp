#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <codecvt>
#include <unordered_map>
#include <string>

using namespace std;

unordered_map<wstring, unordered_map<wchar_t, float>> makeModel(const string& model, int k, int alpha) {
    unordered_map<wstring, unordered_map<wchar_t, float>> res;
    unordered_map<wchar_t, unsigned int> charFrequency;
    unordered_map<wstring, unsigned int> numberOfSuffixes;
    unordered_map<wstring, unordered_map<wchar_t, unsigned int>> frequencyTable;
    wifstream wif(model);
    wif.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wstringstream wss;
    wss << wif.rdbuf();
    wstring data = wss.str();
    wstring kChars;
    wchar_t c;
    int totalChars = 0;
    while ((int) kChars.size() < k) {
        c = data[0];
        data = data.substr(1, data.size());
        // count occurrence of all characters
        if (charFrequency.find(c) == charFrequency.end()) {
            charFrequency[c] = 0;
        }
        charFrequency[c]++;
        totalChars++;
        kChars += c;
    }
    while (!data.empty()) {
        c = data[0];
        data = data.substr(1, data.size());
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
    size_t alphabetSize = charFrequency.size();
    for (const auto& x: frequencyTable) {
        for (auto y: x.second) {
            res[x.first][y.first] = (float) (y.second + alpha) / (float) (numberOfSuffixes[x.first] + alpha * alphabetSize);
        }
    }
    return res;
}

int main(int argc, char* argv[]) {
    int k = 1;
    int alpha = 0;
    int kArg = -1, alphaArg = -1;
    if (argc < 2) {
        cerr << "Usage: findlang <model> <model> ... <model> <text> < [-k <k>] [-a <alpha>]" << endl;
        return 1;
    }
    for (int i = 1; i < argc; i++) {
        if (string(argv[i]) == "-k") {
            k = stoi(argv[i + 1]);
            kArg = i;
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
            alphaArg = i;
            if (alpha < 0) {
                cerr << "alpha must be an integer greater or equal to 0" << endl;
                return 3;
            }
            break;
        }
    }
    vector<string> models;
    for (int i = 1; i < argc; i++) {
        if (i == kArg || i == kArg + 1 || i == alphaArg || i == alphaArg + 1) {
            continue;
        }
        models.emplace_back(string(argv[i]));
    }
    string text = models.back();
    models.pop_back();
    vector<unordered_map<wstring, unordered_map<wchar_t, float>>> probabilityTables;
    probabilityTables.reserve(models.size());
    for (const auto& model: models) {
        probabilityTables.push_back(makeModel(model, k, alpha));
    }
    return 0;
}

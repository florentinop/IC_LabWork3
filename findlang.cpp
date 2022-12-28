#include <iostream>
#include <fstream>
#include <vector>
#include <sstream>
#include <codecvt>
#include <unordered_map>
#include <string>
#include <cmath>
#include <set>

using namespace std;

string trim(const string& s) {
    const string WHITESPACE = " \t\v";
    size_t start = s.find_first_not_of(WHITESPACE);
    string res = (start == string::npos) ? s : s.substr(start);
    size_t end = res.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? res : res.substr(0, end + 1);
}

void printModel(const unordered_map<wstring, unordered_map<wchar_t, unsigned int>>& model) {
    for (const auto& x: model) {
        wcout << x.first << " -> ";
        for (const auto& y: x.second) {
            wcout << "(" << y.first << "," << y.second << ")";
        }
        wcout << endl;
    }
}

unordered_map<string, vector<string>> getTextsFromUser() {
    unordered_map<string, vector<string>> res;
    string userTexts, text;
    do {
        cout << "Provide the path to the texts corresponding to the same language, comma separated: "
                "(<ENTER> for none)" << endl;
        getline(cin, userTexts);
        // ENTER was pressed
        if (userTexts.empty()) {
            break;
        }
        vector<string> texts;
        size_t initialIdx = 0, commaIdx;
        while ((commaIdx = userTexts.find(',')) != string::npos) {
            text = userTexts.substr(initialIdx, commaIdx);
            texts.emplace_back(trim(text));
            initialIdx = commaIdx + 1;
            userTexts = userTexts.substr(initialIdx, userTexts.size());
        }
        texts.emplace_back(trim(userTexts));
        cout << "Name the language of the previous mentioned texts:" << endl;
        getline(cin, userTexts);
        res[trim(userTexts)] = texts;
        texts.clear();
    } while (!userTexts.empty());
    return res;
}

unordered_map<wstring, unordered_map<wchar_t, unsigned int>> makeModel(const string& model,
                                                                       int k) {
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
    return frequencyTable;
}

float estimateBitsFromModel(const string& text,
                            unordered_map<wstring, unordered_map<wchar_t, unsigned int>> model,
                            int k, int alpha) {
    wifstream wif(text);
    wif.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wstringstream wss;
    wss << wif.rdbuf();
    wstring data = wss.str();
    wstring kChars;
    // Initialize kChars
    for (int i = 0; i < k; i++) {
        kChars += L"A";
    }
    set<wchar_t> alphabet;
    // Initialize alphabet with all characters from model
    for (const auto& x: model) {
        for (const auto& ch: x.first) {
            alphabet.insert(ch);
        }
        for (const auto& y: x.second) {
            alphabet.insert(y.first);
        }
    }
    float bits = 0.0;
    wchar_t c;
    while (!data.empty()) {
        c = data[0];
        data = data.substr(1);
        if (model.find(kChars) == model.end()) {
            for (const auto& ch: kChars) {
                alphabet.insert(ch);
            }
            model[kChars] = {{c, 1}};
            alphabet.insert(c);
        }
        else if (model[kChars].find(c) == model[kChars].end()) {
            model[kChars].insert({c, 1});
            alphabet.insert(c);
        }
        unsigned int kCharsSuffixes = 0;
        for (const auto& x: model[kChars]) {
            kCharsSuffixes += x.second;
        }
        bits -= log2((float) (model[kChars][c] + alpha) /
                     (float) (kCharsSuffixes + alpha * alphabet.size()));
        // Update kChars
        kChars += c;
        kChars = kChars.substr(1);
    }
    return bits;
}

int main(int argc, char* argv[]) {
    int k = 1;
    int alpha = 0;
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
    unordered_map<string, vector<string>> languageTexts = getTextsFromUser();
//    for (const auto& x: languageTexts) {
//        cout << x.first << " -> ";
//        for (const auto& y: x.second) {
//            cout << y << ", ";
//        }
//        cout << endl;
//    }
    if (languageTexts.empty()) {
        cerr << "No model files specified! Exiting." << endl;
        return 4;
    }
    string text;
    cout << "Provide the path to the text to have the language it was written in guessed:" << endl;
    getline(cin, text);
    if (text.empty()) {
        cerr << "No text file specified! Exiting." << endl;
        return 5;
    }
    text = trim(text);
    string bestLanguage;
    string bestModel;
    float bestBits = numeric_limits<float>::infinity();
    for (const auto& language: languageTexts) {  // For each language
        for (const auto& modelText: language.second) {  // For each model
            unordered_map<wstring, unordered_map<wchar_t, unsigned int>> model = makeModel(modelText, k);
            float bits = estimateBitsFromModel(text, model, k, alpha);
            if (bits < bestBits) {
                bestBits = bits;
                bestLanguage = language.first;
                bestModel = modelText;
            }
        }
    }
    cout << "The text was likely written in " << bestLanguage << "!" << endl;
    cout << "The best model is " << bestModel << " and needs " << bestBits << " to compress the text."
            << endl;
    return 0;
}

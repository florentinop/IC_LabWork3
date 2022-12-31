#include <iostream>
#include <vector>
#include <unordered_map>
#include <fstream>
#include <sstream>
#include <codecvt>
#include <set>
#include <cmath>
#include <map>
#include <unordered_set>

using namespace std;

string trim(const string& s) {
    const string WHITESPACE = " \t\v";
    size_t start = s.find_first_not_of(WHITESPACE);
    string res = (start == string::npos) ? s : s.substr(start);
    size_t end = res.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? res : res.substr(0, end + 1);
}

unordered_map<string, unordered_set<string>> getTextsFromUser() {
    unordered_map<string, unordered_set<string>> res;
    string userTexts, text;
    do {
        cout << "Provide the path to the model texts corresponding to the same language, "
                "comma separated: (<ENTER> for none)" << endl;
        getline(cin, userTexts);
        // ENTER was pressed
        if (userTexts.empty()) {
            break;
        }
        unordered_set<string> texts;
        size_t initialIdx = 0, commaIdx;
        while ((commaIdx = userTexts.find(',')) != string::npos) {
            text = userTexts.substr(initialIdx, commaIdx);
            texts.insert(trim(text));
            initialIdx = commaIdx + 1;
            userTexts = userTexts.substr(initialIdx, userTexts.size());
        }
        texts.insert(trim(userTexts));
        cout << "Name the language of the previous mentioned texts:" << endl;
        getline(cin, userTexts);
        string language = trim(userTexts);
        if (res.find(language) == res.end()) {
            res[language] = texts;
        } else {
            res[language].insert(texts.begin(), texts.end());
        }
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
    if (data.empty()) {
        return unordered_map<wstring, unordered_map<wchar_t, unsigned int>>();
    }
    data = data.substr(0, 200000);  // Limit data size
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
        data = data.substr(1);
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

unordered_map<unsigned int, float> getSegmentBits(const string& text,
                            unordered_map<wstring, unordered_map<wchar_t, unsigned int>> model,
                            int k, int alpha) {
    const wstring WHITESPACE = L" \t\n\r\v\f";
    unordered_map<unsigned int, float> res;
    wifstream wif(text);
    wif.imbue(locale(locale(), new codecvt_utf8<wchar_t>));
    wstringstream wss;
    wss << wif.rdbuf();
    wstring data = wss.str();
    if (data.empty()) {
        return res;
    }
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
    wchar_t c;
    unsigned int segmentStart = 0;
    size_t segmentEnd = 0;
    while (segmentStart < data.size()) {
        segmentEnd = data.find_first_of(WHITESPACE, segmentStart);
        if (segmentEnd == string::npos) {
            segmentEnd = data.size();
        } else {
            wstring afterWhitespace = data.substr(segmentEnd);
            size_t aux = afterWhitespace.find_first_not_of(WHITESPACE);
            if (aux == string::npos) {
                segmentEnd = data.size();
            } else {
                segmentEnd += aux;
            }
        }
        float bits = 0.0;
        for (size_t i = 0; i < segmentEnd - segmentStart; i++) {
            c = data[segmentStart + i];
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
        res.insert({segmentStart, bits});
        segmentStart = segmentEnd;
    }
    return res;
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
    unordered_map<string, unordered_set<string>> languageTexts = getTextsFromUser();
    if (languageTexts.empty()) {
        cerr << "No model files specified! Exiting." << endl;
        return 4;
    }
    string text;
    cout << "Provide the path to the text composed of multiple languages:" << endl;
    getline(cin, text);
    if (text.empty()) {
        cerr << "No text file specified! Exiting." << endl;
        return 5;
    }
    text = trim(text);
    map<unsigned int, string> segmentIndexes;
    unordered_map<string, unordered_map<unsigned int, float>> languageSegmentBits;
    // Make models and store them
    for (const auto& language: languageTexts) {
        for (const auto& modelText: language.second) {
            unordered_map<wstring, unordered_map<wchar_t, unsigned int>> model = makeModel(modelText, k);
            if (model.empty()) {
                cout << "Skipped " << modelText << endl;
            } else {
                cout << "Finished reading " << modelText << endl;
                unordered_map<unsigned int, float> segmentBits = getSegmentBits(text,model,
                                                                                        k, alpha);
                if (segmentBits.empty()) {
                    cerr << "Invalid text file " << text << endl;
                    return 6;
                }
                if (segmentIndexes.empty()) {
                    // Store start position of all segments
                    for (auto segment: segmentBits) {
                        segmentIndexes.insert({segment.first, language.first});
                    }
                }
                languageSegmentBits.insert({language.first, segmentBits});
            }
        }
    }
    for (const auto& idx: segmentIndexes) {
        float bestBits = languageSegmentBits.begin()->second.find(idx.first)->second;
        string bestLanguage = languageSegmentBits.begin()->first;
        for (const auto& languageSegment: languageSegmentBits) {
            if (languageSegmentBits[languageSegment.first][idx.first] < bestBits) {
                bestBits = languageSegmentBits[languageSegment.first][idx.first];
                bestLanguage = languageSegment.first;
            }
        }
        segmentIndexes[idx.first] = bestLanguage;
    }
    // Print the start of each language
    cout << segmentIndexes.begin()->first << " - " << segmentIndexes.begin()->second;
    auto prevLanguage = segmentIndexes.begin()->second;
    for (const auto& segment: segmentIndexes) {
        if (segment.second != prevLanguage) {
            cout << "; " << segment.first << " - " << segment.second;
            prevLanguage = segment.second;
        }
    }
    cout << endl;
    return 0;
}

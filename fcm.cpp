

#include <iostream>
#include <vector>
#include <fstream>
#include <unordered_map>

using namespace std;

int main(int argc, char* argv[]) {
    ifstream file;
    file.open (argv[argc-1], ios::in);
    char c;

    std::unordered_map<string, unordered_map<char, int>> frequency_table;

    string prev;
    if (file.is_open()){
        while (file.get(c)){
            frequency_table[prev][c]++;
        }
        file.close();
    }
    // cout << frequency_table["a"]['']

    

    return 0;
}
// usealgo.cpp -- using several STL elements
#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <map>
#include <algorithm>
#include <iterator>
#include <cctype>
using namespace std;

char toLower(char ch) {
    return std::tolower(ch);
}
string & ToLower(string & str);
void display(const string & str);

int main()
{
    vector<string> words;
    cout << "Enter words (enter quit to quit):\n";
    string input;
    while (cin >> input && input != "quit") {
        words.push_back(input);
    }

    cout << "You entered the following words:\n";
    for_each(words.begin(), words.end(), display);
    cout << endl;

    //place words in set, convert to lowercase
    set<string> wordset;
    transform(words.begin(), words.end(), insert_iterator<set<string>>(wordset, wordset.begin()), ToLower);
    cout << "\nAlphabetic list of words:\n";
    for_each(wordset.begin(), wordset.end(), display);
    cout << endl;

    //place word and frequency in map
    map<string, int> wordmap;
    set<string>::iterator si;
    for (si = wordset.begin(); si != wordset.end(); si++) {
        wordmap[*si] = count(words.begin(), words.end(), *si);
    }

    //display map contents
    cout << "\nWord frequency:\n";
    for (si = wordset.begin(); si != wordset.end(); si++) {
        cout << *si << ": " << wordmap[*si] << endl;
    }
    cout << endl;
    return 0;
}

string & ToLower(string & str)
{
    transform(str.begin(), str.end(), str.begin(), toLower);
    return str;
}

void display(const string & str)
{
    cout << str << " ";
}
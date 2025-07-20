// strfile.cpp -- read strings from a file
#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

int main()
{
    using namespace std;
    ifstream fin;
    fin.open("tobuy.txt");
    if (!fin.is_open()) {
        cerr << "Could not open the file." << endl;
        exit(EXIT_FAILURE);
    }
    string item;
    int count = 0;
    getline(fin, item,':');
    while (fin) {
        count++;
        cout << count << ": " << item << endl;
        getline(fin, item, ':');
    }
    cout << "Done\n";
    fin.close();
    return 0;
}
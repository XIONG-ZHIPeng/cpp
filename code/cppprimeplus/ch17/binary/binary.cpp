// binary.cpp -- binary file I/O
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

inline void eatline() {
    while (std::cin.get() != '\n')
        continue; // discard rest of line
}

struct planet
{
    char name[20]; // name of planet
    double population; // population
    double g; // acceleration due to gravity
};

const char * file = "planet.dat"; // name of binary file

int main()
{
    using namespace std;
    planet p1;
    cout << fixed << right;

    //show intial contents

    ifstream fin;
    fin.open(file, ios_base::in | ios_base::binary); // binart file
    if (fin.is_open())
    {
        cout << "Here are the current contents of the " << file << " file:\n";
        
        while (fin.read((char *) &p1, sizeof p1))
        {
            cout << setw(20) << p1.name << ": " << setprecision(0) << setw(12) << p1.population
                 << setprecision(2) << setw(6) << p1.g << endl;}
        fin.close();
    }

    // add new data
    ofstream fout(file, ios_base::out | ios_base::app | ios_base::binary); // append to file
    if (!fout.is_open())
    {
        cerr << "Could not open the file " << file << endl;
        exit(EXIT_FAILURE);
    }

    cout << "Enter planet name (empty line to quit): ";
    cin.get(p1.name, 20);
    while (p1.name[0] != '\0')
    {
        eatline();
        cout << "Enter planet population: ";
        cin >> p1.population;
        cout << "Enter planet acceleration due to gravity: ";
        cin >> p1.g;
        eatline();
        fout.write((char *) &p1, sizeof p1);
        cout << "Enter planet name (empty line to quit): ";
        cin.get(p1.name, 20);
    }
    fout.close();

    // show revised file
    fin.clear(); // clear eof flag
    fin.open(file, ios_base::in | ios_base::binary); // open for input
    if (fin.is_open())
    {
        cout << "Here are the new contents of the " << file << " file:\n";
        
        while (fin.read((char *) &p1, sizeof p1))
        {
            cout << setw(20) << p1.name << ": " << setprecision(0) << setw(12) << p1.population
                 << setprecision(2) << setw(6) << p1.g << endl;}
        fin.close();
    }
    cout << "Done.\n";
    return 0;

}
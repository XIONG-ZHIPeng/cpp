// hangman.cpp -- some string methods
#include <iostream>
#include <string>
#include <cstdlib>
#include <ctime>
#include <cctype>
using std::string;
const int NUM = 26;
const string wordlist[NUM] = {
    "apiary", "beetle", "cereal",
    "danger", "ensign", "florid", "garage", "health", "cereal",
    "jackal", "keeper", "loaner", "manage", "nounce", "onset",
    "plaid", "quilt", "remote", "stolid", "train", "useful",
    "valid", "whence", "xenon", "yearn", "zippy"
};

int main()
{
    using std::cout;
    using std::cin;
    using std::tolower;
    using std::endl;
    std::srand(std::time(0)); // seed random-number generator
    char play;
    cout << "Will you play a word game? (y/n) ";
    cin >> play;
    play = tolower(play);
    while (play == 'y')
    {
        string target = wordlist[std::rand() % NUM];
        int length = target.size();
        string attempt(length, '-');
        string badchars;
        int guesses = 6;

        cout << "Guess my secret word. It has " << length
             << " letters. You get " << guesses << " wrong guesses.\n";
        cout << "Your word: " << attempt << endl;

        while (attempt != target && guesses > 0)
        {
            char letter;
            cout << "Guess a letter: ";
            cin >> letter;
            if (badchars.find(letter) != string::npos ||
                attempt.find(letter) != string::npos)
            {
                cout << "You already guessed that. Try again.\n";
                continue;
            }
            int loc = target.find(letter);
            if (loc == string::npos)
            {
                cout << "Oh, bad guess!\n";
                --guesses;
                badchars += letter;
            }
            else
            {
                cout << "Good guess!\n";
                attempt[loc] = letter;
                loc = target.find(letter, loc + 1);
                while (loc != string::npos)
                {
                    attempt[loc] = letter;
                    loc = target.find(letter, loc + 1);
                }
            }
            cout << "Your word: " << attempt << endl;
            if (attempt != target)
            {
                if (badchars.size() > 0)
                    cout << "Bad choices: " << badchars << endl;
                cout << guesses << " bad guesses left.\n";
            }
        }
        if (guesses > 0)
            cout << "That's it! You guessed my word!\n";
        else
            cout << "Sorry, you ran out of guesses.\n"
                 << "The word was " << target << endl;
        cout << "Do you want to play again? (y/n) ";
        cin >> play;
        play = tolower(play);
    }
    cout << "Bye.\n";
    return 0;
}


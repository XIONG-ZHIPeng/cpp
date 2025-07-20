#include <iostream>
int main()
{
    char ch;

    std::cout << "Type, and I shall repeat.\n";
    std::cin.get(ch);

    while (ch != '.')
    {
        if (ch == '\n')
            std::cout << ch; // done if newline
        else
            std::cout << ch + 1; // done otherwise
        std::cin.get(ch);
    }

    std::cout << "Please excuse the slight confusion.\n";
    return 0;
}
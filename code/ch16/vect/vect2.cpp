// vect2.cpp -- methods and iterators
#include <iostream>
#include <string>
#include <vector>

struct Review
{
    std::string title;
    int rating;
};
bool FillReview(Review &rr);
void ShowReview(const Review &rr);

int main()
{
    using std::cout;
    using std::vector;
    vector<Review> books;
    Review temp;
    while (FillReview(temp))
    {
        books.push_back(temp);
    }
    int num = books.size();
    if (num > 0)
    {
        cout << "Thank you for your reviews. You entered the following:\n";
        for (int i = 0; i < num; i++)
        {
            ShowReview(books[i]);
        }
        cout << "Reprising:\n"
        << "Rating\tBook\n";
        vector<Review>::iterator pr;
        for (pr = books.begin(); pr != books.end(); pr++)
        {
            ShowReview(*pr);
        }
        vector <Review> oldlist(books);
        if (num > 3)
        {
            books.erase(books.begin() + 1, books.begin() + 3);
            cout << "After erase:\n";
            for (pr = books.begin(); pr != books.end(); pr++)
            {
                ShowReview(*pr);
            }
            // insert 1 item
            books.insert(books.begin(), oldlist.begin() + 1, oldlist.begin() + 2);
            cout << "After insert:\n";
            for (pr = books.begin(); pr != books.end(); pr++)
            {
                ShowReview(*pr);
            }
            books.swap(oldlist);
            cout << "After swap:\n";
            for (pr = books.begin(); pr != books.end(); pr++)
            {
                ShowReview(*pr);
            }
        }
    }
    else
    {
        cout << "No books? Too bad.\n";
    }
}

bool FillReview(Review &rr)
{
    std::cout << "Enter book title (empty line to quit): ";
    std::getline(std::cin, rr.title);
    if (rr.title.empty())
        return false;
    std::cout << "Enter book rating: ";
    std::cin >> rr.rating;
    while (std::cin.get() != '\n')
        continue;
    return true;
}

void ShowReview(const Review &rr)
{
    std::cout << rr.rating << "\t" << rr.title << std::endl;
}
// stack.h -- class declaration for the stack ADT
#ifndef STACK_H
#define STACK_H

typedef unsigned long Item;

class Stack
{
private:
    enum {MAX = 10};    // constant specific to class
    Item items[MAX];    // holds stack items
    int top;            // index for top stack item
public:
    Stack();
    bool isempty() const;
    bool isfull() const;
    bool push(const Item & item);  // add item to stack
    bool pop(Item & item);          // pop top into item
};

#endif
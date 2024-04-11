#ifndef SCREENSTACK_H
#define SCREENSTACK_H

#include <stack>
#include <cstddef>

struct screenIDs {
    int stackBottom;
    int welcomeID;
    int viewAptsID;
    int bookAptID;
    int changeAptID;
    int helpID;

    // Constructor
    screenIDs() : stackBottom(0), welcomeID(1), viewAptsID(2), bookAptID(3), changeAptID(4), helpID(5) {}
};

class ScreenStack {
private:
    std::stack<int> stack;

public:
    // Constructor
    ScreenStack();

    // Destructor
    ~ScreenStack();

    // Push a screen onto the stack
    void push(int screen_id);

    // Pop the top screen from the stack
    int pop();

    // Peek at the top screen without removing it
    int peek();

    // Check if the stack is empty
    bool isEmpty();

    // Get the size of the stack
    size_t size();
};

#endif // SCREENSTACK_H

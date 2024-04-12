/**
 * @file screenStack.cpp
 * @brief Implementation of the ScreenStack class used for back command functionality.
 */
#include "screenStack.h"
#include <cstddef>




// Constructor
ScreenStack::ScreenStack() {}

// Destructor
ScreenStack::~ScreenStack() {}

// Push a screen ID onto the stack
void ScreenStack::push(int screen_id) {
    stack.push(screen_id);
}

// Pop the top screen ID from the stack
int ScreenStack::pop() {
    int top_screen_id = -1; // Default value for an empty stack
    if (!isEmpty()) {
        top_screen_id = stack.top();
        stack.pop();
    }
    return top_screen_id;
}

// Peek at the top screen ID without removing it
int ScreenStack::peek() {
    int top_screen_id = -1; // Default value for an empty stack
    if (!isEmpty()) {
        top_screen_id = stack.top();
    }
    return top_screen_id;
}

// Check if the stack is empty
bool ScreenStack::isEmpty() {
    return stack.empty();
}

// Get the size of the stack
size_t ScreenStack::size() {
    return stack.size();
}

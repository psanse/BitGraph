// Demonstrates the two asynchronous task helpers provided by UTILS.
//
// Both helpers wait for completion before returning, so this example stays
// simple while still showing how a callable and a member function are used.

#include <iostream>

#include "utils/task_utils.h"

namespace
{
    // A regular function can be submitted directly to run_task_async.
    int multiply(int left, int right)
    {
        return left * right;
    }

    class Calculator
    {
    public:
        // The member-function helper invokes this function on an object.
        int add(int left, int right) const
        {
            return left + right;
        }
    };
}

int main()
{
    // The result is returned after the asynchronously launched task finishes.
    const int product = bitgraph::utils::run_task_async(multiply, 6, 7);

    Calculator calculator;
    const int sum = bitgraph::utils::run_member_task_async(
        &Calculator::add,
        calculator,
        6,
        7);

    std::cout << "free-function task result: " << product << '\n';
    std::cout << "member-function task result: " << sum << '\n';
    return 0;
}

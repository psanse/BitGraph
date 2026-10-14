/**
 * @file utils_collections_simple.cpp
 * @brief Demonstrates basic collection helpers and FixedStack.
 *
 * The collection helpers make it easy to inspect standard containers, while
 * FixedStack provides a fixed-capacity stack for trivial values without
 * reallocating during normal push/pop operations.
 */

#include <iostream>
#include <vector>

#include "utils/collection_utils.h"
#include "utils/fixed_stack.h"

int main()
{
    using bitgraph::utils::FixedStack;
    using bitgraph::utils::all_equal;
    using bitgraph::utils::print_collection;

    const std::vector<int> values{2, 4, 4, 8};
    const std::vector<int> equal_values{7, 7, 7};

    // print_collection writes the elements and appends the collection size.
    std::cout << "values: ";
    print_collection(values, std::cout, true);

    // all_equal checks whether every element compares equal.  Empty and
    // single-element collections also satisfy this condition.
    std::cout << "all values equal: " << std::boolalpha
              << all_equal(values) << '\n';
    std::cout << "all equal_values equal: "
              << all_equal(equal_values) << "\n\n";

    // FixedStack allocates its storage once, at construction time.  The
    // capacity must be large enough for every value that will be pushed.
    FixedStack<int> stack(4);
    stack.push(10);
    stack.push(20);
    stack.push(30);

    std::cout << "stack after pushes: " << stack << '\n'
              << "  size: " << stack.size() << '\n'
              << "  capacity: " << stack.capacity() << '\n'
              << "  bottom: " << stack.bottom() << '\n'
              << "  top: " << stack.top() << '\n';

    // pop removes the most recently pushed value and keeps the allocated
    // capacity available for later operations.
    stack.pop();
    std::cout << "stack after pop: " << stack << '\n';

    return 0;
}

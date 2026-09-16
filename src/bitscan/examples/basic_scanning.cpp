#include <iostream>

#include ""

int main()
{
    using bitgraph::BBObject;
    using bitgraph::bitarray;

    bitarray bits(130);

    bits.set_bit(2);
    bits.set_bit(10);
    bits.set_bit(63);
    bits.set_bit(64);
    bits.set_bit(100);
    bits.set_bit(129);

    // Forward scan: increasing bit positions.
    std::cout << "Forward scan: ";

    bitarray::scan forward(bits);

    for (int bit = forward.next_bit();
        bit != BBObject::noBit;
        bit = forward.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';

    // Reverse scan: decreasing bit positions.
    std::cout << "Reverse scan: ";

    bitarray::scanR reverse(bits);

    for (int bit = reverse.next_bit();
        bit != BBObject::noBit;
        bit = reverse.next_bit()) {
        std::cout << bit << ' ';
    }

    std::cout << '\n';

    return 0;
}
# BTreeMultiset

`BTreeMultiset` is a header-only C++ multiset for `int` values. It stores B-tree nodes in one contiguous vector of blocks, avoiding pointer-based node allocation and keeping tree data cache-friendly.

## Features

- Stores duplicate values.
- Keeps values ordered internally.
- Inserts values in logarithmic tree height.
- Counts occurrences and values relative to a target.
- Requires only the C++ standard library.

## Requirements

- A C++17-compatible compiler, such as `g++` or MSVC.

## Build And Run

From the project directory:

```powershell
g++ -std=c++17 -O2 -Wall -Wextra -pedantic main.cpp -o btree_demo.exe
.\btree_demo.exe
```

The demo provides an interactive menu for inserting values, checking the size, testing emptiness, and performing count queries.

## Library Usage

Include the header directly:

```cpp
#include "BTreeMultiset.hpp"
#include <iostream>

int main() {
    BTreeMultiset values;
    values.insert(7);
    values.insert(3);
    values.insert(7);

    std::cout << values.size() << "\n";                 // 3
    std::cout << values.count(7) << "\n";              // 2
    std::cout << values.lower_bound_count(7) << "\n";  // 1
    std::cout << values.upper_bound_count(7) << "\n";  // 0
}
```

## API

| Function | Description |
| --- | --- |
| `insert(int value)` | Inserts one value, including duplicates. |
| `size()` | Returns the total number of stored values. |
| `empty()` | Returns `true` when no values are stored. |
| `count(int value)` | Returns the number of occurrences of `value`. |
| `lower_bound_count(int value)` | Returns the number of values strictly less than `value`. |
| `upper_bound_count(int value)` | Returns the number of values strictly greater than `value`. |

`insert`, `count`, `lower_bound_count`, and `upper_bound_count` use logarithmic tree height. `size()` computes the total by traversing the stored tree.

## Files

- `BTreeMultiset.hpp` - Header-only container implementation.
- `main.cpp` - Interactive console demonstration.
- `README.md` - Project documentation.

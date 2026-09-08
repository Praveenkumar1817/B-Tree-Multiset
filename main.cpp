#include <iostream>
#include "BTreeMultiset.hpp"

using namespace std;

void display_menu() {
    cout << "\n=========================================\n";
    cout << "  BTreeMultiset Cache-Optimized Console  \n";
    cout << "=========================================\n";
    cout << "1. Insert an element\n";
    cout << "2. Get total size of multiset\n";
    cout << "3. Check if empty\n";
    cout << "4. Count occurrences of an element\n";
    cout << "5. Count elements strictly LESS than X\n";
    cout << "6. Count elements strictly GREATER than X\n";
    cout << "7. Exit\n";
    cout << "Enter your choice (1-7): ";
}

int main() {
    BTreeMultiset multiset;
    int choice = 0;
    int value = 0;

    while (true) {
        display_menu();
        if (!(cin >> choice)) {
            cout << "Invalid input. Please enter a number.\n";
            cin.clear();
            cin.ignore(10000, '\n');
            continue;
        }

        if (choice == 7) {
            cout << "Exiting application. Thank you!\n";
            break;
        }

        switch (choice) {
            case 1:
                cout << "Enter integer value to insert: ";
                cin >> value;
                multiset.insert(value);
                cout << "Successfully inserted " << value << " into the cache pool.\n";
                break;

            case 2:
                cout << "Total elements currently stored: " << multiset.size() << "\n";
                break;

            case 3:
                if (multiset.empty()) {
                    cout << "The multiset is currently EMPTY.\n";
                } else {
                    cout << "The multiset contains data.\n";
                }
                break;

            case 4:
                cout << "Enter value to count occurrences: ";
                cin >> value;
                cout << "Occurrences of " << value << ": " << multiset.count(value) << "\n";
                break;

            case 5:
                cout << "Enter target value (X): ";
                cin >> value;
                cout << "Total elements < " << value << ": " << multiset.lower_bound_count(value) << "\n";
                break;

            case 6:
                cout << "Enter target value (X): ";
                cin >> value;
                cout << "Total elements > " << value << ": " << multiset.upper_bound_count(value) << "\n";
                break;

            default:
                cout << "Invalid choice! Please select an option between 1 and 7.\n";
                break;
        }
    }

    return 0;
}

#include "gamelib.hpp"

// Funktion til at sortere filnavne med naturlig orden (f.eks. "1.png", "10.png", "2.png" bliver til "1.png", "2.png", "10.png")
// Lavet af AI
bool natural_sort_cmp(const std::string& a, const std::string& b) {
    auto is_digit = [](char c) { return std::isdigit(c); };
    size_t i = 0, j = 0;
    while (i < a.size() && j < b.size()) {
        if (is_digit(a[i]) && is_digit(b[j])) {
            long long val_a = 0, val_b = 0;
            while (i < a.size() && is_digit(a[i])) {
                val_a = val_a * 10 + (a[i] - '0');
                i++;
            }
            while (j < b.size() && is_digit(b[j])) {
                val_b = val_b * 10 + (b[j] - '0');
                j++;
            }
            if (val_a != val_b) {
                return val_a < val_b;
            }
        } else {
            if (a[i] != b[j]) {
                return a[i] < b[j];
            }
            i++;
            j++;
        }
    }
    return a.size() < b.size();
}

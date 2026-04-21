#include <bits/stdc++.h>
#include "skip_list.hpp"

// We infer an interface: operations count followed by operations.
// Common format used in skip-list problems:
// m (number of operations)
// op x, where op in {1 insert, 2 delete, 3 search}
// Output for search: 1 if found else 0

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);

    int m;
    if (!(std::cin >> m)) return 0;
    SkipList<long long> sl;
    for (int i = 0; i < m; ++i) {
        int op; long long x;
        std::cin >> op >> x;
        if (op == 1) {
            sl.insert(x);
        } else if (op == 2) {
            sl.deleteItem(x);
        } else if (op == 3) {
            std::cout << (sl.search(x) ? 1 : 0) << '\n';
        }
    }
    return 0;
}


// Skip list implementation header-only template
// Implements insert, search, deleteItem for a generic type T using only operator<.
// Deterministic random level generator for stable behavior.

#ifndef SKIP_LIST_HPP
#define SKIP_LIST_HPP

#include <vector>
#include <optional>
#include <random>

template <typename T>
class SkipList {
private:
    struct Node {
        std::optional<T> value; // disengaged for head sentinel
        std::vector<Node*> forward;

        explicit Node(int lvl) : value(std::nullopt), forward(static_cast<std::size_t>(lvl), nullptr) {}
        Node(int lvl, const T& val) : value(val), forward(static_cast<std::size_t>(lvl), nullptr) {}
    };

    static constexpr int MAX_LEVEL = 32; // supports up to ~2^32 elements comfortably
    static constexpr double P = 0.5;     // promotion probability

    Node* head;      // head sentinel (no value)
    int level;       // current highest level (1-based)

    // Deterministic RNG for portability
    std::mt19937 rng;
    std::bernoulli_distribution coin;

    static bool less_than(const T& a, const T& b) {
        return a < b;
    }
    static bool equals(const T& a, const T& b) {
        return !less_than(a, b) && !less_than(b, a);
    }

    int randomLevel() {
        int lvl = 1;
        while (lvl < MAX_LEVEL && coin(rng)) ++lvl;
        return lvl;
    }

public:
    SkipList()
        : head(new Node(MAX_LEVEL)), level(1), rng(0x9e3779b97f4a7c15ULL ^ 0x85ebca6b), coin(P) {}

    ~SkipList() {
        Node* curr = head->forward[0];
        while (curr) {
            Node* next = curr->forward[0];
            delete curr;
            curr = next;
        }
        delete head;
    }

    void insert(const T& item) {
        Node* update[MAX_LEVEL];
        Node* x = head;
        // walk top-down
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[static_cast<std::size_t>(i)] &&
                   less_than(*(x->forward[static_cast<std::size_t>(i)]->value), item)) {
                x = x->forward[static_cast<std::size_t>(i)];
            }
            update[i] = x;
        }

        x = x->forward[0];
        if (x && equals(*(x->value), item)) {
            return; // already exists
        }

        int newLevel = randomLevel();
        if (newLevel > level) {
            for (int i = level; i < newLevel; ++i) {
                update[i] = head;
            }
            level = newLevel;
        }

        Node* newNode = new Node(newLevel, item);
        for (int i = 0; i < newLevel; ++i) {
            newNode->forward[static_cast<std::size_t>(i)] = update[i]->forward[static_cast<std::size_t>(i)];
            update[i]->forward[static_cast<std::size_t>(i)] = newNode;
        }
    }

    bool search(const T& item) {
        Node* x = head;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[static_cast<std::size_t>(i)] &&
                   less_than(*(x->forward[static_cast<std::size_t>(i)]->value), item)) {
                x = x->forward[static_cast<std::size_t>(i)];
            }
        }
        x = x->forward[0];
        return x && equals(*(x->value), item);
    }

    void deleteItem(const T& item) {
        Node* update[MAX_LEVEL];
        Node* x = head;
        for (int i = level - 1; i >= 0; --i) {
            while (x->forward[static_cast<std::size_t>(i)] &&
                   less_than(*(x->forward[static_cast<std::size_t>(i)]->value), item)) {
                x = x->forward[static_cast<std::size_t>(i)];
            }
            update[i] = x;
        }
        x = x->forward[0];
        if (!x || !equals(*(x->value), item)) return; // not found

        for (int i = 0; i < level; ++i) {
            if (update[i]->forward[static_cast<std::size_t>(i)] != x) break;
            update[i]->forward[static_cast<std::size_t>(i)] = x->forward[static_cast<std::size_t>(i)];
        }
        delete x;
        while (level > 1 && head->forward[static_cast<std::size_t>(level - 1)] == nullptr) {
            --level;
        }
    }
};

#endif // SKIP_LIST_HPP


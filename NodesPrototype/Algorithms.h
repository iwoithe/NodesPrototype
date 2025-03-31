#pragma once

#include <vector>

template<typename T>
int binarySearch(std::vector<T> items, T key)
{
    int l = 0;
    int r = items.size() - 1;

    while (l <= r) {
        int m = (int)((l + r) / 2);
        if (items[m] == key) {
            return m;
        }

        if (items[m] < key) {
            l = m + 1;
        }

        if (items[m] > key) {
            r = m - 1;
        }
    }

    return -1;
}

template<typename A, typename B>
int binarySearch(std::vector<A> items, B key)
{
    int l = 0;
    int r = items.size() - 1;

    while (l <= r) {
        int m = (int)((l + r) / 2);
        if (items[m] == key) {
            return m;
        }

        if (items[m] < key) {
            l = m + 1;
        }

        if (items[m] > key) {
            r = m - 1;
        }
    }

    return -1;
}

#define BINARY_SEARCH(key, vec, sortKeyGetter, res) \
res = -1; \
bool keyFound = false; \
int l = 0; \
int r = (vec).size() - 1; \
while (!keyFound && l <= r) { \
    int m = (int)((l + r) / 2); \
    if (vec[m]->sortKeyGetter() == (key)) { \
        res = m; \
        keyFound = true; \
    } \
    if (vec[m]->sortKeyGetter() < key) { \
        l = m + 1; \
    } \
    if (vec[m]->sortKeyGetter() > key) { \
        r = m - 1; \
    } \
} \


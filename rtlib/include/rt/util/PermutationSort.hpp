#pragma once

/** @file */

#include <functional>
#include <numeric>
#include <vector>

namespace rt
{

/** Permutation result type */
using Permutation = std::vector<std::size_t>;

/**
 * @brief Generate a permutation vector that sorts an input vector using the
 * provided Compare function (Default: std::less)
 *
 * From https://stackoverflow.com/a/17074810/1917043
 */
template <typename T, typename Compare = std::less<T>>
Permutation PermutationSort(const std::vector<T>& vec, Compare c = Compare())
{
    std::vector<std::size_t> p(vec.size());
    std::iota(p.begin(), p.end(), 0);
    std::sort(p.begin(), p.end(), [&](std::size_t i, std::size_t j) {
        return c(vec[i], vec[j]);
    });
    return p;
}

/** @brief Apply a Permutation vector to vector of objects */
template <typename T>
std::vector<T> ApplyPermutation(const std::vector<T>& vec, const Permutation& p)
{
    std::vector<T> sorted_vec(vec.size());
    std::transform(p.begin(), p.end(), sorted_vec.begin(), [&](std::size_t i) {
        return vec[i];
    });
    return sorted_vec;
}
}

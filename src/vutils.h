//
// Created by mate- on 05/04/2022.
//

#ifndef CAPSTONE_UTILS_VECTOR_H
#define CAPSTONE_UTILS_VECTOR_H
// utils from https://www.programmingnotes.org/7112/c-how-to-split-batch-an-array-vector-container-into-smaller-sub-lists-of-n-size-using-c/
// ============================================================================
//    Author: Kenneth Perkins
//    Date:   Feb 24, 2021
//    Taken From: http://programmingnotes.org/
//    File:  Utils.h
//    Description: Handles general utility functions
// ============================================================================
#pragma once
#include <vector>
#include <iterator>

namespace vutils {
    /**
    * FUNCTION: partition
    * USE: Breaks a sequence into smaller sub-lists of a specified size
    *   in the given range [first, last)
    * @param first: The first position of the sequence
    * @param last: The last position of the sequence
    * @param size: The maximum size of each sub-list
    * @return: A container of the smaller sub-lists of the specified size
    */
    template<typename InputIt, typename T = typename std::iterator_traits<InputIt>::value_type>
    std::vector<std::vector<T>> partition(InputIt first, InputIt last, unsigned size) {
        std::vector<std::vector<T>> result;
        std::vector<T>* batch{};
        for (unsigned index = 0, row = 0; first != last; ++first, ++index) {
            if ((index % size) == 0) {
                result.resize(++row);
                batch = &result.back();
                batch->reserve(size);
            }
            batch->push_back(*first);
        }
        return result;
    }
}// http://programmingnotes.org/
#endif //CAPSTONE_UTILS_VECTOR_H

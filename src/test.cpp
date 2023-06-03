//
// Created by asgrim on 02.06.23.
//
#include <iostream>
#include <vector>
#include "test.h"
#include <algorithm>

void test()
{
    std::vector<int> a = {1, 3, 5, 7, 2, 4, 6, 8};
    auto mid = a.begin() + (a.end() - a.begin()) / 2;
    auto s= a.begin();
    auto e = a.end();
    std::inplace_merge(s,mid,e);
    for (auto i: a)
    {
        std::cout << i << "\n";
    }
}
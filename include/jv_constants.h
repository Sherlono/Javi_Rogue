#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

namespace {
    typedef const unsigned char cuchar_t;
    typedef const char cchar_t;
}
namespace jv::ct{
    cuchar_t w1count = 23, w2count = 5, fcount = 50;  // The count for each different block type. Crucial to keep up to date!!!
    constexpr cuchar_t w1w2count = w1count + w2count, block_type_count = w1w2count + fcount;
    const int max_blocks = 110;  // Arbitrary but hopefuly not lower than the size of block_arrays
}

#endif
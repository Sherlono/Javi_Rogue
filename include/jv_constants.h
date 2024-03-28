#ifndef JV_CONSTANTS_H
#define JV_CONSTANTS_H

namespace jv::ct{
    const unsigned char w1count = 23, w2count = 5, fcount = 50;  // The count for each different block type. Crucial to keep up to date!!!
    constexpr const unsigned char w1w2count = w1count + w2count, block_type_count = w1w2count + fcount;
    const size_t max_blocks = 128;  // Arbitrary but hopefuly not lower than the size of the block_array
}

#endif
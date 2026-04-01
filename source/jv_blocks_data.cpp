#include "jv_blocks_data.h"

#include "bn_span.h"

namespace jv{
[[nodiscard]] blocks::cell_span_t blocks::get_block(const int block_index){
    return blocks::cell_span_t(blocks::data.begin() + block_index*16, blocks::data.begin() + block_index*16 + 16);
}
}
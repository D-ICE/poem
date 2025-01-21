//
// Created by frongere on 17/01/25.
//

#ifndef POEM_SPLITTER_H
#define POEM_SPLITTER_H

#include <vector>

#include "exceptions.h"

namespace poem {

  /**
   * Class allowing to split most equally a vector in different parts.
   *
   * This is to be used for parallel calculations on a DimensionPoint vector
   */
  class Splitter {
   public:
    using OffsetSizeContainer = std::vector<std::pair<size_t, size_t>>;
    using SplitterConstIter = OffsetSizeContainer::const_iterator;

    Splitter(size_t size, size_t chunk_size);

    size_t size() const;

    size_t nchunks() const;

    size_t chunk_offset(size_t ichunk) const;

    size_t chunk_size(size_t ichunk) const;

    SplitterConstIter begin() const;

    SplitterConstIter end() const;

   private:

    size_t m_size;
    size_t m_nchunks;

    OffsetSizeContainer m_offsets_sizes;

  };

}  // poem

#endif //POEM_SPLITTER_H

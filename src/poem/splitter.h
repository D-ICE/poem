//
// Created by frongere on 21/07/23.
//

#ifndef POEM_SPLITTER_H
#define POEM_SPLITTER_H

#include <cstdlib>
#include <vector>
#include <algorithm>

namespace poem
{

  class Splitter
  {
  public:
    using OffsetSizeContainer = std::vector<std::pair<size_t, size_t>>;
    using SplitterConstIter = OffsetSizeContainer::const_iterator;

    Splitter(size_t size, size_t chunk_size) : m_size(size)
    {

      if (chunk_size == 0)
      {
        spdlog::critical("Chunk size must be stricly positive");
        CRITICAL_ERROR_POEM
      }

      if (size < 2 * chunk_size)
      {
        // Not justified to split, return only one chunk
        m_nchunks = 1;
        m_offsets_sizes.emplace_back(0, size);
        return;
      }

      m_nchunks = size / chunk_size;
      size_t remainder = size % chunk_size;

      size_t offset_i = 0;
      size_t size_i;
      size_t extra;
      for (size_t ichunk = 0; ichunk < m_nchunks; ++ichunk)
      {
        size_i = chunk_size;

        if (remainder > 0)
        {
          if (remainder > m_nchunks - ichunk)
          {
            extra = 2;
          }
          else
          {
            extra = 1;
          }
        }
        else
        {
          extra = 0;
        }
        remainder -= extra;

        size_i = chunk_size + extra;

        m_offsets_sizes.emplace_back(offset_i, size_i);

        offset_i += size_i;
      }
    }

    size_t size() const { return m_size; }

    size_t nchunks() const { return m_nchunks; }

    size_t chunk_offset(size_t ichunk) const
    {
      return m_offsets_sizes.at(ichunk).first;
    }

    size_t chunk_size(size_t ichunk) const
    {
      return m_offsets_sizes.at(ichunk).second;
    }

    SplitterConstIter begin() const
    {
      return m_offsets_sizes.cbegin();
    }

    SplitterConstIter end() const
    {
      return m_offsets_sizes.cend();
    }

  private:
    size_t m_size;
    size_t m_nchunks;

    OffsetSizeContainer m_offsets_sizes;
  };

} // poem

#endif // POEM_SPLITTER_H

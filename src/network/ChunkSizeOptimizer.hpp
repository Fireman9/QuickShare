#ifndef CHUNK_SIZE_OPTIMIZER_HPP
#define CHUNK_SIZE_OPTIMIZER_HPP

#include <algorithm>
#include <chrono>
#include <numeric>
#include <unordered_map>
#include <vector>

class ChunkSizeOptimizer
{
  public:
    ChunkSizeOptimizer(std::vector<size_t> possible_sizes);

    void recordPerformance(size_t                    chunk_size,
                           std::chrono::microseconds latency);

    size_t getOptimalChunkSize();

  private:
    struct PerformanceData
    {
        std::chrono::microseconds total_latency{0};
        std::chrono::microseconds average_latency{0};
        size_t                    count{0};
    };

    void optimizeChunkSize();

    std::vector<size_t>                         possible_sizes_;
    size_t                                      current_size_index_;
    std::unordered_map<size_t, PerformanceData> performance_data_;
};

#endif // CHUNK_SIZE_OPTIMIZER_HPP

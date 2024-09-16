#include "ChunkSizeOptimizer.hpp"

ChunkSizeOptimizer::ChunkSizeOptimizer(std::vector<size_t> possible_sizes) :
    possible_sizes_(std::move(possible_sizes))
{
    std::sort(possible_sizes_.begin(), possible_sizes_.end());
    current_size_index_ = possible_sizes_.size() / 2;
    for (size_t size : possible_sizes_)
    {
        performance_data_[size] = PerformanceData{};
    }
}

void ChunkSizeOptimizer::recordPerformance(size_t                    chunk_size,
                                           std::chrono::microseconds latency)
{
    auto& data = performance_data_[chunk_size];
    data.total_latency += latency;
    data.count++;
    data.average_latency = data.total_latency / data.count;
}

size_t ChunkSizeOptimizer::getOptimalChunkSize()
{
    optimizeChunkSize();
    return possible_sizes_[current_size_index_];
}

void ChunkSizeOptimizer::optimizeChunkSize()
{
    auto it = std::min_element(
        performance_data_.begin(), performance_data_.end(),
        [](const auto& a, const auto& b) {
            return a.second.average_latency < b.second.average_latency;
        });

    size_t optimal_size = it->first;

    auto size_it =
        std::find(possible_sizes_.begin(), possible_sizes_.end(), optimal_size);
    if (size_it != possible_sizes_.end())
    {
        current_size_index_ = std::distance(possible_sizes_.begin(), size_it);
    }
}

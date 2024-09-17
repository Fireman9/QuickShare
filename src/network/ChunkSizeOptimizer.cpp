#include "ChunkSizeOptimizer.hpp"

ChunkSizeOptimizer::ChunkSizeOptimizer(std::vector<size_t> possible_sizes) :
    possible_sizes_(std::move(possible_sizes)), rng_(std::random_device{}())
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

    double seconds =
        std::chrono::duration<double>(data.average_latency).count();
    data.effective_throughput =
        static_cast<double>(chunk_size) / seconds / (1024 * 1024);
}

size_t ChunkSizeOptimizer::getOptimalChunkSize()
{
    optimizeChunkSize();
    return possible_sizes_[current_size_index_];
}

void ChunkSizeOptimizer::optimizeChunkSize()
{
    // epsilon-greedy strategy
    if (shouldExplore())
    {
        std::uniform_int_distribution<size_t> dist(0,
                                                   possible_sizes_.size() - 1);
        current_size_index_ = dist(rng_);
    } else {
        auto it = std::max_element(performance_data_.begin(),
                                   performance_data_.end(), comparePerformance);

        size_t optimal_size = it->first;
        auto size_it = std::find(possible_sizes_.begin(), possible_sizes_.end(),
                                 optimal_size);
        if (size_it != possible_sizes_.end())
        {
            current_size_index_ =
                std::distance(possible_sizes_.begin(), size_it);
        }
    }
}

bool ChunkSizeOptimizer::comparePerformance(
    const std::pair<const size_t, PerformanceData>& a,
    const std::pair<const size_t, PerformanceData>& b)
{
    double throughput_a =
        a.second.count < 5 ? 0.0 : a.second.effective_throughput;
    double throughput_b =
        b.second.count < 5 ? 0.0 : b.second.effective_throughput;

    return throughput_a < throughput_b;
}

bool ChunkSizeOptimizer::shouldExplore()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng_) < 0.1;
}

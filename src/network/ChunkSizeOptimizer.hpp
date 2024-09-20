#ifndef CHUNK_SIZE_OPTIMIZER_HPP
#define CHUNK_SIZE_OPTIMIZER_HPP

#include <algorithm>
#include <chrono>
#include <deque>
#include <limits>
#include <numeric>
#include <random>
#include <unordered_map>
#include <vector>

class ChunkSizeOptimizer
{
  public:
    ChunkSizeOptimizer(std::vector<size_t> possible_sizes,
                       double exploration_rate = 0.1, size_t window_size = 10);

    void   recordPerformance(size_t                    chunk_size,
                             std::chrono::microseconds latency);
    size_t getOptimalChunkSize();

  private:
    struct PerformanceData
    {
        std::deque<std::chrono::microseconds> latencies;
        double                                average_latency = 0;
        double                                effective_throughput = 0;
        double                                variance = 0;
    };

    void optimizeChunkSize();
    bool
           comparePerformance(const std::pair<const size_t, PerformanceData>& a,
                              const std::pair<const size_t, PerformanceData>& b) const;
    double calculateScore(const PerformanceData& data) const;
    void   updateMetrics(PerformanceData& data);
    bool   shouldExplore();

    std::vector<size_t>                         possible_sizes_;
    size_t                                      current_size_index_;
    std::unordered_map<size_t, PerformanceData> performance_data_;
    std::mt19937                                rng_;
    double                                      exploration_rate_;
    size_t                                      window_size_;
    double                                      max_observed_throughput_;
    double                                      min_observed_latency_;
};

#endif // CHUNK_SIZE_OPTIMIZER_HPP

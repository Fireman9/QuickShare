#include "ChunkSizeOptimizer.hpp"

ChunkSizeOptimizer::ChunkSizeOptimizer(std::vector<size_t> possible_sizes,
                                       double              exploration_rate,
                                       size_t              window_size) :
    possible_sizes_(std::move(possible_sizes)),
    exploration_rate_(exploration_rate), window_size_(window_size),
    rng_(std::random_device{}()), max_observed_throughput_(0),
    min_observed_latency_(std::numeric_limits<double>::max())
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
    data.latencies.push_back(latency);
    if (data.latencies.size() > window_size_)
    {
        data.latencies.pop_front();
    }
    updateMetrics(data);
}

size_t ChunkSizeOptimizer::getOptimalChunkSize()
{
    optimizeChunkSize();
    return possible_sizes_[current_size_index_];
}

void ChunkSizeOptimizer::optimizeChunkSize()
{
    if (shouldExplore())
    {
        std::uniform_int_distribution<size_t> dist(0,
                                                   possible_sizes_.size() - 1);
        current_size_index_ = dist(rng_);
    } else {
        auto it =
            std::max_element(performance_data_.begin(), performance_data_.end(),
                             [this](const auto& a, const auto& b) {
                                 return comparePerformance(a, b);
                             });

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
    const std::pair<const size_t, PerformanceData>& b) const
{
    const auto& data_a = a.second;
    const auto& data_b = b.second;

    if (data_a.latencies.size() < window_size_ ||
        data_b.latencies.size() < window_size_)
    {
        return data_a.latencies.size() < data_b.latencies.size();
    }

    double score_a = calculateScore(data_a);
    double score_b = calculateScore(data_b);
    return score_a < score_b;
}

double ChunkSizeOptimizer::calculateScore(const PerformanceData& data) const
{
    double throughput_weight = 0.6;
    double latency_weight = 0.3;
    double stability_weight = 0.1;

    double normalized_throughput =
        data.effective_throughput / max_observed_throughput_;
    double normalized_latency = min_observed_latency_ / data.average_latency;
    double stability = 1.0 / (1.0 + data.variance);

    return throughput_weight * normalized_throughput +
           latency_weight * normalized_latency + stability_weight * stability;
}

void ChunkSizeOptimizer::updateMetrics(PerformanceData& data)
{
    if (data.latencies.empty()) return;

    data.average_latency =
        std::accumulate(data.latencies.begin(), data.latencies.end(),
                        std::chrono::microseconds(0))
            .count() /
        static_cast<double>(data.latencies.size());

    double sum_squared_diff = 0;
    for (const auto& latency : data.latencies)
    {
        double diff = latency.count() - data.average_latency;
        sum_squared_diff += diff * diff;
    }
    data.variance = sum_squared_diff / data.latencies.size();

    double seconds = data.average_latency / 1e6;
    data.effective_throughput =
        static_cast<double>(data.latencies.front().count()) / seconds /
        (1024 * 1024);

    max_observed_throughput_ =
        std::max(max_observed_throughput_, data.effective_throughput);
    min_observed_latency_ =
        std::min(min_observed_latency_, data.average_latency);
}

bool ChunkSizeOptimizer::shouldExplore()
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng_) < exploration_rate_;
}

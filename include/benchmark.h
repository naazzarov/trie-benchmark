#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <string>
#include <vector>
#include <chrono>
#include <functional>

// Results from a benchmark run
struct BenchmarkResult {
    std::string trieType;
    size_t datasetSize;
    
    double insertionTime;     // microseconds
    double searchTime;        // microseconds
    double searchMissTime;    // microseconds for failed searches
    
    size_t memoryUsage;       // bytes
    size_t nodeCount;
    
    // calculated metrics
    double avgInsertTime;
    double avgSearchTime;
    double memoryPerWord;
    
    void calculateAverages();
    void print() const;
};

// Benchmark runner - loads data and runs tests on all tries
class Benchmark {
private:
    std::vector<std::string> dataset;
    std::vector<std::string> searchKeys;  // real words to search for
    std::vector<std::string> missKeys;    // words not in the dataset
    
public:
    Benchmark() = default;
    
    void loadDictionary(const std::string& filename);
    void generateRandomStrings(size_t count, size_t minLen, size_t maxLen);
    void loadFromFile(const std::string& filename);
    
    template<typename TrieType>
    BenchmarkResult run(const std::string& trieTypeName);
    
    size_t getDatasetSize() const { return dataset.size(); }
    void clearDataset() { dataset.clear(); searchKeys.clear(); missKeys.clear(); }
    
    static size_t getCurrentMemoryUsage();
    
private:
    void prepareSearchKeys(size_t sampleSize);
    void prepareMissKeys(size_t sampleSize);
    
    template<typename TrieType>
    double measureInsertionTime(TrieType& trie);
    
    template<typename TrieType>
    double measureSearchTime(const TrieType& trie, const std::vector<std::string>& keys);
};

// Simple timer for measuring operations
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start;
    
public:
    Timer() : start(std::chrono::high_resolution_clock::now()) {}
    
    void reset() {
        start = std::chrono::high_resolution_clock::now();
    }
    
    // returns time in microseconds
    double elapsed() const {
        auto end = std::chrono::high_resolution_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
    }
};

#endif

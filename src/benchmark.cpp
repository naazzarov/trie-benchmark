#include "benchmark.h"
#include "standard_trie.h"
#include "compressed_trie.h"
#include "double_array_trie.h"
#include <fstream>
#include <iostream>
#include <random>
#include <algorithm>
#include <iomanip>

#ifdef __APPLE__
#include <mach/mach.h>
#elif __linux__
#include <fstream>
#include <sstream>
#endif

void BenchmarkResult::calculateAverages() {
    avgInsertTime = datasetSize > 0 ? insertionTime / datasetSize : 0.0;
    avgSearchTime = datasetSize > 0 ? searchTime / datasetSize : 0.0;
    memoryPerWord = datasetSize > 0 ? static_cast<double>(memoryUsage) / datasetSize : 0.0;
}

void BenchmarkResult::print() const {
    // not used anymore - results go to CSV
}

void Benchmark::loadDictionary(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open dictionary file: " << filename << std::endl;
        return;
    }
    
    std::string word;
    while (std::getline(file, word)) {
        // Remove whitespace and convert to lowercase
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (!word.empty()) {
            std::transform(word.begin(), word.end(), word.begin(), ::tolower);
            dataset.push_back(word);
        }
    }
    
    std::cout << "Loaded " << dataset.size() << " words from " << filename << std::endl;
}

void Benchmark::generateRandomStrings(size_t count, size_t minLen, size_t maxLen) {
    static const char charset[] = "abcdefghijklmnopqrstuvwxyz";
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> lenDist(minLen, maxLen);
    std::uniform_int_distribution<> charDist(0, sizeof(charset) - 2);
    
    dataset.clear();
    dataset.reserve(count);
    
    for (size_t i = 0; i < count; i++) {
        size_t len = lenDist(gen);
        std::string str;
        str.reserve(len);
        
        for (size_t j = 0; j < len; j++) {
            str += charset[charDist(gen)];
        }
        
        dataset.push_back(str);
    }
    
    std::cout << "Generated " << count << " random strings" << std::endl;
}

void Benchmark::loadFromFile(const std::string& filename) {
    loadDictionary(filename);  // Same logic for now
}

template<typename TrieType>
BenchmarkResult Benchmark::run(const std::string& trieTypeName) {
    BenchmarkResult result;
    result.trieType = trieTypeName;
    result.datasetSize = dataset.size();
    
    // Prepare search keys
    prepareSearchKeys(std::min(dataset.size(), size_t(1000)));
    prepareMissKeys(std::min(dataset.size() / 10, size_t(1000)));
    
    // Create trie instance
    TrieType trie;
    
    // Measure insertion time
    result.insertionTime = measureInsertionTime(trie);
    
    // Measure search time (hits)
    result.searchTime = measureSearchTime(trie, searchKeys);
    
    // Measure search time (misses)
    result.searchMissTime = measureSearchTime(trie, missKeys);
    
    // Get memory usage
    result.memoryUsage = trie.getMemoryUsage();
    result.nodeCount = trie.getNodeCount();
    
    // Calculate derived metrics
    result.calculateAverages();
    
    return result;
}

void Benchmark::prepareSearchKeys(size_t sampleSize) {
    searchKeys.clear();
    
    if (dataset.empty()) return;
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, dataset.size() - 1);
    
    for (size_t i = 0; i < sampleSize; i++) {
        searchKeys.push_back(dataset[dist(gen)]);
    }
}

void Benchmark::prepareMissKeys(size_t sampleSize) {
    missKeys.clear();
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> lenDist(5, 15);
    std::uniform_int_distribution<> charDist('a', 'z');
    
    for (size_t i = 0; i < sampleSize; i++) {
        std::string key;
        size_t len = lenDist(gen);
        for (size_t j = 0; j < len; j++) {
            key += static_cast<char>(charDist(gen));
        }
        // Add suffix to ensure it's not in dataset
        key += "_MISS_" + std::to_string(i);
        missKeys.push_back(key);
    }
}

template<typename TrieType>
double Benchmark::measureInsertionTime(TrieType& trie) {
    Timer timer;
    
    for (const auto& word : dataset) {
        trie.insert(word);
    }
    
    return timer.elapsed();
}

template<typename TrieType>
double Benchmark::measureSearchTime(const TrieType& trie, const std::vector<std::string>& keys) {
    Timer timer;
    
    for (const auto& key : keys) {
        trie.search(key);
    }
    
    return timer.elapsed();
}

size_t Benchmark::getCurrentMemoryUsage() {
#ifdef __APPLE__
    struct task_basic_info info;
    mach_msg_type_number_t size = TASK_BASIC_INFO_COUNT;
    kern_return_t kerr = task_info(mach_task_self(), TASK_BASIC_INFO, 
                                    (task_info_t)&info, &size);
    if (kerr == KERN_SUCCESS) {
        return info.resident_size;
    }
#elif __linux__
    std::ifstream stat("/proc/self/statm");
    if (stat.is_open()) {
        size_t size, resident;
        stat >> size >> resident;
        return resident * sysconf(_SC_PAGESIZE);
    }
#endif
    return 0;
}

// Explicit template instantiations
template BenchmarkResult Benchmark::run<StandardTrie>(const std::string&);
template BenchmarkResult Benchmark::run<CompressedTrie>(const std::string&);
template BenchmarkResult Benchmark::run<DoubleArrayTrie>(const std::string&);

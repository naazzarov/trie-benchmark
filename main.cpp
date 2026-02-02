#include <iostream>
#include <vector>
#include <iomanip>
#include <fstream>
#include "standard_trie.h"
#include "compressed_trie.h"
#include "double_array_trie.h"
#include "benchmark.h"

// Helper to write results to CSV for making graphs later
void saveResultsToCSV(const std::vector<BenchmarkResult>& results, const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Could not open " << filename << " for writing\n";
        return;
    }
    
    // Write header
    file << "TrieType,DatasetSize,MemoryKB,InsertTimeMS,SearchTimeMS,BytesPerWord,AvgInsertUS,AvgSearchUS\n";
    
    // Write data
    for (const auto& result : results) {
        file << result.trieType << ","
             << result.datasetSize << ","
             << std::fixed << std::setprecision(2)
             << result.memoryUsage / 1024.0 << ","
             << result.insertionTime / 1000.0 << ","
             << result.searchTime / 1000.0 << ","
             << result.memoryPerWord << ","
             << result.avgInsertTime << ","
             << result.avgSearchTime << "\n";
    }
    
    file.close();
    std::cout << "Results saved to " << filename << "\n";
}

void runComparison(Benchmark& bench, const std::string& datasetName, std::vector<BenchmarkResult>& allResults) {
    std::cout << "\nTesting with: " << datasetName << "\n";
    std::cout << "Dataset size: " << bench.getDatasetSize() << " words\n";
    std::cout << "--\n";
    
    // Run benchmarks for all three variants
    auto stdResult = bench.run<StandardTrie>("Standard Trie");
    allResults.push_back(stdResult);
    
    auto compResult = bench.run<CompressedTrie>("Compressed Trie");
    allResults.push_back(compResult);
    
    auto daResult = bench.run<DoubleArrayTrie>("Double-Array Trie");
    allResults.push_back(daResult);
    
    // Print comparison
    std::cout << "\nResults:\n";
    std::cout << std::left << std::setw(20) << "Implementation"
              << std::setw(15) << "Memory (KB)"
              << std::setw(15) << "Insert (ms)"
              << std::setw(15) << "Search (ms)"
              << std::setw(15) << "Bytes/Word\n";
    std::cout << "--\n";
    
    std::cout << std::fixed << std::setprecision(2);
    
    std::cout << std::setw(20) << "Standard Trie"
              << std::setw(15) << stdResult.memoryUsage / 1024.0
              << std::setw(15) << stdResult.insertionTime / 1000.0
              << std::setw(15) << stdResult.searchTime / 1000.0
              << std::setw(15) << stdResult.memoryPerWord << "\n";
    
    std::cout << std::setw(20) << "Compressed Trie"
              << std::setw(15) << compResult.memoryUsage / 1024.0
              << std::setw(15) << compResult.insertionTime / 1000.0
              << std::setw(15) << compResult.searchTime / 1000.0
              << std::setw(15) << compResult.memoryPerWord << "\n";
    
    std::cout << std::setw(20) << "Double-Array Trie"
              << std::setw(15) << daResult.memoryUsage / 1024.0
              << std::setw(15) << daResult.insertionTime / 1000.0
              << std::setw(15) << daResult.searchTime / 1000.0
              << std::setw(15) << daResult.memoryPerWord << "\n";
}

void quickTest() {
    std::cout << "Quick test with a few words:\n";
    std::cout << "--\n";
    
    std::vector<std::string> testWords = {"apple", "application", "apply", "banana", "band"};
    
    StandardTrie trie;
    for (const auto& word : testWords) {
        trie.insert(word);
    }
    
    std::cout << "Inserted 5 words\n";
    std::cout << "Search 'apple': " << (trie.search("apple") ? "found" : "not found") << "\n";
    std::cout << "Search 'app': " << (trie.search("app") ? "found" : "not found") << "\n";
    std::cout << "Prefix 'app': " << (trie.startsWith("app") ? "yes" : "no") << "\n";
    std::cout << "Total memory: " << trie.getMemoryUsage() << " bytes\n";
}

int main(int argc, char* argv[]) {
    (void)argc; (void)argv; // unused
    
    std::cout << "Trie Benchmark Program\n";
    std::cout << "======================\n\n";
    
    quickTest();
    std::cout << "\n";
    
    std::vector<BenchmarkResult> allResults;
    
    std::cout << "Running benchmarks...\n\n";
    
    // Small test
    Benchmark bench1;
    bench1.generateRandomStrings(1000, 5, 15);
    runComparison(bench1, "1K Random Words", allResults);
    
    // Medium test
    Benchmark bench2;
    bench2.generateRandomStrings(10000, 5, 15);
    runComparison(bench2, "10K Random Words", allResults);
    
    // Large test
    Benchmark bench3;
    bench3.generateRandomStrings(50000, 5, 15);
    runComparison(bench3, "50K Random Words", allResults);
    
    // Check if dictionary exists
    std::ifstream dictFile("dictionary.txt");
    if (dictFile.good()) {
        dictFile.close();
        std::cout << "\nFound dictionary.txt, testing with real words...\n";
        Benchmark dictBench;
        dictBench.loadDictionary("dictionary.txt");
        if (dictBench.getDatasetSize() > 0) {
            runComparison(dictBench, "Real English Dictionary", allResults);
        }
    }
    
    // Save results
    saveResultsToCSV(allResults, "benchmark_results.csv");
    
    std::cout << "\nDone. Check benchmark_results.csv for data.\n";
    
    return 0;
}

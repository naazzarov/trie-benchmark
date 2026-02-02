#ifndef DOUBLE_ARRAY_TRIE_H
#define DOUBLE_ARRAY_TRIE_H

#include <string>
#include <vector>
#include <unordered_set>

// Double-Array Trie - very memory efficient but complex to implement
// Uses two arrays: base[] and check[] for state transitions
// More complex but provides best memory usage
class DoubleArrayTrie {
private:
    static constexpr int INITIAL_SIZE = 10000;
    static constexpr int EMPTY = -1;
    
    std::vector<int> base;   // base array
    std::vector<int> check;  // check array
    std::vector<bool> used;  // track which positions are used
    size_t wordCount;
    size_t maxState;
    
public:
    DoubleArrayTrie();
    ~DoubleArrayTrie() = default;
    
    void insert(const std::string& word);
    bool search(const std::string& word) const;
    bool startsWith(const std::string& prefix) const;
    
    size_t getMemoryUsage() const;
    size_t getArraySize() const { return base.size(); }
    size_t getWordCount() const { return wordCount; }
    size_t getNodeCount() const { return maxState + 1; }
    double getSpaceEfficiency() const;
    
    void clear();
    void compact();
    
private:
    int findBase(int state, const std::unordered_set<char>& chars);
    void resize(size_t newSize);
    int getTransition(int state, char c) const;
    void setTransition(int state, char c, int nextState);
    bool isValidTransition(int state, char c, int nextState) const;
};

#endif

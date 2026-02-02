#ifndef STANDARD_TRIE_H
#define STANDARD_TRIE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

// Standard Trie implementation - basic version with map of children
// Each node stores a map to its children. Simple to implement but uses more memory.
class StandardTrie {
private:
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        bool isEndOfWord;
        
        TrieNode() : isEndOfWord(false) {}
    };
    
    std::unique_ptr<TrieNode> root;
    size_t wordCount;
    size_t nodeCount;
    
public:
    StandardTrie();
    ~StandardTrie() = default;
    
    void insert(const std::string& word);
    bool search(const std::string& word) const;
    bool startsWith(const std::string& prefix) const;
    bool remove(const std::string& word);
    
    size_t getMemoryUsage() const;
    size_t getNodeCount() const { return nodeCount; }
    size_t getWordCount() const { return wordCount; }
    
    void clear();
    std::vector<std::string> getAllWords() const;
    
private:
    void getAllWordsHelper(const TrieNode* node, std::string currentWord, 
                          std::vector<std::string>& words) const;
    size_t calculateMemoryUsage(const TrieNode* node) const;
};

#endif

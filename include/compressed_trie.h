#ifndef COMPRESSED_TRIE_H
#define COMPRESSED_TRIE_H

#include <string>
#include <unordered_map>
#include <memory>
#include <vector>

// Compressed Trie (Radix Tree) - merges single-child paths into edges
// Better memory usage than standard trie. Also called Patricia tree.
class CompressedTrie {
private:
    struct TrieNode {
        std::unordered_map<char, std::unique_ptr<TrieNode>> children;
        std::string edgeLabel;  // The path to this node is stored as a string
        bool isEndOfWord;
        
        TrieNode() : isEndOfWord(false) {}
    };
    
    std::unique_ptr<TrieNode> root;
    size_t wordCount;
    size_t nodeCount;
    
public:
    CompressedTrie();
    ~CompressedTrie() = default;
    
    void insert(const std::string& word);
    bool search(const std::string& word) const;
    bool startsWith(const std::string& prefix) const;
    bool remove(const std::string& word);
    
    size_t getMemoryUsage() const;
    size_t getNodeCount() const { return nodeCount; }
    size_t getWordCount() const { return wordCount; }
    double getCompressionRatio() const;
    
    void clear();
    std::vector<std::string> getAllWords() const;
    
private:
    void getAllWordsHelper(const TrieNode* node, std::string currentWord, 
                          std::vector<std::string>& words) const;
    size_t calculateMemoryUsage(const TrieNode* node) const;
    int matchingPrefixLength(const std::string& str1, const std::string& str2) const;
    void splitNode(TrieNode* node, int splitPos);
};

#endif

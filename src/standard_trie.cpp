#include "standard_trie.h"

StandardTrie::StandardTrie() : wordCount(0), nodeCount(1) {
    root = std::make_unique<TrieNode>();
}

void StandardTrie::insert(const std::string& word) {
    TrieNode* current = root.get();
    
    for (char c : word) {
        if (current->children.find(c) == current->children.end()) {
            current->children[c] = std::make_unique<TrieNode>();
            nodeCount++;
        }
        current = current->children[c].get();
    }
    
    if (!current->isEndOfWord) {
        current->isEndOfWord = true;
        wordCount++;
    }
}

bool StandardTrie::search(const std::string& word) const {
    const TrieNode* current = root.get();
    
    for (char c : word) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return false;
        }
        current = it->second.get();
    }
    
    return current->isEndOfWord;
}

bool StandardTrie::startsWith(const std::string& prefix) const {
    const TrieNode* current = root.get();
    
    for (char c : prefix) {
        auto it = current->children.find(c);
        if (it == current->children.end()) {
            return false;
        }
        current = it->second.get();
    }
    
    return true;
}

bool StandardTrie::remove(const std::string& word) {
    if (!search(word)) {
        return false;
    }
    
    // Simple approach: just unmark the end of word
    // Full deletion with node removal would be more complex
    TrieNode* current = root.get();
    for (char c : word) {
        current = current->children[c].get();
    }
    
    current->isEndOfWord = false;
    wordCount--;
    return true;
}

size_t StandardTrie::getMemoryUsage() const {
    return calculateMemoryUsage(root.get());
}

size_t StandardTrie::calculateMemoryUsage(const TrieNode* node) const {
    if (!node) return 0;
    
    size_t total = sizeof(TrieNode);
    total += node->children.size() * (sizeof(char) + sizeof(std::unique_ptr<TrieNode>));
    
    for (const auto& [c, child] : node->children) {
        total += calculateMemoryUsage(child.get());
    }
    
    return total;
}

void StandardTrie::clear() {
    root = std::make_unique<TrieNode>();
    wordCount = 0;
    nodeCount = 1;
}

std::vector<std::string> StandardTrie::getAllWords() const {
    std::vector<std::string> words;
    getAllWordsHelper(root.get(), "", words);
    return words;
}

void StandardTrie::getAllWordsHelper(const TrieNode* node, std::string currentWord, 
                                     std::vector<std::string>& words) const {
    if (!node) return;
    
    if (node->isEndOfWord) {
        words.push_back(currentWord);
    }
    
    for (const auto& [c, child] : node->children) {
        getAllWordsHelper(child.get(), currentWord + c, words);
    }
}

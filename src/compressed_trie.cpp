#include "compressed_trie.h"
#include <algorithm>

CompressedTrie::CompressedTrie() : wordCount(0), nodeCount(1) {
    root = std::make_unique<TrieNode>();
}

void CompressedTrie::insert(const std::string& word) {
    if (word.empty()) return;
    
    TrieNode* current = root.get();
    std::string remaining = word;
    
    while (!remaining.empty()) {
        char firstChar = remaining[0];
        
        // Check if there's a child with this starting character
        auto it = current->children.find(firstChar);
        
        if (it == current->children.end()) {
            // No matching child - create new node with remaining string as edge label
            auto newNode = std::make_unique<TrieNode>();
            newNode->edgeLabel = remaining;
            newNode->isEndOfWord = true;
            current->children[firstChar] = std::move(newNode);
            nodeCount++;
            wordCount++;
            return;
        }
        
        // Found a matching child
        TrieNode* child = it->second.get();
        int matchLen = matchingPrefixLength(remaining, child->edgeLabel);
        
        if (matchLen == child->edgeLabel.length()) {
            // Full match of edge label
            if (matchLen == remaining.length()) {
                // Exact match - mark as end of word
                if (!child->isEndOfWord) {
                    child->isEndOfWord = true;
                    wordCount++;
                }
                return;
            } else {
                // Continue with remaining part
                current = child;
                remaining = remaining.substr(matchLen);
            }
        } else {
            // Partial match - need to split the edge
            splitNode(child, matchLen);
            
            if (matchLen == remaining.length()) {
                // The split point is our word ending
                if (!child->isEndOfWord) {
                    child->isEndOfWord = true;
                    wordCount++;
                }
                return;
            } else {
                // Continue from split node
                current = child;
                remaining = remaining.substr(matchLen);
            }
        }
    }
}

bool CompressedTrie::search(const std::string& word) const {
    const TrieNode* current = root.get();
    std::string remaining = word;
    
    while (!remaining.empty()) {
        char firstChar = remaining[0];
        auto it = current->children.find(firstChar);
        
        if (it == current->children.end()) {
            return false;
        }
        
        const TrieNode* child = it->second.get();
        const std::string& edgeLabel = child->edgeLabel;
        
        if (remaining.length() < edgeLabel.length()) {
            return false;
        }
        
        if (remaining.substr(0, edgeLabel.length()) != edgeLabel) {
            return false;
        }
        
        remaining = remaining.substr(edgeLabel.length());
        current = child;
    }
    
    return current->isEndOfWord;
}

bool CompressedTrie::startsWith(const std::string& prefix) const {
    const TrieNode* current = root.get();
    std::string remaining = prefix;
    
    while (!remaining.empty()) {
        char firstChar = remaining[0];
        auto it = current->children.find(firstChar);
        
        if (it == current->children.end()) {
            return false;
        }
        
        const TrieNode* child = it->second.get();
        const std::string& edgeLabel = child->edgeLabel;
        
        int matchLen = matchingPrefixLength(remaining, edgeLabel);
        
        if (matchLen < std::min(remaining.length(), edgeLabel.length())) {
            return false;
        }
        
        if (remaining.length() <= edgeLabel.length()) {
            return true;
        }
        
        remaining = remaining.substr(edgeLabel.length());
        current = child;
    }
    
    return true;
}

bool CompressedTrie::remove(const std::string& word) {
    if (!search(word)) {
        return false;
    }
    
    // Simplified removal - just unmark end of word
    // Full removal with node merging would be more complex
    const TrieNode* current = root.get();
    std::string remaining = word;
    
    while (!remaining.empty()) {
        char firstChar = remaining[0];
        auto it = current->children.find(firstChar);
        current = it->second.get();
        remaining = remaining.substr(current->edgeLabel.length());
    }
    
    // Cast away const for modification (in real implementation, we'd track the path)
    const_cast<TrieNode*>(current)->isEndOfWord = false;
    wordCount--;
    return true;
}

size_t CompressedTrie::getMemoryUsage() const {
    return calculateMemoryUsage(root.get());
}

size_t CompressedTrie::calculateMemoryUsage(const TrieNode* node) const {
    if (!node) return 0;
    
    size_t total = sizeof(TrieNode);
    total += node->edgeLabel.capacity();
    total += node->children.size() * (sizeof(char) + sizeof(std::unique_ptr<TrieNode>));
    
    for (const auto& [c, child] : node->children) {
        total += calculateMemoryUsage(child.get());
    }
    
    return total;
}

double CompressedTrie::getCompressionRatio() const {
    // Estimate: compared to standard trie with same words
    // This would need actual comparison in benchmarks
    return static_cast<double>(getMemoryUsage()) / (wordCount * 50.0); // rough estimate
}

void CompressedTrie::clear() {
    root = std::make_unique<TrieNode>();
    wordCount = 0;
    nodeCount = 1;
}

std::vector<std::string> CompressedTrie::getAllWords() const {
    std::vector<std::string> words;
    getAllWordsHelper(root.get(), "", words);
    return words;
}

void CompressedTrie::getAllWordsHelper(const TrieNode* node, std::string currentWord, 
                                       std::vector<std::string>& words) const {
    if (!node) return;
    
    currentWord += node->edgeLabel;
    
    if (node->isEndOfWord) {
        words.push_back(currentWord);
    }
    
    for (const auto& [c, child] : node->children) {
        getAllWordsHelper(child.get(), currentWord, words);
    }
}

int CompressedTrie::matchingPrefixLength(const std::string& str1, const std::string& str2) const {
    int len = 0;
    int maxLen = std::min(str1.length(), str2.length());
    
    while (len < maxLen && str1[len] == str2[len]) {
        len++;
    }
    
    return len;
}

void CompressedTrie::splitNode(TrieNode* node, int splitPos) {
    // Create new child node with the suffix
    auto newChild = std::make_unique<TrieNode>();
    newChild->edgeLabel = node->edgeLabel.substr(splitPos);
    newChild->isEndOfWord = node->isEndOfWord;
    newChild->children = std::move(node->children);
    
    // Update current node
    char nextChar = newChild->edgeLabel[0];
    node->edgeLabel = node->edgeLabel.substr(0, splitPos);
    node->isEndOfWord = false;
    node->children.clear();
    node->children[nextChar] = std::move(newChild);
    
    nodeCount++;
}

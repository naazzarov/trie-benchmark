#include "double_array_trie.h"
#include <algorithm>
#include <climits>

DoubleArrayTrie::DoubleArrayTrie() : wordCount(0), maxState(0) {
    base.resize(INITIAL_SIZE, EMPTY);
    check.resize(INITIAL_SIZE, EMPTY);
    used.resize(INITIAL_SIZE, false);
    
    // Initialize root
    base[0] = 1;
    check[0] = EMPTY;
    used[0] = true;
}

void DoubleArrayTrie::insert(const std::string& word) {
    if (word.empty()) return;
    
    int state = 0;  // Start from root
    
    for (size_t i = 0; i < word.length(); i++) {
        char c = word[i];
        int code = static_cast<unsigned char>(c);
        
        int nextState = getTransition(state, c);
        
        if (nextState == EMPTY) {
            // Need to create new transition
            std::unordered_set<char> chars;
            chars.insert(c);
            
            // For existing children, collect their characters
            for (int ch = 0; ch < 256; ch++) {
                int existing = base[state] + ch;
                if (existing >= 0 && existing < static_cast<int>(base.size()) && 
                    check[existing] == state) {
                    chars.insert(static_cast<char>(ch));
                }
            }
            
            // Find new base value that accommodates all characters
            int newBase = findBase(state, chars);
            
            if (newBase != base[state] && base[state] != EMPTY) {
                // Need to relocate existing children
                for (int ch = 0; ch < 256; ch++) {
                    int oldNext = base[state] + ch;
                    if (oldNext >= 0 && oldNext < static_cast<int>(base.size()) && 
                        check[oldNext] == state) {
                        
                        int newNext = newBase + ch;
                        if (newNext >= static_cast<int>(base.size())) {
                            resize(newNext + 1000);
                        }
                        
                        base[newNext] = base[oldNext];
                        check[newNext] = state;
                        used[newNext] = true;
                        
                        used[oldNext] = false;
                        base[oldNext] = EMPTY;
                        check[oldNext] = EMPTY;
                    }
                }
            }
            
            base[state] = newBase;
            nextState = newBase + code;
            
            if (nextState >= static_cast<int>(base.size())) {
                resize(nextState + 1000);
            }
            
            setTransition(state, c, nextState);
            maxState = std::max(maxState, static_cast<size_t>(nextState));
        }
        
        state = nextState;
    }
    
    // Mark end of word (use negative base value)
    if (base[state] >= 0) {
        base[state] = -base[state] - 1;  // Negative indicates end of word
        wordCount++;
    }
}

bool DoubleArrayTrie::search(const std::string& word) const {
    int state = 0;
    
    for (char c : word) {
        int nextState = getTransition(state, c);
        if (nextState == EMPTY) {
            return false;
        }
        state = nextState;
    }
    
    return base[state] < 0;  // Negative base means end of word
}

bool DoubleArrayTrie::startsWith(const std::string& prefix) const {
    int state = 0;
    
    for (char c : prefix) {
        int nextState = getTransition(state, c);
        if (nextState == EMPTY) {
            return false;
        }
        state = nextState;
    }
    
    return true;
}

size_t DoubleArrayTrie::getMemoryUsage() const {
    return base.size() * sizeof(int) + check.size() * sizeof(int) + 
           used.size() * sizeof(bool);
}

double DoubleArrayTrie::getSpaceEfficiency() const {
    if (base.size() == 0) return 0.0;
    
    size_t usedCount = 0;
    for (bool u : used) {
        if (u) usedCount++;
    }
    
    return static_cast<double>(usedCount) / base.size();
}

void DoubleArrayTrie::clear() {
    base.clear();
    check.clear();
    used.clear();
    base.resize(INITIAL_SIZE, EMPTY);
    check.resize(INITIAL_SIZE, EMPTY);
    used.resize(INITIAL_SIZE, false);
    
    base[0] = 1;
    check[0] = EMPTY;
    used[0] = true;
    
    wordCount = 0;
    maxState = 0;
}

void DoubleArrayTrie::compact() {
    // Trim arrays to only used portion
    size_t newSize = maxState + 1;
    base.resize(newSize);
    check.resize(newSize);
    used.resize(newSize);
}

int DoubleArrayTrie::findBase(int state, const std::unordered_set<char>& chars) {
    for (int b = 1; b < static_cast<int>(base.size()); b++) {
        bool valid = true;
        
        for (char c : chars) {
            int code = static_cast<unsigned char>(c);
            int pos = b + code;
            
            if (pos >= static_cast<int>(base.size())) {
                continue;
            }
            
            if (used[pos] && check[pos] != state) {
                valid = false;
                break;
            }
        }
        
        if (valid) {
            return b;
        }
    }
    
    return base.size();
}

void DoubleArrayTrie::resize(size_t newSize) {
    base.resize(newSize, EMPTY);
    check.resize(newSize, EMPTY);
    used.resize(newSize, false);
}

int DoubleArrayTrie::getTransition(int state, char c) const {
    if (state < 0 || state >= static_cast<int>(base.size())) {
        return EMPTY;
    }
    
    int b = base[state];
    if (b < 0) b = -b - 1;  // Handle end-of-word marker
    
    int code = static_cast<unsigned char>(c);
    int nextState = b + code;
    
    if (nextState >= 0 && nextState < static_cast<int>(check.size()) && 
        check[nextState] == state) {
        return nextState;
    }
    
    return EMPTY;
}

void DoubleArrayTrie::setTransition(int state, char c, int nextState) {
    int code = static_cast<unsigned char>(c);
    check[nextState] = state;
    base[nextState] = 1;  // Default base for new states
    used[nextState] = true;
}

bool DoubleArrayTrie::isValidTransition(int state, char c, int nextState) const {
    if (nextState < 0 || nextState >= static_cast<int>(check.size())) {
        return false;
    }
    
    return check[nextState] == state;
}

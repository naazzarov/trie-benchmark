# Trie Benchmark

This is my research project comparing different trie implementations in C++. I wanted to see how much memory you can actually save by using fancier data structures.

## The three tries I implemented

1. **Standard Trie** - just a basic trie with hash maps for children
2. **Compressed Trie** - merges chains of single-child nodes (also called radix tree)
3. **Double-Array Trie** - stores everything in two arrays, really compact but tricky to code

## How to run it

```bash
make
./trie_benchmark
```

This will test all three with random strings and (if you have it) a dictionary file.

## Results I got

Testing with 50,000 words:

| Trie Type | Memory | Bytes/Word |
|-----------|--------|------------|
| Standard | 20 MB | 415 |
| Compressed | 6.5 MB | 137 |
| Double-Array | 1.1 MB | 23 |

So the double-array uses like 94% less memory than standard, which is pretty cool. But it takes forever to build.

## Project structure

```
include/     - header files
src/         - implementation files
main.cpp     - runs the benchmarks
figures/     - graphs for the paper
paper.tex    - the actual paper (LaTeX)
```

## What I learned

- Compressed tries are probably the best for most cases
- Double-array is good if you build once and search a lot
- Standard trie is fine for small datasets

The paper has more details and proper analysis.

#!/usr/bin/env python3
"""
Generate graphs from benchmark results for the paper
Does not require pandas or matplotlib - creates data files for external tools
"""

import csv
import os
import sys

def load_data(filename='benchmark_results.csv'):
    """Load benchmark data"""
    if not os.path.exists(filename):
        print(f"Error: {filename} not found")
        return None
    
    data = {}
    with open(filename, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            trie_type = row['TrieType']
            size = int(row['DatasetSize'])
            
            if trie_type not in data:
                data[trie_type] = {}
            
            data[trie_type][size] = {
                'memory_kb': float(row['MemoryKB']),
                'insert_ms': float(row['InsertTimeMS']),
                'search_ms': float(row['SearchTimeMS']),
                'bytes_per_word': float(row['BytesPerWord']),
            }
    
    return data

def create_gnuplot_scripts(data):
    """Create gnuplot scripts for graphs"""
    
    print("Creating gnuplot scripts...\n")
    
    # Script 1: Memory vs Dataset Size
    script1 = """set terminal pngcairo size 1000,600 font "Arial,12"
set output 'fig1_memory_scaling.png'
set title 'Memory Usage vs Dataset Size'
set xlabel 'Dataset Size (words)'
set ylabel 'Memory (MB)'
set key autotitle columnheader
set datafile separator ','
plot 'memory_scaling.csv' using 1:2 with linespoints title 'Standard Trie', \\
     '' using 1:3 with linespoints title 'Compressed Trie', \\
     '' using 1:4 with linespoints title 'Double-Array Trie'
"""
    
    with open('plot_memory_scaling.gp', 'w') as f:
        f.write(script1)
    print("Created: plot_memory_scaling.gp")
    
    # Script 2: Bytes per Word
    script2 = """set terminal pngcairo size 800,500 font "Arial,12"
set output 'fig2_memory_per_word.png'
set title 'Memory Efficiency - 50,000 Words'
set ylabel 'Bytes per Word'
set style data histogram
set style histogram cluster gap 1
set boxwidth 0.8
plot 'bytes_per_word_50k.csv' using 2:xtic(1) title ''
"""
    
    with open('plot_bytes_per_word.gp', 'w') as f:
        f.write(script2)
    print("Created: plot_bytes_per_word.gp")
    
    # Create data files
    
    # Memory scaling data
    with open('memory_scaling.csv', 'w') as f:
        f.write('Size,Standard,Compressed,DoubleArray\n')
        
        all_sizes = set()
        for trie_type in data:
            all_sizes.update(data[trie_type].keys())
        
        for size in sorted(all_sizes):
            std = data.get('Standard Trie', {}).get(size, {}).get('memory_kb', 0) / 1024
            comp = data.get('Compressed Trie', {}).get(size, {}).get('memory_kb', 0) / 1024
            darray = data.get('Double-Array Trie', {}).get(size, {}).get('memory_kb', 0) / 1024
            
            f.write(f'{size},{std},{comp},{darray}\n')
    print("Created: memory_scaling.csv")
    
    # Bytes per word for 50K
    with open('bytes_per_word_50k.csv', 'w') as f:
        f.write('Implementation,BytesPerWord\n')
        
        for trie_type in ['Standard Trie', 'Compressed Trie', 'Double-Array Trie']:
            bpw = data.get(trie_type, {}).get(50000, {}).get('bytes_per_word', 0)
            f.write(f'{trie_type},{bpw}\n')
    print("Created: bytes_per_word_50k.csv")
    
    print("\nTo generate graphs, install gnuplot:")
    print("  brew install gnuplot")
    print("\nThen run:")
    print("  gnuplot plot_memory_scaling.gp")
    print("  gnuplot plot_bytes_per_word.gp")

def create_markdown_tables(data):
    """Create markdown tables for the paper"""
    
    print("\nCreating markdown tables for the paper...\n")
    
    # All sizes comparison
    table = "| Size | Standard (KB) | Compressed (KB) | Double-Array (KB) |\n"
    table += "|------|--------------|-----------------|------------------|\n"
    
    all_sizes = set()
    for trie_type in data:
        all_sizes.update(data[trie_type].keys())
    
    for size in sorted(all_sizes):
        std = data.get('Standard Trie', {}).get(size, {}).get('memory_kb', 0)
        comp = data.get('Compressed Trie', {}).get(size, {}).get('memory_kb', 0)
        darray = data.get('Double-Array Trie', {}).get(size, {}).get('memory_kb', 0)
        
        table += f"| {size:,} | {std:,.0f} | {comp:,.0f} | {darray:,.0f} |\n"
    
    with open('table_memory_comparison.md', 'w') as f:
        f.write("# Memory Usage Comparison\n\n")
        f.write(table)
    
    print("Created: table_memory_comparison.md")
    print(table)
    
    # Performance table
    print("\nPerformance on 50,000 Words Dataset:\n")
    
    data_50k = {}
    for trie_type in data:
        if 50000 in data[trie_type]:
            data_50k[trie_type] = data[trie_type][50000]
    
    perf_table = "| Implementation | Memory (MB) | Insert (ms) | Search (ms) | Bytes/Word |\n"
    perf_table += "|---|---|---|---|---|\n"
    
    for trie_type in ['Standard Trie', 'Compressed Trie', 'Double-Array Trie']:
        if trie_type in data_50k:
            d = data_50k[trie_type]
            mem_mb = d['memory_kb'] / 1024
            perf_table += f"| {trie_type} | {mem_mb:.1f} | {d['insert_ms']:.1f} | {d['search_ms']:.2f} | {d['bytes_per_word']:.1f} |\n"
    
    with open('table_performance_50k.md', 'w') as f:
        f.write("# Performance Metrics - 50K Words\n\n")
        f.write(perf_table)
    
    print(perf_table)

if __name__ == "__main__":
    print("Benchmark Data Generation")
    print("=" * 60)
    
    data = load_data()
    if data is None:
        sys.exit(1)
    
    total_entries = sum(len(v) for v in data.values())
    print(f"Loaded {total_entries} benchmark results from {len(data)} implementations\n")
    
    try:
        create_markdown_tables(data)
        create_gnuplot_scripts(data)
        
        print("\n" + "=" * 60)
        print("Data files created successfully!")
        print("\nFor matplotlib graphs: pip3 install matplotlib pandas")
        print("For gnuplot: brew install gnuplot")
        
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)

#!/usr/bin/env python3
"""
Generate figures for the trie benchmark paper
Requires: matplotlib (pip install matplotlib)
"""

import matplotlib.pyplot as plt
import matplotlib.patches as mpatches
import numpy as np
import os

# Create figures directory
os.makedirs('figures', exist_ok=True)

# Benchmark data from our results
datasets = ['1K', '10K', '50K', '370K']
sizes = [1000, 10000, 50000, 370105]

# Memory in KB
memory = {
    'Standard': [470, 4360, 20254, 57212],
    'Compressed': [135, 1280, 6712, 48086],
    'Double-Array': [88, 229, 1144, 4150]
}

# Bytes per word
bytes_per_word = {
    'Standard': [481, 446, 415, 158],
    'Compressed': [138, 131, 137, 133],
    'Double-Array': [90, 23, 23, 11]
}

# Insert time in ms
insert_time = {
    'Standard': [0.78, 7.08, 31.27, 86.29],
    'Compressed': [0.21, 1.80, 19.96, 68.29],
    'Double-Array': [31.91, 1204, 30160, 217110]
}

# Colors
colors = {
    'Standard': '#4285f4',      # Blue
    'Compressed': '#34a853',    # Green
    'Double-Array': '#ea4335'   # Red
}

# ============================================
# Figure 1: Memory Usage Comparison (Bar Chart)
# ============================================
fig, ax = plt.subplots(figsize=(10, 6))

x = np.arange(len(datasets))
width = 0.25

bars1 = ax.bar(x - width, [m/1024 for m in memory['Standard']], width, 
               label='Standard Trie', color=colors['Standard'])
bars2 = ax.bar(x, [m/1024 for m in memory['Compressed']], width, 
               label='Compressed Trie', color=colors['Compressed'])
bars3 = ax.bar(x + width, [m/1024 for m in memory['Double-Array']], width, 
               label='Double-Array Trie', color=colors['Double-Array'])

ax.set_xlabel('Dataset Size', fontsize=12)
ax.set_ylabel('Memory Usage (MB)', fontsize=12)
ax.set_title('Memory Usage Comparison Across Dataset Sizes', fontsize=14, fontweight='bold')
ax.set_xticks(x)
ax.set_xticklabels(datasets)
ax.legend()
ax.grid(axis='y', alpha=0.3)

plt.tight_layout()
plt.savefig('figures/fig1_memory_comparison.png', dpi=150)
plt.savefig('figures/fig1_memory_comparison.pdf')
print("Created: figures/fig1_memory_comparison.png")

# ============================================
# Figure 2: Memory Efficiency (Bytes per Word)
# ============================================
fig, ax = plt.subplots(figsize=(8, 6))

variants = ['Standard\nTrie', 'Compressed\nTrie', 'Double-Array\nTrie']
bpw_50k = [bytes_per_word['Standard'][2], bytes_per_word['Compressed'][2], bytes_per_word['Double-Array'][2]]
bar_colors = [colors['Standard'], colors['Compressed'], colors['Double-Array']]

bars = ax.bar(variants, bpw_50k, color=bar_colors, edgecolor='black', linewidth=1.2)

# Add value labels on bars
for bar, val in zip(bars, bpw_50k):
    ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 10, 
            f'{val}', ha='center', va='bottom', fontsize=14, fontweight='bold')

ax.set_ylabel('Bytes per Word', fontsize=12)
ax.set_title('Memory Efficiency on 50,000 Words', fontsize=14, fontweight='bold')
ax.set_ylim(0, 500)
ax.grid(axis='y', alpha=0.3)

# Add percentage reduction annotations
ax.annotate('67% reduction', xy=(1, 137), xytext=(1.3, 280),
            arrowprops=dict(arrowstyle='->', color='green'),
            fontsize=10, color='green')
ax.annotate('94% reduction', xy=(2, 23), xytext=(1.7, 150),
            arrowprops=dict(arrowstyle='->', color='red'),
            fontsize=10, color='red')

plt.tight_layout()
plt.savefig('figures/fig2_bytes_per_word.png', dpi=150)
plt.savefig('figures/fig2_bytes_per_word.pdf')
print("Created: figures/fig2_bytes_per_word.png")

# ============================================
# Figure 3: Scalability (Line Graph)
# ============================================
fig, ax = plt.subplots(figsize=(10, 6))

ax.plot(sizes, [m/1024 for m in memory['Standard']], 'o-', 
        label='Standard Trie', color=colors['Standard'], linewidth=2, markersize=8)
ax.plot(sizes, [m/1024 for m in memory['Compressed']], 's-', 
        label='Compressed Trie', color=colors['Compressed'], linewidth=2, markersize=8)
ax.plot(sizes, [m/1024 for m in memory['Double-Array']], '^-', 
        label='Double-Array Trie', color=colors['Double-Array'], linewidth=2, markersize=8)

ax.set_xlabel('Number of Words', fontsize=12)
ax.set_ylabel('Memory Usage (MB)', fontsize=12)
ax.set_title('Memory Scalability', fontsize=14, fontweight='bold')
ax.set_xscale('log')
ax.legend()
ax.grid(True, alpha=0.3)

plt.tight_layout()
plt.savefig('figures/fig3_scalability.png', dpi=150)
plt.savefig('figures/fig3_scalability.pdf')
print("Created: figures/fig3_scalability.png")

# ============================================
# Figure 4: Trade-off Summary
# ============================================
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

# Left: Memory on 370K
variants_short = ['Standard', 'Compressed', 'Double-Array']
mem_370k = [memory['Standard'][3]/1024, memory['Compressed'][3]/1024, memory['Double-Array'][3]/1024]

ax1.barh(variants_short, mem_370k, color=[colors['Standard'], colors['Compressed'], colors['Double-Array']])
ax1.set_xlabel('Memory (MB)', fontsize=12)
ax1.set_title('Memory Usage (370K words)', fontsize=12, fontweight='bold')
ax1.grid(axis='x', alpha=0.3)

for i, (val, name) in enumerate(zip(mem_370k, variants_short)):
    ax1.text(val + 1, i, f'{val:.1f} MB', va='center', fontsize=10)

# Right: Insert time on 370K (log scale)
insert_370k = [insert_time['Standard'][3]/1000, insert_time['Compressed'][3]/1000, insert_time['Double-Array'][3]/1000]

ax2.barh(variants_short, insert_370k, color=[colors['Standard'], colors['Compressed'], colors['Double-Array']])
ax2.set_xlabel('Insert Time (seconds)', fontsize=12)
ax2.set_title('Construction Time (370K words)', fontsize=12, fontweight='bold')
ax2.set_xscale('log')
ax2.grid(axis='x', alpha=0.3)

for i, (val, name) in enumerate(zip(insert_370k, variants_short)):
    ax2.text(val * 1.1, i, f'{val:.1f}s', va='center', fontsize=10)

plt.tight_layout()
plt.savefig('figures/fig4_tradeoff.png', dpi=150)
plt.savefig('figures/fig4_tradeoff.pdf')
print("Created: figures/fig4_tradeoff.png")

print("\nAll figures generated in 'figures/' directory!")
print("Use the .pdf versions in LaTeX for best quality.")

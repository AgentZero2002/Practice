# THU Data Structures Sample Code Repository

清华大学数据结构课程 —— 样本代码仓库

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

## 简介

本仓库基于清华大学邓俊辉教授《数据结构（C++语言版）》第3版教材体系构建，提供 **172 个完整可运行 C++ 实现**，涵盖 36 章、6 大知识领域。适合：

- 清华数据结构课程（30240184）的学习者
- 408/826 计算机考研复习
- ICPC/NOI 算法竞赛训练
- 大厂面试系统准备
- 科研与工程中的数据结构参考

## 仓库结构 (6 部 36 章)

```
practice/
├── Hello.cpp                         # 仓库入口 / 欢迎页
│
╔══ Part I: 基础篇 (01-06) — 线性/层次结构 ══════════════════════════════╗
║                                                                        ║
├── 01_introduction/                  # 第1章 绪论
│   ├── fibonacci.cpp                 #   Fibonacci 数列
│   ├── algorithm_analysis.cpp        #   复杂度分析 + 冰雹序列
│   └── recursion_demo.cpp            #   汉诺塔 / 最大子序列和 / 快速幂
│
├── 02_vector/                        # 第2章 向量
│   ├── vector_impl.hpp               #   向量 ADT 模板
│   ├── vector_demo.cpp               #   向量操作演示
│   ├── binary_search.cpp             #   二分查找 (A/B/C 三版本)
│   ├── merge_sort.cpp                #   归并排序
│   └── deduplicate.cpp               #   唯一化算法
│
├── 03_list/                          # 第3章 列表
│   ├── list_impl.hpp                 #   双向链表 ADT
│   ├── list_demo.cpp                 #   链表操作演示
│   ├── reverse_list.cpp              #   链表逆置 (3 种方法)
│   ├── josephus.cpp                  #   约瑟夫斯问题
│   └── polynomial.cpp                #   多项式运算
│
├── 04_stack_queue/                   # 第4章 栈与队列
│   ├── stack_impl.hpp                #   栈 ADT (顺序+链式)
│   ├── queue_impl.hpp                #   队列 ADT (循环+链式)
│   ├── stack_applications.cpp        #   括号匹配/表达式求值/栈混洗
│   ├── hanoi_iterative.cpp           #   汉诺塔非递归
│   ├── maze_solver.cpp               #   迷宫求解 (BFS+DFS)
│   └── deque_impl.hpp                #   双端队列
│
├── 05_binary_tree/                   # 第5章 二叉树
│   ├── bintree_impl.hpp              #   二叉树 ADT 模板
│   ├── traversal.cpp                 #   四种遍历 (递归)
│   ├── traversal_iterative.cpp       #   非递归遍历
│   ├── reconstruct_tree.cpp          #   前序+中序 重构
│   ├── expression_tree.cpp           #   表达式树
│   └── huffman_tree.cpp              #   Huffman 编码树
│
├── 06_graph/                         # 第6章 图
│   ├── graph_impl.hpp                #   图 ADT (邻接矩阵+邻接表)
│   ├── graph_traversal.cpp           #   DFS/BFS 遍历
│   ├── topological_sort.cpp          #   拓扑排序 (Kahn+DFS)
│   ├── mst_prim.cpp                  #   Prim 最小生成树
│   ├── mst_kruskal.cpp               #   Kruskal 最小生成树
│   ├── shortest_path_dijkstra.cpp    #   Dijkstra 最短路径
│   ├── shortest_path_floyd.cpp       #   Floyd 全源最短路径
│   └── critical_path.cpp             #   关键路径 (AOE网)
║                                                                        ║
╚══════════════════════════════════════════════════════════════════════════╝

╔══ Part II: 搜索与索引 (07-12) — 树/哈希/堆/排序 ═══════════════════════╗
║                                                                        ║
├── 07_bst/                           # 第7章 二叉搜索树
│   ├── bst_impl.hpp                  #   BST 模板
│   ├── bst_demo.cpp                  #   BST 操作 + 值域区间查询
│   └── avl_tree.hpp                  #   AVL 平衡树 (4旋转/3+4重构)
│
├── 08_advanced_tree/                 # 第8章 高级搜索树
│   ├── splay_tree.hpp                #   伸展树 (zig/zig-zig/zig-zag)
│   ├── b_tree.hpp                    #   B-树 (5阶)
│   └── red_black_tree.hpp            #   红黑树
│
├── 09_dictionary/                    # 第9章 词典与哈希
│   ├── hash_table.hpp                #   哈希表 (分离链+开放定址)
│   ├── hash_table_demo.cpp           #   哈希表示例
│   └── bloom_filter.cpp              #   布隆过滤器
│
├── 10_priority_queue/                # 第10章 优先级队列
│   ├── heap_impl.hpp                 #   完全二叉堆 + Floyd建堆
│   ├── heap_sort.cpp                 #   堆排序
│   ├── leftist_heap.hpp              #   左式堆 (合并 O(log n))
│   └── heap_applications.cpp         #   Top-K / K-Way归并
│
├── 11_string/                        # 第11章 串匹配
│   ├── brute_force_match.cpp         #   BF 蛮力匹配
│   ├── kmp.cpp                       #   KMP (next + nextval)
│   └── bm.cpp                        #   Boyer-Moore (BC+GS)
│
├── 12_sorting/                       # 第12章 排序
│   ├── quick_sort.cpp                #   快速排序 (LGU+三路划分)
│   ├── shell_sort.cpp                #   希尔排序
│   ├── radix_sort.cpp                #   基数排序 (LSD)
│   └── sort_comparison.cpp           #   排序算法性能对比
║                                                                        ║
╚══════════════════════════════════════════════════════════════════════════╝

╔══ Part III: 高级数据结构 (13-18) — 线段树/持久化/后缀/空间 ════════════╗
║                                                                        ║
├── 13_advanced_structures/           # 高级基础结构
│   ├── segment_tree.cpp              #   线段树 (lazy propagation)
│   ├── fenwick_tree.cpp              #   树状数组 (BIT)
│   ├── trie.cpp                      #   字典树 / 前缀树
│   ├── union_find.cpp                #   并查集 (路径压缩+按秩合并)
│   ├── sparse_table.cpp              #   ST 表 (静态RMQ O(1))
│   └── skip_list.cpp                 #   跳表
│
├── 14_persistent_ds/                 # 可持久化数据结构
│   ├── persistent_segtree.cpp        #   可持久化线段树 (主席树)
│   ├── bit_2d.cpp                    #   二维树状数组
│   ├── persistent_trie.cpp           #   可持久化 Trie (区间异或max)
│   └── persistent_array.cpp          #   可持久化数组 (Path Copying)
│
├── 15_tree_algorithms/               # 树上算法
│   ├── heavy_light_decomp.cpp        #   树链剖分 (HLD)
│   ├── tree_dp.cpp                   #   树形DP (独立集/直径/背包)
│   ├── centroid_decomp.cpp           #   点分治 (树上路径统计)
│   └── dsu_on_tree.cpp               #   DSU on Tree (树上启发式合并)
│
├── 16_suffix_structures/             # 后缀结构
│   ├── suffix_array.cpp              #   后缀数组 (倍增法)
│   ├── lcp_kasai.cpp                 #   LCP 数组 (Kasai O(n))
│   ├── suffix_automaton.cpp          #   后缀自动机 (SAM)
│   ├── sam_applications.cpp          #   SAM 应用 (LCS/出现次数)
│   └── substring_problems.cpp        #   子串问题综合
│
├── 17_wavelet_rmq/                   # 简洁结构与RMQ
│   ├── wavelet_tree.cpp              #   Wavelet Tree (区间第K小)
│   ├── wavelet_matrix.cpp            #   Wavelet Matrix + Rank/Select
│   └── cartesian_tree.cpp            #   Cartesian Tree (RMQ→LCA)
│
├── 18_spatial_structures/            # 空间/ML 数据结构
│   ├── kd_tree.cpp                   #   KD-Tree (K维搜索树)
│   ├── ball_tree.cpp                 #   Ball Tree (度量空间NN)
│   ├── rtree.cpp                     #   R-Tree (空间索引 MBR)
│   ├── grid_index.cpp                #   均匀网格索引
│   └── hilbert_curve.cpp             #   Hilbert 曲线
║                                                                        ║
╚══════════════════════════════════════════════════════════════════════════╝

╔══ Part IV: 图论与网络 (19-22) — 图算法/网络流/谱图/线性规划 ═══════════╗
║                                                                        ║
├── 19_graph_advanced/                # 进阶图算法
│   ├── tarjan_scc.cpp                #   Tarjan 强连通分量
│   ├── bellman_ford.cpp              #   Bellman-Ford (负权边)
│   ├── bridges_cutpoints.cpp         #   桥与割点
│   ├── max_flow_dinic.cpp            #   Dinic 最大流
│   └── hungarian.cpp                 #   匈牙利算法 (二分图匹配)
│
├── 20_network_flow/                  # 网络流进阶
│   ├── mincost_maxflow.cpp           #   最小费用最大流 (SSP)
│   ├── global_mincut.cpp             #   Stoer-Wagner 全局最小割
│   └── flow_lower_bounds.cpp         #   带下界的流
│
├── 21_spectral_graph/                # 谱图论
│   ├── pagerank.cpp                  #   PageRank 算法
│   └── spectral_clustering.cpp       #   谱聚类 (Laplacian)
│
├── 22_linear_programming/            # 线性规划
│   ├── simplex.cpp                   #   Simplex 单纯形算法
│   └── lp_duality.cpp                #   LP 对偶性 (最大流↔最小割)
║                                                                        ║
╚══════════════════════════════════════════════════════════════════════════╝

╔══ Part V: 算法核心 (23-29) — 字符串/DP/数论/几何/随机/在线 ═══════════╗
║                                                                        ║
├── 23_string_advanced/               # 进阶字符串算法
│   ├── rolling_hash.cpp              #   滚动哈希 (Rabin-Karp)
│   ├── manacher.cpp                  #   Manacher O(n) 回文
│   ├── aho_corasick.cpp              #   AC 自动机 (多模式匹配)
│   └── kmp_automaton.cpp             #   KMP 自动机 (DFA)
│
├── 24_dp_fundamentals/               # DP 基础模式
│   ├── knapsack_variants.cpp         #   背包九讲 (01/完全/多重/分组)
│   ├── interval_dp.cpp               #   区间DP (石子合并/矩阵链乘)
│   ├── linear_dp.cpp                 #   线性DP (LIS/LCS/编辑距离)
│   └── grid_dp.cpp                   #   网格DP (路径/正方形/障碍)
│
├── 25_dp_advanced/                   # DP 进阶
│   ├── state_compression.cpp         #   状态压缩DP (TSP/铺砖)
│   ├── digit_dp.cpp                  #   数位DP
│   ├── dp_optimization.cpp           #   DP优化 (单调队列/斜率)
│   └── probability_dp.cpp            #   概率/期望DP
│
├── 26_number_theory/                 # 数论与数学
│   ├── extended_gcd.cpp              #   扩展欧几里得 + 模逆元
│   ├── chinese_remainder.cpp         #   中国剩余定理 (CRT+exCRT)
│   ├── miller_rabin.cpp              #   Miller-Rabin 素性测试
│   ├── pollard_rho.cpp               #   Pollard's Rho 大整数分解
│   ├── fft.cpp                       #   FFT 快速傅里叶变换
│   └── linear_sieve.cpp              #   线性筛 + 积性函数
│
├── 27_computational_geometry/        # 计算几何
│   ├── geo_primitives.cpp            #   几何原语 (叉积/点积)
│   ├── segment_intersection.cpp      #   线段相交 + 扫描线
│   ├── closest_pair.cpp              #   最近点对 (分治)
│   ├── polygon_ops.cpp               #   多边形面积/重心/内点判定
│   ├── rotating_calipers.cpp         #   旋转卡壳
│   └── halfplane_intersection.cpp    #   半平面交
│
├── 28_randomized_algorithms/         # 随机化算法
│   ├── quickselect.cpp               #   QuickSelect O(n)期望
│   └── monte_carlo.cpp               #   Monte Carlo + Las Vegas
│
├── 29_online_streaming/              # 在线与流式算法
│   ├── heavy_hitters.cpp             #   Heavy Hitters (Misra-Gries)
│   └── online_paging.cpp             #   在线分页 (LRU/OPT/竞争比)
║                                                                        ║
╚══════════════════════════════════════════════════════════════════════════╝

╔══ Part VI: 系统与实战 (30-36) — 概率/存储/并发/内存/外存/面试/竞赛 ═══╗
║                                                                        ║
├── 30_probabilistic_ds/              # 概率与相似性
│   ├── count_min_sketch.cpp          #   Count-Min Sketch
│   ├── hyperloglog.cpp               #   HyperLogLog 基数估计
│   ├── consistent_hashing.cpp        #   一致性哈希 + 虚拟节点
│   ├── merkle_tree.cpp               #   Merkle 哈希树
│   ├── cuckoo_filter.cpp             #   Cuckoo Filter
│   ├── lru_k.cpp                     #   LRU-K 缓存淘汰
│   ├── lsh.cpp                       #   LSH (随机超平面)
│   ├── minhash.cpp                   #   MinHash (Jaccard估计)
│   └── simhash.cpp                   #   SimHash (网页去重)
│
├── 31_storage_systems/               # 存储系统
│   ├── bplus_tree.cpp                #   B+树 (数据库索引)
│   ├── lsm_tree.cpp                  #   LSM Tree (写优化)
│   ├── wal.cpp                       #   Write-Ahead Log
│   ├── inverted_index.cpp            #   倒排索引 (搜索引擎)
│   ├── bitmap_index.cpp              #   位图索引 (OLAP)
│   └── sstable.cpp                   #   SSTable / MemTable
│
├── 32_concurrent_ds/                 # 并发数据结构
│   ├── lockfree_stack.cpp            #   无锁栈 (Treiber Stack)
│   ├── lockfree_queue.cpp            #   无锁队列 (Michael-Scott)
│   ├── concurrent_hashmap.cpp        #   分段锁哈希表
│   ├── rcu_demo.cpp                  #   Read-Copy-Update (RCU)
│   ├── spinlock_rwlock.cpp           #   自旋锁 + 读写锁
│   └── ring_buffer.cpp               #   无锁环形缓冲区
│
├── 33_memory_management/             # 内存管理
│   ├── buddy_allocator.cpp           #   Buddy Allocator (伙伴分配器)
│   └── memory_pool.cpp               #   Memory Pool (对象池)
│
├── 34_external_memory/               # 外存算法
│   ├── external_merge_sort.cpp       #   外部归并排序
│   └── kway_merge.cpp               #   K路归并 + 败者树
│
├── 35_interview_patterns/            # 大厂面试高频模板
│   ├── monotonic_stack.cpp           #   单调栈 (接雨水/柱状图)
│   ├── two_pointers.cpp              #   双指针 (对撞/快慢)
│   ├── prefix_sum.cpp                #   前缀和+哈希优化
│   ├── interval_problems.cpp         #   区间合并/会议室/射气球
│   ├── stock_trading.cpp             #   股票买卖 I~IV (DP演化)
│   ├── binary_search_patterns.cpp    #   二分模式 (旋转/峰值/2D)
│   ├── backtracking_template.cpp     #   回溯模板 (排列/子集/组合)
│   └── substring_sliding.cpp         #   滑动窗口
│
├── 36_competitive_prog/              # 算法竞赛进阶
│   ├── mo_algorithm.cpp              #   莫队算法
│   ├── matrix_fastpow.cpp            #   矩阵快速幂
│   ├── sg_theorem.cpp                #   SG定理 (博弈论)
│   ├── convex_hull.cpp               #   凸包 (Graham Scan)
│   ├── cdq_divide.cpp                #   CDQ分治 (三维偏序)
│   └── virtual_tree.cpp              #   虚树
║                                                                        ║
╚══════════════════════════════════════════════════════════════════════════╝

└── exercises/                        # 经典习题集 (12个文件)
    ├── list_problems/                #   链表专题
    ├── tree_problems/                #   树专题
    ├── graph_problems/               #   图专题
    ├── dp_problems/                  #   DP与DS结合
    ├── classic_exam/                 #   经典考题
    └── advanced/                     #   进阶习题
```

## 快速开始

每个 `.cpp` 文件均可独立编译运行：

```bash
g++ -std=c++17 01_introduction/fibonacci.cpp -o fibonacci
./fibonacci
```

## 各章核心算法速查

### Part I — 基础篇
| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 绪论 | 递归、分治、动态规划 | - |
| 向量 | 二分查找、归并排序、唯一化 | O(log n) / O(n log n) |
| 列表 | 插入排序、选择排序、逆置 | O(n²) / O(n) |
| 栈与队列 | 表达式求值、栈混洗、迷宫BFS/DFS | O(n) / 指数 |
| 二叉树 | 遍历、Huffman编码、重构 | O(n) / O(n log n) |
| 图 | DFS/BFS、Dijkstra、Prim、Floyd、拓扑/关键路径 | O(V+E)~O((V+E)log V) |

### Part II — 搜索与索引
| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 搜索树 | BST操作、AVL旋转/3+4重构 | O(h) / O(log n) |
| 高级搜索树 | 伸展、B-树分裂/合并、红黑树 | 均摊 O(log n) |
| 词典 | 哈希查找、布隆过滤器 | O(1)期望 |
| 优先级队列 | Floyd建堆、左式堆合并、Top-K | O(n) / O(log n) |
| 串匹配 | KMP (next/nextval)、BM (BC+GS) | O(n+m) |
| 排序 | 快速排序(LGU+3路)、Shell、基数 | O(n log n) |

### Part III — 高级数据结构
| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 高级基础 | 线段树(lazy)、BIT、Trie、并查集、ST表、跳表 | O(log n)/≈O(1) |
| 可持久化 | 主席树、2D BIT、可持久化Trie/Array | O(log n)~O(log²n) |
| 树上算法 | HLD、树形DP、点分治、DSU on Tree | O(log²n)~O(n log n) |
| 后缀结构 | 后缀数组(倍增)、LCP(Kasai)、SAM、LCS | O(n)~O(n log n) |
| 简洁RMQ | Wavelet Tree/Matrix、Cartesian Tree | O(log σ) / O(n) |
| 空间/ML | KD-Tree、Ball Tree、R-Tree、Grid Index | O(log n) / 近似 |

### Part IV — 图论与网络
| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 图论进阶 | Tarjan SCC、Bellman-Ford、桥/割点、Dinic最大流、匈牙利 | O(V+E)~O(V²E) |
| 网络流 | 最小费用最大流(SSP)、Stoer-Wagner全局最小割、下界流 | O(F·VE)~O(V³) |
| 谱图论 | PageRank、谱聚类、Laplacian分解 | O(n³) 幂迭代 |
| 线性规划 | Simplex单纯形、LP对偶性、最大流-最小割定理 | 指数最坏/多项式平均 |

### Part V — 算法核心
| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 字符串进阶 | 滚动哈希、Manacher O(n)、AC自动机、KMP自动机(DFA) | O(n)~O(n+matches) |
| DP基础 | 背包变体、区间DP、线性DP(LIS/LCS/编辑)、网格DP | O(nW)~O(n³) |
| DP进阶 | 状压DP、数位DP、单调队列/斜率优化、概率DP | O(n log n)~O(3ⁿ) |
| 数论 | 扩展GCD、CRT、Miller-Rabin、Pollard-Rho、FFT、线性筛 | O(log n)~O(n^(1/4)) |
| 计算几何 | 原语、线段交、最近点对、多边形、旋转卡壳、半平面交 | O(n)~O(n log n) |
| 随机化 | QuickSelect(期望O(n))、Monte Carlo/Las Vegas | O(n)期望 |
| 在线/流式 | Misra-Gries Heavy Hitters、在线分页(LRU/OPT/竞争比) | O(k)空间 |

### Part VI — 系统与实战
| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 概率/相似性 | Count-Min/HLL、一致性哈希、Merkle、Cuckoo、LRU-K、LSH | 亚线性/近似 |
| 存储系统 | B+Tree、LSM、WAL、倒排索引、位图索引、SSTable | O(log n)/顺序写 |
| 并发DS | 无锁栈/队列、分段锁Map、RCU、Spinlock/RWLock、Ring Buffer | O(1)无锁 |
| 内存管理 | Buddy Allocator (伙伴)、Memory Pool (对象池) | O(log M)/O(1) |
| 外存算法 | 外存归并排序、K路归并+败者树 | O(N/B · log N) I/O |
| 面试模板 | 单调栈/双指针/前缀和/滑动窗口/回溯/股票DP/二分 | O(n)~O(n log n) |
| 竞赛进阶 | 莫队、矩阵快速幂、SG定理、凸包、CDQ分治、虚树 | O(n√n)~O(n log²n) |

## 近年 826 算法题速览

| 年份 | 题目 | 对应章节 | 代码位置 |
|------|------|----------|----------|
| 2020 | 伸展树 → 完全二叉树 | 第8章 | `08_advanced_tree/splay_tree.hpp` |
| 2021 | BST 值域区间查询 | 第7章 | `07_bst/bst_demo.cpp` |
| 2022 | 一般树层次遍历 | 第5章 | `05_binary_tree/traversal.cpp` |
| 2023 | 从遍历序列求LCA | 第5章 | `exercises/tree_problems/` |
| 2024 | 有根有序树后序遍历 | 第5章 | `05_binary_tree/traversal.cpp` |
| 2025 | 二叉堆化 / 左式堆化 | 第10章 | `10_priority_queue/` |

## 构建说明

本仓库由以下 AI 协作完成：
- **代码架构与实现**: Claude (Anthropic) — 生成全部 C++ 代码，设计目录结构与测试用例
- **推理引擎**: DeepSeek V4 Pro (1M 上下文) — 提供底层推理能力支持

所有代码经人工审查确认，AI 工具仅作为生产力辅助。

## 贡献

欢迎提交 Issue 和 Pull Request！代码使用 C++17 标准，含中文注释和测试用例。

## 许可

MIT License

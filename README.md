# THU Data Structures Sample Code Repository

清华大学数据结构课程 —— 样本代码仓库

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://isocpp.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

## 📖 简介

本仓库基于清华大学邓俊辉教授《数据结构（C++语言版）》第3版教材体系构建，提供教材核心数据结构与算法的**完整可运行 C++ 实现**，并配套**经典习题解答**。适合：

- 清华数据结构课程（30240184）的学习者
- 408/826 计算机考研复习
- 任何想系统学习数据结构与算法的 C++ 开发者

## 🏗️ 仓库结构

```
practice/
├── Hello.cpp                      # 仓库入口 / 欢迎页
│
├── 01_introduction/               # 第1章 绪论
│   ├── fibonacci.cpp              #   Fibonacci 数列（递推/递归/优化）
│   ├── algorithm_analysis.cpp     #   时间复杂度分析示例
│   └── recursion_demo.cpp         #   递归与分治（汉诺塔、最大子序列和）
│
├── 02_vector/                     # 第2章 向量
│   ├── vector_impl.hpp            #   向量 ADT 模板实现
│   ├── vector_demo.cpp            #   向量操作演示
│   ├── binary_search.cpp          #   二分查找（A/B/C三版本）
│   ├── merge_sort.cpp             #   归并排序
│   └── deduplicate.cpp            #   唯一化算法
│
├── 03_list/                       # 第3章 列表
│   ├── list_impl.hpp              #   双向链表 ADT 模板实现
│   ├── list_demo.cpp              #   链表操作演示
│   ├── reverse_list.cpp           #   链表逆置（3种方法）
│   ├── josephus.cpp               #   约瑟夫斯问题
│   └── polynomial.cpp             #   多项式运算
│
├── 04_stack_queue/                # 第4章 栈与队列
│   ├── stack_impl.hpp             #   栈ADT（顺序栈+链式栈）
│   ├── queue_impl.hpp             #   队列ADT（循环队列+链队列）
│   ├── stack_applications.cpp     #   栈应用：括号匹配、表达式求值
│   ├── hanoi_iterative.cpp        #   汉诺塔非递归解法
│   ├── maze_solver.cpp            #   迷宫求解（BFS）
│   └── deque_impl.hpp             #   双端队列
│
├── 05_binary_tree/                # 第5章 二叉树
│   ├── bintree_impl.hpp           #   二叉树 ADT 模板实现
│   ├── traversal.cpp              #   四种遍历（先/中/后序+层次）
│   ├── traversal_iterative.cpp    #   非递归遍历
│   ├── reconstruct_tree.cpp       #   前序+中序 重构二叉树
│   ├── expression_tree.cpp        #   表达式树
│   └── huffman_tree.cpp           #   Huffman 编码树
│
├── 06_graph/                      # 第6章 图
│   ├── graph_impl.hpp             #   图 ADT（邻接矩阵+邻接表）
│   ├── graph_traversal.cpp        #   DFS/BFS 遍历
│   ├── topological_sort.cpp       #   拓扑排序（Kahn + DFS）
│   ├── mst_prim.cpp               #   Prim 最小生成树
│   ├── mst_kruskal.cpp            #   Kruskal 最小生成树
│   ├── shortest_path_dijkstra.cpp #   Dijkstra 最短路径
│   ├── shortest_path_floyd.cpp    #   Floyd 全源最短路径
│   └── critical_path.cpp          #   关键路径（AOE网）
│
├── 07_bst/                        # 第7章 搜索树
│   ├── bst_impl.hpp               #   BST 模板实现
│   ├── bst_demo.cpp               #   BST 操作演示
│   └── avl_tree.hpp               #   AVL 平衡树实现
│
├── 08_advanced_tree/              # 第8章 高级搜索树
│   ├── splay_tree.hpp             #   伸展树
│   ├── b_tree.hpp                 #   B-树
│   └── red_black_tree.hpp         #   红黑树
│
├── 09_dictionary/                 # 第9章 词典
│   ├── hash_table.hpp             #   哈希表（分离链+开放定址）
│   ├── hash_table_demo.cpp        #   哈希表演示
│   └── bloom_filter.cpp           #   布隆过滤器
│
├── 10_priority_queue/             # 第10章 优先级队列
│   ├── heap_impl.hpp              #   完全二叉堆
│   ├── heap_sort.cpp              #   堆排序
│   ├── leftist_heap.hpp           #   左式堆
│   └── heap_applications.cpp      #   堆应用（Top-K、多路归并）
│
├── 11_string/                     # 第11章 串
│   ├── brute_force_match.cpp      #   BF 蛮力匹配
│   ├── kmp.cpp                    #   KMP 算法
│   └── bm.cpp                     #   Boyer-Moore 算法
│
├── 12_sorting/                    # 第12章 排序
│   ├── quick_sort.cpp             #   快速排序（含三路划分）
│   ├── shell_sort.cpp             #   希尔排序
│   ├── radix_sort.cpp             #   基数排序
│   ├── tournament_sort.cpp        #   锦标赛排序
│   └── sort_comparison.cpp        #   排序算法性能对比
│
├── 13_advanced_structures/        # 进阶数据结构 (NEW!)
│   ├── segment_tree.cpp           #   线段树 — 区间查询/修改 O(log n)
│   ├── fenwick_tree.cpp           #   树状数组 (BIT) — 前缀和 O(log n)
│   ├── trie.cpp                   #   字典树 / 前缀树
│   ├── union_find.cpp             #   并查集 — 路径压缩+按秩合并 ≈O(1)
│   ├── sparse_table.cpp           #   ST 表 — 静态 RMQ O(1)
│   └── skip_list.cpp              #   跳表 — 概率平衡的分层链表
│
├── 14_graph_advanced/             # 进阶图算法 (NEW!)
│   ├── tarjan_scc.cpp             #   Tarjan 强连通分量 O(V+E)
│   ├── bellman_ford.cpp           #   Bellman-Ford 含负权边 O(VE)
│   ├── bridges_cutpoints.cpp      #   桥与割点 (Tarjan)
│   ├── max_flow_dinic.cpp         #   Dinic 最大流 — Max-Flow Min-Cut
│   └── hungarian.cpp              #   匈牙利算法 — 二分图最大匹配
│
├── 15_string_advanced/            # 进阶字符串 (NEW!)
│   ├── rolling_hash.cpp           #   滚动哈希 — O(n) 子串匹配
│   ├── manacher.cpp               #   Manacher — 最长回文子串 O(n)
│   └── aho_corasick.cpp           #   AC 自动机 — 多模式匹配
│
└── exercises/                     # 经典习题集
    ├── list_problems/             #   链表专题
    ├── tree_problems/             #   树专题
    ├── graph_problems/            #   图专题
    ├── dp_problems/               #   DP与DS结合
    ├── classic_exam/              #   经典考题
    └── advanced/                  #   进阶习题 (NEW!)
```

## 🚀 快速开始

### 编译运行

每个 `.cpp` 文件均可独立编译运行：

```bash
# 使用 C++17 标准编译
g++ -std=c++17 01_introduction/fibonacci.cpp -o fibonacci

# 运行
./fibonacci
```

### 批量编译

```bash
# 编译所有 .cpp 文件
find . -name "*.cpp" -exec g++ -std=c++17 {} -o {}.out \;
```

## 📚 参考教材

- **教材**: 邓俊辉，《数据结构（C++语言版）》第3版，清华大学出版社，2013（ISBN: 9787302330646）
- **习题解析**: 邓俊辉，《数据结构习题解析》第3版（ISBN: 9787302330653）
- **线上资源**: [课程主页](https://dsa.cs.tsinghua.edu.cn/~deng/ds/dsacpp/)
- **MOOC**: 学堂在线 - 数据结构(上)/(下)
- **OJ**: [THU DSA Online Judge](https://dsa.cs.tsinghua.edu.cn/oj/)

## 📝 各章节核心算法速查

| 章节 | 核心算法 | 复杂度 |
|------|----------|--------|
| 绪论 | 递归、分治、动态规划 | - |
| 向量 | 二分查找、归并排序 | O(log n) / O(n log n) |
| 列表 | 插入排序、选择排序 | O(n²) |
| 栈与队列 | 表达式求值、栈混洗、回溯 | O(n) / 指数 |
| 二叉树 | 遍历、Huffman编码、重构 | O(n) / O(n log n) |
| 图 | DFS/BFS、Dijkstra、Prim、拓扑排序 | O(V+E) / O((V+E)log V) |
| 搜索树 | BST操作、AVL旋转 | O(h) / O(log n) |
| 高级搜索树 | 伸展、B-树分裂/合并、红黑树 | 均摊O(log n) |
| 词典 | 哈希查找、桶排序、基数排序 | O(1)期望 / O(n) |
| 优先级队列 | Floyd建堆、左式堆合并 | O(n) / O(log n) |
| 串 | KMP、BM | O(n+m) |
| 排序 | 快速排序、希尔排序 | O(n log n) / O(n^(4/3)) |
| 进阶结构 | 线段树、树状数组、Trie、并查集、ST表、跳表 | O(log n) / ≈O(1) |
| 进阶图算法 | Tarjan SCC、Bellman-Ford、Dinic最大流、匈牙利 | O(V+E) ~ O(V²E) |
| 进阶字符串 | 滚动哈希、Manacher、AC自动机 | O(n) / O(n+matches) |

## 🔥 近年826算法题速览

| 年份 | 题目 | 对应章节 | 代码位置 |
|------|------|----------|----------|
| 2020 | 伸展树 → 完全二叉树 | 第8章 | `08_advanced_tree/splay_tree.hpp` |
| 2021 | BST 值域区间查询 | 第7章 | `07_bst/bst_demo.cpp` |
| 2022 | 一般树层次遍历 | 第5章 | `05_binary_tree/traversal.cpp` |
| 2023 | 从遍历序列求LCA | 第5章 | `exercises/tree_problems/lowest_common_ancestor.cpp` |
| 2024 | 有根有序树后序遍历 | 第5章 | `05_binary_tree/traversal.cpp` |
| 2025 | 二叉堆化 / 左式堆化 | 第10章 | `10_priority_queue/heap_impl.hpp` |

## 🤖 构建说明

本仓库由以下 AI 协作完成：

- **代码架构与实现**: Claude (Anthropic) — 生成全部 C++ 代码，设计目录结构与测试用例
- **推理引擎**: DeepSeek V4 Pro (1M 上下文) — 提供底层推理能力支持

所有代码经人工审查确认，AI 工具仅作为生产力辅助。

## 🤝 贡献

欢迎提交 Issue 和 Pull Request！请确保代码使用 C++17 标准，包含中文注释和测试用例。

## 📄 许可

MIT License

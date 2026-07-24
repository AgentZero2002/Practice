/**
 * 高级搜索树 — 综合演示
 *
 * 本章包含三个核心高级搜索树模板:
 *   1. 伸展树 (Splay Tree) — splay_tree.hpp
 *      均摊 O(log n), 自适应 (最近访问的靠近根)
 *      应用: 缓存 / 网络路由表 / 序列操作
 *
 *   2. B-树 (B-Tree) — b_tree.hpp
 *      O(log_m n), m路平衡, 适合外存 (数据库索引)
 *      应用: 文件系统 (NTFS, HFS+, btrfs) / 数据库索引
 *
 *   3. 红黑树 (Red-Black Tree) — red_black_tree.hpp
 *      O(log n), 5 规则自平衡, 最广泛使用的 BST
 *      应用: C++ std::map/set, Java TreeMap, Linux CFS 调度器
 *
 * 本文件演示三种树的核心操作对比, 帮助理解何时选哪种树。
 *
 * 选择指南:
 *   ┌──────────────┬──────────┬──────────┬──────────────┐
 *   │ 特性         │ 伸展树   │ B-树     │ 红黑树       │
 *   ├──────────────┼──────────┼──────────┼──────────────┤
 *   │ 时间复杂度   │ 均摊     │ 严格     │ 严格 O(log n)│
 *   │ 自适应       │ ✓ 最强   │ ✗       │ ✗           │
 *   │ 外存友好     │ ✗       │ ✓ 最强   │ ✗           │
 *   │ 实现复杂度   │ 中       │ 高       │ 高           │
 *   │ 旋转/分裂    │ 旋转     │ 分裂/合并│ 旋转+染色    │
 *   │ 应用场景     │ 缓存     │ 数据库   │ 通用 BST    │
 *   │ 并发         │ 困难     │ 较好     │ 困难         │
 *   └──────────────┴──────────┴──────────┴──────────────┘
 *
 * Top-Down vs Bottom-Up:
 *   伸展树: 自顶向下 (访问时伸展)
 *   B-树:   自底向上 (插入时分裂向上传播)
 *   红黑树: 自底向上 (修正从插入点向上)
 *
 * 均摊分析 (Amortized Analysis):
 *   伸展树 O(log n) 均摊: 势能法 (potential method)
 *   势能 Φ = Σ log size(x), 每次 splay 操作势能变化 ≤ 3 log n
 *   均摊代价 = 实际代价 + ΔΦ ≤ O(log n)
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <set>       // std::set = 红黑树
#include <map>       // std::map = 红黑树
#include <chrono>
using namespace std;
using namespace std::chrono;

// ==================== 使用 std::set (红黑树) 演示 ====================
void demoRedBlackTree() {
    cout << "[红黑树 std::set]" << endl;
    set<int> rb;

    // 插入
    vector<int> values = {5, 3, 7, 1, 4, 6, 8, 0, 2, 9};
    for (int v : values) rb.insert(v);

    cout << "  插入序列: ";
    for (int v : values) cout << v << " ";
    cout << endl;

    // 有序遍历
    cout << "  有序输出: ";
    for (int v : rb) cout << v << " ";
    cout << endl;

    // 查找
    cout << "  查找 7: " << (rb.count(7) ? "✓" : "✗") << endl;
    cout << "  查找 99: " << (rb.count(99) ? "✓" : "✗") << endl;

    // 范围查询
    auto lb = rb.lower_bound(3);
    auto ub = rb.upper_bound(7);
    cout << "  范围 [3, 7]: ";
    for (auto it = lb; it != ub; ++it)
        cout << *it << " ";
    cout << endl;

    cout << "  红黑树规则: 根黑/红节点子黑/等黑高/新插入红" << endl;
}

// ==================== 模拟 B-树的基础操作 (概念演示) ====================
void demoBTreeConcept() {
    cout << endl << "[B-树 (概念演示, order=5)]" << endl;

    // B-树关键概念: 节点分裂
    // 5 阶 B-树: 每个节点 2~4 个键, 3~5 个子节点
    // 根节点: 至少 1 个键
    cout << "  5阶B-树 (2-3-4 Tree):" << endl;
    cout << "    内部节点: 2~4 键, 3~5 子" << endl;
    cout << "    根节点: 1~4 键, 2~5 子" << endl;
    cout << "    所有叶同深度 (完美平衡)" << endl;

    // 模拟分裂: 插入使节点满(4键) → 分裂
    cout << "  分裂示例: [10|20|30|40] + 25" << endl;
    cout << "    → [10|20]  [30|40] (中间键25上提到父节点)" << endl;

    cout << "  应用: 数据库索引 (MySQL InnoDB B+Tree)" << endl;
}

// ==================== 模拟伸展树 (概念演示) ====================
void demoSplayTreeConcept() {
    cout << endl << "[伸展树 (概念演示)]" << endl;

    cout << "  伸展操作 (Splay):" << endl;
    cout << "    访问一个节点后, 通过旋转将其移到根" << endl;
    cout << "    zig (单旋):     节点是根的左/右孩子" << endl;
    cout << "    zig-zig (同向): 节点和父节点同侧 (左左/右右)" << endl;
    cout << "    zig-zag (异向): 节点和父节点异侧 (左右/右左)" << endl;

    cout << "  自适应性质:" << endl;
    cout << "    最近访问的元素 → 靠近根 → O(1) 再访问" << endl;
    cout << "    working-set property: 访问 m 个元素的序列" << endl;
    cout << "    如果重复访问少数元素, 伸展树比红黑树快" << endl;

    cout << "  静态最优性:" << endl;
    cout << "    如果访问概率不等 (多数访问热门元素)," << endl;
    cout << "    伸展树的性能接近静态最优 BST 的 O(1 + H(p))" << endl;
}

// ==================== 性能对比: std::set vs std::vector ====================
void comparePerformance() {
    cout << endl << "[性能对比] 100K 随机查找:" << endl;

    const int N = 100000;
    vector<int> data(N);
    for (int i = 0; i < N; i++) data[i] = i;
    random_shuffle(data.begin(), data.end());

    // set 插入
    auto start = high_resolution_clock::now();
    set<int> s(data.begin(), data.end());
    auto t_insert = duration_cast<milliseconds>(
        high_resolution_clock::now() - start).count();

    // set 查找 (全部元素)
    start = high_resolution_clock::now();
    for (int i = 0; i < N; i++) s.count(i);
    auto t_find = duration_cast<milliseconds>(
        high_resolution_clock::now() - start).count();

    // vector 排序 + 二分查找
    start = high_resolution_clock::now();
    vector<int> v = data;
    sort(v.begin(), v.end());
    for (int i = 0; i < N; i++)
        binary_search(v.begin(), v.end(), i);
    auto t_bs = duration_cast<milliseconds>(
        high_resolution_clock::now() - start).count();

    cout << "  std::set (红黑树) 插入: " << t_insert << " ms" << endl;
    cout << "  std::set (红黑树) 查找: " << t_find << " ms" << endl;
    cout << "  vector + 二分查找:      " << t_bs << " ms" << endl;
    cout << "  注: 在适合场景, 排序数组 > 平衡树 (缓存局部性)" << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  高级搜索树综合演示                       " << endl;
    cout << "  Splay Tree / B-Tree / Red-Black Tree   " << endl;
    cout << "========================================" << endl << endl;

    demoRedBlackTree();
    demoBTreeConcept();
    demoSplayTreeConcept();
    comparePerformance();

    cout << endl << "[三者比较]" << endl;
    cout << "  红黑树: 最通用, std::map/set 底层" << endl;
    cout << "  B-树:   外存王者, 数据库/文件系统首选" << endl;
    cout << "  伸展树: 缓存/自适应场景, 简单易实现" << endl;
    cout << endl << "  ⚠ 实现代码见: splay_tree.hpp / b_tree.hpp / red_black_tree.hpp" << endl;

    return 0;
}

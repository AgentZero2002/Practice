/**
 * Las Vegas 算法
 *
 * Las Vegas vs Monte Carlo:
 *   - Monte Carlo: 结果可能错误, 但运行时间确定
 *     (例: Miller-Rabin 素性测试有 1/4 错误概率)
 *   - Las Vegas: 结果总是正确, 但运行时间随机
 *     (例: 随机快速排序, 期望 O(n log n), 结果总是对的)
 *
 * 本文件展示两个经典 Las Vegas 算法:
 *
 * 1. 随机快速排序 (Randomized Quicksort):
 *    - 每次随机选 pivot (而非固定选首/尾)
 *    - 避免最坏 O(n²) 被恶意数据触发
 *    - 期望时间复杂度 O(n log n)
 *
 * 2. 随机选择 (Randomized Select):
 *    - 借助随机 pivot 进行划分
 *    - 期望 O(n) 找到第 k 小元素
 *    - 最坏 O(n²) 但概率极低
 *
 * Las Vegas 算法的哲学:
 *   "宁愿重来一次, 也不返回错误答案"
 *   当验证结果比计算结果更容易时尤其有效。
 *
 * 更多 Las Vegas 算法:
 *   - 随机增量法 (Randomized Incremental Construction)
 *   - 最小圆覆盖 (随机增量期望 O(n))
 *   - 随机化 BST (Treap, Skip List 插入)
 *   - 随机化质数生成 (随机猜+验证)
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <chrono>
using namespace std;
using namespace std::chrono;

// ==================== 随机快速排序 ====================
int randomPartition(vector<int>& arr, int lo, int hi, mt19937& rng) {
    uniform_int_distribution<int> dist(lo, hi);
    int pivotIdx = dist(rng);
    swap(arr[pivotIdx], arr[hi]);  // pivot 移到末尾

    int pivot = arr[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (arr[j] <= pivot)
            swap(arr[++i], arr[j]);
    }
    swap(arr[i + 1], arr[hi]);
    return i + 1;
}

void randomizedQuicksort(vector<int>& arr, int lo, int hi, mt19937& rng) {
    if (lo >= hi) return;
    int pi = randomPartition(arr, lo, hi, rng);
    randomizedQuicksort(arr, lo, pi - 1, rng);
    randomizedQuicksort(arr, pi + 1, hi, rng);
}

// ==================== 随机选择 (Randomized Select) ====================
// 找第 k 小 (k 从 0 开始, 即 order statistic)
int randomizedSelect(vector<int>& arr, int lo, int hi, int k, mt19937& rng) {
    if (lo == hi) return arr[lo];

    int pi = randomPartition(arr, lo, hi, rng);  // pi 的最终位置

    if (k == pi) return arr[pi];
    else if (k < pi)
        return randomizedSelect(arr, lo, pi - 1, k, rng);
    else
        return randomizedSelect(arr, pi + 1, hi, k, rng);
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  Las Vegas 算法                           " << endl;
    cout << "========================================" << endl << endl;

    mt19937 rng(123);

    // 1. 随机快速排序
    vector<int> arr = {38, 27, 43, 3, 9, 82, 10, 15, 56, 23,
                        14, 7, 33, 19, 45, 67, 2, 88, 31, 50};
    vector<int> arr2 = arr;

    cout << "[随机快速排序]" << endl;
    randomizedQuicksort(arr, 0, arr.size() - 1, rng);
    cout << "  排序后: ";
    for (int i = 0; i < 10; i++) cout << arr[i] << " ";
    cout << "... ";
    for (int i = arr.size() - 5; i < (int)arr.size(); i++)
        cout << arr[i] << " ";
    cout << endl;
    cout << "  正确性: " << (is_sorted(arr.begin(), arr.end()) ? "✓" : "✗") << endl;

    // 2. 随机选择
    cout << endl << "[随机选择 (Randomized Select)]" << endl;
    vector<int> searchArr = {7, 10, 4, 3, 20, 15, 8, 12, 6, 1};
    for (int k : {0, 4, 9}) {  // 最小/中位数/最大
        vector<int> tmp = searchArr;
        int val = randomizedSelect(tmp, 0, tmp.size() - 1, k, rng);
        // 验证
        vector<int> sorted = searchArr;
        sort(sorted.begin(), sorted.end());
        cout << "  第" << k + 1 << "小 (k=" << k << "): "
             << val << " (期望 " << sorted[k] << ") "
             << (val == sorted[k] ? "✓" : "✗") << endl;
    }

    cout << endl << "[Las Vegas vs Monte Carlo]" << endl;
    cout << "  Las Vegas  : 答案必对, 时间随机 (Quicksort, Select)" << endl;
    cout << "  Monte Carlo: 时间确定, 答案可能错 (Miller-Rabin)" << endl;
    cout << "  Las Vegas  : 适合验证 < 求解 的场景" << endl;

    return 0;
}

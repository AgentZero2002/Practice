/**
 * 随机化算法 (Randomized Algorithms) — Monte Carlo vs Las Vegas
 *
 * 随机化算法分为两大类:
 *
 * 1. Monte Carlo (蒙特卡洛):
 *    - 运行时间确定, 结果可能错误 (以概率)
 *    - 错误概率可通过重复运行降低
 *    - 例: Miller-Rabin 素性测试, 随机化近似计数
 *
 * 2. Las Vegas (拉斯维加斯):
 *    - 结果总是正确, 但运行时间随机
 *    - 期望时间通常很好
 *    - 例: QuickSort (随机pivot), QuickSelect
 *
 * 关键技巧:
 *   1. Reservoir Sampling (蓄水池抽样):
 *      从未知大小的流中均匀抽样 k 个元素
 *
 *   2. Random Shuffle (Fisher-Yates):
 *      在 O(n) 时间内生成均匀随机排列
 *
 *   3. Monte Carlo 积分:
 *      用随机采样近似计算积分
 */

#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <cmath>
using namespace std;

// ==================== 1. Reservoir Sampling ====================
// 从未知总数 N 的流中均匀抽样 k 个
vector<int> reservoirSample(const vector<int>& stream, int k) {
    vector<int> reservoir(k);
    // 前 k 个直接放入
    for (int i = 0; i < k && i < (int)stream.size(); i++)
        reservoir[i] = stream[i];

    // 对于第 i 个元素 (i ≥ k), 以 k/i 的概率替换蓄水池中随机一个
    for (int i = k; i < (int)stream.size(); i++) {
        int j = rand() % (i + 1);
        if (j < k)
            reservoir[j] = stream[i];
    }
    return reservoir;
}

// ==================== 2. Fisher-Yates Shuffle ====================
void fisherYatesShuffle(vector<int>& arr) {
    int n = arr.size();
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        swap(arr[i], arr[j]);
    }
}

// ==================== 3. Monte Carlo — 估算 π ====================
double estimatePi(int numSamples) {
    int inside = 0;
    for (int i = 0; i < numSamples; i++) {
        double x = (double)rand() / RAND_MAX * 2 - 1;  // [-1, 1]
        double y = (double)rand() / RAND_MAX * 2 - 1;
        if (x*x + y*y <= 1.0) inside++;
    }
    return 4.0 * inside / numSamples;
}

// ==================== 主函数 ====================
int main() {
    srand(time(0));
    cout << "========================================" << endl;
    cout << "  随机化算法 — Monte Carlo + Las Vegas     " << endl;
    cout << "========================================" << endl << endl;

    // Reservoir Sampling
    vector<int> stream = {1,2,3,4,5,6,7,8,9,10};
    cout << "[Reservoir Sampling] 抽样 3 个: ";
    auto sample = reservoirSample(stream, 3);
    for (int x : sample) cout << x << " ";
    cout << endl;

    // Fisher-Yates
    vector<int> arr = {1,2,3,4,5,6,7,8};
    fisherYatesShuffle(arr);
    cout << "[Fisher-Yates Shuffle] ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // Monte Carlo π
    cout << "[Monte Carlo π] 10000 样本 → " << estimatePi(10000)
         << " (真值: 3.14159...)" << endl;
    cout << "  → 多次运行取平均可得更准确值" << endl;

    cout << endl << "[两类随机化算法]" << endl;
    cout << "  Monte Carlo: 结果可能错, 时间确定" << endl;
    cout << "  Las Vegas:   结果总对, 时间随机" << endl;

    return 0;
}

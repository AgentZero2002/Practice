/**
 * 基数排序 (Radix Sort)
 *
 * 对应教材 9.5 节：桶排序与基数排序（属于排序专题）
 *
 * 基数排序是一种非比较排序，按位（digit）排序。
 *
 * LSD (Least Significant Digit) 基数排序:
 *   从最低位开始，对每一位使用稳定的排序（如计数排序），
 *   依次向高位移步。
 *
 * 时间复杂度: O(d × (n + k))
 *   d = 最大位数, k = 基数 (如 10 进制则为 10)
 *
 * 空间复杂度: O(n + k)
 *
 * 特点: 稳定排序，适合整数和等长字符串
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== 计数排序（辅助基数排序） ====================
// 对 exp 位进行计数排序
void countingSortByDigit(vector<int>& A, int exp) {
    int n = A.size();
    vector<int> output(n);
    int count[10] = {0};

    // 统计当前位 (digit) 的出现次数
    for (int x : A)
        count[(x / exp) % 10]++;

    // 前缀和 → 各 digit 的末位置+1
    for (int i = 1; i < 10; i++)
        count[i] += count[i - 1];

    // 逆序遍历原数组（保证稳定性）
    for (int i = n - 1; i >= 0; i--) {
        int digit = (A[i] / exp) % 10;
        output[--count[digit]] = A[i];
    }

    A = output;
}

// ==================== LSD 基数排序 ====================
void radixSort(vector<int>& A) {
    if (A.empty()) return;

    // 找最大数以确定位数
    int maxVal = *max_element(A.begin(), A.end());

    // 从最低位到最高位依次做计数排序
    for (int exp = 1; maxVal / exp > 0; exp *= 10)
        countingSortByDigit(A, exp);
}

// ==================== 对负数的扩展 ====================
void radixSortWithNeg(vector<int>& A) {
    if (A.empty()) return;

    // 分离正负数
    vector<int> neg, pos;
    for (int x : A) {
        if (x < 0) neg.push_back(-x);  // 取相反数
        else pos.push_back(x);
    }

    radixSort(neg);
    radixSort(pos);

    // 合并：负数从大到小（即原值从小到大）
    // neg 已按绝对值从小到大排，逆转后为绝对值从大到小
    // 转回负数后即为从小到大的负数
    reverse(neg.begin(), neg.end());
    int idx = 0;
    for (int x : neg) A[idx++] = -x;
    for (int x : pos) A[idx++] = x;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  基数排序 (Radix Sort — LSD)            " << endl;
    cout << "========================================" << endl << endl;

    // 排序过程演示
    cout << "[逐步演示]" << endl;
    vector<int> arr = {170, 45, 75, 90, 802, 24, 2, 66};
    cout << "  原始: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // 按个位
    countingSortByDigit(arr, 1);
    cout << "  按个位排: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // 按十位
    countingSortByDigit(arr, 10);
    cout << "  按十位排: ";
    for (int x : arr) cout << x << " ";
    cout << endl;

    // 按百位
    countingSortByDigit(arr, 100);
    cout << "  按百位排: ";
    for (int x : arr) cout << x << " ";
    cout << " | " << (is_sorted(arr.begin(), arr.end()) ? "✓" : "✗") << endl;
    cout << endl;

    // 含负数
    cout << "[含负数的基数排序]" << endl;
    vector<int> arr2 = {-5, 3, -10, 8, -2, 0, 7, -1};
    cout << "  排序前: ";
    for (int x : arr2) cout << x << " ";
    cout << endl;
    radixSortWithNeg(arr2);
    cout << "  排序后: ";
    for (int x : arr2) cout << x << " ";
    cout << " | " << (is_sorted(arr2.begin(), arr2.end()) ? "✓" : "✗") << endl;
    cout << endl;

    cout << "[基数排序特性]" << endl;
    cout << "  时间: O(d·(n+k)), d=位数, k=基数" << endl;
    cout << "  空间: O(n+k)" << endl;
    cout << "  稳定性: ✓ 稳定 (LSD关键)" << endl;
    cout << "  优势: 非比较排序，可突破 Ω(n log n) 下界" << endl;
    cout << "  局限: 仅适用于整数和等长字符串" << endl;

    return 0;
}

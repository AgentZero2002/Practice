/**
 * 二分查找模式 (Binary Search Patterns) — 面试10分钟必写题
 *
 * 二分查找不仅是"找数字"，更是一种思想：
 *   在有序 / 部分有序 / 单调性空间中快速缩小范围。
 *
 * 经典变体:
 *   1. 搜索旋转排序数组  (LeetCode 33)
 *   2. 寻找峰值          (LeetCode 162) — 非有序但单调
 *   3. 搜索二维矩阵      (LeetCode 74) — 二维降一维
 */

#include <iostream>
#include <vector>
using namespace std;

// ==================== 1. 搜索旋转排序数组 ====================
// 旋转数组: [4,5,6,7,0,1,2], target=0 → 4
int searchRotated(const vector<int>& nums, int target) {
    int lo = 0, hi = nums.size() - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] == target) return mid;

        // 判断 mid 在左半段还是右半段
        if (nums[lo] <= nums[mid]) {  // 左半段有序
            if (nums[lo] <= target && target < nums[mid])
                hi = mid - 1;  // target 在左侧有序段
            else
                lo = mid + 1;
        } else {  // 右半段有序
            if (nums[mid] < target && target <= nums[hi])
                lo = mid + 1;  // target 在右侧有序段
            else
                hi = mid - 1;
        }
    }
    return -1;
}

// ==================== 2. 寻找峰值 ====================
// 峰值: 比左右邻居都大 nums[i] > nums[i-1] && nums[i] > nums[i+1]
// 二分的秘密: 沿着上升方向走
int findPeak(const vector<int>& nums) {
    int lo = 0, hi = nums.size() - 1;

    while (lo < hi) {
        int mid = lo + (hi - lo) / 2;
        if (nums[mid] > nums[mid + 1])
            hi = mid;     // 下降 → 峰值在左侧(含mid)
        else
            lo = mid + 1; // 上升 → 峰值在右侧
    }
    return lo;
}

// ==================== 3. 搜索二维矩阵 ====================
// 每行从左到右递增，每行第一个 > 上一行最后一个
// 方法: 视为一维有序数组
bool searchMatrix(const vector<vector<int>>& matrix, int target) {
    int m = matrix.size(), n = matrix[0].size();
    int lo = 0, hi = m * n - 1;

    while (lo <= hi) {
        int mid = lo + (hi - lo) / 2;
        int val = matrix[mid / n][mid % n];  // 一维 → 二维
        if (val == target) return true;
        if (val < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return false;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  二分查找模式 (3大经典变体)              " << endl;
    cout << "========================================" << endl << endl;

    // 1. 旋转数组
    vector<int> rotated = {4, 5, 6, 7, 0, 1, 2};
    int target = 0;
    cout << "[旋转数组搜索]" << endl;
    cout << "  数组: "; for (int x : rotated) cout << x << " ";
    cout << endl << "  target=" << target << " → 索引 "
         << searchRotated(rotated, target) << " (期望: 4)" << endl << endl;

    // 2. 峰值
    vector<int> peaks = {1, 2, 3, 1};
    int idx = findPeak(peaks);
    cout << "[寻找峰值]" << endl;
    cout << "  数组: "; for (int x : peaks) cout << x << " ";
    cout << endl << "  峰值索引: " << idx << " (值=" << peaks[idx] << ")" << endl << endl;

    // 3. 二维矩阵
    vector<vector<int>> mat = {{1, 3, 5, 7}, {10, 11, 16, 20}, {23, 30, 34, 60}};
    cout << "[搜索二维矩阵]" << endl;
    cout << "  search(3) = " << (searchMatrix(mat, 3) ? "找到" : "未找到") << endl;
    cout << "  search(13) = " << (searchMatrix(mat, 13) ? "找到" : "未找到") << endl;

    cout << endl << "[二分精髓] 每次排除一半, O(log n) 从有序空间缩小范围" << endl;
    return 0;
}

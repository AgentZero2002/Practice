/**
 * 滑动窗口最大值 (Sliding Window Maximum)
 *
 * 经典题: LeetCode 239
 *
 * 问题: 给定数组 nums 和窗口大小 k，求每个窗口的最大值。
 *
 * 解法: 单调递减队列 (Monotonic Deque)
 *   维护一个双端队列，队列中的元素从左到右递减。
 *   每个元素最多入队/出队一次 → O(n)
 *
 * 单调队列维护规则:
 *   入队: 从队尾弹出所有小于新元素的 (保持递减)
 *   出队: 若队首元素已滑出窗口，弹出
 *   窗口最大值: 队首元素
 *
 * 扩展: 单调栈也是类似思想，用于 "下一个更大元素" 问题
 */

#include <iostream>
#include <vector>
#include <deque>
using namespace std;

vector<int> maxSlidingWindow(const vector<int>& nums, int k) {
    int n = nums.size();
    if (n == 0) return {};

    vector<int> result;
    deque<int> dq;  // 存下标，按值递减

    for (int i = 0; i < n; i++) {
        // 1. 移除滑出窗口的元素
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        // 2. 维护递减性：弹出队尾所有小于当前值的
        while (!dq.empty() && nums[dq.back()] < nums[i])
            dq.pop_back();

        // 3. 当前元素入队
        dq.push_back(i);

        // 4. 当前窗口的最大值 = 队首
        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }

    return result;
}

// ==================== 最小值的变体 ====================
vector<int> minSlidingWindow(const vector<int>& nums, int k) {
    int n = nums.size();
    vector<int> result;
    deque<int> dq;

    for (int i = 0; i < n; i++) {
        if (!dq.empty() && dq.front() <= i - k)
            dq.pop_front();

        while (!dq.empty() && nums[dq.back()] > nums[i])  // 改为 >
            dq.pop_back();

        dq.push_back(i);

        if (i >= k - 1)
            result.push_back(nums[dq.front()]);
    }

    return result;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  滑动窗口最大值 (Monotonic Deque)        " << endl;
    cout << "========================================" << endl << endl;

    vector<int> nums = {1, 3, -1, -3, 5, 3, 6, 7};
    int k = 3;

    cout << "  数组: ";
    for (int x : nums) cout << x << " ";
    cout << endl << "  窗口大小 k = " << k << endl << endl;

    cout << "[每个窗口的最大值]" << endl;
    auto maxVals = maxSlidingWindow(nums, k);
    for (int i = 0; i < (int)maxVals.size(); i++)
        cout << "  窗口 [" << i << ".." << i + k - 1 << "]: max = " << maxVals[i] << endl;
    cout << "  期望: [3, 3, 5, 5, 6, 7]" << endl << endl;

    cout << "[每个窗口的最小值]" << endl;
    auto minVals = minSlidingWindow(nums, k);
    for (int i = 0; i < (int)minVals.size(); i++)
        cout << "  窗口 [" << i << ".." << i + k - 1 << "]: min = " << minVals[i] << endl;

    cout << endl << "[复杂度]" << endl;
    cout << "  时间: O(n), 每个元素至多入队/出队一次" << endl;
    cout << "  空间: O(k), 双端队列大小" << endl;
    cout << endl;
    cout << "[相关题型]" << endl;
    cout << "  · 下一个更大元素 (单调栈)" << endl;
    cout << "  · 滑动窗口中位数 (双堆)" << endl;
    cout << "  · 最大连续 1 的个数 III" << endl;

    return 0;
}

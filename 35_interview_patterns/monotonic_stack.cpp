/**
 * 单调栈 (Monotonic Stack) — 大厂面试最高频题型之一
 *
 * 单调栈维护一个单调递增（或递减）的元素序列，
 * 用于解决"下一个更大/更小元素"类问题。
 *
 * 核心思想: 遍历时，栈中保持单调性；不满足单调性的出栈，
 *           出栈时即可获得"下一个更X元素"的信息。
 *
 * 经典题型:
 *   1. 下一个更大元素 I   (LeetCode 496)
 *   2. 每日温度           (LeetCode 739)
 *   3. 接雨水             (LeetCode 42) ★最经典
 *   4. 柱状图中最大的矩形 (LeetCode 84) ★Hard
 */

#include <iostream>
#include <vector>
#include <stack>
#include <algorithm>
using namespace std;

// ==================== 1. 下一个更大元素 ====================
// 返回每个元素右侧第一个更大的元素 (无则为 -1)
vector<int> nextGreaterElement(const vector<int>& nums) {
    int n = nums.size();
    vector<int> result(n, -1);
    stack<int> st;  // 单调递减栈 (存下标)

    for (int i = 0; i < n; i++) {
        while (!st.empty() && nums[st.top()] < nums[i]) {
            result[st.top()] = nums[i];
            st.pop();
        }
        st.push(i);
    }
    return result;
}

// ==================== 2. 每日温度 ====================
// 返回: 距下一次更高温度的天数间隔
vector<int> dailyTemperatures(const vector<int>& temps) {
    int n = temps.size();
    vector<int> result(n, 0);
    stack<int> st;  // 单调递减栈

    for (int i = 0; i < n; i++) {
        while (!st.empty() && temps[st.top()] < temps[i]) {
            result[st.top()] = i - st.top();
            st.pop();
        }
        st.push(i);
    }
    return result;
}

// ==================== 3. 接雨水 ★ ====================
// 给定柱子高度，计算能接多少雨水
int trapRain(const vector<int>& height) {
    int n = height.size();
    if (n < 3) return 0;

    stack<int> st;  // 单调递减栈
    int water = 0;

    for (int i = 0; i < n; i++) {
        while (!st.empty() && height[st.top()] < height[i]) {
            int bottom = st.top(); st.pop();
            if (st.empty()) break;
            int left = st.top();
            int w = i - left - 1;
            int h = min(height[left], height[i]) - height[bottom];
            water += w * h;
        }
        st.push(i);
    }
    return water;
}

// ==================== 4. 柱状图中最大的矩形 ★ ====================
int largestRectangleArea(const vector<int>& heights) {
    int n = heights.size();
    stack<int> st;  // 单调递增栈
    int maxArea = 0;

    for (int i = 0; i <= n; i++) {
        int h = (i == n ? 0 : heights[i]);  // 末尾哨兵
        while (!st.empty() && heights[st.top()] > h) {
            int height = heights[st.top()]; st.pop();
            int width = st.empty() ? i : i - st.top() - 1;
            maxArea = max(maxArea, height * width);
        }
        st.push(i);
    }
    return maxArea;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  单调栈 (Monotonic Stack)               " << endl;
    cout << "  — 四大经典面试题                       " << endl;
    cout << "========================================" << endl << endl;

    // 1. 下一个更大元素
    vector<int> nums = {2, 1, 2, 4, 3};
    cout << "[下一个更大元素]" << endl;
    cout << "  输入: 2 1 2 4 3" << endl;
    auto nge = nextGreaterElement(nums);
    cout << "  输出: "; for (int x : nge) cout << x << " "; cout << endl << endl;

    // 2. 每日温度
    vector<int> temps = {73, 74, 75, 71, 69, 72, 76, 73};
    cout << "[每日温度]" << endl;
    cout << "  输入: 73 74 75 71 69 72 76 73" << endl;
    auto days = dailyTemperatures(temps);
    cout << "  输出: "; for (int x : days) cout << x << " "; cout << " (等待天数)" << endl << endl;

    // 3. 接雨水
    vector<int> bars = {0, 1, 0, 2, 1, 0, 1, 3, 2, 1, 2, 1};
    cout << "[接雨水]" << endl;
    cout << "  输入: 0 1 0 2 1 0 1 3 2 1 2 1" << endl;
    cout << "  雨水总量: " << trapRain(bars) << " (期望: 6)" << endl << endl;

    // 4. 最大矩形
    vector<int> rect = {2, 1, 5, 6, 2, 3};
    cout << "[柱状图最大矩形]" << endl;
    cout << "  输入: 2 1 5 6 2 3" << endl;
    cout << "  最大面积: " << largestRectangleArea(rect) << " (期望: 10)" << endl;
    cout << endl;
    cout << "[刷题建议] 单调栈是 LeetCode Hard 题的核心技巧之一," << endl;
    cout << "  掌握后可在 10 分钟内写出接雨水/最大矩形的正确解。" << endl;

    return 0;
}

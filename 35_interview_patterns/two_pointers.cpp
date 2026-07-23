/**
 * 双指针 (Two Pointers) — 面试核心技巧
 *
 * 双指针通过两个指针协同移动，将 O(n²) 降为 O(n)。
 *
 * 经典模式:
 *   1. 对撞指针: 左右指针向中间移动 — 有序数组
 *   2. 快慢指针: 一快一慢 — 链表、去重
 *   3. 滑动窗口: 两个边界指针 — 子数组/子串
 *
 * 本文件覆盖:
 *   1. 盛最多水的容器     (LeetCode 11)
 *   2. 三数之和           (LeetCode 15) ★最经典
 *   3. 删除有序数组重复项 (LeetCode 26)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

// ==================== 1. 盛最多水的容器 ★ ====================
// 对撞指针: 每次移动较短的边
int maxArea(const vector<int>& height) {
    int left = 0, right = height.size() - 1;
    int maxWater = 0;

    while (left < right) {
        int water = min(height[left], height[right]) * (right - left);
        maxWater = max(maxWater, water);

        // 核心: 移动较短的那条边 (只有它可能增加面积)
        if (height[left] < height[right])
            left++;
        else
            right--;
    }
    return maxWater;
}

// ==================== 2. 三数之和 ====================
// 排序 + 固定一个数 + 对撞指针找另外两个
vector<vector<int>> threeSum(vector<int>& nums) {
    vector<vector<int>> result;
    sort(nums.begin(), nums.end());
    int n = nums.size();

    for (int i = 0; i < n - 2; i++) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;  // 跳过重复

        int left = i + 1, right = n - 1;
        while (left < right) {
            int sum = nums[i] + nums[left] + nums[right];
            if (sum == 0) {
                result.push_back({nums[i], nums[left], nums[right]});
                // 跳过重复的 left 和 right
                while (left < right && nums[left] == nums[left + 1]) left++;
                while (left < right && nums[right] == nums[right - 1]) right--;
                left++; right--;
            } else if (sum < 0) {
                left++;
            } else {
                right--;
            }
        }
    }
    return result;
}

// ==================== 3. 删除有序数组中的重复项 ====================
// 快慢指针: slow 指向已去重部分的末尾, fast 探索新元素
int removeDuplicates(vector<int>& nums) {
    if (nums.empty()) return 0;
    int slow = 0;
    for (int fast = 1; fast < (int)nums.size(); fast++) {
        if (nums[fast] != nums[slow])
            nums[++slow] = nums[fast];
    }
    return slow + 1;  // 去重后长度
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  双指针 (Two Pointers)                  " << endl;
    cout << "========================================" << endl << endl;

    // 1. 盛水
    vector<int> h = {1, 8, 6, 2, 5, 4, 8, 3, 7};
    cout << "[盛最多水的容器]" << endl;
    cout << "  高度: "; for (int x : h) cout << x << " ";
    cout << endl << "  最大容量: " << maxArea(h) << " (期望: 49)" << endl << endl;

    // 2. 三数之和
    vector<int> nums = {-1, 0, 1, 2, -1, -4};
    cout << "[三数之和 = 0]" << endl;
    cout << "  输入: "; for (int x : nums) cout << x << " ";
    auto triples = threeSum(nums);
    cout << endl << "  结果: ";
    for (auto& t : triples) {
        cout << "[";
        for (size_t i = 0; i < t.size(); i++) {
            if (i > 0) cout << ",";
            cout << t[i];
        }
        cout << "] ";
    }
    cout << endl << endl;

    // 3. 去重
    vector<int> arr = {0, 0, 1, 1, 1, 2, 2, 3, 3, 4};
    cout << "[删除有序数组重复项]" << endl;
    cout << "  原数组: "; for (int x : arr) cout << x << " ";
    int newLen = removeDuplicates(arr);
    cout << endl << "  去重后 (" << newLen << " 项): ";
    for (int i = 0; i < newLen; i++) cout << arr[i] << " ";
    cout << endl;

    cout << endl << "[双指针模式速查]" << endl;
    cout << "  对撞:  盛水、两数之和II、回文验证" << endl;
    cout << "  快慢:  链表环检测、原地去重" << endl;
    cout << "  滑动:  见 substring_sliding.cpp" << endl;

    return 0;
}

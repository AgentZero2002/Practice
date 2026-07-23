/**
 * 回溯算法模板 (Backtracking Template)
 *
 * 回溯 = 深度优先搜索 + 剪枝
 * 用于求解排列、组合、子集 等 "全部可行解" 问题。
 *
 * 统一模板:
 *   void backtrack(路径, 选择列表) {
 *       if (满足结束条件) { 加入结果; return; }
 *       for (选择 in 选择列表) {
 *           做选择;
 *           backtrack(路径, 新选择列表);
 *           撤销选择;  // ← 回溯的精髓
 *       }
 *   }
 *
 * 经典题:
 *   1. 全排列   (LeetCode 46)
 *   2. 子集     (LeetCode 78)
 *   3. 组合总和 (LeetCode 39)
 */

#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;

class Backtracking {
public:
    // ==================== 1. 全排列 ====================
    vector<vector<int>> permute(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> path;
        vector<bool> used(nums.size(), false);
        backtrackPermute(nums, used, path, result);
        return result;
    }

    // ==================== 2. 子集 ====================
    vector<vector<int>> subsets(vector<int>& nums) {
        vector<vector<int>> result;
        vector<int> path;
        backtrackSubsets(nums, 0, path, result);
        return result;
    }

    // ==================== 3. 组合总和 ====================
    // 从 candidates 中选数，每个数可用无限次，和为 target
    vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
        vector<vector<int>> result;
        vector<int> path;
        backtrackCombSum(candidates, target, 0, path, result);
        return result;
    }

private:
    void backtrackPermute(vector<int>& nums, vector<bool>& used,
                          vector<int>& path, vector<vector<int>>& result) {
        if (path.size() == nums.size()) {
            result.push_back(path);
            return;
        }

        for (int i = 0; i < (int)nums.size(); i++) {
            if (used[i]) continue;       // 剪枝: 已使用
            used[i] = true;
            path.push_back(nums[i]);
            backtrackPermute(nums, used, path, result);
            path.pop_back();             // 撤销
            used[i] = false;
        }
    }

    void backtrackSubsets(vector<int>& nums, int start,
                          vector<int>& path, vector<vector<int>>& result) {
        result.push_back(path);  // 每个状态都是一个子集

        for (int i = start; i < (int)nums.size(); i++) {
            path.push_back(nums[i]);
            backtrackSubsets(nums, i + 1, path, result);  // i+1 避免重复
            path.pop_back();
        }
    }

    void backtrackCombSum(vector<int>& cand, int remain, int start,
                          vector<int>& path, vector<vector<int>>& result) {
        if (remain == 0) {
            result.push_back(path);
            return;
        }
        if (remain < 0) return;  // 剪枝

        for (int i = start; i < (int)cand.size(); i++) {
            path.push_back(cand[i]);
            backtrackCombSum(cand, remain - cand[i], i, path, result);  // 注意 i 不是 i+1
            path.pop_back();
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  回溯算法模板 (Backtracking)            " << endl;
    cout << "========================================" << endl << endl;

    Backtracking bt;

    // 1. 全排列
    vector<int> nums1 = {1, 2, 3};
    cout << "[全排列] {1,2,3}:" << endl;
    auto perms = bt.permute(nums1);
    cout << "  共 " << perms.size() << " 种 (期望: 6)" << endl;
    for (auto& p : perms) {
        cout << "  ";
        for (int x : p) cout << x << " ";
        cout << endl;
    }
    cout << endl;

    // 2. 子集
    vector<int> nums2 = {1, 2, 3};
    auto subs = bt.subsets(nums2);
    cout << "[子集] {1,2,3}: 共 " << subs.size() << " 种 (期望: 8=2³)" << endl;

    // 3. 组合总和
    vector<int> cand = {2, 3, 6, 7};
    auto combs = bt.combinationSum(cand, 7);
    cout << endl << "[组合总和] candidates={2,3,6,7}, target=7:" << endl;
    for (auto& c : combs) {
        cout << "  ";
        for (int x : c) cout << x << " ";
        cout << endl;
    }

    cout << endl << "[回溯四要素]" << endl;
    cout << "  1. 路径:  当前已选中的元素" << endl;
    cout << "  2. 选择列表: 下一步可选的元素" << endl;
    cout << "  3. 结束条件: 何时记录结果" << endl;
    cout << "  4. 撤销操作: 恢复状态以尝试其他选择 ← 回溯精髓" << endl;

    return 0;
}

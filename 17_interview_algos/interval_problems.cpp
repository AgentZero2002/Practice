/**
 * 区间问题 (Interval Problems) — 面试核心专题
 *
 * 区间类问题在排序后通常变得简单。
 *
 * 经典题:
 *   1. 合并区间            (LeetCode 56)
 *   2. 插入区间            (LeetCode 57)
 *   3. 会议室 II           (LeetCode 253) ★扫描线经典
 *   4. 用最少箭射爆气球     (LeetCode 452) — 区间选点
 */

#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>
using namespace std;

// ==================== 1. 合并区间 ====================
vector<vector<int>> mergeIntervals(vector<vector<int>>& intervals) {
    if (intervals.empty()) return {};
    // 按起点排序
    sort(intervals.begin(), intervals.end());

    vector<vector<int>> merged;
    merged.push_back(intervals[0]);

    for (int i = 1; i < (int)intervals.size(); i++) {
        auto& last = merged.back();
        if (intervals[i][0] <= last[1]) {
            last[1] = max(last[1], intervals[i][1]);  // 扩展
        } else {
            merged.push_back(intervals[i]);  // 新区间
        }
    }
    return merged;
}

// ==================== 2. 插入区间 ====================
// 在有序的不重叠区间列表中插入新区间
vector<vector<int>> insertInterval(vector<vector<int>>& intervals,
                                    vector<int>& newInterval) {
    vector<vector<int>> result;
    int i = 0, n = intervals.size();

    // 左侧：在新区间之前的全部放入
    while (i < n && intervals[i][1] < newInterval[0])
        result.push_back(intervals[i++]);

    // 中间：合并重叠
    while (i < n && intervals[i][0] <= newInterval[1]) {
        newInterval[0] = min(newInterval[0], intervals[i][0]);
        newInterval[1] = max(newInterval[1], intervals[i][1]);
        i++;
    }
    result.push_back(newInterval);

    // 右侧：剩余
    while (i < n) result.push_back(intervals[i++]);

    return result;
}

// ==================== 3. 会议室 II (最少会议室数) ====================
// 扫描线: 起点+1, 终点-1 → 求最大重叠数
int minMeetingRooms(vector<vector<int>>& intervals) {
    vector<pair<int, int>> events;  // {时间, 类型}  +1=开始, -1=结束
    for (auto& inv : intervals) {
        events.push_back({inv[0], 1});
        events.push_back({inv[1], -1});
    }
    sort(events.begin(), events.end());

    int rooms = 0, maxRooms = 0;
    for (auto& [time, delta] : events) {
        rooms += delta;
        maxRooms = max(maxRooms, rooms);
    }
    return maxRooms;
}

// ==================== 4. 射气球 (最少箭数) ====================
int findMinArrowShots(vector<vector<int>>& points) {
    if (points.empty()) return 0;
    // 按终点排序
    sort(points.begin(), points.end(),
         [](auto& a, auto& b) { return a[1] < b[1]; });

    int arrows = 1;
    int arrowPos = points[0][1];

    for (auto& p : points) {
        if (p[0] > arrowPos) {  // 当前位置射不到此气球
            arrows++;
            arrowPos = p[1];
        }
    }
    return arrows;
}

// ==================== 辅助打印 ====================
void printIntervals(const vector<vector<int>>& invs) {
    for (auto& inv : invs)
        cout << "[" << inv[0] << "," << inv[1] << "] ";
    cout << endl;
}

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  区间问题 (Interval Problems)           " << endl;
    cout << "========================================" << endl << endl;

    // 1. 合并
    vector<vector<int>> inv1 = {{1,3},{2,6},{8,10},{15,18}};
    auto merged = mergeIntervals(inv1);
    cout << "[合并区间] "; printIntervals(inv1);
    cout << "  → "; printIntervals(merged); cout << endl;

    // 2. 插入
    vector<vector<int>> inv2 = {{1,3},{6,9}};
    vector<int> newInv = {2, 5};
    auto inserted = insertInterval(inv2, newInv);
    cout << "[插入区间] "; printIntervals(inv2);
    cout << "  插入 [2,5] → "; printIntervals(inserted); cout << endl;

    // 3. 会议室
    vector<vector<int>> meetings = {{0,30},{5,10},{15,20}};
    cout << "[最少会议室] 需要 " << minMeetingRooms(meetings)
         << " 间 (期望: 2)" << endl;

    // 4. 射气球
    vector<vector<int>> balloons = {{10,16},{2,8},{1,6},{7,12}};
    cout << "[最少箭] 需要 " << findMinArrowShots(balloons)
         << " 支 (期望: 2)" << endl;

    return 0;
}

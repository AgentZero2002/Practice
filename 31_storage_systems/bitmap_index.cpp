/**
 * 位图索引 (Bitmap Index) — OLAP 列存加速
 *
 * 位图索引是为低基数 (low cardinality) 列设计的索引结构,
 * 广泛应用于 OLAP (在线分析处理) 数据库和数据仓库。
 *
 * 核心思想:
 *   对列的每个可能值, 创建一个位图 (bitmap),
 *   位图的第 i 位表示第 i 行是否具有该值。
 *
 * 示例:
 *   性别列: 男/女
 *    Bitmap(男) = 10110...  (第1,3,4行的性别=男)
 *    Bitmap(女) = 01001...  (第2,5行的性别=女)
 *
 * 查询:
 *   WHERE gender='女' AND city='北京'
 *   = Bitmap(女) & Bitmap(北京)    ← 位运算, 极快!
 *
 * 优点:
 *   - 查询速度极快 (位运算, 硬件加速)
 *   - 多个 AND/OR/NOT 组合直接对应位操作
 *   - 压缩率高 (对低基数列)
 *
 * 缺点:
 *   - 高基数列 (如 ID) 位图稀疏, 效率低
 *   - 更新开销大 (需要更新多个位图)
 *
 * 压缩技术:
 *   - WAH (Word-Aligned Hybrid)
 *   - EWAH (Enhanced WAH)
 *   - Roaring Bitmap (广泛用于 Lucene, Spark, Druid)
 *     分层: 高 16 位 → Container, 低 16 位 → Bitmap/Array/RLE
 *
 * 典型应用:
 *   - Druid / ClickHouse / Apache Kylin 的过滤加速
 *   - PostgreSQL Bitmap Index Scan
 *   - Oracle Bitmap Index
 */

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <map>
using namespace std;

const int MAX_ROWS = 16;

class BitmapIndex {
private:
    // column → value → bitset
    map<string, map<string, bitset<MAX_ROWS>>> index;
    int nextRowID = 0;

public:
    // 插入一行
    void insert(const map<string, string>& row) {
        for (auto& [col, val] : row) {
            index[col][val].set(nextRowID);
        }
        nextRowID++;
    }

    // 单条件查询
    bitset<MAX_ROWS> query(const string& col, const string& val) {
        if (index.count(col) && index[col].count(val))
            return index[col][val];
        return bitset<MAX_ROWS>();
    }

    // AND 查询
    bitset<MAX_ROWS> andQuery(const vector<pair<string,string>>& conditions) {
        bitset<MAX_ROWS> result;
        result.set();  // 全 1
        for (auto& [col, val] : conditions) {
            result &= query(col, val);
        }
        return result;
    }

    // OR 查询
    bitset<MAX_ROWS> orQuery(const vector<pair<string,string>>& conditions) {
        bitset<MAX_ROWS> result;
        for (auto& [col, val] : conditions) {
            result |= query(col, val);
        }
        return result;
    }

    // 打印行号
    string bitsetToRows(const bitset<MAX_ROWS>& bs) {
        string s;
        for (int i = 0; i < MAX_ROWS && i < nextRowID; i++)
            if (bs[i]) s += to_string(i) + " ";
        return s.empty() ? "(空)" : s;
    }

    void printStats() {
        int totalBitmaps = 0;
        for (auto& [col, vals] : index) totalBitmaps += vals.size();
        cout << "  列数: " << index.size()
             << ", 位图总数: " << totalBitmaps << endl;
    }

    void printAll() {
        for (auto& [col, vals] : index) {
            cout << "  [" << col << "]" << endl;
            for (auto& [val, bs] : vals) {
                cout << "    \"" << val << "\" → " << bs.to_string().substr(MAX_ROWS - nextRowID) << endl;
            }
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  位图索引 (Bitmap Index)                 " << endl;
    cout << "========================================" << endl << endl;

    BitmapIndex bmi;

    cout << "[数据加载]" << endl;
    bmi.insert({{"city", "北京"}, {"gender", "男"}, {"vip", "是"}});
    bmi.insert({{"city", "上海"}, {"gender", "女"}, {"vip", "否"}});
    bmi.insert({{"city", "北京"}, {"gender", "女"}, {"vip", "是"}});
    bmi.insert({{"city", "深圳"}, {"gender", "男"}, {"vip", "否"}});
    bmi.insert({{"city", "北京"}, {"gender", "男"}, {"vip", "否"}});

    bmi.printStats();
    bmi.printAll();
    cout << endl;

    cout << "[查询] city=北京" << endl;
    auto r = bmi.query("city", "北京");
    cout << "  → rows: " << bmi.bitsetToRows(r) << endl;

    cout << endl << "[AND 查询] city=北京 AND vip=是" << endl;
    r = bmi.andQuery({{"city", "北京"}, {"vip", "是"}});
    cout << "  → rows: " << bmi.bitsetToRows(r) << " (期望: 0,2)" << endl;

    cout << endl << "[OR 查询] gender=女 OR city=深圳" << endl;
    r = bmi.orQuery({{"gender", "女"}, {"city", "深圳"}});
    cout << "  → rows: " << bmi.bitsetToRows(r) << " (期望: 1,2,3)" << endl;

    cout << endl << "[位图索引优劣]" << endl;
    cout << "  优点: AND/OR/NOT → 位运算, 极快" << endl;
    cout << "  局限: 高基数列位图稀疏, 需 Roaring Bitmap 压缩" << endl;
    cout << "  应用: ClickHouse, Druid, Spark 等 OLAP 引擎" << endl;

    return 0;
}

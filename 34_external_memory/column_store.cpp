/**
 * 列式存储 (Column-Oriented Storage)
 *
 * 传统数据库用行式存储 (Row Store): 一行所有列连续存放。
 * 列式存储则将每列分开放: 表 → 每个列独立存储。
 *
 * 行式存储 (N-ary Storage Model, NSM):
 *   Row 0: [id=1, name="Alice", age=30, salary=50000]
 *   Row 1: [id=2, name="Bob",   age=25, salary=60000]
 *   磁盘布局: [1,"Alice",30,50000, 2,"Bob",25,60000, ...]
 *
 * 列式存储 (Decomposition Storage Model, DSM):
 *   Col id:   [1,    2,    ...]
 *   Col name: ["Alice","Bob",...]
 *   Col age:  [30,   25,   ...]
 *   Col salary:[50000,60000,...]
 *   磁盘布局: 每列独立存储, 压缩
 *
 * 列式存储的优势:
 *   1. 查询只读需要的列 (OLAP 查询通常只需 2-3 列)
 *      → 减少 I/O: 读 1/100 列 = 省 99% I/O
 *   2. 更好的压缩: 同列数据同类型, 值范围集中
 *      → RLE (Run-Length Encoding), Delta, Dictionary 编码
 *   3. 向量化执行: SIMD 一次处理多个值
 *      → "列处理" (column-at-a-time) vs "行处理" (row-at-a-time)
 *   4. 适合聚合: SUM(price), AVG(age) 直接扫描列数组
 *
 * 列式存储的劣势:
 *   1. 单行查询慢: 需要读所有列并拼接
 *   2. 更新/插入慢: 需要修改多个独立列文件
 *   3. 事务处理 (OLTP) 性能差
 *
 * 混合存储 (PAX / Hybrid):
 *   每页内按列组织 (Partition Attributes Across)
 *   行式的大粒度 (page), 列式的小粒度 (page内)
 *
 * 主流列式数据库:
 *   - ClickHouse, Vertica, MonetDB, Apache Parquet (文件格式)
 *   - BigQuery, Redshift (云数仓)
 */

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <unordered_map>
using namespace std;

// ==================== 简化的列式存储表 ====================
class ColumnStore {
    // 列存: 每列一个 vector
    vector<int> col_id;
    vector<string> col_name;
    vector<int> col_age;
    vector<double> col_salary;

public:
    void insert(int id, const string& name, int age, double salary) {
        col_id.push_back(id);
        col_name.push_back(name);
        col_age.push_back(age);
        col_salary.push_back(salary);
    }

    int rowCount() const { return col_id.size(); }

    // OLAP 查询: SELECT AVG(salary) WHERE age > 25
    // 只读 age 和 salary 两列, 忽略 id 和 name
    double avgSalaryByAge(int minAge) const {
        double sum = 0.0;
        int count = 0;
        for (int i = 0; i < (int)col_age.size(); i++) {
            if (col_age[i] > minAge) {
                sum += col_salary[i];
                count++;
            }
        }
        return count > 0 ? sum / count : 0.0;
    }

    // 分组聚合: SELECT dept, COUNT(*), SUM(salary)
    // (简化: 用年龄分组)
    void groupByAge() const {
        cout << "[GROUP BY age]" << endl;
        // 收集所有年龄→汇总
        unordered_map<int, pair<int,double>> groups;  // age → {count, sum_salary}
        for (int i = 0; i < (int)col_age.size(); i++) {
            int age = col_age[i];
            groups[age].first++;
            groups[age].second += col_salary[i];
        }
        cout << "  Age   Count   AvgSalary" << endl;
        for (auto& [age, data] : groups) {
            cout << "  " << setw(3) << age
                 << "   " << setw(4) << data.first
                 << "   " << fixed << setprecision(0) << data.second / data.first
                 << endl;
        }
    }

    // 列压缩: Dictionary Encoding for name column
    void showCompression() const {
        cout << "[列压缩分析]" << endl;
        cout << "  id:     " << col_id.size() << " ints → "
             << col_id.size() * 4 << " bytes" << endl;
        cout << "  name:   " << col_name.size() << " strings → "
             << "可为" << col_name.size() * 1 << " bytes (字典编码)" << endl;
        cout << "  age:    " << col_age.size() << " ints → "
             << col_age.size() * 1 << " bytes (8-bit δ编码)" << endl;
        cout << "  salary: " << col_salary.size() << " doubles → "
             << "适合位压缩或δ编码" << endl;
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  列式存储 (Column Store)                  " << endl;
    cout << "========================================" << endl << endl;

    ColumnStore cs;

    // 模拟员工表
    cs.insert(1, "Alice",   30, 50000);
    cs.insert(2, "Bob",     25, 60000);
    cs.insert(3, "Charlie", 35, 75000);
    cs.insert(4, "Diana",   28, 55000);
    cs.insert(5, "Eve",     30, 62000);
    cs.insert(6, "Frank",   40, 90000);
    cs.insert(7, "Grace",   25, 52000);
    cs.insert(8, "Henry",   35, 80000);

    cout << "[表] Employees (" << cs.rowCount() << " rows)" << endl;

    // OLAP 查询
    double avg = cs.avgSalaryByAge(25);
    cout << endl << "[查询] AVG(salary) WHERE age > 25" << endl;
    cout << "  结果: " << avg << endl;
    cout << "  只读了 age 和 salary 列 (I/O 减半)" << endl;

    // 分组聚合
    cout << endl;
    cs.groupByAge();

    // 压缩
    cout << endl;
    cs.showCompression();

    cout << endl << "[行式 vs 列式]" << endl;
    cout << "  OLTP (INSERT/UPDATE/DELETE 单行): 行式胜" << endl;
    cout << "  OLAP (SELECT 几列的聚合): 列式胜" << endl;
    cout << "  压缩比: 列式 >> 行式 (同类型, 窄范围)" << endl;
    cout << "  格式: Parquet, ORC, Arrow (现代列式文件格式)" << endl;

    return 0;
}

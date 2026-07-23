/**
 * 倒排索引 (Inverted Index) — 搜索引擎核心
 *
 * 倒排索引是全文搜索引擎 (Elasticsearch, Lucene, Google) 的
 * 底层数据结构。它建立"词 → 文档列表"的映射, 实现亚秒级全文搜索。
 *
 * 正排索引 (Forward Index): docID → 该文档包含的所有词
 *   文档1: "hello world"
 *   文档2: "hello lucene"
 *
 * 倒排索引 (Inverted Index): term → 包含该词的文档ID列表
 *   "hello"  → [1, 2]
 *   "world"  → [1]
 *   "lucene" → [2]
 *
 * 核心数据结构:
 *   1. Term Dictionary (词典): 有序词表, 可用 B-Tree / Trie / FST 存储
 *   2. Posting List (倒排列表): 每个词对应的文档ID列表
 *      - 通常压缩为差值 (delta encoding) + 变长编码 (vInt/Simple9/PForDelta)
 *   3. 位置信息 (可选): 词在文档中的具体位置, 用于短语查询
 *
 * 查询处理:
 *   单个词: 直接查词典 → 返回 Posting List
 *   AND 查询: 两个 Posting List 取交集 (多路归并)
 *   OR 查询:  两个 Posting List 取并集
 *   NOT 查询: 集合差
 *
 * 优化:
 *   - Skip List 加速 AND 查询
 *   - BitSet 加速多条件组合
 *   - 分片 (Sharding): 多台机器各管一部分索引
 */

#include <iostream>
#include <vector>
#include <map>
#include <set>
#include <string>
#include <algorithm>
#include <sstream>
using namespace std;

class InvertedIndex {
private:
    // term → set of docIDs (实际系统用排序列表 + 压缩)
    map<string, set<int>> index;

public:
    // 添加文档
    void addDocument(int docId, const string& text) {
        istringstream iss(text);
        string word;
        map<string, bool> seen;  // 同一文档去重
        while (iss >> word) {
            // 转小写 (简化)
            transform(word.begin(), word.end(), word.begin(), ::tolower);
            if (!seen[word]) {
                index[word].insert(docId);
                seen[word] = true;
            }
        }
    }

    // 单词查询
    set<int> search(const string& term) {
        string lower = term;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        if (index.count(lower))
            return index[lower];
        return {};
    }

    // AND 查询 (交集)
    set<int> andQuery(const string& t1, const string& t2) {
        set<int> r1 = search(t1), r2 = search(t2);
        set<int> result;
        set_intersection(r1.begin(), r1.end(), r2.begin(), r2.end(),
                         inserter(result, result.begin()));
        return result;
    }

    // OR 查询 (并集)
    set<int> orQuery(const string& t1, const string& t2) {
        set<int> r1 = search(t1), r2 = search(t2);
        set<int> result = r1;
        result.insert(r2.begin(), r2.end());
        return result;
    }

    void printStats() {
        cout << "  倒排词典大小: " << index.size() << " 个词" << endl;
        int totalPostings = 0;
        for (auto& [term, docs] : index) totalPostings += docs.size();
        cout << "  总倒排条目: " << totalPostings << " 条" << endl;
    }

    void printIndex() {
        cout << endl << "[倒排索引]" << endl;
        for (auto& [term, docs] : index) {
            cout << "  \"" << term << "\" → doc[";
            for (int d : docs) cout << d << " ";
            cout << "]" << endl;
        }
    }
};

// ==================== 主函数 ====================
int main() {
    cout << "========================================" << endl;
    cout << "  倒排索引 (Inverted Index)               " << endl;
    cout << "========================================" << endl << endl;

    InvertedIndex idx;

    cout << "[索引构建]" << endl;
    idx.addDocument(1, "Hello World Algorithm");
    idx.addDocument(2, "Hello Data Structure");
    idx.addDocument(3, "World of Algorithm Design");
    idx.addDocument(4, "Data Mining and Machine Learning");

    idx.printStats();
    idx.printIndex();
    cout << endl;

    cout << "[查询]" << endl;
    auto r = idx.search("hello");
    cout << "  \"hello\" → doc["; for (int d : r) cout << d << " "; cout << "]" << endl;

    r = idx.search("algorithm");
    cout << "  \"algorithm\" → doc["; for (int d : r) cout << d << " "; cout << "]" << endl;

    cout << endl << "[AND 查询] \"hello\" AND \"algorithm\"" << endl;
    r = idx.andQuery("hello", "algorithm");
    cout << "  → doc["; for (int d : r) cout << d << " "; cout << "] (期望: 1)" << endl;

    cout << endl << "[OR 查询] \"hello\" OR \"data\"" << endl;
    r = idx.orQuery("hello", "data");
    cout << "  → doc["; for (int d : r) cout << d << " "; cout << "] (期望: 1,2,4)" << endl;

    cout << endl << "[倒排索引关键优化]" << endl;
    cout << "  - Delta Encoding: Posting List 存储差值减少空间" << endl;
    cout << "  - Skip List: 加速 Posting List 的 AND 归并" << endl;
    cout << "  - FST (Finite State Transducer): 词典压缩" << endl;
    cout << "  - 分片: 水平切分倒排索引到多台机器" << endl;

    return 0;
}

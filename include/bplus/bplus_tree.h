//
// B+树索引实现
// 用于优化查询性能
//

#pragma once
#include "../catalog/types.h"
#include "../storage/async_aliases.h"
#include "../log/component_loggers.h"
#include <vector>
#include <memory>
#include <unordered_map>

// B+树节点类型
enum class BPlusNodeType {
    LEAF,    // 叶子节点
    INTERNAL // 内部节点
};

// B+树节点基类
class BPlusNode {
public:
    BPlusNodeType type_;
    int page_id_;
    bool is_dirty_;
    
    BPlusNode(BPlusNodeType type, int page_id) 
        : type_(type), page_id_(page_id), is_dirty_(false) {}
    virtual ~BPlusNode() = default;
    
    virtual bool IsLeaf() const = 0;
    virtual int GetKeyCount() const = 0;
    virtual bool IsFull(int max_keys) const = 0;
    virtual bool IsUnderflow(int max_keys) const = 0;
};

// 叶子节点：存储实际的键值对
class BPlusLeafNode : public BPlusNode {
public:
    std::vector<Value> keys_;           // 键值
    std::vector<int> record_ids_;       // 对应的记录ID
    int next_leaf_page_id_;             // 下一个叶子节点页ID
    int prev_leaf_page_id_;             // 上一个叶子节点页ID
    
    BPlusLeafNode(int page_id) 
        : BPlusNode(BPlusNodeType::LEAF, page_id), 
          next_leaf_page_id_(-1), prev_leaf_page_id_(-1) {}
    
    bool IsLeaf() const override { return true; }
    int GetKeyCount() const override { return static_cast<int>(keys_.size()); }
    bool IsFull(int max_keys) const override { return GetKeyCount() >= max_keys; }
    bool IsUnderflow(int max_keys) const override { return GetKeyCount() < (max_keys + 1) / 2; }
    
    // 插入键值对
    bool Insert(const Value& key, int record_id);
    // 删除键值对
    bool Remove(const Value& key);
    // 查找键值对
    int Find(const Value& key) const;
    // 范围查找
    std::vector<int> FindRange(const Value& start_key, const Value& end_key) const;
};

// 内部节点：存储键和子节点指针
class BPlusInternalNode : public BPlusNode {
public:
    std::vector<Value> keys_;           // 键值
    std::vector<int> child_page_ids_;   // 子节点页ID
    
    BPlusInternalNode(int page_id) 
        : BPlusNode(BPlusNodeType::INTERNAL, page_id) {}
    
    bool IsLeaf() const override { return false; }
    int GetKeyCount() const override { return static_cast<int>(keys_.size()); }
    bool IsFull(int max_keys) const override { return GetKeyCount() >= max_keys; }
    bool IsUnderflow(int max_keys) const override { return GetKeyCount() < (max_keys + 1) / 2; }
    
    // 插入键和子节点
    bool Insert(const Value& key, int child_page_id);
    // 删除键和子节点
    bool Remove(const Value& key);
    // 查找子节点
    int FindChild(const Value& key) const;
};

// B+树索引
class BPlusTreeIndex {
public:
    BPlusTreeIndex(BufferPoolManager* bpm, const std::string& table_name, 
                   const std::string& column_name, int column_index);
    ~BPlusTreeIndex();
    
    // 基本操作
    bool Insert(const Value& key, int record_id);
    bool Remove(const Value& key);
    std::vector<int> Find(const Value& key) const;
    std::vector<int> FindRange(const Value& start_key, const Value& end_key) const;
    
    // 批量操作
    bool InsertBatch(const std::vector<std::pair<Value, int>>& key_value_pairs);
    bool RemoveBatch(const std::vector<Value>& keys);
    
    // 预读优化
    void EnablePrefetch(bool enable) { prefetch_enabled_ = enable; }
    void SetPrefetchConfig(size_t max_prefetch_pages) { max_prefetch_pages_ = max_prefetch_pages; }
    
    // 索引维护
    bool Rebuild();
    bool Validate() const;
    
    // 统计信息
    int GetHeight() const;
    int GetNodeCount() const;
    int GetKeyCount() const;
    
    // 序列化/反序列化
    bool Serialize();
    bool Deserialize();
    
private:
    BufferPoolManager* bpm_;
    std::string table_name_;
    std::string column_name_;
    int column_index_;
    DataType key_type_;
    
    int root_page_id_;
    int max_keys_per_node_;
    int height_;
    
    // 预读配置
    bool prefetch_enabled_ = true;
    size_t max_prefetch_pages_ = 5;
    
    // 日志器
    std::shared_ptr<DatabaseSystem::Log::BPlusTreeLogger> logger_;
    
    // 节点管理
    std::shared_ptr<BPlusNode> LoadNode(int page_id) const;
    bool SaveNode(std::shared_ptr<BPlusNode> node);
    int AllocateNewPage();
    void DeallocatePage(int page_id);
    
    // 树操作
    std::shared_ptr<BPlusNode> FindLeaf(const Value& key) const;
    bool InsertInternal(std::shared_ptr<BPlusNode> node, const Value& key, int record_id);
    bool RemoveInternal(std::shared_ptr<BPlusNode> node, const Value& key);
    bool SplitNode(std::shared_ptr<BPlusNode> node);
    void MergeNodes(std::shared_ptr<BPlusNode> left, std::shared_ptr<BPlusNode> right);
    
    // 分裂辅助方法
    bool SplitLeafNode(std::shared_ptr<BPlusLeafNode> leaf);
    bool SplitInternalNode(std::shared_ptr<BPlusInternalNode> internal);
    bool InsertIntoParent(std::shared_ptr<BPlusNode> left_child, 
                         const Value& separator_key, 
                         std::shared_ptr<BPlusNode> right_child);
    std::shared_ptr<BPlusInternalNode> FindParent(int child_page_id) const;
    
    // 合并辅助方法
    bool MergeLeafNodes(std::shared_ptr<BPlusLeafNode> left, std::shared_ptr<BPlusLeafNode> right);
    bool MergeInternalNodes(std::shared_ptr<BPlusInternalNode> left, std::shared_ptr<BPlusInternalNode> right);
    bool RedistributeNodes(std::shared_ptr<BPlusNode> left, std::shared_ptr<BPlusNode> right);
    
    // 比较函数
    int CompareValues(const Value& a, const Value& b) const;
};

// 索引管理器
class IndexManager {
public:
    IndexManager(BufferPoolManager* bpm);
    ~IndexManager();
    
    // 索引管理
    bool CreateIndex(const std::string& table_name, const std::string& column_name);
    bool DropIndex(const std::string& table_name, const std::string& column_name);
    bool HasIndex(const std::string& table_name, const std::string& column_name) const;
    
    // 查询优化
    std::vector<int> QueryWithIndex(const std::string& table_name, 
                                   const std::string& column_name,
                                   const Value& key) const;
    std::vector<int> QueryRangeWithIndex(const std::string& table_name,
                                        const std::string& column_name,
                                        const Value& start_key,
                                        const Value& end_key) const;
    
    // 索引维护
    bool InsertRecord(const std::string& table_name, const Record& record, int record_id);
    bool UpdateRecord(const std::string& table_name, const Record& old_record, 
                     const Record& new_record, int record_id);
    bool DeleteRecord(const std::string& table_name, const Record& record, int record_id);
    
    // 批量操作
    bool RebuildAllIndexes(const std::string& table_name);
    bool DropAllIndexes(const std::string& table_name);
    
private:
    BufferPoolManager* bpm_;
    std::unordered_map<std::string, std::shared_ptr<BPlusTreeIndex>> indexes_;
    
    std::string GetIndexKey(const std::string& table_name, const std::string& column_name) const;
};

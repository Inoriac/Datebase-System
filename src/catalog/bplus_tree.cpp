//
// B+树索引实现
//

#include "../../include/catalog/bplus_tree.h"
#include "../../include/catalog/record_serializer.h"
#include <iostream>
#include <algorithm>
#include <cstring>

// ========== BPlusLeafNode 实现 ==========

bool BPlusLeafNode::Insert(const Value& key, int record_id) {
    // 找到插入位置
    auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
    int pos = static_cast<int>(it - keys_.begin());
    
    // 插入键值对
    keys_.insert(it, key);
    record_ids_.insert(record_ids_.begin() + pos, record_id);
    
    is_dirty_ = true;
    return true;
}

bool BPlusLeafNode::Remove(const Value& key) {
    auto it = std::find(keys_.begin(), keys_.end(), key);
    if (it == keys_.end()) return false;
    
    int pos = static_cast<int>(it - keys_.begin());
    keys_.erase(it);
    record_ids_.erase(record_ids_.begin() + pos);
    
    is_dirty_ = true;
    return true;
}

int BPlusLeafNode::Find(const Value& key) const {
    // 只在查询特定键值时打印
    if (std::holds_alternative<int>(key) && std::get<int>(key) == 500) {
        std::cout << "DEBUG: BPlusLeafNode::Find called with key=500, searching in " << keys_.size() << " keys" << std::endl;
    }
    
    for (size_t i = 0; i < keys_.size(); ++i) {
        if (keys_[i] == key) {
            if (std::holds_alternative<int>(key) && std::get<int>(key) == 500) {
                std::cout << "DEBUG: Found matching key at position " << i << ", record_id=" << record_ids_[i] << std::endl;
            }
            return record_ids_[i];
        }
    }
    
    if (std::holds_alternative<int>(key) && std::get<int>(key) == 500) {
        std::cout << "DEBUG: Key 500 not found in leaf node" << std::endl;
    }
    return -1;
}

std::vector<int> BPlusLeafNode::FindRange(const Value& start_key, const Value& end_key) const {
    std::vector<int> result;
    
    auto start_it = std::lower_bound(keys_.begin(), keys_.end(), start_key);
    auto end_it = std::upper_bound(keys_.begin(), keys_.end(), end_key);
    
    int start_pos = static_cast<int>(start_it - keys_.begin());
    int end_pos = static_cast<int>(end_it - keys_.begin());
    
    for (int i = start_pos; i < end_pos; ++i) {
        result.push_back(record_ids_[i]);
    }
    
    return result;
}

// ========== BPlusInternalNode 实现 ==========

bool BPlusInternalNode::Insert(const Value& key, int child_page_id) {
    // 找到插入位置
    auto it = std::lower_bound(keys_.begin(), keys_.end(), key);
    int pos = static_cast<int>(it - keys_.begin());
    
    // 插入键和子节点
    keys_.insert(it, key);
    child_page_ids_.insert(child_page_ids_.begin() + pos + 1, child_page_id);
    
    is_dirty_ = true;
    return true;
}

bool BPlusInternalNode::Remove(const Value& key) {
    auto it = std::find(keys_.begin(), keys_.end(), key);
    if (it == keys_.end()) return false;
    
    int pos = static_cast<int>(it - keys_.begin());
    keys_.erase(it);
    child_page_ids_.erase(child_page_ids_.begin() + pos + 1);
    
    is_dirty_ = true;
    return true;
}

int BPlusInternalNode::FindChild(const Value& key) const {
    // 找到第一个大于key的位置
    auto it = std::upper_bound(keys_.begin(), keys_.end(), key);
    int pos = static_cast<int>(it - keys_.begin());
    
    return child_page_ids_[pos];
}

// ========== BPlusTreeIndex 实现 ==========

BPlusTreeIndex::BPlusTreeIndex(BufferPoolManager* bpm, const std::string& table_name, 
                               const std::string& column_name, int column_index)
    : bpm_(bpm), table_name_(table_name), column_name_(column_name), 
      column_index_(column_index), root_page_id_(-1), max_keys_per_node_(50), height_(0) {
    
    // 根据列类型设置键类型
    // 简化实现，根据列名判断类型
    if (column_name == "table_name") {
        key_type_ = DataType::Varchar;
    } else if (column_name == "id") {
        key_type_ = DataType::Int;
    } else {
        key_type_ = DataType::Int; // 默认类型
    }
    
    std::cout << "DEBUG: BPlusTreeIndex created with key_type=" << 
        (key_type_ == DataType::Int ? "INT" : 
         key_type_ == DataType::Varchar ? "STRING" : "BOOL") << std::endl;
}

BPlusTreeIndex::~BPlusTreeIndex() {
    // 清理资源
}

bool BPlusTreeIndex::Insert(const Value& key, int record_id) {
    std::cout << "DEBUG: BPlusTreeIndex::Insert called with key=";
    if (std::holds_alternative<int>(key)) {
        std::cout << std::get<int>(key);
    } else if (std::holds_alternative<std::string>(key)) {
        std::cout << std::get<std::string>(key);
    } else if (std::holds_alternative<bool>(key)) {
        std::cout << (std::get<bool>(key) ? "true" : "false");
    } else {
        std::cout << "unknown type";
    }
    std::cout << ", record_id=" << record_id << std::endl;
    
    if (root_page_id_ == -1) {
        // 创建根节点
        root_page_id_ = AllocateNewPage();
        if (root_page_id_ == -1) {
            std::cout << "DEBUG: Failed to allocate root page" << std::endl;
            return false;
        }
        
        auto root = std::make_shared<BPlusLeafNode>(root_page_id_);
        root->Insert(key, record_id);
        bool saved = SaveNode(root);
        std::cout << "DEBUG: Root node saved: " << (saved ? "success" : "failed") << std::endl;
        height_ = 1;
        return saved;
    }
    
    // 找到应该插入的叶子节点
    auto leaf = FindLeaf(key);
    if (!leaf) {
        std::cout << "DEBUG: Could not find leaf for insertion" << std::endl;
        return false;
    }
    
    auto leaf_node = std::static_pointer_cast<BPlusLeafNode>(leaf);
    
    // 检查叶子节点是否已满
    if (leaf_node->keys_.size() < max_keys_per_node_) {
        // 叶子节点未满，直接插入
        leaf_node->Insert(key, record_id);
        bool saved = SaveNode(leaf_node);
        std::cout << "DEBUG: Leaf node saved: " << (saved ? "success" : "failed") << std::endl;
        return saved;
    } else {
        // 叶子节点已满，需要分裂
        std::cout << "DEBUG: Leaf node is full, splitting..." << std::endl;
        leaf_node->Insert(key, record_id);
        SplitNode(leaf_node);
        return true;
    }
}

bool BPlusTreeIndex::Remove(const Value& key) {
    if (root_page_id_ == -1) return false;
    
    auto leaf = FindLeaf(key);
    if (!leaf) return false;
    
    auto leaf_node = std::static_pointer_cast<BPlusLeafNode>(leaf);
    if (leaf_node->Remove(key)) {
        SaveNode(leaf_node);
        return true;
    }
    
    return false;
}

std::vector<int> BPlusTreeIndex::Find(const Value& key) const {
    // 只在查询特定键值时打印
    if (std::holds_alternative<int>(key) && std::get<int>(key) == 500) {
        std::cout << "DEBUG: BPlusTreeIndex::Find called with key=500, root_page_id=" << root_page_id_ << std::endl;
    }
    
    if (root_page_id_ == -1) {
        std::cout << "DEBUG: No root page, returning empty" << std::endl;
        return {};
    }
    
    auto leaf = FindLeaf(key);
    if (!leaf) {
        std::cout << "DEBUG: Could not find leaf node, returning empty" << std::endl;
        return {};
    }
    
    auto leaf_node = std::static_pointer_cast<BPlusLeafNode>(leaf);
    int record_id = leaf_node->Find(key);
    
    std::cout << "DEBUG: Leaf node found record_id=" << record_id << std::endl;
    
    if (record_id == -1) {
        std::cout << "DEBUG: Key not found in leaf, returning empty" << std::endl;
        return {};
    }
    return {record_id};
}

std::vector<int> BPlusTreeIndex::FindRange(const Value& start_key, const Value& end_key) const {
    if (root_page_id_ == -1) return {};
    
    auto leaf = FindLeaf(start_key);
    if (!leaf) return {};
    
    auto leaf_node = std::static_pointer_cast<BPlusLeafNode>(leaf);
    return leaf_node->FindRange(start_key, end_key);
}

std::shared_ptr<BPlusNode> BPlusTreeIndex::FindLeaf(const Value& key) const {
    if (root_page_id_ == -1) {
        return nullptr;
    }
    
    auto current = LoadNode(root_page_id_);
    if (!current) {
        return nullptr;
    }
    
    while (!current->IsLeaf()) {
        auto internal = std::static_pointer_cast<BPlusInternalNode>(current);
        int child_page_id = internal->FindChild(key);
        current = LoadNode(child_page_id);
        if (!current) {
            return nullptr;
        }
    }
    
    return current;
}

bool BPlusTreeIndex::SplitNode(std::shared_ptr<BPlusNode> node) {
    if (node->IsLeaf()) {
        // 分裂叶子节点
        auto leaf = std::static_pointer_cast<BPlusLeafNode>(node);
        int mid = leaf->GetKeyCount() / 2;
        
        // 创建新的叶子节点
        int new_page_id = AllocateNewPage();
        auto new_leaf = std::make_shared<BPlusLeafNode>(new_page_id);
        
        // 移动一半的键值对到新节点
        for (int i = mid; i < leaf->GetKeyCount(); ++i) {
            new_leaf->keys_.push_back(leaf->keys_[i]);
            new_leaf->record_ids_.push_back(leaf->record_ids_[i]);
        }
        
        // 删除原节点中的一半键值对
        leaf->keys_.erase(leaf->keys_.begin() + mid, leaf->keys_.end());
        leaf->record_ids_.erase(leaf->record_ids_.begin() + mid, leaf->record_ids_.end());
        
        // 更新叶子节点链表
        new_leaf->next_leaf_page_id_ = leaf->next_leaf_page_id_;
        new_leaf->prev_leaf_page_id_ = leaf->page_id_;
        leaf->next_leaf_page_id_ = new_page_id;
        
        // 保存节点
        SaveNode(leaf);
        SaveNode(new_leaf);
        
        // 如果这是根节点，需要创建新的根节点
        if (leaf->page_id_ == root_page_id_) {
            int new_root_id = AllocateNewPage();
            auto new_root = std::make_shared<BPlusInternalNode>(new_root_id);
            
            // 使用新叶子节点的第一个键作为分隔键
            // 在B+树中，内部节点的键用于分隔子节点
            new_root->keys_.push_back(new_leaf->keys_[0]);
            new_root->child_page_ids_.push_back(leaf->page_id_);
            new_root->child_page_ids_.push_back(new_page_id);
            
            root_page_id_ = new_root_id;
            height_++;
            SaveNode(new_root);
            
            std::cout << "DEBUG: Created new root with separator key=" << std::get<int>(new_leaf->keys_[0]) 
                      << ", left child=" << leaf->page_id_ << ", right child=" << new_page_id << std::endl;
        } else {
            // 如果不是根节点，需要将分隔键插入到父节点
            // 这里需要实现向上传播的逻辑
            // 暂时简化处理，直接保存
            std::cout << "DEBUG: Leaf split but not root - parent update not implemented" << std::endl;
        }
        
        return true;
    } else {
        // 分裂内部节点
        auto internal = std::static_pointer_cast<BPlusInternalNode>(node);
        int mid = internal->GetKeyCount() / 2;
        
        // 创建新的内部节点
        int new_page_id = AllocateNewPage();
        auto new_internal = std::make_shared<BPlusInternalNode>(new_page_id);
        
        // 移动一半的键和子节点到新节点
        Value middle_key = internal->keys_[mid];
        for (int i = mid + 1; i < internal->GetKeyCount(); ++i) {
            new_internal->keys_.push_back(internal->keys_[i]);
        }
        for (int i = mid + 1; i < static_cast<int>(internal->child_page_ids_.size()); ++i) {
            new_internal->child_page_ids_.push_back(internal->child_page_ids_[i]);
        }
        
        // 删除原节点中的一半键和子节点
        internal->keys_.erase(internal->keys_.begin() + mid, internal->keys_.end());
        internal->child_page_ids_.erase(internal->child_page_ids_.begin() + mid + 1, internal->child_page_ids_.end());
        
        // 保存节点
        SaveNode(internal);
        SaveNode(new_internal);
        
        // 如果这是根节点，需要创建新的根节点
        if (internal->page_id_ == root_page_id_) {
            int new_root_id = AllocateNewPage();
            auto new_root = std::make_shared<BPlusInternalNode>(new_root_id);
            
            new_root->keys_.push_back(middle_key);
            new_root->child_page_ids_.push_back(internal->page_id_);
            new_root->child_page_ids_.push_back(new_page_id);
            
            root_page_id_ = new_root_id;
            height_++;
            SaveNode(new_root);
        }
    }
}

std::shared_ptr<BPlusNode> BPlusTreeIndex::LoadNode(int page_id) const {
    std::cout << "DEBUG: BPlusTreeIndex::LoadNode called for page_id=" << page_id << std::endl;
    
    Page* page = bpm_->FetchPage(page_id);
    if (!page) {
        std::cout << "DEBUG: Could not fetch page" << std::endl;
        return nullptr;
    }
    
    const char* data = page->GetData();
    
    // 读取节点类型
    BPlusNodeType type;
    std::memcpy(&type, data, sizeof(BPlusNodeType));
    
    std::cout << "DEBUG: Node type=" << (type == BPlusNodeType::LEAF ? "LEAF" : "INTERNAL") << std::endl;
    
    std::shared_ptr<BPlusNode> node;
    if (type == BPlusNodeType::LEAF) {
        node = std::make_shared<BPlusLeafNode>(page_id);
        auto leaf = std::static_pointer_cast<BPlusLeafNode>(node);
        
        // 反序列化叶子节点数据
        int offset = sizeof(BPlusNodeType);
        const int PAGE_SIZE = 4096;
        
        // 检查边界
        if (offset + sizeof(int) > PAGE_SIZE) {
            std::cout << "DEBUG: Page size exceeded during key count read" << std::endl;
            return nullptr;
        }
        
        int key_count;
        std::memcpy(&key_count, data + offset, sizeof(int));
        offset += sizeof(int);
        
        // 检查键数量是否合理
        if (key_count < 0 || key_count > max_keys_per_node_) {
            std::cout << "DEBUG: Invalid key count " << key_count << " (max=" << max_keys_per_node_ << ")" << std::endl;
            return nullptr;
        }
        
        std::cout << "DEBUG: Leaf node has " << key_count << " keys" << std::endl;
        
        leaf->keys_.resize(key_count);
        leaf->record_ids_.resize(key_count);
        
        for (int i = 0; i < key_count; ++i) {
            // 检查边界
            if (offset + 8 > PAGE_SIZE) { // 8 = max(int) + sizeof(int)
                std::cout << "DEBUG: Page size exceeded during key " << i << " deserialization" << std::endl;
                return nullptr;
            }
            
            // 根据key_type_反序列化键值
            if (key_type_ == DataType::Int) {
                int key_value;
                std::memcpy(&key_value, data + offset, sizeof(int));
                leaf->keys_[i] = key_value;
                offset += sizeof(int);
            } else if (key_type_ == DataType::Varchar) {
                // 读取字符串长度
                int str_len;
                std::memcpy(&str_len, data + offset, sizeof(int));
                offset += sizeof(int);
                
                // 检查字符串长度是否合理
                if (str_len < 0 || str_len > 1000 || offset + str_len + sizeof(int) > PAGE_SIZE) {
                    std::cout << "DEBUG: Invalid string length " << str_len << " at key " << i << std::endl;
                    return nullptr;
                }
                
                // 读取字符串内容
                std::string key_value(data + offset, str_len);
                offset += str_len;
                leaf->keys_[i] = key_value;
            } else if (key_type_ == DataType::Bool) {
                bool key_value;
                std::memcpy(&key_value, data + offset, sizeof(bool));
                leaf->keys_[i] = key_value;
                offset += sizeof(bool);
            }
            
            // 读取记录ID
            std::memcpy(&leaf->record_ids_[i], data + offset, sizeof(int));
            offset += sizeof(int);
            
            // 只在记录ID异常时打印
            if (leaf->record_ids_[i] > 100) {
                std::cout << "DEBUG: Loaded key=";
                if (std::holds_alternative<int>(leaf->keys_[i])) {
                    std::cout << std::get<int>(leaf->keys_[i]);
                } else if (std::holds_alternative<std::string>(leaf->keys_[i])) {
                    std::cout << std::get<std::string>(leaf->keys_[i]);
                } else if (std::holds_alternative<bool>(leaf->keys_[i])) {
                    std::cout << (std::get<bool>(leaf->keys_[i]) ? "true" : "false");
                }
                std::cout << ", record_id=" << leaf->record_ids_[i] << std::endl;
            }
        }
        
        std::memcpy(&leaf->next_leaf_page_id_, data + offset, sizeof(int));
        offset += sizeof(int);
        std::memcpy(&leaf->prev_leaf_page_id_, data + offset, sizeof(int));
        
    } else {
        node = std::make_shared<BPlusInternalNode>(page_id);
        auto internal = std::static_pointer_cast<BPlusInternalNode>(node);
        
        // 反序列化内部节点数据
        int offset = sizeof(BPlusNodeType);
        int key_count;
        std::memcpy(&key_count, data + offset, sizeof(int));
        offset += sizeof(int);
        
        internal->keys_.resize(key_count);
        internal->child_page_ids_.resize(key_count + 1);
        
        for (int i = 0; i < key_count; ++i) {
            int key_value;
            std::memcpy(&key_value, data + offset, sizeof(int));
            internal->keys_[i] = key_value;
            offset += sizeof(int);
        }
        
        for (int i = 0; i <= key_count; ++i) {
            std::memcpy(&internal->child_page_ids_[i], data + offset, sizeof(int));
            offset += sizeof(int);
        }
    }
    
    bpm_->UnpinPage(page_id);
    return node;
}

bool BPlusTreeIndex::SaveNode(std::shared_ptr<BPlusNode> node) {
    Page* page = bpm_->FetchPage(node->page_id_);
    if (!page) return false;
    
    char* data = page->GetData();
    const int PAGE_SIZE = 4096; // 页面大小
    
    // 写入节点类型
    std::memcpy(data, &node->type_, sizeof(BPlusNodeType));
    int offset = sizeof(BPlusNodeType);
    
    // 检查边界
    if (offset >= PAGE_SIZE) {
        std::cout << "DEBUG: Page size exceeded during node type write" << std::endl;
        return false;
    }
    
    if (node->IsLeaf()) {
        auto leaf = std::static_pointer_cast<BPlusLeafNode>(node);
        
        // 序列化叶子节点数据
        int key_count = leaf->GetKeyCount();
        
        // 检查键数量是否合理
        if (key_count > max_keys_per_node_) {
            std::cout << "DEBUG: Key count " << key_count << " exceeds max " << max_keys_per_node_ << std::endl;
            return false;
        }
        
        std::memcpy(data + offset, &key_count, sizeof(int));
        offset += sizeof(int);
        
        // 检查边界
        if (offset >= PAGE_SIZE) {
            std::cout << "DEBUG: Page size exceeded after key count write" << std::endl;
            return false;
        }
        
        for (int i = 0; i < key_count; ++i) {
            // 检查每次写入前的边界
            if (offset + 8 >= PAGE_SIZE) { // 8 = max(int) + sizeof(int)
                std::cout << "DEBUG: Page size exceeded at key " << i << ", offset=" << offset << std::endl;
                return false;
            }
            
            // 根据key_type_序列化键值
            if (key_type_ == DataType::Int) {
                int key_value = std::get<int>(leaf->keys_[i]);
                std::memcpy(data + offset, &key_value, sizeof(int));
                offset += sizeof(int);
            } else if (key_type_ == DataType::Varchar) {
                std::string key_value = std::get<std::string>(leaf->keys_[i]);
                int str_len = static_cast<int>(key_value.length());
                if (offset + sizeof(int) + str_len + sizeof(int) >= PAGE_SIZE) {
                    std::cout << "DEBUG: Page size exceeded for varchar key " << i << std::endl;
                    return false;
                }
                std::memcpy(data + offset, &str_len, sizeof(int));
                offset += sizeof(int);
                std::memcpy(data + offset, key_value.c_str(), str_len);
                offset += str_len;
            } else if (key_type_ == DataType::Bool) {
                bool key_value = std::get<bool>(leaf->keys_[i]);
                std::memcpy(data + offset, &key_value, sizeof(bool));
                offset += sizeof(bool);
            }
            
            std::memcpy(data + offset, &leaf->record_ids_[i], sizeof(int));
            offset += sizeof(int);
        }
        
        std::memcpy(data + offset, &leaf->next_leaf_page_id_, sizeof(int));
        offset += sizeof(int);
        std::memcpy(data + offset, &leaf->prev_leaf_page_id_, sizeof(int));
        
    } else {
        auto internal = std::static_pointer_cast<BPlusInternalNode>(node);
        
        // 序列化内部节点数据
        int key_count = internal->GetKeyCount();
        std::memcpy(data + offset, &key_count, sizeof(int));
        offset += sizeof(int);
        
        for (int i = 0; i < key_count; ++i) {
            int key_value = std::get<int>(internal->keys_[i]);
            std::memcpy(data + offset, &key_value, sizeof(int));
            offset += sizeof(int);
        }
        
        for (int i = 0; i <= key_count; ++i) {
            std::memcpy(data + offset, &internal->child_page_ids_[i], sizeof(int));
            offset += sizeof(int);
        }
    }
    
    page->SetDirty(true);
    bpm_->UnpinPage(node->page_id_);
    return true;
}

int BPlusTreeIndex::AllocateNewPage() {
    // 从BufferPoolManager分配新页面
    int page_id;
    Page* page = bpm_->NewPage(&page_id);
    if (!page) {
        std::cout << "DEBUG: Failed to allocate new page" << std::endl;
        return -1;
    }
    
    std::cout << "DEBUG: Allocated new page with ID=" << page_id << std::endl;
    
    // 初始化页面数据
    char* data = page->GetData();
    std::memset(data, 0, PAGE_SIZE);
    
    // 标记页面为脏页
    page->SetDirty(true);
    
    // 取消固定页面
    bpm_->UnpinPage(page_id);
    
    return page_id;
}

void BPlusTreeIndex::DeallocatePage(int page_id) {
    // 这里需要实现页释放逻辑
    // 简化实现，暂时不处理
}

int BPlusTreeIndex::CompareValues(const Value& a, const Value& b) const {
    if (a < b) return -1;
    if (a > b) return 1;
    return 0;
}

// ========== IndexManager 实现 ==========

IndexManager::IndexManager(BufferPoolManager* bpm) : bpm_(bpm) {
}

IndexManager::~IndexManager() {
    // 清理所有索引
    indexes_.clear();
}

bool IndexManager::CreateIndex(const std::string& table_name, const std::string& column_name) {
    std::cout << "DEBUG: IndexManager::CreateIndex called for table=" << table_name << ", column=" << column_name << std::endl;
    
    // 这里需要从表结构中获取列索引
    // 简化实现，假设列索引为0
    int column_index = 0;
    
    std::string index_key = GetIndexKey(table_name, column_name);
    std::cout << "DEBUG: Index key: " << index_key << std::endl;
    
    auto index = std::make_shared<BPlusTreeIndex>(bpm_, table_name, column_name, column_index);
    indexes_[index_key] = index;
    
    std::cout << "DEBUG: Index created successfully, total indexes: " << indexes_.size() << std::endl;
    return true;
}

bool IndexManager::DropIndex(const std::string& table_name, const std::string& column_name) {
    std::string index_key = GetIndexKey(table_name, column_name);
    auto it = indexes_.find(index_key);
    if (it == indexes_.end()) return false;
    
    indexes_.erase(it);
    return true;
}

bool IndexManager::HasIndex(const std::string& table_name, const std::string& column_name) const {
    std::string index_key = GetIndexKey(table_name, column_name);
    return indexes_.find(index_key) != indexes_.end();
}

std::vector<int> IndexManager::QueryWithIndex(const std::string& table_name, 
                                             const std::string& column_name,
                                             const Value& key) const {
    std::string index_key = GetIndexKey(table_name, column_name);
    auto it = indexes_.find(index_key);
    if (it == indexes_.end()) return {};
    
    return it->second->Find(key);
}

std::vector<int> IndexManager::QueryRangeWithIndex(const std::string& table_name,
                                                  const std::string& column_name,
                                                  const Value& start_key,
                                                  const Value& end_key) const {
    std::string index_key = GetIndexKey(table_name, column_name);
    auto it = indexes_.find(index_key);
    if (it == indexes_.end()) return {};
    
    return it->second->FindRange(start_key, end_key);
}

std::string IndexManager::GetIndexKey(const std::string& table_name, const std::string& column_name) const {
    return table_name + "." + column_name;
}

bool IndexManager::InsertRecord(const std::string& table_name, const Record& record, int record_id) {
    std::cout << "DEBUG: IndexManager::InsertRecord called for table=" << table_name << ", record_id=" << record_id << std::endl;
    std::cout << "DEBUG: Available indexes: " << indexes_.size() << std::endl;
    
    // 为所有有索引的列更新索引
    for (auto& [index_key, index] : indexes_) {
        std::cout << "DEBUG: Checking index key: " << index_key << std::endl;
        
        // 解析表名和列名
        size_t dot_pos = index_key.find('.');
        if (dot_pos == std::string::npos) continue;
        
        std::string indexed_table = index_key.substr(0, dot_pos);
        std::string indexed_column = index_key.substr(dot_pos + 1);
        
        std::cout << "DEBUG: Indexed table=" << indexed_table << ", column=" << indexed_column << std::endl;
        
        if (indexed_table != table_name) {
            std::cout << "DEBUG: Table mismatch, skipping" << std::endl;
            continue;
        }
        
        // 这里需要根据列名找到对应的值
        // 简化实现，假设主键列在索引0位置
        if (!record.values_.empty()) {
            const Value& key_value = record.values_[0];
            
            // 安全地打印键值
            std::cout << "DEBUG: Inserting key=";
            if (std::holds_alternative<int>(key_value)) {
                std::cout << std::get<int>(key_value);
            } else if (std::holds_alternative<std::string>(key_value)) {
                std::cout << std::get<std::string>(key_value);
            } else if (std::holds_alternative<bool>(key_value)) {
                std::cout << (std::get<bool>(key_value) ? "true" : "false");
            } else {
                std::cout << "unknown type";
            }
            std::cout << ", record_id=" << record_id << " into index" << std::endl;
            
            index->Insert(key_value, record_id);
            std::cout << "DEBUG: Index insert completed" << std::endl;
        } else {
            std::cout << "DEBUG: Empty record values" << std::endl;
        }
    }
    return true;
}

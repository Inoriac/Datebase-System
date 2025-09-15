//
// B+树索引实现
//

#include "../../include/catalog/bplus_tree.h"
#include "../../include/catalog/record_serializer.h"
#include "../../include/log/log_config.h"
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
    // 打印所有查找操作
    std::cout << "DEBUG: BPlusLeafNode::Find called with key=";
    if (std::holds_alternative<std::string>(key)) {
        std::cout << std::get<std::string>(key);
    } else if (std::holds_alternative<int>(key)) {
        std::cout << std::get<int>(key);
    }
    std::cout << ", searching in " << keys_.size() << " keys" << std::endl;
    
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
    
    // 如果找到的键大于key，选择左子节点（pos不变）
    // 如果没找到（pos == keys_.size()），选择最后一个子节点
    if (pos == keys_.size()) {
        pos = keys_.size(); // 选择最后一个子节点
    }
    
    return child_page_ids_[pos];
}

// ========== BPlusTreeIndex 实现 ==========

BPlusTreeIndex::BPlusTreeIndex(BufferPoolManager* bpm, const std::string& table_name, 
                               const std::string& column_name, int column_index)
    : bpm_(bpm), table_name_(table_name), column_name_(column_name), 
      column_index_(column_index), root_page_id_(-1), max_keys_per_node_(50), height_(0) {
    
    // 初始化日志器
    logger_ = DatabaseSystem::Log::LogConfig::GetBPlusTreeLogger();
    
    // 根据列类型设置键类型
    // 简化实现，根据列名判断类型
    if (column_name == "table_name" || column_name == "name") {
        key_type_ = DataType::Varchar;
    } else if (column_name == "id") {
        key_type_ = DataType::Int;
    } else {
        key_type_ = DataType::Int; // 默认类型
    }
    
    logger_->Info("BPlusTreeIndex created with key_type={}", 
        (key_type_ == DataType::Int ? "INT" : 
         key_type_ == DataType::Varchar ? "STRING" : "BOOL"));
}

BPlusTreeIndex::~BPlusTreeIndex() {
    // 清理资源
}

bool BPlusTreeIndex::Insert(const Value& key, int record_id) {
    // 将键值转换为字符串用于日志
    std::string key_str;
    if (std::holds_alternative<int>(key)) {
        key_str = std::to_string(std::get<int>(key));
    } else if (std::holds_alternative<std::string>(key)) {
        key_str = std::get<std::string>(key);
    } else if (std::holds_alternative<bool>(key)) {
        key_str = std::get<bool>(key) ? "true" : "false";
    } else {
        key_str = "unknown type";
    }
    
    logger_->LogInsert(key_str, record_id);
    
    if (root_page_id_ == -1) {
        // 创建根节点
        root_page_id_ = AllocateNewPage();
        if (root_page_id_ == -1) {
            logger_->Error("Failed to allocate root page");
            return false;
        }
        
        auto root = std::make_shared<BPlusLeafNode>(root_page_id_);
        root->Insert(key, record_id);
        bool saved = SaveNode(root);
        if (saved) {
            logger_->Debug("Root node saved successfully");
        } else {
            logger_->Error("Failed to save root node");
        }
        height_ = 1;
        return saved;
    }
    
    // 找到应该插入的叶子节点
    auto leaf = FindLeaf(key);
    if (!leaf) {
        logger_->Error("Could not find leaf for insertion");
        return false;
    }
    
    auto leaf_node = std::static_pointer_cast<BPlusLeafNode>(leaf);
    
    // 检查键是否已经存在
    int existing_record_id = leaf_node->Find(key);
    if (existing_record_id != -1) {
        logger_->Warn("Key already exists with record_id={}", existing_record_id);
        return false;
    }
    
    // 检查叶子节点是否已满
    if (leaf_node->keys_.size() < max_keys_per_node_) {
        // 叶子节点未满，直接插入
        leaf_node->Insert(key, record_id);
        bool saved = SaveNode(leaf_node);
        if (saved) {
            logger_->Debug("Leaf node saved successfully");
        } else {
            logger_->Error("Failed to save leaf node");
        }
        return saved;
    } else {
        // 叶子节点已满，需要分裂
        logger_->Info("Leaf node is full, splitting...");
        logger_->Debug("Before split - node has {} keys", leaf_node->GetKeyCount());
        leaf_node->Insert(key, record_id);
        SplitNode(leaf_node);
        logger_->Debug("After split - node has {} keys", leaf_node->GetKeyCount());
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
    // 将键值转换为字符串用于日志
    std::string key_str;
    if (std::holds_alternative<std::string>(key)) {
        key_str = std::get<std::string>(key);
    } else if (std::holds_alternative<int>(key)) {
        key_str = std::to_string(std::get<int>(key));
    } else if (std::holds_alternative<bool>(key)) {
        key_str = std::get<bool>(key) ? "true" : "false";
    } else {
        key_str = "unknown type";
    }
    
    logger_->Debug("Find called with key={}, root_page_id={}", key_str, root_page_id_);
    
    if (root_page_id_ == -1) {
        logger_->Debug("No root page, returning empty");
        return {};
    }
    
    auto leaf = FindLeaf(key);
    if (!leaf) {
        logger_->Error("Could not find leaf node, returning empty");
        return {};
    }
    
    auto leaf_node = std::static_pointer_cast<BPlusLeafNode>(leaf);
    int record_id = leaf_node->Find(key);
    
    logger_->LogFind(key_str, record_id != -1, record_id);
    
    if (record_id == -1) {
        return {};
    }
    return {record_id};
}

std::vector<int> BPlusTreeIndex::FindRange(const Value& start_key, const Value& end_key) const {
    if (root_page_id_ == -1) return {};
    
    std::vector<int> result;
    
    // 找到包含start_key的叶子节点
    auto leaf = FindLeaf(start_key);
    if (!leaf) return {};
    
    auto current_leaf = std::static_pointer_cast<BPlusLeafNode>(leaf);
    
    // 遍历所有相关的叶子节点
    while (current_leaf) {
        std::cout << "DEBUG: Searching in leaf node with " << current_leaf->keys_.size() << " keys" << std::endl;
        
        // 在当前叶子节点中搜索范围
        auto leaf_results = current_leaf->FindRange(start_key, end_key);
        std::cout << "DEBUG: Found " << leaf_results.size() << " results in current leaf node" << std::endl;
        result.insert(result.end(), leaf_results.begin(), leaf_results.end());
        
        // 检查是否需要继续到下一个叶子节点
        if (current_leaf->keys_.empty() || current_leaf->keys_.back() < end_key) {
            // 如果当前叶子节点的最大键小于end_key，继续到下一个叶子节点
            if (current_leaf->next_leaf_page_id_ != -1) {
                current_leaf = std::static_pointer_cast<BPlusLeafNode>(LoadNode(current_leaf->next_leaf_page_id_));
                if (!current_leaf) break;
            } else {
                // 没有下一个叶子节点，停止搜索
                break;
            }
        } else {
            // 当前叶子节点已经包含了所有可能的键，停止搜索
            break;
        }
    }
    
    std::cout << "DEBUG: Range query found " << result.size() << " records" << std::endl;
    return result;
}

std::shared_ptr<BPlusNode> BPlusTreeIndex::FindLeaf(const Value& key) const {
    if (root_page_id_ == -1) {
        return nullptr;
    }
    
    auto current = LoadNode(root_page_id_);
    if (!current) {
        return nullptr;
    }
    
    std::cout << "DEBUG: Starting from root page " << root_page_id_ << ", height=" << height_ << std::endl;
    
    while (!current->IsLeaf()) {
        auto internal = std::static_pointer_cast<BPlusInternalNode>(current);
        
        // 调试：打印内部节点的键
        std::cout << "DEBUG: Internal node has " << internal->keys_.size() << " keys: ";
        for (size_t i = 0; i < internal->keys_.size(); ++i) {
            if (std::holds_alternative<int>(internal->keys_[i])) {
                std::cout << std::get<int>(internal->keys_[i]) << " ";
            } else if (std::holds_alternative<std::string>(internal->keys_[i])) {
                std::cout << std::get<std::string>(internal->keys_[i]) << " ";
            } else if (std::holds_alternative<bool>(internal->keys_[i])) {
                std::cout << (std::get<bool>(internal->keys_[i]) ? "true" : "false") << " ";
            }
        }
        std::cout << std::endl;
        
        int child_page_id = internal->FindChild(key);
        std::cout << "DEBUG: Looking for key ";
        if (std::holds_alternative<int>(key)) {
            std::cout << std::get<int>(key);
        } else if (std::holds_alternative<std::string>(key)) {
            std::cout << std::get<std::string>(key);
        } else if (std::holds_alternative<bool>(key)) {
            std::cout << (std::get<bool>(key) ? "true" : "false");
        } else {
            std::cout << "unknown type";
        }
        std::cout << ", found child page " << child_page_id << std::endl;
        
        current = LoadNode(child_page_id);
        if (!current) {
            return nullptr;
        }
    }
    
    std::cout << "DEBUG: Reached leaf node with " << current->GetKeyCount() << " keys" << std::endl;
    return current;
}

bool BPlusTreeIndex::SplitNode(std::shared_ptr<BPlusNode> node) {
    if (node->IsLeaf()) {
        return SplitLeafNode(std::static_pointer_cast<BPlusLeafNode>(node));
    } else {
        return SplitInternalNode(std::static_pointer_cast<BPlusInternalNode>(node));
    }
}

// ========== 完善的分裂逻辑实现 ==========

bool BPlusTreeIndex::SplitLeafNode(std::shared_ptr<BPlusLeafNode> leaf) {
    std::cout << "DEBUG: Splitting leaf node with " << leaf->GetKeyCount() << " keys" << std::endl;
    
    int mid = leaf->GetKeyCount() / 2;
    if (mid <= 0) {
        std::cout << "DEBUG: Leaf node has too few keys to split" << std::endl;
        return false;
    }
    
    // 创建新的叶子节点
    int new_page_id = AllocateNewPage();
    if (new_page_id == -1) {
        std::cout << "DEBUG: Failed to allocate new page for leaf split" << std::endl;
        return false;
    }
    
    auto new_leaf = std::make_shared<BPlusLeafNode>(new_page_id);
    
    // 移动一半的键值对到新节点（从mid+1开始，保留mid在原节点中）
    for (int i = mid + 1; i < leaf->GetKeyCount(); ++i) {
        new_leaf->keys_.push_back(leaf->keys_[i]);
        new_leaf->record_ids_.push_back(leaf->record_ids_[i]);
    }
    
    // 删除原节点中的一半键值对（保留mid在原节点中）
    leaf->keys_.erase(leaf->keys_.begin() + mid + 1, leaf->keys_.end());
    leaf->record_ids_.erase(leaf->record_ids_.begin() + mid + 1, leaf->record_ids_.end());
    
    // 更新叶子节点链表
    new_leaf->next_leaf_page_id_ = leaf->next_leaf_page_id_;
    new_leaf->prev_leaf_page_id_ = leaf->page_id_;
    leaf->next_leaf_page_id_ = new_page_id;
    
    // 更新前一个叶子节点的next指针
    if (new_leaf->next_leaf_page_id_ != -1) {
        auto next_leaf = std::static_pointer_cast<BPlusLeafNode>(LoadNode(new_leaf->next_leaf_page_id_));
        if (next_leaf) {
            next_leaf->prev_leaf_page_id_ = new_page_id;
            SaveNode(next_leaf);
        }
    }
    
    // 保存节点
    if (!SaveNode(leaf) || !SaveNode(new_leaf)) {
        std::cout << "DEBUG: Failed to save nodes during leaf split" << std::endl;
        return false;
    }
    
    // 使用新叶子节点的第一个键作为分隔键
    Value separator_key = new_leaf->keys_[0];
    
    std::cout << "DEBUG: Split completed - Left node has " << leaf->keys_.size() 
              << " keys, Right node has " << new_leaf->keys_.size() 
              << " keys, Separator key: ";
    if (std::holds_alternative<std::string>(separator_key)) {
        std::cout << std::get<std::string>(separator_key);
    } else if (std::holds_alternative<int>(separator_key)) {
        std::cout << std::get<int>(separator_key);
    }
    std::cout << std::endl;
    
    // 打印左节点的键
    std::cout << "DEBUG: Left node keys: ";
    for (size_t i = 0; i < leaf->keys_.size(); ++i) {
        if (std::holds_alternative<std::string>(leaf->keys_[i])) {
            std::cout << std::get<std::string>(leaf->keys_[i]) << " ";
        }
    }
    std::cout << std::endl;
    
    // 打印右节点的键
    std::cout << "DEBUG: Right node keys: ";
    for (size_t i = 0; i < new_leaf->keys_.size(); ++i) {
        if (std::holds_alternative<std::string>(new_leaf->keys_[i])) {
            std::cout << std::get<std::string>(new_leaf->keys_[i]) << " ";
        }
    }
    std::cout << std::endl;
    
    // 如果这是根节点，需要创建新的根节点
    if (leaf->page_id_ == root_page_id_) {
        int new_root_id = AllocateNewPage();
        if (new_root_id == -1) {
            std::cout << "DEBUG: Failed to allocate new root page" << std::endl;
            return false;
        }
        
        auto new_root = std::make_shared<BPlusInternalNode>(new_root_id);
        new_root->keys_.push_back(separator_key);
        new_root->child_page_ids_.push_back(leaf->page_id_);
        new_root->child_page_ids_.push_back(new_page_id);
        
        root_page_id_ = new_root_id;
        height_++;
        
        if (!SaveNode(new_root)) {
            std::cout << "DEBUG: Failed to save new root node" << std::endl;
            return false;
        }
        
        std::cout << "DEBUG: Created new root with separator key, left child=" 
                  << leaf->page_id_ << ", right child=" << new_page_id << std::endl;
    } else {
        // 将分隔键插入到父节点
        if (!InsertIntoParent(leaf, separator_key, new_leaf)) {
            std::cout << "DEBUG: Failed to insert separator key into parent" << std::endl;
            return false;
        }
    }
    
    std::cout << "DEBUG: Leaf split completed successfully" << std::endl;
    return true;
}

bool BPlusTreeIndex::SplitInternalNode(std::shared_ptr<BPlusInternalNode> internal) {
    std::cout << "DEBUG: Splitting internal node with " << internal->GetKeyCount() << " keys" << std::endl;
    
    int mid = internal->GetKeyCount() / 2;
    if (mid <= 0) {
        std::cout << "DEBUG: Internal node has too few keys to split" << std::endl;
        return false;
    }
    
    // 创建新的内部节点
    int new_page_id = AllocateNewPage();
    if (new_page_id == -1) {
        std::cout << "DEBUG: Failed to allocate new page for internal split" << std::endl;
        return false;
    }
    
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
    if (!SaveNode(internal) || !SaveNode(new_internal)) {
        std::cout << "DEBUG: Failed to save nodes during internal split" << std::endl;
        return false;
    }
    
    // 如果这是根节点，需要创建新的根节点
    if (internal->page_id_ == root_page_id_) {
        int new_root_id = AllocateNewPage();
        if (new_root_id == -1) {
            std::cout << "DEBUG: Failed to allocate new root page" << std::endl;
            return false;
        }
        
        auto new_root = std::make_shared<BPlusInternalNode>(new_root_id);
        new_root->keys_.push_back(middle_key);
        new_root->child_page_ids_.push_back(internal->page_id_);
        new_root->child_page_ids_.push_back(new_page_id);
        
        root_page_id_ = new_root_id;
        height_++;
        
        if (!SaveNode(new_root)) {
            std::cout << "DEBUG: Failed to save new root node" << std::endl;
            return false;
        }
        
        std::cout << "DEBUG: Created new root with separator key, left child=" 
                  << internal->page_id_ << ", right child=" << new_page_id << std::endl;
    } else {
        // 将分隔键插入到父节点
        if (!InsertIntoParent(internal, middle_key, new_internal)) {
            std::cout << "DEBUG: Failed to insert separator key into parent" << std::endl;
            return false;
        }
    }
    
    std::cout << "DEBUG: Internal split completed successfully" << std::endl;
    return true;
}

bool BPlusTreeIndex::InsertIntoParent(std::shared_ptr<BPlusNode> left_child, 
                                     const Value& separator_key, 
                                     std::shared_ptr<BPlusNode> right_child) {
    std::cout << "DEBUG: Inserting separator key into parent" << std::endl;
    
    // 查找父节点
    auto parent = FindParent(left_child->page_id_);
    if (!parent) {
        std::cout << "DEBUG: Could not find parent node" << std::endl;
        return false;
    }
    
    // 检查父节点是否有空间
    // 对于内部节点，分裂阈值应该与叶子节点不同
    // 内部节点的分裂阈值应该更小，以确保及时分裂
    int max_keys_for_internal = max_keys_per_node_ / 2; // 使用更小的阈值
    if (parent->GetKeyCount() < max_keys_for_internal) {
        // 父节点有空间，直接插入
        parent->Insert(separator_key, right_child->page_id_);
        if (!SaveNode(parent)) {
            std::cout << "DEBUG: Failed to save parent node" << std::endl;
            return false;
        }
        std::cout << "DEBUG: Inserted separator key into parent successfully" << std::endl;
        return true;
    } else {
        // 父节点已满，需要先分裂父节点
        std::cout << "DEBUG: Parent node is full, splitting parent first" << std::endl;
        
        // 先插入分隔键
        parent->Insert(separator_key, right_child->page_id_);
        
        // 分裂父节点
        if (!SplitNode(parent)) {
            std::cout << "DEBUG: Failed to split parent node" << std::endl;
            return false;
        }
        
        std::cout << "DEBUG: Parent node split and separator key inserted successfully" << std::endl;
        return true;
    }
}

std::shared_ptr<BPlusInternalNode> BPlusTreeIndex::FindParent(int child_page_id) const {
    if (root_page_id_ == -1 || child_page_id == root_page_id_) {
        return nullptr; // 根节点没有父节点
    }
    
    // 从根节点开始搜索
    auto current = LoadNode(root_page_id_);
    if (!current) {
        return nullptr;
    }
    
    // 如果根节点是叶子节点，说明树只有一层
    if (current->IsLeaf()) {
        return nullptr;
    }
    
    // 使用BFS搜索父节点
    std::vector<std::shared_ptr<BPlusInternalNode>> queue;
    queue.push_back(std::static_pointer_cast<BPlusInternalNode>(current));
    
    while (!queue.empty()) {
        auto node = queue.front();
        queue.erase(queue.begin());
        
        // 检查是否包含目标子节点
        for (int child_id : node->child_page_ids_) {
            if (child_id == child_page_id) {
                return node;
            }
        }
        
        // 将子节点加入队列
        for (int child_id : node->child_page_ids_) {
            auto child = LoadNode(child_id);
            if (child && !child->IsLeaf()) {
                queue.push_back(std::static_pointer_cast<BPlusInternalNode>(child));
            }
        }
    }
    
    return nullptr;
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
            // 根据key_type_反序列化键值
            if (key_type_ == DataType::Int) {
                int key_value;
                std::memcpy(&key_value, data + offset, sizeof(int));
                internal->keys_[i] = key_value;
                offset += sizeof(int);
            } else if (key_type_ == DataType::Varchar) {
                // 读取字符串长度
                int str_len;
                std::memcpy(&str_len, data + offset, sizeof(int));
                offset += sizeof(int);
                
                // 读取字符串内容
                std::string key_value(data + offset, str_len);
                offset += str_len;
                internal->keys_[i] = key_value;
            } else if (key_type_ == DataType::Bool) {
                bool key_value;
                std::memcpy(&key_value, data + offset, sizeof(bool));
                internal->keys_[i] = key_value;
                offset += sizeof(bool);
            }
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
                if (!std::holds_alternative<int>(leaf->keys_[i])) {
                    std::cout << "DEBUG: Expected int key but got different type at index " << i << std::endl;
                    return false;
                }
                int key_value = std::get<int>(leaf->keys_[i]);
                std::memcpy(data + offset, &key_value, sizeof(int));
                offset += sizeof(int);
            } else if (key_type_ == DataType::Varchar) {
                if (!std::holds_alternative<std::string>(leaf->keys_[i])) {
                    std::cout << "DEBUG: Expected string key but got different type at index " << i << std::endl;
                    return false;
                }
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
                if (!std::holds_alternative<bool>(leaf->keys_[i])) {
                    std::cout << "DEBUG: Expected bool key but got different type at index " << i << std::endl;
                    return false;
                }
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
            // 根据key_type_序列化键值
            if (key_type_ == DataType::Int) {
                if (!std::holds_alternative<int>(internal->keys_[i])) {
                    std::cout << "DEBUG: Expected int key but got different type at internal index " << i << std::endl;
                    return false;
                }
                int key_value = std::get<int>(internal->keys_[i]);
                std::memcpy(data + offset, &key_value, sizeof(int));
                offset += sizeof(int);
            } else if (key_type_ == DataType::Varchar) {
                if (!std::holds_alternative<std::string>(internal->keys_[i])) {
                    std::cout << "DEBUG: Expected string key but got different type at internal index " << i << std::endl;
                    return false;
                }
                std::string key_value = std::get<std::string>(internal->keys_[i]);
                int str_len = static_cast<int>(key_value.length());
                std::memcpy(data + offset, &str_len, sizeof(int));
                offset += sizeof(int);
                std::memcpy(data + offset, key_value.c_str(), str_len);
                offset += str_len;
            } else if (key_type_ == DataType::Bool) {
                if (!std::holds_alternative<bool>(internal->keys_[i])) {
                    std::cout << "DEBUG: Expected bool key but got different type at internal index " << i << std::endl;
                    return false;
                }
                bool key_value = std::get<bool>(internal->keys_[i]);
                std::memcpy(data + offset, &key_value, sizeof(bool));
                offset += sizeof(bool);
            }
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

// ========== 统计信息方法 ==========

int BPlusTreeIndex::GetHeight() const {
    return height_;
}

int BPlusTreeIndex::GetNodeCount() const {
    // 简化实现，返回根节点ID（如果存在）
    return root_page_id_ != -1 ? 1 : 0;
}

int BPlusTreeIndex::GetKeyCount() const {
    if (root_page_id_ == -1) return 0;
    
    // 遍历所有叶子节点计算总键数
    int total_keys = 0;
    auto current = LoadNode(root_page_id_);
    if (!current) return 0;
    
    // 如果根节点是叶子节点
    if (current->IsLeaf()) {
        return current->GetKeyCount();
    }
    
    // 如果根节点是内部节点，找到第一个叶子节点
    auto internal = std::static_pointer_cast<BPlusInternalNode>(current);
    if (internal->child_page_ids_.empty()) return 0;
    
    auto leaf = LoadNode(internal->child_page_ids_[0]);
    if (!leaf || !leaf->IsLeaf()) return 0;
    
    // 遍历叶子节点链表
    auto current_leaf = std::static_pointer_cast<BPlusLeafNode>(leaf);
    while (current_leaf) {
        total_keys += current_leaf->GetKeyCount();
        if (current_leaf->next_leaf_page_id_ != -1) {
            current_leaf = std::static_pointer_cast<BPlusLeafNode>(LoadNode(current_leaf->next_leaf_page_id_));
        } else {
            current_leaf = nullptr;
        }
    }
    
    return total_keys;
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

// ========== 节点合并和重新平衡逻辑 ==========

bool BPlusTreeIndex::MergeLeafNodes(std::shared_ptr<BPlusLeafNode> left, std::shared_ptr<BPlusLeafNode> right) {
    std::cout << "DEBUG: Merging leaf nodes" << std::endl;
    
    // 检查合并后是否会超过最大键数
    if (left->GetKeyCount() + right->GetKeyCount() > max_keys_per_node_) {
        std::cout << "DEBUG: Cannot merge - would exceed max keys per node" << std::endl;
        return false;
    }
    
    // 将右节点的键值对移动到左节点
    for (size_t i = 0; i < right->keys_.size(); ++i) {
        left->keys_.push_back(right->keys_[i]);
        left->record_ids_.push_back(right->record_ids_[i]);
    }
    
    // 更新叶子节点链表
    left->next_leaf_page_id_ = right->next_leaf_page_id_;
    if (right->next_leaf_page_id_ != -1) {
        auto next_leaf = std::static_pointer_cast<BPlusLeafNode>(LoadNode(right->next_leaf_page_id_));
        if (next_leaf) {
            next_leaf->prev_leaf_page_id_ = left->page_id_;
            SaveNode(next_leaf);
        }
    }
    
    // 保存左节点，删除右节点
    if (!SaveNode(left)) {
        std::cout << "DEBUG: Failed to save merged leaf node" << std::endl;
        return false;
    }
    
    DeallocatePage(right->page_id_);
    std::cout << "DEBUG: Leaf nodes merged successfully" << std::endl;
    return true;
}

bool BPlusTreeIndex::MergeInternalNodes(std::shared_ptr<BPlusInternalNode> left, std::shared_ptr<BPlusInternalNode> right) {
    std::cout << "DEBUG: Merging internal nodes" << std::endl;
    
    // 检查合并后是否会超过最大键数
    if (left->GetKeyCount() + right->GetKeyCount() + 1 > max_keys_per_node_) {
        std::cout << "DEBUG: Cannot merge - would exceed max keys per node" << std::endl;
        return false;
    }
    
    // 添加分隔键（从父节点获取）
    // 这里需要从父节点获取分隔键，简化实现使用右节点的第一个键
    if (!right->keys_.empty()) {
        left->keys_.push_back(right->keys_[0]);
    }
    
    // 将右节点的键和子节点移动到左节点
    for (size_t i = 1; i < right->keys_.size(); ++i) {
        left->keys_.push_back(right->keys_[i]);
    }
    for (int child_id : right->child_page_ids_) {
        left->child_page_ids_.push_back(child_id);
    }
    
    // 保存左节点，删除右节点
    if (!SaveNode(left)) {
        std::cout << "DEBUG: Failed to save merged internal node" << std::endl;
        return false;
    }
    
    DeallocatePage(right->page_id_);
    std::cout << "DEBUG: Internal nodes merged successfully" << std::endl;
    return true;
}

bool BPlusTreeIndex::RedistributeNodes(std::shared_ptr<BPlusNode> left, std::shared_ptr<BPlusNode> right) {
    std::cout << "DEBUG: Redistributing nodes" << std::endl;
    
    if (left->IsLeaf()) {
        auto left_leaf = std::static_pointer_cast<BPlusLeafNode>(left);
        auto right_leaf = std::static_pointer_cast<BPlusLeafNode>(right);
        
        // 计算总键数
        int total_keys = left_leaf->GetKeyCount() + right_leaf->GetKeyCount();
        int target_keys = total_keys / 2;
        
        // 如果左节点键数过多，移动一些到右节点
        if (left_leaf->GetKeyCount() > target_keys) {
            int move_count = left_leaf->GetKeyCount() - target_keys;
            
            // 将左节点的最后几个键值对移动到右节点
            for (int i = 0; i < move_count; ++i) {
                int index = left_leaf->GetKeyCount() - 1;
                right_leaf->keys_.insert(right_leaf->keys_.begin(), left_leaf->keys_[index]);
                right_leaf->record_ids_.insert(right_leaf->record_ids_.begin(), left_leaf->record_ids_[index]);
                
                left_leaf->keys_.erase(left_leaf->keys_.begin() + index);
                left_leaf->record_ids_.erase(left_leaf->record_ids_.begin() + index);
            }
        }
        // 如果右节点键数过多，移动一些到左节点
        else if (right_leaf->GetKeyCount() > target_keys) {
            int move_count = right_leaf->GetKeyCount() - target_keys;
            
            // 将右节点的前几个键值对移动到左节点
            for (int i = 0; i < move_count; ++i) {
                left_leaf->keys_.push_back(right_leaf->keys_[0]);
                left_leaf->record_ids_.push_back(right_leaf->record_ids_[0]);
                
                right_leaf->keys_.erase(right_leaf->keys_.begin());
                right_leaf->record_ids_.erase(right_leaf->record_ids_.begin());
            }
        }
        
        // 保存节点
        if (!SaveNode(left_leaf) || !SaveNode(right_leaf)) {
            std::cout << "DEBUG: Failed to save redistributed leaf nodes" << std::endl;
            return false;
        }
        
    } else {
        auto left_internal = std::static_pointer_cast<BPlusInternalNode>(left);
        auto right_internal = std::static_pointer_cast<BPlusInternalNode>(right);
        
        // 内部节点的重新分布逻辑类似
        int total_keys = left_internal->GetKeyCount() + right_internal->GetKeyCount();
        int target_keys = total_keys / 2;
        
        if (left_internal->GetKeyCount() > target_keys) {
            int move_count = left_internal->GetKeyCount() - target_keys;
            
            for (int i = 0; i < move_count; ++i) {
                int index = left_internal->GetKeyCount() - 1;
                right_internal->keys_.insert(right_internal->keys_.begin(), left_internal->keys_[index]);
                right_internal->child_page_ids_.insert(right_internal->child_page_ids_.begin(), 
                                                     left_internal->child_page_ids_[index + 1]);
                
                left_internal->keys_.erase(left_internal->keys_.begin() + index);
                left_internal->child_page_ids_.erase(left_internal->child_page_ids_.begin() + index + 1);
            }
        }
        else if (right_internal->GetKeyCount() > target_keys) {
            int move_count = right_internal->GetKeyCount() - target_keys;
            
            for (int i = 0; i < move_count; ++i) {
                left_internal->keys_.push_back(right_internal->keys_[0]);
                left_internal->child_page_ids_.push_back(right_internal->child_page_ids_[0]);
                
                right_internal->keys_.erase(right_internal->keys_.begin());
                right_internal->child_page_ids_.erase(right_internal->child_page_ids_.begin());
            }
        }
        
        // 保存节点
        if (!SaveNode(left_internal) || !SaveNode(right_internal)) {
            std::cout << "DEBUG: Failed to save redistributed internal nodes" << std::endl;
            return false;
        }
    }
    
    std::cout << "DEBUG: Nodes redistributed successfully" << std::endl;
    return true;
}

void BPlusTreeIndex::MergeNodes(std::shared_ptr<BPlusNode> left, std::shared_ptr<BPlusNode> right) {
    if (left->IsLeaf()) {
        MergeLeafNodes(std::static_pointer_cast<BPlusLeafNode>(left), 
                      std::static_pointer_cast<BPlusLeafNode>(right));
    } else {
        MergeInternalNodes(std::static_pointer_cast<BPlusInternalNode>(left), 
                          std::static_pointer_cast<BPlusInternalNode>(right));
    }
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

bool IndexManager::UpdateRecord(const std::string& table_name, const Record& old_record, 
                               const Record& new_record, int record_id) {
    std::cout << "DEBUG: IndexManager::UpdateRecord called for table=" << table_name << ", record_id=" << record_id << std::endl;
    
    // 先删除旧记录
    if (!DeleteRecord(table_name, old_record, record_id)) {
        std::cout << "DEBUG: Failed to delete old record" << std::endl;
        return false;
    }
    
    // 再插入新记录
    if (!InsertRecord(table_name, new_record, record_id)) {
        std::cout << "DEBUG: Failed to insert new record" << std::endl;
        return false;
    }
    
    std::cout << "DEBUG: Record updated successfully" << std::endl;
    return true;
}

bool IndexManager::DeleteRecord(const std::string& table_name, const Record& record, int record_id) {
    std::cout << "DEBUG: IndexManager::DeleteRecord called for table=" << table_name << ", record_id=" << record_id << std::endl;
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
            std::cout << "DEBUG: Deleting key=";
            if (std::holds_alternative<int>(key_value)) {
                std::cout << std::get<int>(key_value);
            } else if (std::holds_alternative<std::string>(key_value)) {
                std::cout << std::get<std::string>(key_value);
            } else if (std::holds_alternative<bool>(key_value)) {
                std::cout << (std::get<bool>(key_value) ? "true" : "false");
            } else {
                std::cout << "unknown type";
            }
            std::cout << ", record_id=" << record_id << " from index" << std::endl;
            
            index->Remove(key_value);
            std::cout << "DEBUG: Index delete completed" << std::endl;
        } else {
            std::cout << "DEBUG: Empty record values" << std::endl;
        }
    }
    return true;
}

# B+树分裂和合并逻辑测试

本目录包含了B+树分裂和合并逻辑的详细测试用例。

## 测试文件说明

### 1. test_bplus_tree_split.cpp
基本的分裂逻辑测试，包括：
- 基本叶子节点分裂
- 多次叶子节点分裂
- 内部节点分裂
- 分裂后数据完整性验证
- 字符串键分裂测试
- 边界情况测试
- 性能压力测试
- 索引管理器集成测试

### 2. test_bplus_tree_split_detailed.cpp
详细的分裂逻辑测试，包括：
- 精确边界分裂测试
- 多次分裂测试
- 字符串键分裂测试
- 内部节点分裂测试
- 随机键分裂测试
- 分裂后操作测试
- 边界情况测试
- 性能压力测试

### 3. test_bplus_tree_merge.cpp
节点合并和重新平衡测试，包括：
- 基本叶子节点合并
- 字符串键合并测试
- 内部节点合并测试
- 节点重新分布测试
- 边界情况测试
- 性能测试
- 索引管理器集成测试

## 编译和运行

### 编译测试
```bash
# 在项目根目录下
mkdir -p build
cd build
cmake ..
make

# 编译特定测试
make test_bplus_tree_split
make test_bplus_tree_split_detailed
make test_bplus_tree_merge
```

### 运行测试
```bash
# 运行所有B+树测试
./test_bplus_tree_split
./test_bplus_tree_split_detailed
./test_bplus_tree_merge

# 运行特定测试用例
./test_bplus_tree_split --gtest_filter="BPlusTreeSplitTest.BasicLeafSplit"
./test_bplus_tree_split_detailed --gtest_filter="BPlusTreeSplitDetailedTest.LeafSplitExactBoundary"
./test_bplus_tree_merge --gtest_filter="BPlusTreeMergeTest.BasicLeafMerge"
```

## 测试覆盖的功能

### 分裂逻辑
- ✅ 叶子节点分裂
- ✅ 内部节点分裂
- ✅ 分裂后父节点更新
- ✅ 分裂后数据完整性
- ✅ 多次分裂处理
- ✅ 不同数据类型的分裂

### 合并逻辑
- ✅ 叶子节点合并
- ✅ 内部节点合并
- ✅ 节点重新分布
- ✅ 合并后数据完整性
- ✅ 删除操作后的合并

### 边界情况
- ✅ 最小键数分裂
- ✅ 重复键处理
- ✅ 空节点处理
- ✅ 单键节点处理
- ✅ 随机键顺序

### 性能测试
- ✅ 大规模插入测试
- ✅ 混合操作测试
- ✅ 压力测试
- ✅ 内存使用测试

## 预期结果

所有测试都应该通过，表明：
1. B+树分裂逻辑正确实现
2. 分裂后数据完整性得到保证
3. 父节点更新机制正常工作
4. 节点合并和重新平衡功能正常
5. 各种边界情况得到正确处理
6. 性能表现符合预期

## 调试信息

测试运行时会输出详细的调试信息，包括：
- 分裂过程的详细信息
- 节点状态变化
- 数据完整性检查结果
- 性能统计信息

如果测试失败，请检查调试输出以确定问题所在。

## 注意事项

1. 每个测试都会创建独立的临时数据库文件
2. 测试完成后会自动清理临时文件
3. 如果测试过程中出现错误，请检查磁盘空间和权限
4. 某些测试可能需要较长时间完成（特别是性能测试）

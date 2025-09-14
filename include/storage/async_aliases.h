//
// 异步IO类型别名 - 让现有代码无缝切换到异步版本
//

#pragma once
#include "storage/async_disk_manager.h"
#include "storage/async_buffer_pool_manager.h"

// 类型别名，让现有代码可以直接使用异步版本
using DiskManager = AsyncDiskManager;
using BufferPoolManager = AsyncBufferPoolManager;

#pragma once

#include <sys/syscall.h>
#include <unistd.h>

namespace CurrentThread {
extern __thread int t_cachedTid;

void cacheTid();

inline int tid() {
  // 分支提示，大概率为0,以便编译器来优化
  if (__builtin_expect(t_cachedTid == 0, 0)) {
    cacheTid();
  }
  return t_cachedTid;
}
}  // namespace CurrentThread
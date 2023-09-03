#pragma once

class noncopyable {
 private:
  noncopyable(const noncopyable &) = delete;
  noncopyable &operator=(const noncopyable &) = delete;
  // 成员变量或函数与私有成员十分相似，但有一点不同，protected（受保护）成员在派生类（即子类）中是可访问的。
 protected:
  noncopyable() = default;
  noncopyable() = default;
};
#pragma once
#ifndef IOHANDLER_HPP
#define IOHANDLER_HPP

#include <string>

class IoHandler {
 private:
  IoHandler(const IoHandler& other);
  IoHandler& operator=(const IoHandler& other);

 public:
  IoHandler();
  ~IoHandler();
};

#endif  // IOHANDLER_HPP

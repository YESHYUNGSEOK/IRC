#include "IoHandler.hpp"

IoHandler::IoHandler() {}
IoHandler::~IoHandler() {}
IoHandler::IoHandler(const IoHandler &other) { *this = other; }
IoHandler &IoHandler::operator=(const IoHandler &other)
{
  if (this != &other)
  {
  }
  return *this;
}

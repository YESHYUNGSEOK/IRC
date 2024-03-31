#include "Utility.hpp"

#include "Client.hpp"

bool DerefLess::operator()(const Client *p1, const int i) const {
  return p1->get_fd() < i;
}

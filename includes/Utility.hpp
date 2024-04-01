#pragma once
#ifndef UTILITY_HPP
#define UTILITY_HPP

class Client;

struct DerefLess {
  template <class Ptr>
  bool operator()(Ptr const &p1, Ptr const &p2) const {
    return *p1 < *p2;
  }
  bool operator()(const Client *p1, const int i) const;
};

#endif

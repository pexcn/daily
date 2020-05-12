#include "netaggregate.hpp"
#include <algorithm>
#include <iostream>
#include <vector>

int main(int argc, char **argv) {
  if (argc > 1) {
    std::cout << "Programm accepts IP networks/addresses\ne.g. 3ffe::2/120 "
                 "10.10.0.1/16 3ffe::15 172.16.14.14\nas stdin and outputs "
                 "aggregated/summarized list to stdout"
              << std::endl;
    return 1;
  }
  std::vector<IPv6Net> ipv6s{};
  std::vector<IPv4Net> ipv4s{};
  for (std::string line; std::getline(std::cin, line);) {
    // min is '::' , max is 'ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff/128'
    if (line.length() < 2 || line.length() > 43) {
      std::cerr << "stdin wrong IP lenght, ignoring " << line << std::endl;
    } else if (line.find(':') != std::string::npos) {
      try {
        auto ip6 = IPv6Net(line);
        ipv6s.push_back(ip6);
      } catch (...) {
        std::cerr << "malformed IPv6: " << line
                  << ", ignoring" << std::endl;
      }
    } else if (line.find('.') != std::string::npos) {
      try {
        auto ip4 = IPv4Net(line);
        ipv4s.push_back(ip4);
      } catch (...) {
        std::cerr << "malformed IPv4: " << line
                  << ", ignoring" << std::endl;
      }
    } else {
      std::cerr << "stdin not an IPv6 or IPv4, ignoring " << line << std::endl;
    }
  }
  std::sort(ipv4s.begin(), ipv4s.end());
  auto result4 = CollapsNets(ipv4s);
  result4 = MergeNets(result4);
  for (auto i = result4.begin(); i != result4.end(); i++) {
    std::cout << i->net() << "/" << i->mask() << std::endl;
  }

  std::sort(ipv6s.begin(), ipv6s.end());
  auto result6 = CollapsNets(ipv6s);
  result6 = MergeNets(result6);
  for (auto i = result6.begin(); i != result6.end(); i++) {
    std::cout << i->net() << "/" << i->mask() << std::endl;
  }

  return 0;
}

#include <iostream>

#include "dns_cache.h"

int main() {
  DNSCache::InitDNSCache(10);
  DNSCache& d_c = DNSCache::GetDNSCache();
  return 0;
}

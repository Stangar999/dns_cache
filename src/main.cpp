#include <iostream>

#include "dns_cache.h"

int main() {
  DNSCache& d_c = DNSCache::GetDNSCache(10);
  DNSCache& d_c_1 = DNSCache::GetDNSCache();
  d_c_1.update("dns", "ip");
  return 0;
}

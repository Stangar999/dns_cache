#include <gtest/gtest.h>

#include <charconv>
#include <iostream>
#include <map>
#include <random>

#include "dns_cache.h"

class TestPrepare : public ::testing::Test {
 protected:
  static void SetUpTestSuite() {
    DNSCache::InitDNSCache(3);
  }
  static void TearDownTestSuite() {
  }
  void SetUp() {
  }
  void TearDown() {
  }

  DNSCache& d_c = DNSCache::GetDNSCache();
  std::mt19937 generator;
  std::uniform_int_distribution<char> uniform_dist_char{'A', 'Z'};
  std::uniform_int_distribution<unsigned> uniform_dist_u{0, 255};
};

TEST_F(TestPrepare, SaveIp) {
  std::map<std::string, std::string> holder;
  for (int i = 0; i < 3; ++i) {
    std::string dns;
    for (int j = 0; j < 3; ++j) {
      dns += uniform_dist_char(generator);
    }
    std::string ip;
    for (int j = 0; j < 4; ++j) {
      std::string path;
      path.resize(3);
      auto [ptr, ec] = std::to_chars(path.data(), path.data() + path.size(), uniform_dist_char(generator));
      ip.append(path.data(), ptr);
      ip += '.';
    }
    ip.pop_back();
    holder.emplace(dns, ip);
    d_c.update(dns, ip);
  }
  for (auto& [dns, ip] : holder) {
    ASSERT_EQ(d_c.resolve(dns), ip);
  }
}

TEST_F(TestPrepare, DeleteLastIP) {
  std::map<std::string, std::string> holder_true;
  std::map<std::string, std::string> holder_empty;

  for (int i = 0; i < 20; ++i) {
    std::string dns;
    for (int j = 0; j < 3; ++j) {
      dns += uniform_dist_char(generator);
    }
    std::string ip;
    for (int j = 0; j < 4; ++j) {
      std::string path;
      path.resize(3);
      auto [ptr, ec] = std::to_chars(path.data(), path.data() + path.size(), uniform_dist_char(generator));
      ip.append(path.data(), ptr);
      ip += '.';
    }
    ip.pop_back();
    if (i < 17) {
      holder_empty.emplace(dns, ip);
    }
    if (i >= 17) {
      holder_true.emplace(dns, ip);
    }
    d_c.update(dns, ip);
  }
  for (auto& [dns, ip] : holder_true) {
    ASSERT_EQ(d_c.resolve(dns), ip);
  }
  for (auto& [dns, ip] : holder_empty) {
    ASSERT_EQ(d_c.resolve(dns), "");
  }
}

TEST(CrossUpdate, ALL) {
  DNSCache& d_c = DNSCache::GetDNSCache();
  d_c.update("dns1", "ip1");
  d_c.update("dns2", "ip2");
  d_c.update("dns3", "ip3");

  d_c.update("dns2", "ip2");
  ASSERT_EQ(d_c.resolve("dns2"), "ip2");
  ASSERT_EQ(d_c.resolve("dns3"), "ip3");
  ASSERT_EQ(d_c.resolve("dns1"), "ip1");

  d_c.update("dns2", "ip1");
  ASSERT_EQ(d_c.resolve("dns2"), "ip1");
  ASSERT_EQ(d_c.resolve("dns1"), "");
  ASSERT_EQ(d_c.resolve("dns3"), "ip3");

  d_c.update("dns2", "ip4");
  ASSERT_EQ(d_c.resolve("dns2"), "ip4");
  ASSERT_EQ(d_c.resolve("dns1"), "");
  ASSERT_EQ(d_c.resolve("dns3"), "ip3");
}

TEST(UpdateHistoryOnUpdate, ALL) {
  DNSCache& d_c = DNSCache::GetDNSCache();
  d_c.update("dns1", "ip1");
  d_c.update("dns2", "ip2");
  d_c.update("dns3", "ip3");

  d_c.update("dns1", "ip1");
  d_c.update("dns4", "ip4");
  ASSERT_EQ(d_c.resolve("dns4"), "ip4");
  ASSERT_EQ(d_c.resolve("dns1"), "ip1");
  ASSERT_EQ(d_c.resolve("dns3"), "ip3");
  ASSERT_EQ(d_c.resolve("dns2"), "");
}

TEST(UpdateHistoryOnResolve, ALL) {
  DNSCache& d_c = DNSCache::GetDNSCache();
  d_c.update("dns1", "ip1");
  d_c.update("dns2", "ip2");
  d_c.update("dns3", "ip3");

  d_c.resolve("dns1");
  d_c.update("dns4", "ip4");
  ASSERT_EQ(d_c.resolve("dns4"), "ip4");
  ASSERT_EQ(d_c.resolve("dns1"), "ip1");
  ASSERT_EQ(d_c.resolve("dns3"), "ip3");
  ASSERT_EQ(d_c.resolve("dns2"), "");
}

int main(int argc, char* argv[]) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <list>
#include <memory>
#include <mutex>
#include <queue>
#include <shared_mutex>
#include <string>
#include <unordered_map>

class DNSCache {
 public:
  static void InitDNSCache(size_t max_size) {
    static std::unique_ptr<DNSCache, Deleter> p_dn_cache(new DNSCache(max_size));
    _p = p_dn_cache.get();
  }
  static DNSCache& GetDNSCache() {
    return *_p;
  }

  // O(1)
  void update(const std::string& name, const std::string& ip);
  // O(1)
  std::string resolve(const std::string& name);

 private:
  struct Deleter {
    void operator()(DNSCache* p) {
      delete p;
    }
  };
  using L_it = std::list<std::string>::iterator;

  struct IpIt {
    std::string ip;
    mutable L_it it_on_history;
  };

  struct HashIpIt {
    size_t operator()(const IpIt& ip_it) const {
      return std::hash<std::string>{}(ip_it.ip);
    }
  };

  struct IsEqualIpIt {
    size_t operator()(const IpIt& lft, const IpIt& rht) const {
      return lft.ip == rht.ip;
    }
  };

  using Um_it = std::unordered_map<std::string, IpIt>::iterator;
  using DnsIpDict =
      boost::bimap<boost::bimaps::unordered_set_of<boost::bimaps::tagged<std::string, struct DNS>>,
                   boost::bimaps::unordered_set_of<boost::bimaps::tagged<IpIt, IpIt>, HashIpIt, IsEqualIpIt>>;

  explicit DNSCache(size_t max_size);
  ~DNSCache() = default;
  DNSCache(const DNSCache&) = delete;
  DNSCache& operator=(const DNSCache&) = delete;

  void check_size_and_insert(const std::string& name, const std::string& ip);

  template <typename It, typename DataType>
  void check_exist_and_delete_old_data(It it) {
    if (it != _dns_ip_hold.by<DataType>().end()) {
      const auto& [dns, ip_it] = *it;
      _history.erase(ip_it.it_on_history);  // удаляет имя домена из истории что бы потом записать его в начало
      _dns_ip_hold.by<DNS>().erase(dns);
    }
  }

  // перенести имя в начало истории
  void update_history(const IpIt& ip_it);
  // максимальный размер хранения
  size_t _max_size;
  // история запросов и обновлений
  std::list<std::string> _history;
  // хранилище dns и ip, один dns один ip и наоборот (один ip один dns),
  // если придет новый dns с занятым ip то старый
  // dns освободиться, а новый запишется
  DnsIpDict _dns_ip_hold;
  std::mutex _m;
  inline static DNSCache* _p = nullptr;
};

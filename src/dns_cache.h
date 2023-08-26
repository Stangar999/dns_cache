#include <list>
#include <memory>
#include <mutex>
#include <string>
#include <unordered_map>

class DNSCache {
 public:
  // 10 заглушка что бы размер не надо было передавать каждый раз
  // в ветке сделал по другому
  static DNSCache& GetDNSCache(size_t max_size = 10) {
    static std::unique_ptr<DNSCache> p_dn_cache(new DNSCache(max_size));
    return *p_dn_cache;
  }

  // O(1)
  void update(const std::string& name, const std::string& ip);
  // O(1)
  std::string resolve(const std::string& name);

 private:
  using L_it = std::list<const std::string*>::iterator;
  struct IpIt {
    std::string ip;
    L_it it_on_history;
  };
  using Um_it = std::unordered_map<std::string, IpIt>::iterator;

  // синглтон
  explicit DNSCache(size_t max_size);
  DNSCache(const DNSCache&) = delete;
  DNSCache& operator=(const DNSCache&) = delete;

  // проверят размер и вставляет
  void check_size_and_insert(const std::string& name, const std::string& ip);
  // перенести имя в начало истории
  void update_history(Um_it& dns_ip_it);
  // максимальный размер хранения
  size_t _max_size;
  // история запросов и обновлений
  std::list<const std::string*> _history;
  // хранилище dns и ip c итератором на историю
  std::unordered_map<std::string, IpIt> _dns_ip_holder;
  std::mutex m;
};

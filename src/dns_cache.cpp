#include "dns_cache.h"

#include <iostream>

DNSCache::DNSCache(size_t max_size) : _max_size(max_size) {
}

void DNSCache::update(const std::string& name, const std::string& ip) {
  std::unique_lock l(_m);

  // если обновления по данным есть
  auto it_dns = _dns_ip_hold.by<DNS>().find(name);
  check_exist_and_delete_old_data<decltype(it_dns), DNS>(it_dns);

  auto it_ip_it = _dns_ip_hold.by<IpIt>().find(IpIt{.ip = ip});
  check_exist_and_delete_old_data<decltype(it_ip_it), IpIt>(it_ip_it);

  check_size_and_insert(name, ip);
}

std::string DNSCache::resolve(const std::string& name) {
  std::unique_lock l(_m);
  if (auto it = _dns_ip_hold.by<DNS>().find(name); it != _dns_ip_hold.by<DNS>().end()) {
    const auto& [dns, ip_it] = *it;
    update_history(ip_it);
    return ip_it.ip;
  }
  return {};
}

void DNSCache::check_size_and_insert(const std::string& name, const std::string& ip) {
  if (_dns_ip_hold.size() >= _max_size && !_dns_ip_hold.empty()) {
    _dns_ip_hold.by<DNS>().erase(std::string(_history.back()));
    _history.pop_back();
  }
  if (_dns_ip_hold.size() < _max_size) {
    // сохраняем имя, что бы при превышении max_size по нему из хранилища удалить самую старую
    _history.push_front(name);
    _dns_ip_hold.insert({name, IpIt{.ip = ip, .it_on_history = _history.begin()}});
  }
}

void DNSCache::update_history(const IpIt& ip_it) {
  std::string dns = *ip_it.it_on_history;
  _history.erase(ip_it.it_on_history);
  _history.push_front(std::move(dns));
  ip_it.it_on_history = _history.begin();
}

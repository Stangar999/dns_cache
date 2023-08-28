#include "dns_cache.h"

DNSCache::DNSCache(size_t max_size) : _max_size(max_size) {
}

void DNSCache::update(const std::string& name, const std::string& ip) {
  std::unique_lock l(m);
  if (auto dns_ip_it = _dns_ip_holder.find(name); dns_ip_it != _dns_ip_holder.end()) {  // обновить элемент
    auto& [_, ip_it] = *dns_ip_it;
    ip_it.ip = ip;              // обновить ip
    update_history(dns_ip_it);  // поднять в истории
  } else {
    check_size_and_insert(name, ip);
  }
}

std::string DNSCache::resolve(const std::string& name) {
  std::unique_lock l(m);
  if (auto dns_ip_it = _dns_ip_holder.find(name); dns_ip_it != _dns_ip_holder.end()) {
    update_history(dns_ip_it);
    return dns_ip_it->second.ip;
  }
  return {};
}

void DNSCache::check_size_and_insert(const std::string& name, const std::string& ip) {
  if (_dns_ip_holder.size() >= _max_size && !_dns_ip_holder.empty()) {
    _dns_ip_holder.erase(*_history.back());  // тут string_view приходиться выделять в std::string поэтому string*
    _history.pop_back();
  }
  if (_dns_ip_holder.size() < _max_size) {
    auto [dns_ip_it, flag] = _dns_ip_holder.emplace(name, IpIt{.ip = ip});
    auto& [key_dns_name, ip_it] = *dns_ip_it;
    _history.push_front(
        &key_dns_name);  // добавляем в лист имя домена, что бы при превышении max_size по нему удалить ip
    ip_it.it_on_history = _history.begin();  // добавляем в ip_holder информацию о том в каком узле лежит имя домена что
    // бы при обновлении его за константу найти его и положить в начало списка
  }
}

void DNSCache::update_history(Um_it& dns_ip_it) {
  auto& [key_dns, ip_it] = *dns_ip_it;
  _history.erase(ip_it.it_on_history);  // удаляем имя домена из листа что бы записать его в начало
  _history.push_front(&key_dns);  // записываем в начало имя домена
  ip_it.it_on_history = _history.begin();  // обновляем итератор на перемещенный в начало элемент
}

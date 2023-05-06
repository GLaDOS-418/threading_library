#ifndef CONCURRENT_HASHMAP_H
#define CONCURRENT_HASHMAP_H

#include <iostream>
#include <array>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <optional>
#include <utility>
#include <atomic>

namespace ds { 
  template <class KeyT, class ValueT, class HashFn = std::hash<KeyT>,  const size_t BUCKETS = 1031>
  class ConcurrentHashMap {
    struct Bucket {
      std::unordered_map<KeyT,ValueT> bucket_;
      mutable std::shared_mutex rwlock_;
    };

    std::array<Bucket, BUCKETS> m_buckets;
    HashFn m_hasher;
    std::atomic<size_t> m_size{ 0 };

    inline size_t get_bucket( const KeyT& key) const 
    { return m_hasher(key) % BUCKETS; }

    public:

    void insert( KeyT&& key, ValueT&& value) {
      auto bucket_id = get_bucket(key);

      auto& bucket   = m_buckets[bucket_id].bucket_;
      auto& rwlock   = m_buckets[bucket_id].rwlock_;

      std::lock_guard<std::shared_mutex> guard(rwlock);
      bucket.emplace(std::forward<KeyT>(key), std::forward<ValueT>(value));
      ++m_size;
    }

    std::optional<ValueT> remove( const KeyT& key ){
      auto  bucket_id = get_bucket(key);
      auto& bucket    = m_buckets[bucket_id].bucket_;
      auto& rwlock    = m_buckets[bucket_id].rwlock_;

      std::lock_guard<std::shared_mutex> guard(rwlock);
      auto pos = bucket.find(key);
      if( pos != bucket.end( )){
        auto retVal = std::move(pos->second);
        bucket.erase( pos );
        --m_size;

        return { retVal };
      }

      return { }; // key not found
    }

    std::optional<ValueT> get( const KeyT& key) const {
      auto bucket_id     = get_bucket(key);
      const auto& bucket = m_buckets[bucket_id].bucket_;
      auto& rwlock       = m_buckets[bucket_id].rwlock_;

      std::shared_lock<std::shared_mutex> guard(rwlock);

      auto pos = bucket.find( key );
      if( pos != bucket.end( ) )
        return {pos->second};

      return { };
    }

    size_t was_size( ) const {
      return m_size;
    }

    bool was_empty( ) const {
      return !m_size;
    }

  };
}
#endif //CONCURRENT_HASMAP

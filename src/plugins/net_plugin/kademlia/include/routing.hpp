//        Copyright The Authors 2018.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

//   https://github.com/abdes/blocxxi

#pragma once

#include <chrono>
#include <deque>
#include <forward_list>
#include <mutex>
#include <optional>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include <boost/iterator/iterator_adaptor.hpp>
#include <boost/iterator/reverse_iterator.hpp>

#include "../../config/include/network_config.hpp"
#include "kbucket.hpp"

namespace tethys {
namespace net_plugin {

class RoutingTable {
public:
  template <typename TValue, typename TIterator>
  class BucketIterator
      : public boost::iterator_adaptor<BucketIterator<TValue, TIterator>, TIterator, TValue, std::bidirectional_iterator_tag> {
  public:
    BucketIterator() : BucketIterator::iterator_adaptor_() {}
    explicit BucketIterator(TIterator node) : BucketIterator::iterator_adaptor_(node) {}

    template <class OtherValue>
    BucketIterator(BucketIterator<OtherValue, TIterator> const &other,
                   typename std::enable_if<std::is_convertible<OtherValue *, TValue *>::value>::type)
        : BucketIterator::iterator_adaptor_(other.base()) {}
  };

  using iterator = BucketIterator<KBucket, std::deque<KBucket>::iterator>;
  using const_iterator = BucketIterator<KBucket const, std::deque<KBucket>::const_iterator>;
  using reverse_iterator = boost::reverse_iterator<iterator>;
  using const_reverse_iterator = boost::reverse_iterator<const_iterator>;

public:
  RoutingTable(Node node, std::size_t ksize);

  RoutingTable(const RoutingTable &) = delete;

  RoutingTable &operator=(const RoutingTable &) = delete;

  RoutingTable(RoutingTable &&) = default;

  RoutingTable &operator=(RoutingTable &&) = default;

  ~RoutingTable() = default;

  iterator begin() noexcept {
    return iterator(m_buckets.begin());
  };
  const_iterator begin() const noexcept {
    return const_iterator(m_buckets.cbegin());
  }
  iterator end() noexcept {
    return iterator(m_buckets.end());
  }
  const_iterator end() const noexcept {
    return const_iterator(m_buckets.cend());
  }

  reverse_iterator rbegin() noexcept {
    return reverse_iterator(end());
  }
  const_reverse_iterator rbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  reverse_iterator rend() noexcept {
    return reverse_iterator(begin());
  }
  const_reverse_iterator rend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  const_iterator cbegin() const noexcept {
    return const_iterator(m_buckets.cbegin());
  }
  const_iterator cend() const noexcept {
    return const_iterator(m_buckets.cend());
  }
  const_reverse_iterator crbegin() const noexcept {
    return const_reverse_iterator(cend());
  }
  const_reverse_iterator crend() const noexcept {
    return const_reverse_iterator(cbegin());
  }

  const Node &thisNode() const {
    return m_my_node;
  }

  std::size_t nodesCount() const;

  std::size_t bucketsCount() const;

  bool empty() const;

  bool addPeer(Node &&peer);

  void removePeer(const Node &peer);

  bool peerTimedOut(Node const &peer);

  std::vector<Node> findNeighbors(hashed_net_id_type const &id) {
    return findNeighbors(id, m_ksize);
  };

  std::vector<Node> findNeighbors(hashed_net_id_type const &hashed_id, std::size_t max_number);

  std::optional<Node> findNode(const hashed_net_id_type &hashed_id);

  size_t getBucketIndexFor(const hashed_net_id_type &node) const;
private:
  Node m_my_node;

  std::size_t m_ksize;

  std::deque<KBucket> m_buckets;

  std::mutex m_buckets_mutex;
};

std::ostream &operator<<(std::ostream &out, RoutingTable const &rt);

} // namespace net_plugin
} // namespace tethys

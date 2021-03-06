//        Copyright The Authors 2018.
//    Distributed under the 3-Clause BSD License.
//    (See accompanying file LICENSE or copy at
//   https://opensource.org/licenses/BSD-3-Clause)

//   https://github.com/abdes/blocxxi

#include <sstream> // for ToString() implementation

#include <boost/multiprecision/cpp_int.hpp>

#include "include/routing.hpp"

namespace tethys {
namespace net_plugin {

using boost::multiprecision::cpp_int_backend;
using boost::multiprecision::number;
using boost::multiprecision::unchecked;
using boost::multiprecision::unsigned_magnitude;

constexpr unsigned int DEPTH_B = 5;

RoutingTable::RoutingTable(Node node, std::size_t ksize) : m_my_node(std::move(node)), m_ksize(ksize) {
  m_buckets.emplace_front(KBucket(m_my_node, 0, m_ksize));
}

std::size_t RoutingTable::nodesCount() const {
  std::size_t total = 0;
  for (auto &kb : m_buckets)
    total += kb.size().first;
  return total;
}

std::size_t RoutingTable::bucketsCount() const {
  return m_buckets.size();
}

bool RoutingTable::empty() const {
  return m_buckets.front().empty();
}

std::size_t RoutingTable::getBucketIndexFor(const hashed_net_id_type &node) const {
  auto num_buckets = m_buckets.size();

  auto bucket = m_buckets.begin();
  while (bucket != m_buckets.end()) {
    if (bucket->canHoldNode(node)) {
      auto bucket_index = static_cast<std::size_t>(std::distance(m_buckets.begin(), bucket));

      return bucket_index;
    }
    ++bucket;
  }

  return num_buckets - 1;
}

bool RoutingTable::addPeer(Node &&peer) {
  if (m_my_node == peer) {
    return true;
  }

  peer.initReqFailuresCount();

  {
    std::lock_guard<std::mutex> lock(m_buckets_mutex);
    auto bucket_index = getBucketIndexFor(peer.getIdHash());
    auto bucket = m_buckets.begin();
    std::advance(bucket, bucket_index);
    if (bucket->addNode(std::move(peer)))
      return true;

    auto can_split = false;

    auto bucket_has_in_range_my_node = (bucket_index == (m_buckets.size() - 1));
    can_split |= bucket_has_in_range_my_node;

    auto shared_prefix_test = (bucket->depth() < DEPTH_B) && ((bucket->depth() % DEPTH_B) != 0);
    can_split |= shared_prefix_test;

    can_split &= (m_buckets.size() < 160);

    can_split &= !(m_buckets.size() > 1 && bucket_index == 0);

    if (can_split) {
      std::pair<KBucket, KBucket> pair = bucket->split();
      bucket = m_buckets.insert(bucket, std::move(pair.second));
      bucket = m_buckets.insert(bucket, std::move(pair.first));

      m_buckets.erase(bucket + 2);

      return true;
    }
  }
  return false;
}

void RoutingTable::removePeer(const Node &peer) {
  {
    std::lock_guard<std::mutex> lock(m_buckets_mutex);
    auto bucket_index = getBucketIndexFor(peer.getIdHash());
    auto bucket = m_buckets.begin();
    std::advance(bucket, bucket_index);

    bucket->removeNode(peer);
  }
}

bool RoutingTable::peerTimedOut(Node const &peer) {
  for (auto bucket = m_buckets.rbegin(); bucket != m_buckets.rend(); ++bucket) {
    for (auto bn = bucket->begin(); bn != bucket->end(); ++bn) {
      if (bn->getIdHash() == peer.getIdHash()) {
        if (bn->isStale()) {
          {
            std::lock_guard<std::mutex> lock(m_buckets_mutex);
            bucket->removeNode(bn);
          }

          return true;
        } else {
          {
            std::lock_guard<std::mutex> lock(m_buckets_mutex);
            bn->incReqFailuresCount();
          }

          return false;
        }
      }
    }
  }

  return false;
}

std::optional<Node> RoutingTable::findNode(const hashed_net_id_type &hashed_id) {
  {
    std::lock_guard<std::mutex> lock(m_buckets_mutex);

    auto bucket_index = getBucketIndexFor(hashed_id);
    auto bucket = m_buckets.begin();
    std::advance(bucket, bucket_index);

    for (auto &node : *bucket) {
      if (hashed_id == node.getIdHash()) {
        return node;
      }
    }
    return {};
  }
}

std::vector<Node> RoutingTable::findNeighbors(hashed_net_id_type const &hashed_id, std::size_t max_number) {
  std::vector<Node> neighbors;

  bool use_left = true;
  bool has_more = true;

  {
    std::lock_guard<std::mutex> lock(m_buckets_mutex);

    auto bucket_index = getBucketIndexFor(hashed_id);
    auto bucket = m_buckets.begin();
    std::advance(bucket, bucket_index);
    auto left = bucket;
    auto right = (bucket != m_buckets.end()) ? bucket + 1 : bucket;

    auto current_bucket = bucket;

    while (has_more) {
      has_more = false;

      for (auto const &neighbor : *current_bucket) {
        // Exclude the node
        if (neighbor.getIdHash() != hashed_id) {
          neighbors.emplace_back(neighbor);

          if (neighbors.size() == max_number)
            return neighbors;
        }
      }

      if (right == m_buckets.end())
        use_left = true;

      if (left != m_buckets.begin()) {
        has_more = true;
        if (use_left) {
          --left;
          current_bucket = left;
          use_left = false;
          continue;
        }
      }

      if (right != m_buckets.end()) {
        has_more = true;
        current_bucket = right;
        ++right;
      }
      use_left = true;
    }

    return neighbors;
  }
}
} // namespace net_plugin
} // namespace tethys

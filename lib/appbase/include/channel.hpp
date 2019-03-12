#pragma once

#include <memory>
#include <boost/signals2.hpp>
#include <boost/asio/io_context.hpp>

namespace appbase {
  using namespace boost::signals2;
  using namespace boost::asio;

  class AbstractChannel {
  public:
    virtual ~AbstractChannel() = default;
  };

  template<typename Data>
  class Channel : public AbstractChannel {
  public:
    Channel(std::shared_ptr<io_context> &p) : ioc_ptr(p) {}

    class Handle {
    public:
      ~Handle() {
        unsubscribe();
      }

      Handle() = default;

      Handle(Handle &&) = default;

      Handle &operator=(Handle &&rhs) = default;

      Handle(const Handle &) = delete;

      Handle &operator=(const Handle &) = delete;

      void unsubscribe() {
        if (handle.connected()) {
          handle.disconnect();
        }
      }

    private:
      using handle_type = connection;

      explicit Handle(handle_type &&handle) : handle(std::move(handle)) {}

      handle_type handle;

      friend class Channel;
    };

    template<typename Callback>
    Handle subscribe(Callback callback) {
      return Handle(s.connect(callback));
    }

    void publish(Data &data) {
      if (has_subscribers()) {
        ioc_ptr->post([this, data]() {
          s(data);
        });
      }
    }

  private:
    bool has_subscribers() {
      return s.num_slots() > 0;
    }

    std::shared_ptr<io_context> ioc_ptr;
    signal<void(const Data &)> s;
  };

  template<typename Data>
  struct ChannelTypeTemplate {
    using channel_type = Channel<Data>;
    using data_type = Data;
  };
}

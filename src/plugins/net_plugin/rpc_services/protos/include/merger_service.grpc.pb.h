// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: merger_service.proto
#ifndef GRPC_merger_5fservice_2eproto__INCLUDED
#define GRPC_merger_5fservice_2eproto__INCLUDED

#include "merger_service.pb.h"

#include <grpcpp/impl/codegen/async_generic_service.h>
#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/proto_utils.h>
#include <grpcpp/impl/codegen/rpc_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/status.h>
#include <grpcpp/impl/codegen/stub_options.h>
#include <grpcpp/impl/codegen/sync_stream.h>

namespace grpc {
class CompletionQueue;
class Channel;
class ServerCompletionQueue;
class ServerContext;
}  // namespace grpc

namespace grpc_merger {

class TethysMergerService final {
 public:
  static constexpr char const* service_full_name() {
    return "grpc_merger.TethysMergerService";
  }
  class StubInterface {
   public:
    virtual ~StubInterface() {}
    virtual ::grpc::Status MergerService(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc_merger::MsgStatus* response) = 0;
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::grpc_merger::MsgStatus>> AsyncMergerService(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::grpc_merger::MsgStatus>>(AsyncMergerServiceRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::grpc_merger::MsgStatus>> PrepareAsyncMergerService(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReaderInterface< ::grpc_merger::MsgStatus>>(PrepareAsyncMergerServiceRaw(context, request, cq));
    }
  private:
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::grpc_merger::MsgStatus>* AsyncMergerServiceRaw(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) = 0;
    virtual ::grpc::ClientAsyncResponseReaderInterface< ::grpc_merger::MsgStatus>* PrepareAsyncMergerServiceRaw(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) = 0;
  };
  class Stub final : public StubInterface {
   public:
    Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel);
    ::grpc::Status MergerService(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc_merger::MsgStatus* response) override;
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::grpc_merger::MsgStatus>> AsyncMergerService(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::grpc_merger::MsgStatus>>(AsyncMergerServiceRaw(context, request, cq));
    }
    std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::grpc_merger::MsgStatus>> PrepareAsyncMergerService(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) {
      return std::unique_ptr< ::grpc::ClientAsyncResponseReader< ::grpc_merger::MsgStatus>>(PrepareAsyncMergerServiceRaw(context, request, cq));
    }

   private:
    std::shared_ptr< ::grpc::ChannelInterface> channel_;
    ::grpc::ClientAsyncResponseReader< ::grpc_merger::MsgStatus>* AsyncMergerServiceRaw(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) override;
    ::grpc::ClientAsyncResponseReader< ::grpc_merger::MsgStatus>* PrepareAsyncMergerServiceRaw(::grpc::ClientContext* context, const ::grpc_merger::RequestMsg& request, ::grpc::CompletionQueue* cq) override;
    const ::grpc::internal::RpcMethod rpcmethod_MergerService_;
  };
  static std::unique_ptr<Stub> NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options = ::grpc::StubOptions());

  class Service : public ::grpc::Service {
   public:
    Service();
    virtual ~Service();
    virtual ::grpc::Status MergerService(::grpc::ServerContext* context, const ::grpc_merger::RequestMsg* request, ::grpc_merger::MsgStatus* response);
  };
  template <class BaseClass>
  class WithAsyncMethod_MergerService : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithAsyncMethod_MergerService() {
      ::grpc::Service::MarkMethodAsync(0);
    }
    ~WithAsyncMethod_MergerService() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status MergerService(::grpc::ServerContext* context, const ::grpc_merger::RequestMsg* request, ::grpc_merger::MsgStatus* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestMergerService(::grpc::ServerContext* context, ::grpc_merger::RequestMsg* request, ::grpc::ServerAsyncResponseWriter< ::grpc_merger::MsgStatus>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  typedef WithAsyncMethod_MergerService<Service > AsyncService;
  template <class BaseClass>
  class WithGenericMethod_MergerService : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithGenericMethod_MergerService() {
      ::grpc::Service::MarkMethodGeneric(0);
    }
    ~WithGenericMethod_MergerService() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status MergerService(::grpc::ServerContext* context, const ::grpc_merger::RequestMsg* request, ::grpc_merger::MsgStatus* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
  };
  template <class BaseClass>
  class WithRawMethod_MergerService : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithRawMethod_MergerService() {
      ::grpc::Service::MarkMethodRaw(0);
    }
    ~WithRawMethod_MergerService() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable synchronous version of this method
    ::grpc::Status MergerService(::grpc::ServerContext* context, const ::grpc_merger::RequestMsg* request, ::grpc_merger::MsgStatus* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    void RequestMergerService(::grpc::ServerContext* context, ::grpc::ByteBuffer* request, ::grpc::ServerAsyncResponseWriter< ::grpc::ByteBuffer>* response, ::grpc::CompletionQueue* new_call_cq, ::grpc::ServerCompletionQueue* notification_cq, void *tag) {
      ::grpc::Service::RequestAsyncUnary(0, context, request, response, new_call_cq, notification_cq, tag);
    }
  };
  template <class BaseClass>
  class WithStreamedUnaryMethod_MergerService : public BaseClass {
   private:
    void BaseClassMustBeDerivedFromService(const Service *service) {}
   public:
    WithStreamedUnaryMethod_MergerService() {
      ::grpc::Service::MarkMethodStreamed(0,
        new ::grpc::internal::StreamedUnaryHandler< ::grpc_merger::RequestMsg, ::grpc_merger::MsgStatus>(std::bind(&WithStreamedUnaryMethod_MergerService<BaseClass>::StreamedMergerService, this, std::placeholders::_1, std::placeholders::_2)));
    }
    ~WithStreamedUnaryMethod_MergerService() override {
      BaseClassMustBeDerivedFromService(this);
    }
    // disable regular version of this method
    ::grpc::Status MergerService(::grpc::ServerContext* context, const ::grpc_merger::RequestMsg* request, ::grpc_merger::MsgStatus* response) override {
      abort();
      return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
    }
    // replace default version of method with streamed unary
    virtual ::grpc::Status StreamedMergerService(::grpc::ServerContext* context, ::grpc::ServerUnaryStreamer< ::grpc_merger::RequestMsg,::grpc_merger::MsgStatus>* server_unary_streamer) = 0;
  };
  typedef WithStreamedUnaryMethod_MergerService<Service > StreamedUnaryService;
  typedef Service SplitStreamedService;
  typedef WithStreamedUnaryMethod_MergerService<Service > StreamedService;
};

}  // namespace grpc_merger


#endif  // GRPC_merger_5fservice_2eproto__INCLUDED

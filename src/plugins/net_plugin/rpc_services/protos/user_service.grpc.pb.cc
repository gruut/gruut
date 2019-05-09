// Generated by the gRPC C++ plugin.
// If you make any local change, they will be lost.
// source: user_service.proto

#include "include/user_service.grpc.pb.h"
#include "include/user_service.pb.h"

#include <grpcpp/impl/codegen/async_stream.h>
#include <grpcpp/impl/codegen/async_unary_call.h>
#include <grpcpp/impl/codegen/channel_interface.h>
#include <grpcpp/impl/codegen/client_unary_call.h>
#include <grpcpp/impl/codegen/method_handler_impl.h>
#include <grpcpp/impl/codegen/rpc_service_method.h>
#include <grpcpp/impl/codegen/service_type.h>
#include <grpcpp/impl/codegen/sync_stream.h>
namespace grpc_user {

static const char* GruutUserService_method_names[] = {
  "/grpc_user.GruutUserService/OpenChannel",
  "/grpc_user.GruutUserService/KeyExService",
  "/grpc_user.GruutUserService/UserService",
};

std::unique_ptr< GruutUserService::Stub> GruutUserService::NewStub(const std::shared_ptr< ::grpc::ChannelInterface>& channel, const ::grpc::StubOptions& options) {
  (void)options;
  std::unique_ptr< GruutUserService::Stub> stub(new GruutUserService::Stub(channel));
  return stub;
}

GruutUserService::Stub::Stub(const std::shared_ptr< ::grpc::ChannelInterface>& channel)
  : channel_(channel), rpcmethod_OpenChannel_(GruutUserService_method_names[0], ::grpc::internal::RpcMethod::BIDI_STREAMING, channel)
  , rpcmethod_KeyExService_(GruutUserService_method_names[1], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  , rpcmethod_UserService_(GruutUserService_method_names[2], ::grpc::internal::RpcMethod::NORMAL_RPC, channel)
  {}

::grpc::ClientReaderWriter< ::grpc_user::Identity, ::grpc_user::Message>* GruutUserService::Stub::OpenChannelRaw(::grpc::ClientContext* context) {
  return ::grpc::internal::ClientReaderWriterFactory< ::grpc_user::Identity, ::grpc_user::Message>::Create(channel_.get(), rpcmethod_OpenChannel_, context);
}

::grpc::ClientAsyncReaderWriter< ::grpc_user::Identity, ::grpc_user::Message>* GruutUserService::Stub::AsyncOpenChannelRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq, void* tag) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::grpc_user::Identity, ::grpc_user::Message>::Create(channel_.get(), cq, rpcmethod_OpenChannel_, context, true, tag);
}

::grpc::ClientAsyncReaderWriter< ::grpc_user::Identity, ::grpc_user::Message>* GruutUserService::Stub::PrepareAsyncOpenChannelRaw(::grpc::ClientContext* context, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncReaderWriterFactory< ::grpc_user::Identity, ::grpc_user::Message>::Create(channel_.get(), cq, rpcmethod_OpenChannel_, context, false, nullptr);
}

::grpc::Status GruutUserService::Stub::KeyExService(::grpc::ClientContext* context, const ::grpc_user::Request& request, ::grpc_user::Reply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_KeyExService_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::grpc_user::Reply>* GruutUserService::Stub::AsyncKeyExServiceRaw(::grpc::ClientContext* context, const ::grpc_user::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::grpc_user::Reply>::Create(channel_.get(), cq, rpcmethod_KeyExService_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::grpc_user::Reply>* GruutUserService::Stub::PrepareAsyncKeyExServiceRaw(::grpc::ClientContext* context, const ::grpc_user::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::grpc_user::Reply>::Create(channel_.get(), cq, rpcmethod_KeyExService_, context, request, false);
}

::grpc::Status GruutUserService::Stub::UserService(::grpc::ClientContext* context, const ::grpc_user::Request& request, ::grpc_user::Reply* response) {
  return ::grpc::internal::BlockingUnaryCall(channel_.get(), rpcmethod_UserService_, context, request, response);
}

::grpc::ClientAsyncResponseReader< ::grpc_user::Reply>* GruutUserService::Stub::AsyncUserServiceRaw(::grpc::ClientContext* context, const ::grpc_user::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::grpc_user::Reply>::Create(channel_.get(), cq, rpcmethod_UserService_, context, request, true);
}

::grpc::ClientAsyncResponseReader< ::grpc_user::Reply>* GruutUserService::Stub::PrepareAsyncUserServiceRaw(::grpc::ClientContext* context, const ::grpc_user::Request& request, ::grpc::CompletionQueue* cq) {
  return ::grpc::internal::ClientAsyncResponseReaderFactory< ::grpc_user::Reply>::Create(channel_.get(), cq, rpcmethod_UserService_, context, request, false);
}

GruutUserService::Service::Service() {
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GruutUserService_method_names[0],
      ::grpc::internal::RpcMethod::BIDI_STREAMING,
      new ::grpc::internal::BidiStreamingHandler< GruutUserService::Service, ::grpc_user::Identity, ::grpc_user::Message>(
          std::mem_fn(&GruutUserService::Service::OpenChannel), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GruutUserService_method_names[1],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< GruutUserService::Service, ::grpc_user::Request, ::grpc_user::Reply>(
          std::mem_fn(&GruutUserService::Service::KeyExService), this)));
  AddMethod(new ::grpc::internal::RpcServiceMethod(
      GruutUserService_method_names[2],
      ::grpc::internal::RpcMethod::NORMAL_RPC,
      new ::grpc::internal::RpcMethodHandler< GruutUserService::Service, ::grpc_user::Request, ::grpc_user::Reply>(
          std::mem_fn(&GruutUserService::Service::UserService), this)));
}

GruutUserService::Service::~Service() {
}

::grpc::Status GruutUserService::Service::OpenChannel(::grpc::ServerContext* context, ::grpc::ServerReaderWriter< ::grpc_user::Message, ::grpc_user::Identity>* stream) {
  (void) context;
  (void) stream;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status GruutUserService::Service::KeyExService(::grpc::ServerContext* context, const ::grpc_user::Request* request, ::grpc_user::Reply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}

::grpc::Status GruutUserService::Service::UserService(::grpc::ServerContext* context, const ::grpc_user::Request* request, ::grpc_user::Reply* response) {
  (void) context;
  (void) request;
  (void) response;
  return ::grpc::Status(::grpc::StatusCode::UNIMPLEMENTED, "");
}


}  // namespace grpc_user

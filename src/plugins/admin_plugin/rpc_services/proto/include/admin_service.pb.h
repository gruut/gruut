// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: admin_service.proto

#ifndef PROTOBUF_INCLUDED_admin_5fservice_2eproto
#define PROTOBUF_INCLUDED_admin_5fservice_2eproto

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3006001
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3006001 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/generated_message_table_driven.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/inlined_string_field.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/message.h>
#include <google/protobuf/repeated_field.h>  // IWYU pragma: export
#include <google/protobuf/extension_set.h>  // IWYU pragma: export
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)
#define PROTOBUF_INTERNAL_EXPORT_protobuf_admin_5fservice_2eproto 

namespace protobuf_admin_5fservice_2eproto {
// Internal implementation detail -- do not use these members.
struct TableStruct {
  static const ::google::protobuf::internal::ParseTableField entries[];
  static const ::google::protobuf::internal::AuxillaryParseTableField aux[];
  static const ::google::protobuf::internal::ParseTable schema[8];
  static const ::google::protobuf::internal::FieldMetadata field_metadata[];
  static const ::google::protobuf::internal::SerializationTable serialization_table[];
  static const ::google::protobuf::uint32 offsets[];
};
void AddDescriptors();
}  // namespace protobuf_admin_5fservice_2eproto
namespace grpc_admin {
class ReqSetup;
class ReqSetupDefaultTypeInternal;
extern ReqSetupDefaultTypeInternal _ReqSetup_default_instance_;
class ReqStart;
class ReqStartDefaultTypeInternal;
extern ReqStartDefaultTypeInternal _ReqStart_default_instance_;
class ReqStatus;
class ReqStatusDefaultTypeInternal;
extern ReqStatusDefaultTypeInternal _ReqStatus_default_instance_;
class ReqStop;
class ReqStopDefaultTypeInternal;
extern ReqStopDefaultTypeInternal _ReqStop_default_instance_;
class ResSetup;
class ResSetupDefaultTypeInternal;
extern ResSetupDefaultTypeInternal _ResSetup_default_instance_;
class ResStart;
class ResStartDefaultTypeInternal;
extern ResStartDefaultTypeInternal _ResStart_default_instance_;
class ResStatus;
class ResStatusDefaultTypeInternal;
extern ResStatusDefaultTypeInternal _ResStatus_default_instance_;
class ResStop;
class ResStopDefaultTypeInternal;
extern ResStopDefaultTypeInternal _ResStop_default_instance_;
}  // namespace grpc_admin
namespace google {
namespace protobuf {
template<> ::grpc_admin::ReqSetup* Arena::CreateMaybeMessage<::grpc_admin::ReqSetup>(Arena*);
template<> ::grpc_admin::ReqStart* Arena::CreateMaybeMessage<::grpc_admin::ReqStart>(Arena*);
template<> ::grpc_admin::ReqStatus* Arena::CreateMaybeMessage<::grpc_admin::ReqStatus>(Arena*);
template<> ::grpc_admin::ReqStop* Arena::CreateMaybeMessage<::grpc_admin::ReqStop>(Arena*);
template<> ::grpc_admin::ResSetup* Arena::CreateMaybeMessage<::grpc_admin::ResSetup>(Arena*);
template<> ::grpc_admin::ResStart* Arena::CreateMaybeMessage<::grpc_admin::ResStart>(Arena*);
template<> ::grpc_admin::ResStatus* Arena::CreateMaybeMessage<::grpc_admin::ResStatus>(Arena*);
template<> ::grpc_admin::ResStop* Arena::CreateMaybeMessage<::grpc_admin::ResStop>(Arena*);
}  // namespace protobuf
}  // namespace google
namespace grpc_admin {

// ===================================================================

class ReqSetup : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ReqSetup) */ {
 public:
  ReqSetup();
  virtual ~ReqSetup();

  ReqSetup(const ReqSetup& from);

  inline ReqSetup& operator=(const ReqSetup& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ReqSetup(ReqSetup&& from) noexcept
    : ReqSetup() {
    *this = ::std::move(from);
  }

  inline ReqSetup& operator=(ReqSetup&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ReqSetup& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ReqSetup* internal_default_instance() {
    return reinterpret_cast<const ReqSetup*>(
               &_ReqSetup_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    0;

  void Swap(ReqSetup* other);
  friend void swap(ReqSetup& a, ReqSetup& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ReqSetup* New() const final {
    return CreateMaybeMessage<ReqSetup>(NULL);
  }

  ReqSetup* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ReqSetup>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ReqSetup& from);
  void MergeFrom(const ReqSetup& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ReqSetup* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // string password = 1;
  void clear_password();
  static const int kPasswordFieldNumber = 1;
  const ::std::string& password() const;
  void set_password(const ::std::string& value);
  #if LANG_CXX11
  void set_password(::std::string&& value);
  #endif
  void set_password(const char* value);
  void set_password(const char* value, size_t size);
  ::std::string* mutable_password();
  ::std::string* release_password();
  void set_allocated_password(::std::string* password);

  // @@protoc_insertion_point(class_scope:grpc_admin.ReqSetup)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  ::google::protobuf::internal::ArenaStringPtr password_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ResSetup : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ResSetup) */ {
 public:
  ResSetup();
  virtual ~ResSetup();

  ResSetup(const ResSetup& from);

  inline ResSetup& operator=(const ResSetup& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ResSetup(ResSetup&& from) noexcept
    : ResSetup() {
    *this = ::std::move(from);
  }

  inline ResSetup& operator=(ResSetup&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ResSetup& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ResSetup* internal_default_instance() {
    return reinterpret_cast<const ResSetup*>(
               &_ResSetup_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    1;

  void Swap(ResSetup* other);
  friend void swap(ResSetup& a, ResSetup& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ResSetup* New() const final {
    return CreateMaybeMessage<ResSetup>(NULL);
  }

  ResSetup* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ResSetup>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ResSetup& from);
  void MergeFrom(const ResSetup& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ResSetup* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bool success = 1;
  void clear_success();
  static const int kSuccessFieldNumber = 1;
  bool success() const;
  void set_success(bool value);

  // @@protoc_insertion_point(class_scope:grpc_admin.ResSetup)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool success_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ReqStart : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ReqStart) */ {
 public:
  ReqStart();
  virtual ~ReqStart();

  ReqStart(const ReqStart& from);

  inline ReqStart& operator=(const ReqStart& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ReqStart(ReqStart&& from) noexcept
    : ReqStart() {
    *this = ::std::move(from);
  }

  inline ReqStart& operator=(ReqStart&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ReqStart& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ReqStart* internal_default_instance() {
    return reinterpret_cast<const ReqStart*>(
               &_ReqStart_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    2;

  void Swap(ReqStart* other);
  friend void swap(ReqStart& a, ReqStart& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ReqStart* New() const final {
    return CreateMaybeMessage<ReqStart>(NULL);
  }

  ReqStart* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ReqStart>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ReqStart& from);
  void MergeFrom(const ReqStart& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ReqStart* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:grpc_admin.ReqStart)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ResStart : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ResStart) */ {
 public:
  ResStart();
  virtual ~ResStart();

  ResStart(const ResStart& from);

  inline ResStart& operator=(const ResStart& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ResStart(ResStart&& from) noexcept
    : ResStart() {
    *this = ::std::move(from);
  }

  inline ResStart& operator=(ResStart&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ResStart& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ResStart* internal_default_instance() {
    return reinterpret_cast<const ResStart*>(
               &_ResStart_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    3;

  void Swap(ResStart* other);
  friend void swap(ResStart& a, ResStart& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ResStart* New() const final {
    return CreateMaybeMessage<ResStart>(NULL);
  }

  ResStart* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ResStart>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ResStart& from);
  void MergeFrom(const ResStart& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ResStart* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bool success = 1;
  void clear_success();
  static const int kSuccessFieldNumber = 1;
  bool success() const;
  void set_success(bool value);

  // @@protoc_insertion_point(class_scope:grpc_admin.ResStart)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool success_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ReqStop : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ReqStop) */ {
 public:
  ReqStop();
  virtual ~ReqStop();

  ReqStop(const ReqStop& from);

  inline ReqStop& operator=(const ReqStop& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ReqStop(ReqStop&& from) noexcept
    : ReqStop() {
    *this = ::std::move(from);
  }

  inline ReqStop& operator=(ReqStop&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ReqStop& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ReqStop* internal_default_instance() {
    return reinterpret_cast<const ReqStop*>(
               &_ReqStop_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    4;

  void Swap(ReqStop* other);
  friend void swap(ReqStop& a, ReqStop& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ReqStop* New() const final {
    return CreateMaybeMessage<ReqStop>(NULL);
  }

  ReqStop* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ReqStop>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ReqStop& from);
  void MergeFrom(const ReqStop& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ReqStop* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:grpc_admin.ReqStop)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ResStop : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ResStop) */ {
 public:
  ResStop();
  virtual ~ResStop();

  ResStop(const ResStop& from);

  inline ResStop& operator=(const ResStop& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ResStop(ResStop&& from) noexcept
    : ResStop() {
    *this = ::std::move(from);
  }

  inline ResStop& operator=(ResStop&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ResStop& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ResStop* internal_default_instance() {
    return reinterpret_cast<const ResStop*>(
               &_ResStop_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    5;

  void Swap(ResStop* other);
  friend void swap(ResStop& a, ResStop& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ResStop* New() const final {
    return CreateMaybeMessage<ResStop>(NULL);
  }

  ResStop* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ResStop>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ResStop& from);
  void MergeFrom(const ResStop& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ResStop* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bool success = 1;
  void clear_success();
  static const int kSuccessFieldNumber = 1;
  bool success() const;
  void set_success(bool value);

  // @@protoc_insertion_point(class_scope:grpc_admin.ResStop)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool success_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ReqStatus : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ReqStatus) */ {
 public:
  ReqStatus();
  virtual ~ReqStatus();

  ReqStatus(const ReqStatus& from);

  inline ReqStatus& operator=(const ReqStatus& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ReqStatus(ReqStatus&& from) noexcept
    : ReqStatus() {
    *this = ::std::move(from);
  }

  inline ReqStatus& operator=(ReqStatus&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ReqStatus& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ReqStatus* internal_default_instance() {
    return reinterpret_cast<const ReqStatus*>(
               &_ReqStatus_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    6;

  void Swap(ReqStatus* other);
  friend void swap(ReqStatus& a, ReqStatus& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ReqStatus* New() const final {
    return CreateMaybeMessage<ReqStatus>(NULL);
  }

  ReqStatus* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ReqStatus>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ReqStatus& from);
  void MergeFrom(const ReqStatus& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ReqStatus* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // @@protoc_insertion_point(class_scope:grpc_admin.ReqStatus)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// -------------------------------------------------------------------

class ResStatus : public ::google::protobuf::Message /* @@protoc_insertion_point(class_definition:grpc_admin.ResStatus) */ {
 public:
  ResStatus();
  virtual ~ResStatus();

  ResStatus(const ResStatus& from);

  inline ResStatus& operator=(const ResStatus& from) {
    CopyFrom(from);
    return *this;
  }
  #if LANG_CXX11
  ResStatus(ResStatus&& from) noexcept
    : ResStatus() {
    *this = ::std::move(from);
  }

  inline ResStatus& operator=(ResStatus&& from) noexcept {
    if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
      if (this != &from) InternalSwap(&from);
    } else {
      CopyFrom(from);
    }
    return *this;
  }
  #endif
  static const ::google::protobuf::Descriptor* descriptor();
  static const ResStatus& default_instance();

  static void InitAsDefaultInstance();  // FOR INTERNAL USE ONLY
  static inline const ResStatus* internal_default_instance() {
    return reinterpret_cast<const ResStatus*>(
               &_ResStatus_default_instance_);
  }
  static constexpr int kIndexInFileMessages =
    7;

  void Swap(ResStatus* other);
  friend void swap(ResStatus& a, ResStatus& b) {
    a.Swap(&b);
  }

  // implements Message ----------------------------------------------

  inline ResStatus* New() const final {
    return CreateMaybeMessage<ResStatus>(NULL);
  }

  ResStatus* New(::google::protobuf::Arena* arena) const final {
    return CreateMaybeMessage<ResStatus>(arena);
  }
  void CopyFrom(const ::google::protobuf::Message& from) final;
  void MergeFrom(const ::google::protobuf::Message& from) final;
  void CopyFrom(const ResStatus& from);
  void MergeFrom(const ResStatus& from);
  void Clear() final;
  bool IsInitialized() const final;

  size_t ByteSizeLong() const final;
  bool MergePartialFromCodedStream(
      ::google::protobuf::io::CodedInputStream* input) final;
  void SerializeWithCachedSizes(
      ::google::protobuf::io::CodedOutputStream* output) const final;
  ::google::protobuf::uint8* InternalSerializeWithCachedSizesToArray(
      bool deterministic, ::google::protobuf::uint8* target) const final;
  int GetCachedSize() const final { return _cached_size_.Get(); }

  private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const final;
  void InternalSwap(ResStatus* other);
  private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const {
    return NULL;
  }
  inline void* MaybeArenaPtr() const {
    return NULL;
  }
  public:

  ::google::protobuf::Metadata GetMetadata() const final;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // bool alive = 1;
  void clear_alive();
  static const int kAliveFieldNumber = 1;
  bool alive() const;
  void set_alive(bool value);

  // @@protoc_insertion_point(class_scope:grpc_admin.ResStatus)
 private:

  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool alive_;
  mutable ::google::protobuf::internal::CachedSize _cached_size_;
  friend struct ::protobuf_admin_5fservice_2eproto::TableStruct;
};
// ===================================================================


// ===================================================================

#ifdef __GNUC__
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif  // __GNUC__
// ReqSetup

// string password = 1;
inline void ReqSetup::clear_password() {
  password_.ClearToEmptyNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string& ReqSetup::password() const {
  // @@protoc_insertion_point(field_get:grpc_admin.ReqSetup.password)
  return password_.GetNoArena();
}
inline void ReqSetup::set_password(const ::std::string& value) {
  
  password_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:grpc_admin.ReqSetup.password)
}
#if LANG_CXX11
inline void ReqSetup::set_password(::std::string&& value) {
  
  password_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::move(value));
  // @@protoc_insertion_point(field_set_rvalue:grpc_admin.ReqSetup.password)
}
#endif
inline void ReqSetup::set_password(const char* value) {
  GOOGLE_DCHECK(value != NULL);
  
  password_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), ::std::string(value));
  // @@protoc_insertion_point(field_set_char:grpc_admin.ReqSetup.password)
}
inline void ReqSetup::set_password(const char* value, size_t size) {
  
  password_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
      ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:grpc_admin.ReqSetup.password)
}
inline ::std::string* ReqSetup::mutable_password() {
  
  // @@protoc_insertion_point(field_mutable:grpc_admin.ReqSetup.password)
  return password_.MutableNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string* ReqSetup::release_password() {
  // @@protoc_insertion_point(field_release:grpc_admin.ReqSetup.password)
  
  return password_.ReleaseNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void ReqSetup::set_allocated_password(::std::string* password) {
  if (password != NULL) {
    
  } else {
    
  }
  password_.SetAllocatedNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(), password);
  // @@protoc_insertion_point(field_set_allocated:grpc_admin.ReqSetup.password)
}

// -------------------------------------------------------------------

// ResSetup

// bool success = 1;
inline void ResSetup::clear_success() {
  success_ = false;
}
inline bool ResSetup::success() const {
  // @@protoc_insertion_point(field_get:grpc_admin.ResSetup.success)
  return success_;
}
inline void ResSetup::set_success(bool value) {
  
  success_ = value;
  // @@protoc_insertion_point(field_set:grpc_admin.ResSetup.success)
}

// -------------------------------------------------------------------

// ReqStart

// -------------------------------------------------------------------

// ResStart

// bool success = 1;
inline void ResStart::clear_success() {
  success_ = false;
}
inline bool ResStart::success() const {
  // @@protoc_insertion_point(field_get:grpc_admin.ResStart.success)
  return success_;
}
inline void ResStart::set_success(bool value) {
  
  success_ = value;
  // @@protoc_insertion_point(field_set:grpc_admin.ResStart.success)
}

// -------------------------------------------------------------------

// ReqStop

// -------------------------------------------------------------------

// ResStop

// bool success = 1;
inline void ResStop::clear_success() {
  success_ = false;
}
inline bool ResStop::success() const {
  // @@protoc_insertion_point(field_get:grpc_admin.ResStop.success)
  return success_;
}
inline void ResStop::set_success(bool value) {
  
  success_ = value;
  // @@protoc_insertion_point(field_set:grpc_admin.ResStop.success)
}

// -------------------------------------------------------------------

// ReqStatus

// -------------------------------------------------------------------

// ResStatus

// bool alive = 1;
inline void ResStatus::clear_alive() {
  alive_ = false;
}
inline bool ResStatus::alive() const {
  // @@protoc_insertion_point(field_get:grpc_admin.ResStatus.alive)
  return alive_;
}
inline void ResStatus::set_alive(bool value) {
  
  alive_ = value;
  // @@protoc_insertion_point(field_set:grpc_admin.ResStatus.alive)
}

#ifdef __GNUC__
  #pragma GCC diagnostic pop
#endif  // __GNUC__
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------

// -------------------------------------------------------------------


// @@protoc_insertion_point(namespace_scope)

}  // namespace grpc_admin

// @@protoc_insertion_point(global_scope)

#endif  // PROTOBUF_INCLUDED_admin_5fservice_2eproto

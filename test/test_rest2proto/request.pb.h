// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: request.proto

#ifndef PROTOBUF_request_2eproto__INCLUDED
#define PROTOBUF_request_2eproto__INCLUDED

#include <string>

#include <google/protobuf/stubs/common.h>

#if GOOGLE_PROTOBUF_VERSION < 3000000
#error This file was generated by a newer version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please update
#error your headers.
#endif
#if 3000000 < GOOGLE_PROTOBUF_MIN_PROTOC_VERSION
#error This file was generated by an older version of protoc which is
#error incompatible with your Protocol Buffer headers.  Please
#error regenerate this file with a newer version of protoc.
#endif

#include <google/protobuf/arena.h>
#include <google/protobuf/arenastring.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/generated_enum_reflection.h>
#include <google/protobuf/generated_message_util.h>
#include <google/protobuf/message.h>
#include <google/protobuf/metadata.h>
#include <google/protobuf/repeated_field.h>
#include <google/protobuf/unknown_field_set.h>
// @@protoc_insertion_point(includes)

namespace interfaces {

// Internal implementation detail -- do not call these.
void
protobuf_AddDesc_request_2eproto();
void
protobuf_AssignDesc_request_2eproto();
void
protobuf_ShutdownFile_request_2eproto();

class MessageBlock;
class Request;
class Request_Parameters;

enum Request_MethodType
{
  Request_MethodType_GET = 0,
  Request_MethodType_HEAD = 1,
  Request_MethodType_POST = 2,
  Request_MethodType_PUT = 3,
  Request_MethodType_DELETE = 4,
  Request_MethodType_CONNECT = 5,
  Request_MethodType_OPTIONS = 6,
  Request_MethodType_TRACE = 7,
  Request_MethodType_Request_MethodType_INT_MIN_SENTINEL_DO_NOT_USE_ =
    ::google::protobuf::kint32min,
  Request_MethodType_Request_MethodType_INT_MAX_SENTINEL_DO_NOT_USE_ =
    ::google::protobuf::kint32max
};
bool
Request_MethodType_IsValid(int value);
const Request_MethodType Request_MethodType_MethodType_MIN =
  Request_MethodType_GET;
const Request_MethodType Request_MethodType_MethodType_MAX =
  Request_MethodType_TRACE;
const int Request_MethodType_MethodType_ARRAYSIZE =
  Request_MethodType_MethodType_MAX + 1;

const ::google::protobuf::EnumDescriptor*
Request_MethodType_descriptor();
inline const ::std::string&
Request_MethodType_Name(Request_MethodType value)
{
  return ::google::protobuf::internal::NameOfEnum(
    Request_MethodType_descriptor(), value);
}
inline bool
Request_MethodType_Parse(const ::std::string& name, Request_MethodType* value)
{
  return ::google::protobuf::internal::ParseNamedEnum<Request_MethodType>(
    Request_MethodType_descriptor(), name, value);
}
// ===================================================================

class Request_Parameters : public ::google::protobuf::Message
{
public:
  Request_Parameters();
  virtual ~Request_Parameters();

  Request_Parameters(const Request_Parameters& from);

  inline Request_Parameters& operator=(const Request_Parameters& from)
  {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Request_Parameters& default_instance();

  void Swap(Request_Parameters* other);

  // implements Message ----------------------------------------------

  inline Request_Parameters* New() const { return New(NULL); }

  Request_Parameters* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Request_Parameters& from);
  void MergeFrom(const Request_Parameters& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }

private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Request_Parameters* other);

private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const
  {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const
  {
    return _internal_metadata_.raw_arena_ptr();
  }

public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // optional string key = 1;
  void clear_key();
  static const int kKeyFieldNumber = 1;
  const ::std::string& key() const;
  void set_key(const ::std::string& value);
  void set_key(const char* value);
  void set_key(const char* value, size_t size);
  ::std::string* mutable_key();
  ::std::string* release_key();
  void set_allocated_key(::std::string* key);

  // optional string value = 2;
  void clear_value();
  static const int kValueFieldNumber = 2;
  const ::std::string& value() const;
  void set_value(const ::std::string& value);
  void set_value(const char* value);
  void set_value(const char* value, size_t size);
  ::std::string* mutable_value();
  ::std::string* release_value();
  void set_allocated_value(::std::string* value);

  // @@protoc_insertion_point(class_scope:interfaces.Request.Parameters)
private:
  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr key_;
  ::google::protobuf::internal::ArenaStringPtr value_;
  mutable int _cached_size_;
  friend void protobuf_AddDesc_request_2eproto();
  friend void protobuf_AssignDesc_request_2eproto();
  friend void protobuf_ShutdownFile_request_2eproto();

  void InitAsDefaultInstance();
  static Request_Parameters* default_instance_;
};
// -------------------------------------------------------------------

class Request : public ::google::protobuf::Message
{
public:
  Request();
  virtual ~Request();

  Request(const Request& from);

  inline Request& operator=(const Request& from)
  {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const Request& default_instance();

  void Swap(Request* other);

  // implements Message ----------------------------------------------

  inline Request* New() const { return New(NULL); }

  Request* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const Request& from);
  void MergeFrom(const Request& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }

private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(Request* other);

private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const
  {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const
  {
    return _internal_metadata_.raw_arena_ptr();
  }

public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  typedef Request_Parameters Parameters;

  typedef Request_MethodType MethodType;
  static const MethodType GET = Request_MethodType_GET;
  static const MethodType HEAD = Request_MethodType_HEAD;
  static const MethodType POST = Request_MethodType_POST;
  static const MethodType PUT = Request_MethodType_PUT;
  static const MethodType DELETE = Request_MethodType_DELETE;
  static const MethodType CONNECT = Request_MethodType_CONNECT;
  static const MethodType OPTIONS = Request_MethodType_OPTIONS;
  static const MethodType TRACE = Request_MethodType_TRACE;
  static inline bool MethodType_IsValid(int value)
  {
    return Request_MethodType_IsValid(value);
  }
  static const MethodType MethodType_MIN = Request_MethodType_MethodType_MIN;
  static const MethodType MethodType_MAX = Request_MethodType_MethodType_MAX;
  static const int MethodType_ARRAYSIZE =
    Request_MethodType_MethodType_ARRAYSIZE;
  static inline const ::google::protobuf::EnumDescriptor*
  MethodType_descriptor()
  {
    return Request_MethodType_descriptor();
  }
  static inline const ::std::string& MethodType_Name(MethodType value)
  {
    return Request_MethodType_Name(value);
  }
  static inline bool MethodType_Parse(const ::std::string& name,
                                      MethodType* value)
  {
    return Request_MethodType_Parse(name, value);
  }

  // accessors -------------------------------------------------------

  // optional string path = 1;
  void clear_path();
  static const int kPathFieldNumber = 1;
  const ::std::string& path() const;
  void set_path(const ::std::string& value);
  void set_path(const char* value);
  void set_path(const char* value, size_t size);
  ::std::string* mutable_path();
  ::std::string* release_path();
  void set_allocated_path(::std::string* path);

  // optional .interfaces.Request.MethodType type = 2;
  void clear_type();
  static const int kTypeFieldNumber = 2;
  ::interfaces::Request_MethodType type() const;
  void set_type(::interfaces::Request_MethodType value);

  // repeated .interfaces.Request.Parameters params = 3;
  int params_size() const;
  void clear_params();
  static const int kParamsFieldNumber = 3;
  const ::interfaces::Request_Parameters& params(int index) const;
  ::interfaces::Request_Parameters* mutable_params(int index);
  ::interfaces::Request_Parameters* add_params();
  ::google::protobuf::RepeatedPtrField<::interfaces::Request_Parameters>*
  mutable_params();
  const ::google::protobuf::RepeatedPtrField<
    ::interfaces::Request_Parameters>&
  params() const;

  // optional string body = 4;
  void clear_body();
  static const int kBodyFieldNumber = 4;
  const ::std::string& body() const;
  void set_body(const ::std::string& value);
  void set_body(const char* value);
  void set_body(const char* value, size_t size);
  ::std::string* mutable_body();
  ::std::string* release_body();
  void set_allocated_body(::std::string* body);

  // @@protoc_insertion_point(class_scope:interfaces.Request)
private:
  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::internal::ArenaStringPtr path_;
  ::google::protobuf::RepeatedPtrField<::interfaces::Request_Parameters>
    params_;
  ::google::protobuf::internal::ArenaStringPtr body_;
  int type_;
  mutable int _cached_size_;
  friend void protobuf_AddDesc_request_2eproto();
  friend void protobuf_AssignDesc_request_2eproto();
  friend void protobuf_ShutdownFile_request_2eproto();

  void InitAsDefaultInstance();
  static Request* default_instance_;
};
// -------------------------------------------------------------------

class MessageBlock : public ::google::protobuf::Message
{
public:
  MessageBlock();
  virtual ~MessageBlock();

  MessageBlock(const MessageBlock& from);

  inline MessageBlock& operator=(const MessageBlock& from)
  {
    CopyFrom(from);
    return *this;
  }

  static const ::google::protobuf::Descriptor* descriptor();
  static const MessageBlock& default_instance();

  void Swap(MessageBlock* other);

  // implements Message ----------------------------------------------

  inline MessageBlock* New() const { return New(NULL); }

  MessageBlock* New(::google::protobuf::Arena* arena) const;
  void CopyFrom(const ::google::protobuf::Message& from);
  void MergeFrom(const ::google::protobuf::Message& from);
  void CopyFrom(const MessageBlock& from);
  void MergeFrom(const MessageBlock& from);
  void Clear();
  bool IsInitialized() const;

  int ByteSize() const;
  bool MergePartialFromCodedStream(
    ::google::protobuf::io::CodedInputStream* input);
  void SerializeWithCachedSizes(
    ::google::protobuf::io::CodedOutputStream* output) const;
  ::google::protobuf::uint8* SerializeWithCachedSizesToArray(
    ::google::protobuf::uint8* output) const;
  int GetCachedSize() const { return _cached_size_; }

private:
  void SharedCtor();
  void SharedDtor();
  void SetCachedSize(int size) const;
  void InternalSwap(MessageBlock* other);

private:
  inline ::google::protobuf::Arena* GetArenaNoVirtual() const
  {
    return _internal_metadata_.arena();
  }
  inline void* MaybeArenaPtr() const
  {
    return _internal_metadata_.raw_arena_ptr();
  }

public:
  ::google::protobuf::Metadata GetMetadata() const;

  // nested types ----------------------------------------------------

  // accessors -------------------------------------------------------

  // repeated .interfaces.Request request = 1;
  int request_size() const;
  void clear_request();
  static const int kRequestFieldNumber = 1;
  const ::interfaces::Request& request(int index) const;
  ::interfaces::Request* mutable_request(int index);
  ::interfaces::Request* add_request();
  ::google::protobuf::RepeatedPtrField<::interfaces::Request>*
  mutable_request();
  const ::google::protobuf::RepeatedPtrField<::interfaces::Request>&
  request() const;

  // @@protoc_insertion_point(class_scope:interfaces.MessageBlock)
private:
  ::google::protobuf::internal::InternalMetadataWithArena _internal_metadata_;
  bool _is_default_instance_;
  ::google::protobuf::RepeatedPtrField<::interfaces::Request> request_;
  mutable int _cached_size_;
  friend void protobuf_AddDesc_request_2eproto();
  friend void protobuf_AssignDesc_request_2eproto();
  friend void protobuf_ShutdownFile_request_2eproto();

  void InitAsDefaultInstance();
  static MessageBlock* default_instance_;
};
// ===================================================================

// ===================================================================

#if !PROTOBUF_INLINE_NOT_IN_HEADERS
// Request_Parameters

// optional string key = 1;
inline void
Request_Parameters::clear_key()
{
  key_.ClearToEmptyNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string&
Request_Parameters::key() const
{
  // @@protoc_insertion_point(field_get:interfaces.Request.Parameters.key)
  return key_.GetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request_Parameters::set_key(const ::std::string& value)
{

  key_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                  value);
  // @@protoc_insertion_point(field_set:interfaces.Request.Parameters.key)
}
inline void
Request_Parameters::set_key(const char* value)
{

  key_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                  ::std::string(value));
  // @@protoc_insertion_point(field_set_char:interfaces.Request.Parameters.key)
}
inline void
Request_Parameters::set_key(const char* value, size_t size)
{

  key_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                  ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:interfaces.Request.Parameters.key)
}
inline ::std::string*
Request_Parameters::mutable_key()
{

  // @@protoc_insertion_point(field_mutable:interfaces.Request.Parameters.key)
  return key_.MutableNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string*
Request_Parameters::release_key()
{

  return key_.ReleaseNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request_Parameters::set_allocated_key(::std::string* key)
{
  if (key != NULL) {

  } else {
  }
  key_.SetAllocatedNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), key);
  // @@protoc_insertion_point(field_set_allocated:interfaces.Request.Parameters.key)
}

// optional string value = 2;
inline void
Request_Parameters::clear_value()
{
  value_.ClearToEmptyNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string&
Request_Parameters::value() const
{
  // @@protoc_insertion_point(field_get:interfaces.Request.Parameters.value)
  return value_.GetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request_Parameters::set_value(const ::std::string& value)
{

  value_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set:interfaces.Request.Parameters.value)
}
inline void
Request_Parameters::set_value(const char* value)
{

  value_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    ::std::string(value));
  // @@protoc_insertion_point(field_set_char:interfaces.Request.Parameters.value)
}
inline void
Request_Parameters::set_value(const char* value, size_t size)
{

  value_.SetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(),
    ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:interfaces.Request.Parameters.value)
}
inline ::std::string*
Request_Parameters::mutable_value()
{

  // @@protoc_insertion_point(field_mutable:interfaces.Request.Parameters.value)
  return value_.MutableNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string*
Request_Parameters::release_value()
{

  return value_.ReleaseNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request_Parameters::set_allocated_value(::std::string* value)
{
  if (value != NULL) {

  } else {
  }
  value_.SetAllocatedNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), value);
  // @@protoc_insertion_point(field_set_allocated:interfaces.Request.Parameters.value)
}

// -------------------------------------------------------------------

// Request

// optional string path = 1;
inline void
Request::clear_path()
{
  path_.ClearToEmptyNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string&
Request::path() const
{
  // @@protoc_insertion_point(field_get:interfaces.Request.path)
  return path_.GetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request::set_path(const ::std::string& value)
{

  path_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                   value);
  // @@protoc_insertion_point(field_set:interfaces.Request.path)
}
inline void
Request::set_path(const char* value)
{

  path_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                   ::std::string(value));
  // @@protoc_insertion_point(field_set_char:interfaces.Request.path)
}
inline void
Request::set_path(const char* value, size_t size)
{

  path_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                   ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:interfaces.Request.path)
}
inline ::std::string*
Request::mutable_path()
{

  // @@protoc_insertion_point(field_mutable:interfaces.Request.path)
  return path_.MutableNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string*
Request::release_path()
{

  return path_.ReleaseNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request::set_allocated_path(::std::string* path)
{
  if (path != NULL) {

  } else {
  }
  path_.SetAllocatedNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), path);
  // @@protoc_insertion_point(field_set_allocated:interfaces.Request.path)
}

// optional .interfaces.Request.MethodType type = 2;
inline void
Request::clear_type()
{
  type_ = 0;
}
inline ::interfaces::Request_MethodType
Request::type() const
{
  // @@protoc_insertion_point(field_get:interfaces.Request.type)
  return static_cast<::interfaces::Request_MethodType>(type_);
}
inline void
Request::set_type(::interfaces::Request_MethodType value)
{

  type_ = value;
  // @@protoc_insertion_point(field_set:interfaces.Request.type)
}

// repeated .interfaces.Request.Parameters params = 3;
inline int
Request::params_size() const
{
  return params_.size();
}
inline void
Request::clear_params()
{
  params_.Clear();
}
inline const ::interfaces::Request_Parameters&
Request::params(int index) const
{
  // @@protoc_insertion_point(field_get:interfaces.Request.params)
  return params_.Get(index);
}
inline ::interfaces::Request_Parameters*
Request::mutable_params(int index)
{
  // @@protoc_insertion_point(field_mutable:interfaces.Request.params)
  return params_.Mutable(index);
}
inline ::interfaces::Request_Parameters*
Request::add_params()
{
  // @@protoc_insertion_point(field_add:interfaces.Request.params)
  return params_.Add();
}
inline ::google::protobuf::RepeatedPtrField<
  ::interfaces::Request_Parameters>*
Request::mutable_params()
{
  // @@protoc_insertion_point(field_mutable_list:interfaces.Request.params)
  return &params_;
}
inline const ::google::protobuf::RepeatedPtrField<
  ::interfaces::Request_Parameters>&
Request::params() const
{
  // @@protoc_insertion_point(field_list:interfaces.Request.params)
  return params_;
}

// optional string body = 4;
inline void
Request::clear_body()
{
  body_.ClearToEmptyNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline const ::std::string&
Request::body() const
{
  // @@protoc_insertion_point(field_get:interfaces.Request.body)
  return body_.GetNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request::set_body(const ::std::string& value)
{

  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                   value);
  // @@protoc_insertion_point(field_set:interfaces.Request.body)
}
inline void
Request::set_body(const char* value)
{

  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                   ::std::string(value));
  // @@protoc_insertion_point(field_set_char:interfaces.Request.body)
}
inline void
Request::set_body(const char* value, size_t size)
{

  body_.SetNoArena(&::google::protobuf::internal::GetEmptyStringAlreadyInited(),
                   ::std::string(reinterpret_cast<const char*>(value), size));
  // @@protoc_insertion_point(field_set_pointer:interfaces.Request.body)
}
inline ::std::string*
Request::mutable_body()
{

  // @@protoc_insertion_point(field_mutable:interfaces.Request.body)
  return body_.MutableNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline ::std::string*
Request::release_body()
{

  return body_.ReleaseNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited());
}
inline void
Request::set_allocated_body(::std::string* body)
{
  if (body != NULL) {

  } else {
  }
  body_.SetAllocatedNoArena(
    &::google::protobuf::internal::GetEmptyStringAlreadyInited(), body);
  // @@protoc_insertion_point(field_set_allocated:interfaces.Request.body)
}

// -------------------------------------------------------------------

// MessageBlock

// repeated .interfaces.Request request = 1;
inline int
MessageBlock::request_size() const
{
  return request_.size();
}
inline void
MessageBlock::clear_request()
{
  request_.Clear();
}
inline const ::interfaces::Request&
MessageBlock::request(int index) const
{
  // @@protoc_insertion_point(field_get:interfaces.MessageBlock.request)
  return request_.Get(index);
}
inline ::interfaces::Request*
MessageBlock::mutable_request(int index)
{
  // @@protoc_insertion_point(field_mutable:interfaces.MessageBlock.request)
  return request_.Mutable(index);
}
inline ::interfaces::Request*
MessageBlock::add_request()
{
  // @@protoc_insertion_point(field_add:interfaces.MessageBlock.request)
  return request_.Add();
}
inline ::google::protobuf::RepeatedPtrField<::interfaces::Request>*
MessageBlock::mutable_request()
{
  // @@protoc_insertion_point(field_mutable_list:interfaces.MessageBlock.request)
  return &request_;
}
inline const ::google::protobuf::RepeatedPtrField<::interfaces::Request>&
MessageBlock::request() const
{
  // @@protoc_insertion_point(field_list:interfaces.MessageBlock.request)
  return request_;
}

#endif // !PROTOBUF_INLINE_NOT_IN_HEADERS
// -------------------------------------------------------------------

// -------------------------------------------------------------------

// @@protoc_insertion_point(namespace_scope)

} // namespace interfaces

#ifndef SWIG
namespace google {
namespace protobuf {

template<>
struct is_proto_enum<::interfaces::Request_MethodType>
  : ::google::protobuf::internal::true_type
{
};
template<>
inline const EnumDescriptor*
GetEnumDescriptor<::interfaces::Request_MethodType>()
{
  return ::interfaces::Request_MethodType_descriptor();
}

} // namespace protobuf
} // namespace google
#endif // SWIG

// @@protoc_insertion_point(global_scope)

#endif // PROTOBUF_request_2eproto__INCLUDED

/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include <thrift/protocol/TNativeClientProtocol.h>

#include <limits>

#include <boost/make_shared.hpp>
#include <math.h>
#include <thrift/protocol/TBase64Utils.h>
#include <thrift/transport/TBufferTransports.h>

#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_array.h"
#include "ppapi/cpp/var_dictionary.h"

// Largest integer such that all smaller integers can be represented
// using a double without losing precision. 2^53 = 9007199254740992
static const double kMaxPreciseDouble = 9007199254740992.0L;
static const double kMinPreciseDouble = -9007199254740992.0L;

using namespace apache::thrift::transport;

namespace apache { namespace thrift { namespace protocol {

TNativeClientProtocol::TNativeClientProtocol()
  : TVirtualProtocol<TNativeClientProtocol>(
      boost::shared_ptr<TTransport>(boost::make_shared<TMemoryBuffer>())) {
}

TNativeClientProtocol::TNativeClientProtocol(boost::shared_ptr<const pp::Var> var) :
  TVirtualProtocol<TNativeClientProtocol>(
      boost::shared_ptr<TTransport>(boost::make_shared<TMemoryBuffer>())),
  root_var_(var) {
}

void TNativeClientProtocol::reset() {
  while (!writer_stack_.empty()) {
    popWriterContext();
  }
  while (!reader_stack_.empty()) {
    popReaderContext();
  }
  root_var_.reset();
}

void TNativeClientProtocol::setVar(boost::shared_ptr<const pp::Var> var) {
  reset();
  root_var_ = var;
}

void TNativeClientProtocol::writeVar(const pp::Var& var) {
  if (writer_stack_.empty()) {
    if (!var.is_dictionary()) {
      throw TProtocolException(TProtocolException::UNKNOWN,
                               "Root node must be a dictionary");
    }
    root_var_ = boost::make_shared<pp::Var>(var);
  } else {
    WriterContext* context = writer_stack_.top();
    context->writeVar(var);
  }
}

boost::shared_ptr<const pp::Var> TNativeClientProtocol::readVar() {
  if (reader_stack_.empty()) {
    if (!root_var_) {
      throw TProtocolException(TProtocolException::UNKNOWN,
          "Trying to read but pp::Var has not been set "
          "(use TNativeClientProtocol(shared_ptr<const pp::Var>) constructor)");
    }
    return root_var_;
  } else {
    ReaderContext* context = reader_stack_.top();
    T_DEBUG("readVar: %d %d", reader_stack_.size(), context->getIndex());

    boost::shared_ptr<pp::Var> var = boost::make_shared<pp::Var>();
    context->nextVar(var.get());
    context->advance();
    return var;
  }
}

void TNativeClientProtocol::pushWriterContext(WriterContext* context) {
  assert(context->getVar().is_dictionary() || context->getVar().is_array());
  writeVar(context->getVar());
  writer_stack_.push(context);
}

void TNativeClientProtocol::popWriterContext() {
  WriterContext* context = writer_stack_.top();
  writer_stack_.pop();
  delete context;
}

void TNativeClientProtocol::pushReaderContext(ReaderContext* context) {
  assert(context->getVar().is_dictionary() || context->getVar().is_array());
  reader_stack_.push(context);
}

void TNativeClientProtocol::popReaderContext() {
  ReaderContext* context = reader_stack_.top();
  reader_stack_.pop();
  delete context;
}

uint32_t TNativeClientProtocol::writeMessageBegin(const std::string& name,
                                                 const TMessageType messageType,
                                                 const int32_t seqid) {
  throw TProtocolException(TProtocolException::NOT_IMPLEMENTED);
  return 0;
}

uint32_t TNativeClientProtocol::writeMessageEnd() {
  throw TProtocolException(TProtocolException::NOT_IMPLEMENTED);
  return 0;
}

uint32_t TNativeClientProtocol::writeStructBegin(const char* name) {
  T_DEBUG("writeStructBegin: %s", name);
  pushWriterContext(WriterContext::createDictionaryContext());
  return 0;
}

uint32_t TNativeClientProtocol::writeStructEnd() {
  T_DEBUG("writeStructEnd");
  popWriterContext();
  return 0;
}

uint32_t TNativeClientProtocol::writeFieldBegin(const char* name,
                                                const TType fieldType,
                                                const int16_t fieldId) {
  T_DEBUG("writeFieldBegin: %s", name);
  writer_stack_.top()->setFieldName(name); 
  return 0;
}

uint32_t TNativeClientProtocol::writeFieldEnd() {
  T_DEBUG("writeFieldEnd");
  return 0;
}

uint32_t TNativeClientProtocol::writeFieldStop() {
  return 0;
}

uint32_t TNativeClientProtocol::writeMapBegin(const TType keyType,
                                              const TType valType,
                                              const uint32_t size) {
  T_DEBUG("writeMapBegin: %u", size);
  pushWriterContext(WriterContext::createMapContext());
  return 0;
}

uint32_t TNativeClientProtocol::writeMapEnd() {
  T_DEBUG("writeMapEnd");
  popWriterContext();
  return 0;
}

uint32_t TNativeClientProtocol::writeListBegin(const TType elemType,
                                               const uint32_t size) {
  T_DEBUG("writeListBegin: %u", size);
  pushWriterContext(WriterContext::createListContext());
  return 0;
}

uint32_t TNativeClientProtocol::writeListEnd() {
  T_DEBUG("writeListEnd");
  popWriterContext();
  return 0;
}

uint32_t TNativeClientProtocol::writeSetBegin(const TType elemType,
                                              const uint32_t size) {
  T_DEBUG("writeSetBegin: %u", size);
  pushWriterContext(WriterContext::createListContext());
  return 0;
}

uint32_t TNativeClientProtocol::writeSetEnd() {
  T_DEBUG("writeSetEnd");
  popWriterContext();
  return 0;
}

uint32_t TNativeClientProtocol::writeBool(const bool value) {
  writeVar(pp::Var(value));
  return 0;
}

uint32_t TNativeClientProtocol::writeByte(const int8_t byte) {
  writeVar(pp::Var(static_cast<int32_t>(byte)));
  return 0;
}

uint32_t TNativeClientProtocol::writeI16(const int16_t i16) {
  writeVar(pp::Var(static_cast<int32_t>(i16)));
  return 0;
}

uint32_t TNativeClientProtocol::writeI32(const int32_t i32) {
  writeVar(pp::Var(i32));
  return 0;
}

uint32_t TNativeClientProtocol::writeI64(const int64_t i64) {
  // Javascript does not support larger integers without losing precision.
  if (i64 > kMaxPreciseDouble || i64 < kMinPreciseDouble) {
    throw TProtocolException(TProtocolException::SIZE_LIMIT,
        "Int64 value too large to be represented as a double");
  }
  writeVar(pp::Var(static_cast<double>(i64)));
  return 0;
}

uint32_t TNativeClientProtocol::writeDouble(const double dbl) {
  writeVar(pp::Var(dbl));
  return 0;
}

uint32_t TNativeClientProtocol::writeString(const std::string& str) {
  writeVar(pp::Var(str));
  return 0;
}

uint32_t TNativeClientProtocol::writeBinary(const std::string& str) {
  std::string b64str;
  base64EncodeString(str, &b64str);
  writeVar(pp::Var(b64str));
  return 0;
}

uint32_t TNativeClientProtocol::readMessageBegin(std::string& name,
                                                 TMessageType& messageType,
                                                 int32_t& seqid) {
  throw TProtocolException(TProtocolException::NOT_IMPLEMENTED);
  return 0;
}

uint32_t TNativeClientProtocol::readMessageEnd() {
  throw TProtocolException(TProtocolException::NOT_IMPLEMENTED);
  return 0;
}

uint32_t TNativeClientProtocol::readStructBegin(std::string& name) {
  T_DEBUG("readStructBegin: %s", name.c_str());
  boost::shared_ptr<const pp::Var> var(readVar());
  pushReaderContext(ReaderContext::createDictionaryContext(var));
  return 0;
}

uint32_t TNativeClientProtocol::readStructEnd() {
  T_DEBUG("readStructEnd");
  popReaderContext();
  return 0;
}

uint32_t TNativeClientProtocol::readFieldBegin(std::string& name,
                                               TType& fieldType,
                                               int16_t& fieldId) {
  ReaderContext* context = reader_stack_.top();

  if (context->isAtEnd()) {
    fieldType = ::apache::thrift::protocol::T_STOP;
  } else {
    pp::Var key;
    pp::Var value;
    context->nextKeyValue(&key, &value);

    if (value.is_null() || value.is_undefined()) {
      name = "";
    } else {
      if (!key.is_string()) {
        // TODO: Support other key types.
        throw TProtocolException(TProtocolException::NOT_IMPLEMENTED,
                                 "Map keys must be strings");
      }
      name = key.AsString();
    }

    fieldType = T_UNKNOWN;
    fieldId = kFieldIdUnknown;
  }

  T_DEBUG("readFieldBegin: %s", name.c_str());
  return 0;
}

uint32_t TNativeClientProtocol::readFieldEnd() {
  T_DEBUG("readFieldEnd");
  return 0;
}

uint32_t TNativeClientProtocol::readMapBegin(TType& keyType,
                                             TType& valType,
                                             uint32_t& size) {
  boost::shared_ptr<const pp::Var> var(readVar());
  ReaderContext* context = ReaderContext::createMapContext(var);
  pushReaderContext(context);
  size = context->size();

  T_DEBUG("readMapBegin: %d", size);
  
  return 0;
}

uint32_t TNativeClientProtocol::readMapEnd() {
  T_DEBUG("readMapEnd");
  popReaderContext();
  return 0;
}

uint32_t TNativeClientProtocol::readListBegin(TType& elemType,
                                              uint32_t& size) {
  boost::shared_ptr<const pp::Var> var(readVar());
  ReaderContext* context = ReaderContext::createListContext(var);
  pushReaderContext(context);
  size = context->size(); 

  T_DEBUG("readListBegin: %d", size);

  return 0;
}

uint32_t TNativeClientProtocol::readListEnd() {
  T_DEBUG("readListEnd");
  popReaderContext();
  return 0;
}

uint32_t TNativeClientProtocol::readSetBegin(TType& elemType,
                                             uint32_t& size) {
  boost::shared_ptr<const pp::Var> var(readVar());
  ReaderContext* context = ReaderContext::createListContext(var);
  pushReaderContext(context);
  size = context->size(); 

  T_DEBUG("readSetBegin: %d", size);

  return 0;
}

uint32_t TNativeClientProtocol::readSetEnd() {
  T_DEBUG("readSetEnd");
  popReaderContext();
  return 0;
}

uint32_t TNativeClientProtocol::readBool(bool& value) {
  boost::shared_ptr<const pp::Var> var(readVar());

  if (!var->is_bool()) {
    throw TProtocolException(TProtocolException::INVALID_DATA,
                             "Expected boolean value");
  }
  value = var->AsBool();

  T_DEBUG("readBool: %s", value ? "true" : "false");

  return 0;
}

int64_t TNativeClientProtocol::readIntegerValue() {
  boost::shared_ptr<const pp::Var> var(readVar());

  int64_t int_value;

  if (var->is_int()) {
    int_value = static_cast<int64_t>(var->AsInt());
  } else if (var->is_double()) {
    double dbl_value = var->AsDouble();

    if (fmod(dbl_value, 1.0) != 0.0) {
      throw TProtocolException(TProtocolException::INVALID_DATA,
                               "Expected integer value");
    }
    if (dbl_value > kMaxPreciseDouble || dbl_value < kMinPreciseDouble) {
      throw TProtocolException(TProtocolException::SIZE_LIMIT,
                               "Double value too large for integer");
    }
    int_value = static_cast<int64_t>(dbl_value);
  } else {
      throw TProtocolException(TProtocolException::INVALID_DATA,
                               "Expected integer value");
  }

  return int_value;
}

#define READ_INTEGER_VALUE(type, value) \
  int64_t int_value = readIntegerValue(); \
  if (int_value > std::numeric_limits<type>::max() || \
      int_value < std::numeric_limits<type>::min()) { \
    throw TProtocolException(TProtocolException::SIZE_LIMIT, \
                             "Value too large for " #type); \
  } \
  value = static_cast<type>(int_value);


uint32_t TNativeClientProtocol::readByte(int8_t& byte) {
  READ_INTEGER_VALUE(int8_t, byte);
  T_DEBUG("readI8: %c", byte);

  return 0;
}

uint32_t TNativeClientProtocol::readI16(int16_t& i16) {
  READ_INTEGER_VALUE(int16_t, i16);
  T_DEBUG("readI16: %hd", i16);

  return 0;
}

uint32_t TNativeClientProtocol::readI32(int32_t& i32) {
  READ_INTEGER_VALUE(int32_t, i32);
  T_DEBUG("readI32: %d", i32);

  return 0;
}

uint32_t TNativeClientProtocol::readI64(int64_t& i64) {
  i64 = readIntegerValue();
  T_DEBUG("readI64: %lld", i64);
  return 0;
}

uint32_t TNativeClientProtocol::readDouble(double& dbl) {
  boost::shared_ptr<const pp::Var> var(readVar());

  if (var->is_double()) {
    dbl = var->AsDouble();
  } else if (var->is_int()) {
    dbl = static_cast<double>(var->AsInt());
  } else {
    throw TProtocolException(TProtocolException::INVALID_DATA,
                             "Expected double value");
  }
  dbl = var->AsDouble();
  T_DEBUG("readDouble: %f", dbl);

  return 0;
}

uint32_t TNativeClientProtocol::readString(std::string &str) {
  boost::shared_ptr<const pp::Var> var(readVar());

  if (!var->is_string()) {
    throw TProtocolException(TProtocolException::INVALID_DATA,
                             "Expected string value");
  }
  str = var->AsString();
  T_DEBUG("readString: %s", str.c_str());

  return 0;
}

uint32_t TNativeClientProtocol::readBinary(std::string &str) {
  boost::shared_ptr<const pp::Var> var(readVar());

  if (!var->is_string()) {
    throw TProtocolException(TProtocolException::INVALID_DATA,
                             "Expected base 64 string value");
  }

  if (!base64DecodeString(var->AsString(), &str)) {
    throw TProtocolException(TProtocolException::INVALID_DATA,
                             "Invalid base 64 string");
  }

  T_DEBUG("readBinary: %d", str.size());

  return 0;
}

uint32_t TNativeClientProtocol::skip(TType type) {
  T_DEBUG("skip");

  if (!reader_stack_.empty()) {
    reader_stack_.top()->advance();
  }

  return 0;
}

/**
 *  WriterContext 
 */

TNativeClientProtocol::WriterContext::WriterContext(
    boost::shared_ptr<pp::Var> var, ContextType type)
    : var_(var), type_(type) {
  assert((var->is_dictionary() &&
          (type_ == DICTIONARY_CONTEXT || type_ == MAP_KEY_CONTEXT)) ||
         (var_->is_array() && type_ == LIST_CONTEXT));
}

pp::VarDictionary* TNativeClientProtocol::WriterContext::asDictionary() {
  assert(var_->is_dictionary());
  return static_cast<pp::VarDictionary*>(var_.get());
}

pp::VarArray* TNativeClientProtocol::WriterContext::asArray() {
  assert(var_->is_array());
  return static_cast<pp::VarArray*>(var_.get());
}

void TNativeClientProtocol::WriterContext::writeVar(const pp::Var& var) {
  switch (type_) {
    case DICTIONARY_CONTEXT:
      asDictionary()->Set(pp::Var(getFieldName()), var); 
      break;

    case LIST_CONTEXT:
      asArray()->Set(asArray()->GetLength(), var);
      break;

    case MAP_KEY_CONTEXT:
      map_key_ = var;
      type_ = MAP_VALUE_CONTEXT;
      break;

    case MAP_VALUE_CONTEXT:
      asDictionary()->Set(map_key_, var); 
      type_ = MAP_KEY_CONTEXT;
      break;
  }
}

TNativeClientProtocol::WriterContext*
TNativeClientProtocol::WriterContext::createDictionaryContext() {
  boost::shared_ptr<pp::Var> var = boost::make_shared<pp::VarDictionary>();
  return new WriterContext(var, DICTIONARY_CONTEXT);
}

TNativeClientProtocol::WriterContext*
TNativeClientProtocol::WriterContext::createMapContext() {
  boost::shared_ptr<pp::Var> var = boost::make_shared<pp::VarDictionary>();
  return new WriterContext(var, MAP_KEY_CONTEXT);
}

TNativeClientProtocol::WriterContext*
TNativeClientProtocol::WriterContext::createListContext() {
  boost::shared_ptr<pp::Var> var = boost::make_shared<pp::VarArray>();
  return new WriterContext(var, LIST_CONTEXT);
}

/** 
 * ReaderContext
 */

TNativeClientProtocol::ReaderContext::ReaderContext(
    boost::shared_ptr<const pp::Var> var, ContextType type)
    : var_(var), type_(type), index_(0) {
  if (var_->is_dictionary()) {
    assert(type_ == DICTIONARY_CONTEXT || type_ == MAP_KEY_CONTEXT);
    keys_ = asDictionary()->GetKeys();
  } else {
    assert(var_->is_array() && type_ == LIST_CONTEXT);
  }
}

const pp::VarDictionary* TNativeClientProtocol::ReaderContext::asDictionary() const {
  assert(var_->is_dictionary());
  return static_cast<const pp::VarDictionary*>(var_.get());
}

const pp::VarArray* TNativeClientProtocol::ReaderContext::asArray() const {
  assert(var_->is_array());
  return static_cast<const pp::VarArray*>(var_.get());
}

int TNativeClientProtocol::ReaderContext::size() const {
  if (var_->is_dictionary()) {
    return keys_.GetLength();
  } else {
    return asArray()->GetLength();
  }
}

bool TNativeClientProtocol::ReaderContext::isAtEnd() const {
  return index_ >= size(); 
}

void TNativeClientProtocol::ReaderContext::advance() {
  if (isAtEnd()) {
    return;
  }

  switch (type_) {
    case DICTIONARY_CONTEXT:
      ++index_;
      break;

    case LIST_CONTEXT:
      ++index_;
      break;

    case MAP_KEY_CONTEXT:
      type_ = MAP_VALUE_CONTEXT;
      break;

    case MAP_VALUE_CONTEXT:
      type_ = MAP_KEY_CONTEXT;
      ++index_;
      break;
  }
}

void TNativeClientProtocol::ReaderContext::nextKeyValue(pp::Var* key,
                                                        pp::Var* value) {
  assert(index_ < keys_.GetLength());
  *key = keys_.Get(index_);
  *value = asDictionary()->Get(*key);
}

void TNativeClientProtocol::ReaderContext::nextKey(pp::Var* key) {
  assert(index_ < keys_.GetLength());
  *key = keys_.Get(index_);
}

void TNativeClientProtocol::ReaderContext::nextValue(pp::Var* value) {
  assert(index_ < keys_.GetLength());
  *value = asDictionary()->Get(keys_.Get(index_));
}

void TNativeClientProtocol::ReaderContext::nextArrayValue(pp::Var* value) {
  assert(index_ < asArray()->GetLength());
  *value = asArray()->Get(index_); 
}

void TNativeClientProtocol::ReaderContext::nextVar(pp::Var* var) {
  switch (getType()) {
    case DICTIONARY_CONTEXT:
      nextValue(var);
      break;

    case LIST_CONTEXT:
      nextArrayValue(var);
      break;

    case MAP_KEY_CONTEXT:
      nextKey(var);
      break;

    case MAP_VALUE_CONTEXT:
      nextValue(var);
      break;
  }
}

TNativeClientProtocol::ReaderContext*
TNativeClientProtocol::ReaderContext::createDictionaryContext(
    boost::shared_ptr<const pp::Var> var) {
  if (!var->is_dictionary()) {
    throw TProtocolException(TProtocolException::UNKNOWN,
        "Attempt to createDictionaryContext without a dictionary");
  }
  return new ReaderContext(var, DICTIONARY_CONTEXT);
}

TNativeClientProtocol::ReaderContext*
TNativeClientProtocol::ReaderContext::createMapContext(
    boost::shared_ptr<const pp::Var> var) {
  if (!var->is_dictionary()) {
    throw TProtocolException(TProtocolException::UNKNOWN,
        "Attempt to createMapContext without a dictionary");
  }
  return new ReaderContext(var, MAP_KEY_CONTEXT);
}

TNativeClientProtocol::ReaderContext*
TNativeClientProtocol::ReaderContext::createListContext(
    boost::shared_ptr<const pp::Var> var) {
  if (!var->is_array()) {
    throw TProtocolException(TProtocolException::UNKNOWN,
        "Attempt to createListContext without an array");
  }
  return new ReaderContext(var, LIST_CONTEXT);
}

}}} // apache::thrift::protocol

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

#ifndef _THRIFT_PROTOCOL_TNATIVECLIENTPROTOCOL_H_
#define _THRIFT_PROTOCOL_TNATIVECLIENTPROTOCOL_H_ 1

#include <thrift/protocol/TVirtualProtocol.h>

#include <stack>

#include "ppapi/cpp/var.h"
#include "ppapi/cpp/var_dictionary.h"

namespace apache { namespace thrift { namespace protocol {

class TNativeClientContext;

/**
 * Protocol for Native Client messages.
 *
 */
class TNativeClientProtocol : public TVirtualProtocol<TNativeClientProtocol> {
 public:

  TNativeClientProtocol();
  explicit TNativeClientProtocol(boost::shared_ptr<const pp::Var> var);

  void reset();

  void setVar(boost::shared_ptr<const pp::Var> var);
  boost::shared_ptr<const pp::Var> getVar() const { return root_var_; }

  /**
   * Writing functions.
   */

  uint32_t writeMessageBegin(const std::string& name,
                             const TMessageType messageType,
                             const int32_t seqid);
  uint32_t writeMessageEnd();

  uint32_t writeStructBegin(const char* name);
  uint32_t writeStructEnd();

  uint32_t writeFieldBegin(const char* name,
                           const TType fieldType,
                           const int16_t fieldId);
  uint32_t writeFieldEnd();
  uint32_t writeFieldStop();

  uint32_t writeMapBegin(const TType keyType,
                         const TType valType,
                         const uint32_t size);
  uint32_t writeMapEnd();

  uint32_t writeListBegin(const TType elemType,
                          const uint32_t size);
  uint32_t writeListEnd();

  uint32_t writeSetBegin(const TType elemType,
                         const uint32_t size);
  uint32_t writeSetEnd();

  uint32_t writeBool(const bool value);
  uint32_t writeByte(const int8_t byte);
  uint32_t writeI16(const int16_t i16);
  uint32_t writeI32(const int32_t i32);
  uint32_t writeI64(const int64_t i64);
  uint32_t writeDouble(const double dub);
  uint32_t writeString(const std::string& str);
  uint32_t writeBinary(const std::string& str);

  /**
   * Reading functions
   */

  uint32_t readMessageBegin(std::string& name,
                            TMessageType& messageType,
                            int32_t& seqid);
  uint32_t readMessageEnd();

  uint32_t readStructBegin(std::string& name);
  uint32_t readStructEnd();

  uint32_t readFieldBegin(std::string& name,
                          TType& fieldType,
                          int16_t& fieldId);
  uint32_t readFieldEnd();

  uint32_t readMapBegin(TType& keyType,
                        TType& valType,
                        uint32_t& size);
  uint32_t readMapEnd();

  uint32_t readListBegin(TType& elemType,
                         uint32_t& size);
  uint32_t readListEnd();

  uint32_t readSetBegin(TType& elemType,
                        uint32_t& size);
  uint32_t readSetEnd();

  uint32_t readBool(bool& value);
  // Provide the default readBool() implementation for std::vector<bool>
  using TVirtualProtocol<TNativeClientProtocol>::readBool;

  uint32_t readByte(int8_t& byte);
  uint32_t readI16(int16_t& i16);
  uint32_t readI32(int32_t& i32);
  uint32_t readI64(int64_t& i64);
  uint32_t readDouble(double& dub);
  uint32_t readString(std::string& str);
  uint32_t readBinary(std::string& str);

  uint32_t skip(TType type);

 private:
  enum ContextType {
    DICTIONARY_CONTEXT,
    MAP_KEY_CONTEXT,
    MAP_VALUE_CONTEXT,
    LIST_CONTEXT
  };

  class WriterContext {
   public:
    WriterContext(boost::shared_ptr<pp::Var> var, ContextType type);

    inline const std::string& getFieldName() const { return field_name_; }
    inline void setFieldName(const std::string& field_name) {
      field_name_ = field_name;
    }

    inline const pp::Var& getVar() const { return *var_; }
    void writeVar(const pp::Var& var);

    static WriterContext* createDictionaryContext();
    static WriterContext* createMapContext();
    static WriterContext* createListContext();

  private:
    pp::VarDictionary* asDictionary();
    pp::VarArray* asArray();

    boost::shared_ptr<pp::Var> var_;
    ContextType type_;
    std::string field_name_;
    pp::Var map_key_;
  };

  class ReaderContext {
   public:
    ReaderContext(boost::shared_ptr<const pp::Var> var, ContextType type);

    inline const pp::Var& getVar() const { return *var_; }
    inline ContextType getType() const { return type_; }
    inline int getIndex() const { return index_; }

    int size() const;
    bool isAtEnd() const;
    void advance();

    void nextKey(pp::Var* value);
    void nextValue(pp::Var* value);
    void nextKeyValue(pp::Var* key, pp::Var* value);
    void nextArrayValue(pp::Var* value);
    void nextVar(pp::Var* var);

    static ReaderContext* createDictionaryContext(
        boost::shared_ptr<const pp::Var> var);
    static ReaderContext* createMapContext(
        boost::shared_ptr<const pp::Var> var);
    static ReaderContext* createListContext(
        boost::shared_ptr<const pp::Var> var);

   private:
    const pp::VarDictionary* asDictionary() const;
    const pp::VarArray* asArray() const;

    boost::shared_ptr<const pp::Var> var_;
    pp::VarArray keys_;
    ContextType type_;
    int index_;
  };

 private:
  void writeVar(const pp::Var& var);
  boost::shared_ptr<const pp::Var> readVar();

  void pushWriterContext(WriterContext* context);
  void popWriterContext();

  void pushReaderContext(ReaderContext* context);
  void popReaderContext();

  int64_t readIntegerValue();

 private:
  std::stack<WriterContext*> writer_stack_;
  std::stack<ReaderContext*> reader_stack_;

  boost::shared_ptr<const pp::Var> root_var_;
};


/**
 * Constructs input and output protocol objects given transports.
 */
class TNativeClientProtocolFactory : public TProtocolFactory {
 public:
  TNativeClientProtocolFactory() {}

  virtual ~TNativeClientProtocolFactory() {}

  boost::shared_ptr<TProtocol> getProtocol(boost::shared_ptr<TTransport> trans) {
    return boost::shared_ptr<TProtocol>(new TNativeClientProtocol());
  }
};

}}} // apache::thrift::protocol

#endif // #define _THRIFT_PROTOCOL_TNATIVECLIENTPROTOCOL_H_ 1

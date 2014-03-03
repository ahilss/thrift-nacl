#ifndef MESSAGE_HANDLER_H_
#define MESSAGE_HANDLER_H_

#include <boost/shared_ptr.hpp>

#include "ppapi/cpp/var.h"
#include "thrift/protocol/TNativeClientProtocol.h"
#include "thrift_nacl_types.h"

#include <map>
#include <string>

namespace thrift_nacl {

typedef bool (*MessageHandler)(const boost::shared_ptr<pp::Var>& in,
                               boost::shared_ptr<const pp::Var>* out,
                               boost::shared_ptr<const pp::Var>* error);

bool RegisterMessageHandler(const std::string& message_type,
                            MessageHandler handler);

}  // namespace thrift_nacl

#define MAKE_HANDLER_WRAPPER(handler) \
bool handler##Wrapper(const boost::shared_ptr<pp::Var>& in, \
                      boost::shared_ptr<const pp::Var>* out, \
                      boost::shared_ptr<const pp::Var>* err) { \
  boost::shared_ptr<apache::thrift::protocol::TNativeClientProtocol> protocol(\
      new apache::thrift::protocol::TNativeClientProtocol()); \
  handler##Request request; \
  handler##Response response; \
  ThriftNaClError error; \
\
  protocol->setVar(in); \
  request.read(protocol.get()); \
\
  if (handler(request, &response, &error)) { \
    response.write(protocol.get()); \
    *out = protocol->getVar(); \
    return true; \
  } else { \
    error.write(protocol.get()); \
    *err = protocol->getVar(); \
    return false; \
  } \
}

#define REGISTER_MESSAGE_HANDLER(message_type, handler) \
MAKE_HANDLER_WRAPPER(handler) \
static bool handler##_result = thrift_nacl::RegisterMessageHandler( \
  std::string(message_type), handler##Wrapper);


#endif  // MESSAGE_HANDLER_H_

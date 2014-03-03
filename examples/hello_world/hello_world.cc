#include "hello_world_types.h"
#include "thrift_nacl.h"

namespace hello_world {

bool TestMessage(const TestMessageRequest& request,
                 TestMessageResponse* response,
                 ThriftNaClError* error) {
  if (request.has_name() && !request.get_name().empty()) {
    response->set_result("Hello " + request.get_name());
  } else {
    response->set_result("Hello World!");
  }
  return true;
}

REGISTER_MESSAGE_HANDLER("testMessage", TestMessage)

}  // namespace hello_world

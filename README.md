Thrift for Native Client
========================

Thrift for Native Client is a port of the Apache Thrift (http://thrift.apache.org/) as well as a framework for simplifying implementation of Native Client modules.

Prerequisites:
--------------

- Native Client SDK must be installed (https://developers.google.com/native-client/dev/).
- naclports must be installed (https://code.google.com/p/naclports/).
- Environment variable NACL_SDK_ROOT must be set to the desired pepper_* directory.
- Environment variable NACLPORTS_DIR must be set to the naclports location.

Getting Started:
----------------

This package includes an example of an Native Client module using the Thrift NaCl framework.  To build the example, open examples/hello_world and type ‘make’.  After successfully building the module, it will need to be hosted on a web server.  A script has been included to start a local web server by running ./run_server.sh.  The example can be viewed by opening http://localhost:5103/hello_world.html in the Google Chrome web browser.

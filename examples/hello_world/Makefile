VALID_TOOLCHAINS := pnacl newlib

include $(NACL_SDK_ROOT)/tools/common.mk


TARGET = hello_world
DEPS =
LIBS = $(DEPS) thrift z nacl_io ppapi_cpp ppapi

THIRD_PARTY_PREFIX=$(TC_PATH)/$(OSNAME)_$(TOOLCHAIN)
CXXFLAGS = -Wall -I. -Igen-cpp -I$(THIRD_PARTY_PREFIX)/usr/include
LDFLAGS =  -I$(THIRD_PARTY_PREFIX)/usr/lib

SOURCES = \
gen-cpp/hello_world_types.cpp \
gen-cpp/thrift_nacl_types.cpp \
hello_world.cc \
thrift_nacl.cc

THRIFT = ../../build/usr/bin/thrift

$(THRIFT):
	../../tools/build-deps.sh

gen-cpp/%_types.cpp: %.thrift $(THRIFT)
	$(THRIFT) --gen cpp $<

# Build rules generated by macros from common.mk:

$(foreach dep,$(DEPS),$(eval $(call DEPEND_RULE,$(dep))))
$(foreach src,$(SOURCES),$(eval $(call COMPILE_RULE,$(src),$(CXXFLAGS))))

ifeq ($(CONFIG),Release)
$(eval $(call LINK_RULE,$(TARGET)_unstripped,$(SOURCES),$(LIBS),$(DEPS),$(LDFLAGS)))
$(eval $(call STRIP_RULE,$(TARGET),$(TARGET)_unstripped))
else
$(eval $(call LINK_RULE,$(TARGET),$(SOURCES),$(LIBS),$(DEPS),$(LDFLAGS)))
endif

$(eval $(call NMF_RULE,$(TARGET),))

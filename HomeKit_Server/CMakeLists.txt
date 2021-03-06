cmake_minimum_required(VERSION 3.5)
project(homekit)
SET(TARGET homekit_demo)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -m32")
include_directories(include accessory profiles hap
        rfc6234-master
        srp
        Chacha20 poly1305-opt-master ed25519-donna
        curve25519
        )
include_directories(/usr/include/sys)
link_directories(./lib)

set(SOURCE_FILES  main.c
        accessory/accessory.c
        profiles/window_covering.c profiles/speaker.c
        profiles/bridge.c
        hap/bonjour.c hap/mthread.c hap/http_handle.c hap/ip.c hap/pairing.c hap/tlv.c hap/list.c hap/controller.c hap/ChaCha20-Poly1305.c
        rfc6234-master/hkdf.c  rfc6234-master/hmac.c  rfc6234-master/sha_m.c
        Chacha20/chacha20_simple.c
        poly1305-opt-master/poly1305.c
        ed25519-donna/ed25519.c
        curve25519/curve25519-donna.c
        profiles/profile.c)

add_library(libsrp.a STATIC
        srp/cstr.c  srp/srp6_server.c  srp/srp.c  srp/t_conf.c  srp/t_conv.c
        srp/t_math.c  srp/t_misc.c  srp/t_pw.c  srp/t_read.c  srp/t_truerand.c
        )

add_executable(${TARGET} ${SOURCE_FILES} )
target_link_libraries(${TARGET}
        -lpthread -lssl -lcrypto -ljson-c
        libsrp.a libdns_sd.so
        )
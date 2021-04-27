cmake_minimum_required(VERSION 3.16)
project(ecdsa_cpp)

#set(CMAKE_CXX_STANDARD 11)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2 -pthread -fopenmp")

#add_executable(ecdsa main.cpp algorithm/Ecdsa.cpp algorithm/Ecdsa.h algorithm/BigInt.cpp algorithm/BigInt.h algorithm/hash/Sha256.cpp algorithm/hash/Sha256.h algorithm/hash/Hmac.cpp algorithm/hash/Hmac.h algorithm/Int.cpp algorithm/Int.h algorithm/EcdsaBigInt.cpp algorithm/EcdsaBigInt.h)

aux_source_directory(algorithm ALG)
aux_source_directory(algorithm/hash HASH)
add_executable(ecdsa main.cpp ${ALG} ${HASH})
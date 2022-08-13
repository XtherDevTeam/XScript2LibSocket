//
// Created by p0010 on 22-8-13.
//

#ifndef NATIVE_LIBRARY_H
#define NATIVE_LIBRARY_H

#include "Share/Utils.hpp"
#include "LibNetworking/Utils/NetworkApi.hpp"

extern "C" XScript::NativeLibraryInformation Initialize();

XScript::EnvClassObject * CloneClientObject(XScript::BytecodeInterpreter *Interpreter);

/**
 * SocketClient.connect(addr)
 * @function Connect to a remote socket server
 * @param addr like 192.168.1.1:65535
 * @return A SocketClient object, with __server_fd__
 */
void SocketClient_connect(XScript::ParamToMethod Param);

/**
 * SocketClient.receive(size)
 * @function receive data from remote socket server
 * @param size receive size
 * @return If success, return a EnvBytesObject buffer. Otherwise, return a InternalError object.
 * @warning This method requires a initialized SocketClient object.
 */
void SocketClient_receive(XScript::ParamToMethod Param);

/**
 * SocketClient.send(buffer)
 * @function send data to remote socket server
 * @param buffer EnvBytesObject, can get with String.toBytes()
 * @return If success, return the object itself. Otherwise, return a InternalError object.
 * @warning This method requires a initialized SocketClient object.
 */
void SocketClient_send(XScript::ParamToMethod Param);

/**
 * SocketClient.close()
 * @function close socket file
 * @return If success, return the object itself. Otherwise, return a InternalError object.
 * @warning This method requires a initialized SocketClient object.
 */
void SocketClient_close(XScript::ParamToMethod Param);

#endif //NATIVE_LIBRARY_H

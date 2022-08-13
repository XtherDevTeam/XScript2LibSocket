//
// Created by p0010 on 22-8-13.
//

#include "library.h"
#include "LibNetworking/Socket/SocketAddress.hpp"
#include "LibNetworking/Utils/LibNetworkException.hpp"

XScript::NativeLibraryInformation Initialize() {
    XScript::XMap<XScript::XIndexType, XScript::NativeMethodInformation> Methods;
    XScript::XMap<XScript::XIndexType, XScript::NativeClassInformation> Classes;

    Methods[XScript::Hash(L"connect")] = {2, SocketClient_connect};
    Methods[XScript::Hash(L"receive")] = {2, SocketClient_receive};
    Methods[XScript::Hash(L"send")] = {2, SocketClient_send};
    Methods[XScript::Hash(L"close")] = {2, SocketClient_close};
    Classes[XScript::Hash(L"SocketClient")] = {L"SocketClient", Methods};
    return {
            L"Jerry Chou",
            L"XScript 2 LibSocket NativeLibrary Part",
            Classes};
}

XScript::EnvClassObject *CloneClientObject(XScript::BytecodeInterpreter *Interpreter) {
    XScript::EnvClassObject *This = Interpreter->InterpreterEnvironment->Heap.HeapData[
            Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.Elements[
                    Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.FramesInformation.back().From].Value.HeapPointerVal
    ].Value.ClassObjectPointer;
    XScript::EnvClassObject *New = XScript::NewEnvClassObject();
    *New = XScript::EnvClassObject{*This};

    return New;
}

void SocketClient_connect(XScript::ParamToMethod Param) {
    using namespace XScript;
    auto *Interpreter = static_cast<BytecodeInterpreter *>(Param.InterpreterPointer);
    LibNetworking::SocketAddress Addr;
    auto String = GetStringObject(
            *Interpreter,
            Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.PopValueFromStack());
    try {
        Addr = LibNetworking::SocketAddress(wstring2string(CovertToXString(String)));
    } catch (const LibNetworking::LibNetworkException &E) {
        PushClassObjectStructure(Interpreter, ConstructInternalErrorStructure(
                Interpreter,
                L"SocketError",
                string2wstring(E.what())));
        Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
        return;
    }

    int SocketFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (SocketFd == -1) {
        PushClassObjectStructure(Interpreter, ConstructInternalErrorStructure(
                Interpreter,
                L"SocketError",
                string2wstring(strerror(errno))));
        Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
        return;
    }

    sockaddr_in ServerAddr{};
    ServerAddr.sin_family = AF_INET;
    ServerAddr.sin_addr.s_addr = inet_addr(Addr.Host.c_str());
    ServerAddr.sin_port = htons(Addr.Port);
    if (connect(SocketFd, (sockaddr *) &ServerAddr, sizeof(ServerAddr)) == -1) {
        close(SocketFd);

        PushClassObjectStructure(Interpreter, ConstructInternalErrorStructure(
                Interpreter,
                L"SocketError",
                string2wstring(strerror(errno))));
        Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
        return;
    }

    auto C = CloneClientObject(Interpreter);
    C->Members[Hash(L"__server_fd__")] = Interpreter->InterpreterEnvironment->Heap.PushElement(
            {XScript::EnvObject::ObjectKind::Integer, (EnvObject::ObjectValue) static_cast<XInteger>(SocketFd)});
    PushClassObjectStructure(Interpreter, C);

    Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
}

void SocketClient_receive(XScript::ParamToMethod Param) {
    using namespace XScript;
    auto *Interpreter = static_cast<BytecodeInterpreter *>(Param.InterpreterPointer);

    XScript::EnvClassObject *This = Interpreter->InterpreterEnvironment->Heap.HeapData[
            Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.Elements[
                    Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.FramesInformation.back().From].Value.HeapPointerVal
    ].Value.ClassObjectPointer;

    XInteger SocketFileNo = Interpreter->InterpreterEnvironment->Heap.HeapData[This->Members[Hash(
            L"__server_fd__")]].Value.IntegerValue;

    XInteger RecvSize = Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.PopValueFromStack().Value.IntVal;
    EnvBytesObject *Buffer = CreateEnvBytesObject(RecvSize);
    if (recv(static_cast<int>(SocketFileNo), &Buffer->Dest, RecvSize, 0) == -1) {
        PushClassObjectStructure(Interpreter, ConstructInternalErrorStructure(
                Interpreter,
                L"SocketError",
                string2wstring(strerror(errno))));
        Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
        return;
    }

    Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.PushValueToStack(
            {
                    EnvironmentStackItem::ItemKind::HeapPointer,
                    (EnvironmentStackItem::ItemValue) Interpreter->InterpreterEnvironment->Heap.PushElement(
                            {XScript::EnvObject::ObjectKind::BytesObject, (EnvObject::ObjectValue) Buffer})});
    Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
}

void SocketClient_send(XScript::ParamToMethod Param) {
    using namespace XScript;
    auto *Interpreter = static_cast<BytecodeInterpreter *>(Param.InterpreterPointer);

    XScript::EnvClassObject *This = Interpreter->InterpreterEnvironment->Heap.HeapData[
            Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.Elements[
                    Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.FramesInformation.back().From].Value.HeapPointerVal
    ].Value.ClassObjectPointer;

    XInteger SocketFileNo = Interpreter->InterpreterEnvironment->Heap.HeapData[This->Members[Hash(
            L"__server_fd__")]].Value.IntegerValue;

    auto Buffer = Interpreter->InterpreterEnvironment->Heap.HeapData[
            Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.PopValueFromStack().Value.HeapPointerVal].Value.BytesObjectPointer;

    if (send(static_cast<int>(SocketFileNo), &Buffer->Dest, Buffer->Length, 0) == -1) {
        PushClassObjectStructure(Interpreter, ConstructInternalErrorStructure(
                Interpreter,
                L"SocketError",
                string2wstring(strerror(errno))));
        Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
        return;
    }

    Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
}

void SocketClient_close(XScript::ParamToMethod Param) {
    using namespace XScript;
    auto *Interpreter = static_cast<BytecodeInterpreter *>(Param.InterpreterPointer);

    XScript::EnvClassObject *This = Interpreter->InterpreterEnvironment->Heap.HeapData[
            Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.Elements[
                    Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.FramesInformation.back().From].Value.HeapPointerVal
    ].Value.ClassObjectPointer;

    XInteger SocketFileNo = Interpreter->InterpreterEnvironment->Heap.HeapData[This->Members[Hash(
            L"__server_fd__")]].Value.IntegerValue;

    if (close(static_cast<int>(SocketFileNo)) == -1) {
        PushClassObjectStructure(Interpreter, ConstructInternalErrorStructure(
                Interpreter,
                L"SocketError",
                string2wstring(strerror(errno))));
        Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
        return;
    }

    Interpreter->InstructionFuncReturn((BytecodeStructure::InstructionParam) {(XIndexType) {}});
}

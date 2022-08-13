//
// Created by p0010 on 22-8-13.
//

#ifndef NATIVE_UTILS_HPP
#define NATIVE_UTILS_HPP


#include "../XScript2/Executor/EnvStringObject.hpp"
#include "../XScript2/Executor/BytecodeInterpreter.hpp"

XScript::EnvClassObject *
ConstructInternalErrorStructure(XScript::BytecodeInterpreter *Interpreter, const XScript::XString &ErrorName,
                                const XScript::XString &ErrorDescription);

void PushClassObjectStructure(XScript::BytecodeInterpreter *Interpreter, XScript::EnvClassObject *Object);

XScript::XHeapIndexType CloneObject(XScript::BytecodeInterpreter *Interpreter, XScript::XHeapIndexType Target);

XScript::EnvStringObject *GetStringObject(XScript::BytecodeInterpreter &Interpreter, const XScript::EnvironmentStackItem &Item);


#endif //NATIVE_UTILS_HPP

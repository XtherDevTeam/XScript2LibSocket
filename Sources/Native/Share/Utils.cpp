//
// Created by p0010 on 22-8-13.
//

#include "Utils.hpp"

XScript::EnvClassObject *
ConstructInternalErrorStructure(XScript::BytecodeInterpreter *Interpreter, const XScript::XString &ErrorName,
                                const XScript::XString &ErrorDescription) {
    using namespace XScript;
    EnvClassObject *Object = NewEnvClassObject();
    Object->Self = {Hash(L"Error"), Hash(L"Error")};
    Object->Members[Hash(L"errorName")] = Interpreter->InterpreterEnvironment->Heap.PushElement(
            {
                    XScript::EnvObject::ObjectKind::StringObject,
                    (EnvObject::ObjectValue) CreateEnvStringObjectFromXString(ErrorName)
            });
    Object->Members[Hash(L"errorDescription")] = Interpreter->InterpreterEnvironment->Heap.PushElement(
            {
                    XScript::EnvObject::ObjectKind::StringObject,
                    (EnvObject::ObjectValue) CreateEnvStringObjectFromXString(ErrorDescription)
            });

    return Object;
}


XScript::EnvStringObject *
GetStringObject(XScript::BytecodeInterpreter &Interpreter, const XScript::EnvironmentStackItem &Item) {
    if (Item.Kind == XScript::EnvironmentStackItem::ItemKind::HeapPointer) {
        auto &First = Interpreter.InterpreterEnvironment->Heap.HeapData[Item.Value.HeapPointerVal];
        switch (First.Kind) {
            case XScript::EnvObject::ObjectKind::StringObject:
                return First.Value.StringObjectPointer;
            case XScript::EnvObject::ObjectKind::ClassObject:
                return Interpreter.InterpreterEnvironment->Heap.HeapData[First.Value.ClassObjectPointer->Members[XScript::Hash(
                        L"__buffer__")]].Value.StringObjectPointer;
            default:
                return nullptr;
        }
    }
    return nullptr;
}

void PushClassObjectStructure(XScript::BytecodeInterpreter *Interpreter, XScript::EnvClassObject *Object) {
    using namespace XScript;
    Interpreter->InterpreterEnvironment->Threads[Interpreter->ThreadID].Stack.PushValueToStack(
            {
                    EnvironmentStackItem::ItemKind::HeapPointer,
                    (EnvironmentStackItem::ItemValue) Interpreter->InterpreterEnvironment->Heap.PushElement(
                            {EnvObject::ObjectKind::ClassObject,
                             (EnvObject::ObjectValue) Object
                            })
            });
}

XScript::XHeapIndexType CloneObject(XScript::BytecodeInterpreter *Interpreter, XScript::XHeapIndexType Target) {
    using namespace XScript;
    auto I = Interpreter->InterpreterEnvironment->Heap.HeapData[Target];
    switch (I.Kind) {
        case XScript::EnvObject::ObjectKind::ArrayObject: {
            EnvArrayObject *Arr = NewEnvArrayObject(I.Value.ArrayObjectPointer->Length());
            XIndexType N = 0;
            for (auto &item : Arr->Elements) {
                Arr->Elements[N++] = CloneObject(Interpreter, item);
            }
            return Interpreter->InterpreterEnvironment->Heap.PushElement(
                    {XScript::EnvObject::ObjectKind::ArrayObject, (EnvObject::ObjectValue) Arr});
        }
        case XScript::EnvObject::ObjectKind::StringObject: {
            EnvStringObject *Str = CreateEnvStringObject(I.Value.StringObjectPointer->Length);
            for (XIndexType Idx = 0;Idx < I.Value.StringObjectPointer->Length;Idx++) {
                (&Str->Dest)[Idx] = (&I.Value.StringObjectPointer->Dest)[Idx];
            }
            return Interpreter->InterpreterEnvironment->Heap.PushElement(
                    {XScript::EnvObject::ObjectKind::StringObject, (EnvObject::ObjectValue) Str});
        }
        case XScript::EnvObject::ObjectKind::ClassObject: {
            EnvClassObject *Obj = NewEnvClassObject();
            XIndexType N = 0;
            Obj->Self = I.Value.ClassObjectPointer->Self;
            Obj->Parent = I.Value.ClassObjectPointer->Parent;
            for (auto &item : I.Value.ClassObjectPointer->Members) {
                Obj->Members[item.first] = CloneObject(Interpreter, item.second);
            }
            return Interpreter->InterpreterEnvironment->Heap.PushElement(
                    {XScript::EnvObject::ObjectKind::ArrayObject, (EnvObject::ObjectValue) Obj});
        }
        case XScript::EnvObject::ObjectKind::BytesObject: {
            EnvBytesObject *Str = CreateEnvBytesObject(I.Value.BytesObjectPointer->Length);
            for (XIndexType Idx = 0;Idx < I.Value.BytesObjectPointer->Length;Idx++) {
                (&Str->Dest)[Idx] = (&I.Value.BytesObjectPointer->Dest)[Idx];
            }
            return Interpreter->InterpreterEnvironment->Heap.PushElement(
                    {XScript::EnvObject::ObjectKind::BytesObject, (EnvObject::ObjectValue) Str});
        }
        default:
            return Interpreter->InterpreterEnvironment->Heap.PushElement(I);
    }
}
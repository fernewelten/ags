//=============================================================================
//
// Adventure Game Studio (AGS)
//
// Copyright (C) 1999-2011 Chris Jones and 2011-20xx others
// The full list of copyright holders can be found in the Copyright.txt
// file, which is part of this source code distribution.
//
// The AGS source code is provided under the Artistic License 2.0.
// A copy of this license can be found in the file License.txt and at
// http://www.opensource.org/licenses/artistic-license-2.0.php
//
//=============================================================================

#include <string.h>
#include "cc_delegate.h"

int CCDelegate::Dispose(const char *address, bool force) {

    if (nullptr == address || force)
        return; // *shrug*

    DelegateObject &delg = Address2DelegateObject(address);

    for (auto it = delg.Funcs.begin(); it != delg.Funcs.end(); ++it)
        if (it->ObjHandle != 0)
            ccReleaseObjectReference(it->ObjHandle);
}

int CCDelegate::Serialize(const char *address, char *buffer, int const bufsize) {

    if (nullptr == address)
        return; // *shrug*

    DelegateObject &delg = Address2DelegateObject(address);

    int const needed_size = DelegateObject::SerializedSize(delg.Funcs.size());
    if (needed_size > bufsize || nullptr == buffer)
        return -needed_size; // Request a buffer that is large enough

    // First int32: Number of entries in the delegate object
    int32_t *count_ptr = reinterpret_cast<int32_t *>(buffer);
    *count_ptr = delg.Funcs.size();

    // Next entries: Blocks of (function offset, handle)
    FuncRef *entries = reinterpret_cast<FuncRef *>(buffer + sizeof(int32_t));
    for (size_t idx = 0; idx < *count_ptr; idx++)
        entries[idx] = delg.Funcs[idx];

    return needed_size;
}

void CCDelegate::Unserialize(int index, const char *buffer, int data_size) {

    int32_t count = *reinterpret_cast<int32_t const *>(buffer);
    if (data_size < DelegateObject::SerializedSize(count))
        return; // Something's off, but there doesn't seem to be a way to warn.

    FuncRef const *entries = reinterpret_cast<FuncRef const *>(buffer + sizeof(int32_t));

    DelegateObject *delg = new DelegateObject();
    delg->Funcs = DelegateObject::FuncRefsT{ entries, entries + count };

    ccRegisterUnserializedObject(index, delg, this);
}

DynObjectRef CCDelegate::Create(void)
{
    DelegateObject * const new_delg = new DelegateObject();
    int32_t const handle = ccRegisterManagedObject(new_delg, this);

    if (!handle)
    {
        delete new_delg;
        return DynObjectRef(0, nullptr);
    }
    return DynObjectRef(handle, new_delg);
}

CCDelegate globalDelegate;



#include "script/script_api.h"
#include "script/script_runtime.h"

void Delegate_Add(CCDelegate::DelegateObject *delg, int32_t const func_offset, int32_t const obj_handle)
{
    if (nullptr == delg)
        return; // *shrug*
    delg->Funcs.emplace_back(func_offset, obj_handle);
}

void Delegate_Clear(CCDelegate::DelegateObject *delg)
{
    if (nullptr == delg)
        return; // *shrug*
    delg->Funcs.clear();

}

int32_t const *Delegate_GetBegin(CCDelegate::DelegateObject *delg)
{
    if (nullptr == delg)
        return 0; // *shrug*
    return reinterpret_cast<int32_t const *>(&delg->Funcs.cbegin());
}

int32_t const * Delegate_GetEnd(CCDelegate::DelegateObject *delg)
{
    if (nullptr == delg)
        return 0; // *shrug*
    return reinterpret_cast<int32_t const *>(&delg->Funcs.cend());
}

void Delegate_Remove(CCDelegate::DelegateObject *delg, int32_t const func_offset, int32_t const obj_handle)
{
    if (nullptr == delg)
        return; // *shrug*

    int offset = 0u;
    auto &Funcs = delg->Funcs;

    for (size_t idx = 0u; idx < Funcs.size(); idx++)
    {
        if (offset > 0u)
            Funcs[idx - offset] = Funcs[idx];
        if (func_offset == Funcs[idx].FuncOffset && obj_handle == Funcs[idx].ObjHandle)
            offset++;
    }
    if (offset > 0u)
        Funcs.resize(Funcs.size() - offset);
}


RuntimeScriptValue Sc_DynamicArray_Length(const RuntimeScriptValue *params, int32_t param_count)
{
    API_SCALL_INT_POBJ(DynamicArray_Length, void);
}

void RegisterDynamicArrayAPI()
{
    ccAddExternalStaticFunction("__Builtin_DynamicArrayLength^1", Sc_DynamicArray_Length);
}


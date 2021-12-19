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
#ifndef __CC_DELEGATE_H
#define __CC_DELEGATE_H

#include <vector>
#include "ac/dynobj/cc_dynamicobject.h"   // ICCDynamicObject

struct CCDelegate final : ICCDynamicObject
{
public:
    static constexpr char *CC_DELEGATE_TYPE_NAME = "CCDelegate";

    struct FuncRef
    {
        int32_t FuncOffset; // An offset to a function, with FIXUP_IMPORT already implied
        int32_t ObjHandle;  // If the function is a non-static struct function, a dynpointer to its object
    };
    typedef std::vector<FuncRef> FuncRefsT;

    // The object that will be allocated in memory
    struct DelegateObject 
    {
        FuncRefsT Funcs = {};

        inline void Add(int32_t func_offset, int32_t obj_handle) { Funcs.emplace_back(func_offset, obj_handle); }
        inline void Clear() { Funcs.clear(); }
        inline int32_t const *GetBegin() const { return reinterpret_cast<int32_t const *>(&Funcs.begin()); }
        inline int32_t const *GetEnd() const { return reinterpret_cast<int32_t const *>(&Funcs.end()); }
        void Remove(int32_t func_offset, int32_t obj_handle);
    };

    // Return the type name of the object
    const char *GetType() override { return CC_DELEGATE_TYPE_NAME; }
    int Dispose(const char *address, bool force) override;

    // Serialize the object into 'buffer' (which is 'bufsize' bytes), return number of bytes used.
    // If the 'buffer' is too small, don't serialize but return the negative of the number of bytes needed
    int Serialize(const char *address, char *buffer, int bufsize) override;

    // Retrieve the delegate from 'buffer'.
    virtual void Unserialize(int index, const char *buffer, int data_size);

    // Create delegate
    DynObjectRef Create();

    // Legacy NON-support for reading and writing object values by their relative offset:
    // Those functions are UNsupported. Nothing good can come from writing integers
    // into the place where a std::vector is, or reading integers from that place.
    // So all the readers will return 0, all the writers won't do anything.
    const char* GetFieldPtr(const char *address, intptr_t offset) override { return nullptr; }
    void    Read(const char *address, intptr_t offset, void *dest, int size) override { return; }
    uint8_t ReadInt8(const char *address, intptr_t offset) override { return 0; }
    int16_t ReadInt16(const char *address, intptr_t offset) override { return 0; }
    int32_t ReadInt32(const char *address, intptr_t offset) override { return 0; }
    float   ReadFloat(const char *address, intptr_t offset) override { return 0; }
    void    Write(const char *address, intptr_t offset, void *src, int size) override { return; }
    void    WriteInt8(const char *address, intptr_t offset, uint8_t val) override { return; }
    void    WriteInt16(const char *address, intptr_t offset, int16_t val) override { return; }
    void    WriteInt32(const char *address, intptr_t offset, int32_t val) override { return; }
    void    WriteFloat(const char *address, intptr_t offset, float val) override { return; }

private:
    // Cast a char * so that it becomes the pointer of a delegate object
    inline auto &Address2DelegateObject(const char *address)
        { return *reinterpret_cast<DelegateObject *>(const_cast<char *>(address)); }

    // Number of bytes needed to serialize a delegate object.
    inline static size_t SerializedSize(size_t count) { return sizeof(int32_t) + count * sizeof(FuncRef); }
};

extern CCDelegate globalDelegate;

#endif

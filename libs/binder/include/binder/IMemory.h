/*
 * Copyright (C) 2007 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#pragma once

#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>

#include <utils/RefBase.h>
#include <utils/Errors.h>
#include <binder/Common.h>
#include <binder/IInterface.h>

namespace android {

// ----------------------------------------------------------------------------

class LIBBINDER_EXPORTED IMemoryHeap : public IInterface {
public:
    DECLARE_META_INTERFACE(MemoryHeap)

    // flags returned by getFlags()
    enum {
        READ_ONLY   = 0x00000001
    };

    virtual int         getHeapID() const = 0;
    virtual void*       getBase() const = 0;
    virtual size_t      getSize() const = 0;
    virtual uint32_t    getFlags() const = 0;
    virtual off_t       getOffset() const = 0;

    // these are there just for backward source compatibility
    int32_t heapID() const { return getHeapID(); }
    void*   base() const  { return getBase(); }
    size_t  virtualSize() const { return getSize(); }
};

class LIBBINDER_EXPORTED BnMemoryHeap : public BnInterface<IMemoryHeap> {
public:
    // NOLINTNEXTLINE(google-default-arguments)
    virtual status_t onTransact(
            uint32_t code,
            const Parcel& data,
            Parcel* reply,
            uint32_t flags = 0);
    
    BnMemoryHeap();
protected:
    virtual ~BnMemoryHeap();
};

// ----------------------------------------------------------------------------

class LIBBINDER_EXPORTED IMemory : public IInterface {
public:
    DECLARE_META_INTERFACE(Memory)

    // NOLINTNEXTLINE(google-default-arguments)
    virtual sp<IMemoryHeap> getMemory(ssize_t* offset=nullptr, size_t* size=nullptr) const = 0;

    // helpers

    // Accessing the underlying pointer must be done with caution, as there are
    // some inherent security risks associated with it. When receiving an
    // IMemory from an untrusted process, there is currently no way to guarantee
    // that this process would't change the content after the fact. This may
    // lead to TOC/TOU class of security bugs. In most cases, when performance
    // is not an issue, the recommended practice is to immediately copy the
    // buffer upon reception, then work with the copy, e.g.:
    //
    // std::string private_copy(mem.size(), '\0');
    // memcpy(private_copy.data(), mem.unsecurePointer(), mem.size());
    //
    // In cases where performance is an issue, this matter must be addressed on
    // an ad-hoc basis.
    void* unsecurePointer() const;

    size_t size() const;
    ssize_t offset() const;

private:
    // These are now deprecated and are left here for backward-compatibility
    // with prebuilts that may reference these symbol at runtime.
    // Instead, new code should use unsecurePointer()/unsecureFastPointer(),
    // which do the same thing, but make it more obvious that there are some
    // security-related pitfalls associated with them.
    void* pointer() const;
    void* fastPointer(const sp<IBinder>& heap, ssize_t offset) const;
};

class LIBBINDER_EXPORTED BnMemory : public BnInterface<IMemory> {
public:
    // NOLINTNEXTLINE(google-default-arguments)
    virtual status_t onTransact(
            uint32_t code,
            const Parcel& data,
            Parcel* reply,
            uint32_t flags = 0);

    BnMemory();
protected:
    virtual ~BnMemory();
};

// ----------------------------------------------------------------------------

} // namespace android

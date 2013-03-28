// 
// Cache of loaded executable module symbols:
// Map: module -> symbol session
// 

#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "symengine.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

struct SymCacheModuleKey {
    std::string name;
    ULONG  size;
    ULONG  timeStamp;
    ULONG  checkSum;

    bool operator < ( const SymCacheModuleKey& key ) const
    {
        if ( name < key.name )
            return true;
        if ( name > key.name )
            return false;

        if ( size < key.size )
            return true;
        if ( size > key.size )
            return false;

        if ( timeStamp < key.timeStamp )
            return true;
        if ( timeStamp > key.timeStamp )
            return false;

        return checkSum < key.checkSum;
    }
};

////////////////////////////////////////////////////////////////////////////////

bool findSymCacheEntry(const SymCacheModuleKey &cacheKey, SymbolSessionPtr &symSession);

void insertSymCacheEntry(ULONG64 modBase, const SymCacheModuleKey &cacheKey, SymbolSessionPtr symSession);

void eraseSymCacheEntry(const SymCacheModuleKey &cacheKey);

void clearSymCache();

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////

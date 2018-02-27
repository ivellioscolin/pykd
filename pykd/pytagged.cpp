
#include "stdafx.h"

#include "pytagged.h"
#include "kdlib\tagged.h"
#include "kdlib\exceptions.h"

#include "pythreadstate.h"
#include "stladaptor.h"

#include <rpc.h>
#pragma comment(lib, "Rpcrt4.lib")


namespace pykd {


void __declspec(noreturn) throwRpcStatus( const std::string  &functionName, RPC_STATUS status ) 
{
    std::stringstream sstr;
    sstr << "Call " << functionName << " failed\n";
    sstr << "RPC_STATUS 0x" << std::hex << status;
    throw kdlib::DbgException( sstr.str() );
}

python::list enumTagged()
{
    std::list<kdlib::TaggedId> ids;
    {
        AutoRestorePyState pystate;
        ids = std::move( kdlib::enumTagged() );
    }

    python::list result;

    for (const auto &id : ids)
    {
        RPC_WSTR id_str = nullptr;
        auto status = ::UuidToString(&id, &id_str);
        if (RPC_S_OK != status)
            throwRpcStatus("UuidToString", status);

        const auto stringFree = 
            [](RPC_WSTR *str)
            {
                auto status = ::RpcStringFree(str);
                if (RPC_S_OK != status)
                    throwRpcStatus("RpcStringFree", status);
            };
        std::unique_ptr<RPC_WSTR, decltype(stringFree)> freeGuard{&id_str, stringFree};

        result.append( std::wstring(reinterpret_cast<wchar_t *>(id_str)) );
    }
    return result;
}

python::list loadTaggedBuffer(const std::wstring &id_str)
{
    const auto rcp_str = 
        reinterpret_cast<RPC_WSTR>( const_cast<wchar_t *>(id_str.c_str()) );
    kdlib::TaggedId id;
    auto status = ::UuidFromString(rcp_str, &id);
    if (RPC_S_OK != status)
        throwRpcStatus("UuidFromString", status);


    kdlib::TaggedBuffer buff;
    {
        AutoRestorePyState pystate;
        buff = std::move( kdlib::loadTaggedBuffer(id) );
    }

    return vectorToList(buff);
}

}   // namespace pykd

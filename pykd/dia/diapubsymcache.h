
#pragma once

namespace pykd {

//////////////////////////////////////////////////////////////////////////////

class DiaPublicSymbolCache : boost::noncopyable
{
public:
    DiaPublicSymbolCache(DiaSymbolPtr globalScope);

    // Lookup entry by name
    // Sta.
    DiaSymbolPtr lookup(const std::string &name) const;

    LONGLONG getBuildTimeInSeconds() const {
        return m_buildTimeInSeconds;
    }

private:
    typedef std::map< std::string, DiaSymbolPtr > Impl;
    Impl m_impl;

    LONGLONG m_buildTimeInSeconds;
};
typedef boost::shared_ptr< DiaPublicSymbolCache > DiaPublicSymbolCachePtr;

//////////////////////////////////////////////////////////////////////////////

} // end pykd namespace

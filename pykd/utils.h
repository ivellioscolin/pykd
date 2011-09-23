
#pragma once

#include <vector>
#include <list>

////////////////////////////////////////////////////////////////////////////////
// std::string -> const WCHAR *
////////////////////////////////////////////////////////////////////////////////
class toWStr {
public:
    toWStr(const std::string &sz)
    {
        m_buf.resize( sz.size() + 1, L'\0' );
        ::MultiByteToWideChar(
            CP_ACP,
            0,
            sz.c_str(),
            static_cast<int>( sz.size() ),
            &m_buf[0],
            static_cast<int>( m_buf.size() ) );
    }

    operator const WCHAR *() const {
        return m_buf.empty() ? NULL : &m_buf[0];
    }

private:
    std::vector<WCHAR> m_buf;
};

////////////////////////////////////////////////////////////////////////////////
// BSTR wrapper
////////////////////////////////////////////////////////////////////////////////
class autoBstr {
public:
    autoBstr(BSTR bstr = NULL) : m_bstr(bstr) {}
    ~autoBstr() {
        free();
    }

    void free()
    {
        if (m_bstr)
        {
            SysFreeString(m_bstr);
            m_bstr = NULL;
        }
    }

    BSTR *operator &() {
        return &m_bstr;
    }
    operator BSTR() const {
        return m_bstr;
    }

    std::string asStr() const
    {
        return asStr(m_bstr);
    }
    static std::string asStr(BSTR bstr)
    {
        if (!bstr)
            return "";

        const int chars = static_cast<int>( wcslen(bstr) );
        if (!chars)
            return "";

        std::string ret;
        ret.resize(chars);
        ::WideCharToMultiByte(
            CP_ACP,
            0,
            bstr,
            chars,
            &ret[0],
            static_cast<int>(ret.size()),
            NULL,
            NULL);
        return ret;
    }

private:
    BSTR m_bstr;
};

////////////////////////////////////////////////////////////////////////////////
// std::list -> python::list
////////////////////////////////////////////////////////////////////////////////
template <typename TElem>
python::list toPyList(const std::list< TElem > &stdList)
{
    python::list pyList;
    for (std::list< TElem >::const_iterator it = stdList.begin(); it != stdList.end(); ++it)
        pyList.append( *it );
    return pyList;
}

////////////////////////////////////////////////////////////////////////////////

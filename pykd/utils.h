
#pragma once

////////////////////////////////////////////////////////////////////////////////
// std::string -> const WCHAR *
////////////////////////////////////////////////////////////////////////////////
class toWStr {
public:
    toWStr(const std::string &sz)
    {
        m_buf.resize( sz.size() + 1, L'\0' );
        ::MultiByteToWideChar( CP_ACP, 0, sz.c_str(), sz.size(), &m_buf[0], m_buf.size() );
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
    autoBstr() : m_bstr(NULL) {}
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
        ret.resize(chars + 1, '\0');
        ::WideCharToMultiByte(
            CP_ACP,
            0,
            bstr,
            chars,
            &ret[0],
            ret.size(),
            NULL,
            NULL);
        return ret;
    }

private:
    BSTR m_bstr;
};

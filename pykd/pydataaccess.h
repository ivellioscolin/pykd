#pragma once 


#include "boost/python/object.hpp"
#include "boost/python/wrapper.hpp"

#include "kdlib/dataaccessor.h"
#include "kdlib/exceptions.h"

#include "pythreadstate.h"

namespace python = boost::python;

namespace pykd {

///////////////////////////////////////////////////////////////////////////////

using kdlib::ImplementException;

///////////////////////////////////////////////////////////////////////////////

class PythonObjectAccessor  : public kdlib::DataAccessor
{

public:

    PythonObjectAccessor(python::object&  obj, size_t  pos = 0) : m_object(obj) 
    {
        m_pystate = PyThreadState_Get();
        m_startPos = pos;
    }

public:

    virtual size_t getLength() const {
        AutoSavePythonState  pystate(&m_pystate);
        return python::len(m_object);
    }

    virtual unsigned char readByte(size_t pos=0) const {
        return readValue<unsigned char>(pos);
    }

    virtual void writeByte(unsigned char value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual char readSignByte(size_t pos=0) const {
        return readValue<char>(pos);
    }

    virtual void writeSignByte(char value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual unsigned short readWord(size_t pos=0) const {
        return readValue<unsigned short>(pos);
    }

    virtual void writeWord(unsigned short value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual short readSignWord(size_t pos=0) const {
        return readValue<short>(pos);
    }

    virtual void writeSignWord(short value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual unsigned long readDWord(size_t pos=0) const {
        return readValue<unsigned long>(pos);
    }

    virtual void writeDWord(unsigned long value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual long readSignDWord(size_t pos=0) const  {
        return readValue<long>(pos);
    }

    virtual void writeSignDWord(long value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual unsigned long long readQWord(size_t pos=0) const  {
          return readValue<unsigned long long>(pos);
    }

    virtual void writeQWord(unsigned long long value, size_t pos=0)  {
        writeValue(value,pos);
    }

    virtual long long readSignQWord(size_t pos=0) const {
        return readValue<long long>(pos);
    }

    virtual void writeSignQWord(long long value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual float readFloat(size_t pos=0) const {
        return readValue<float>(pos);
    }

    virtual void writeFloat(float value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual double readDouble(size_t pos=0) const {
        return readValue<double>(pos);
    }

    virtual void writeDouble(double value, size_t pos=0) {
        writeValue(value,pos);
    }

    virtual void readBytes( std::vector<unsigned char>&  dataRange, size_t count, size_t pos=0) const {
        readValues(dataRange, count, pos);
    }

    virtual void writeBytes( const std::vector<unsigned char>&  dataRange, size_t pos=0) {
        writeValues(dataRange,pos);
    }

    void readWords( std::vector<unsigned short>&  dataRange, size_t count, size_t  pos) const {
        readValues(dataRange, count, pos);
    }

    virtual void writeWords( const std::vector<unsigned short>&  dataRange, size_t pos=0) {
        writeValues(dataRange,pos);
    }

    virtual void readDWords( std::vector<unsigned long>&  dataRange, size_t count, size_t  pos=0) const  {
        readValues(dataRange, count, pos);
    }

    virtual void writeDWords( const std::vector<unsigned long>&  dataRange, size_t  pos=0)  {
        writeValues(dataRange,pos);
    }

    virtual void readQWords( std::vector<unsigned long long>&  dataRange, size_t count, size_t  pos=0) const  {
        readValues(dataRange, count, pos);
    }

    virtual void writeQWords( const std::vector<unsigned long long>&  dataRange, size_t  pos=0)  {
        writeValues(dataRange,pos);
    }

    virtual void readSignBytes( std::vector<char>&  dataRange, size_t count, size_t  pos=0) const {
        readValues(dataRange, count, pos);
    }

    virtual void writeSignBytes( const std::vector<char>&  dataRange, size_t  pos=0)  {
        writeValues(dataRange,pos);
    }

    virtual void readSignWords( std::vector<short>&  dataRange, size_t count, size_t  pos=0) const  {
        readValues(dataRange, count, pos);
    }

    virtual void writeSignWords( const std::vector<short>&  dataRange, size_t  pos=0)  {
        writeValues(dataRange,pos);
    }

    virtual void readSignDWords( std::vector<long>&  dataRange, size_t count, size_t  pos=0)  const {
        readValues(dataRange, count, pos);
    }

    virtual void writeSignDWords( const std::vector<long>&  dataRange, size_t  pos=0) {
        writeValues(dataRange, pos);
    }

    virtual void readSignQWords( std::vector<long long>&  dataRange, size_t count, size_t  pos=0) const {
        readValues(dataRange, count, pos);
    }

    virtual void writeSignQWords( const std::vector<long long>&  dataRange, size_t  pos=0) {
        writeValues(dataRange,pos);
    }

    virtual void readFloats( std::vector<float>&  dataRange, size_t count, size_t  pos=0) const {
        readValues(dataRange, count, pos);
    }

    virtual void writeFloats( const std::vector<float>&  dataRange, size_t  pos=0) {
        writeValues(dataRange, pos);
    }

    virtual void readDoubles( std::vector<double>&  dataRange, size_t count, size_t  pos=0) const {
        readValues(dataRange, count, pos);
    }

    virtual void writeDoubles( const std::vector<double>&  dataRange, size_t  pos=0) {
        writeValues(dataRange, pos);
    }

    virtual kdlib::DataAccessorPtr copy( size_t  startOffset = 0, size_t  length = 0 ) {
        AutoSavePythonState  pystate(&m_pystate);
        return kdlib::DataAccessorPtr( new PythonObjectAccessor(m_object, startOffset) );
    }

    virtual std::wstring getLocationAsStr() const {
        return L"python byte sequance";
    }

    virtual kdlib::MEMOFFSET_64 getAddress() const {
        throw kdlib::DbgException("python accessor error");
    }

    virtual kdlib::VarStorage getStorageType() const {
        throw kdlib::DbgException("python accessor error");
    }

    virtual std::wstring getRegisterName() const {
        throw kdlib::DbgException("python accessor error");
    }

private:

    python::object   m_object;

    mutable PyThreadState*  m_pystate;

    size_t  m_startPos;

    template<typename T>
    T readValue(size_t pos) const 
    {
        AutoSavePythonState  pystate(&m_pystate);
        try
        {
            return readValueUnsafe<T>(pos);
        }
        catch( python::error_already_set const & )
        {}

        throw kdlib::DbgException("python accessor error");
    }

    template<typename T>
    T readValueUnsafe(size_t pos) const 
    {
        unsigned long long  value = 0;
        for ( size_t i = sizeof(T); i > 0; --i)
            value = ( value << 8 ) + python::extract<unsigned char>(m_object[m_startPos + pos*sizeof(T) + i - 1])();

        return *reinterpret_cast<T*>(&value);
    }

    template<typename T>
    void writeValue(T value, size_t pos)
    {
        AutoSavePythonState  pystate(&m_pystate);
        try
        {
            writeValueUnsafe(value,pos);
            return;
        }
        catch( python::error_already_set const & )
        {}

        throw kdlib::DbgException("python accessor error");
    }

    template<typename T>
    void writeValueUnsafe(T value, size_t pos)
    {
        unsigned long long  val = *reinterpret_cast<unsigned long long*>(&value);
        for ( size_t i = 0; i < sizeof(T); ++i )
            m_object[m_startPos + pos*sizeof(T) + i] = static_cast<unsigned char>( ( val >> (8*i)) & 0xFF );
    }

    template<typename T>
    void readValues( std::vector<T>&  dataRange, size_t count, size_t pos) const
    {
        AutoSavePythonState  pystate(&m_pystate);
        try 
        {
            dataRange.resize(count);
            for ( size_t  i = 0; i < count; ++i )
                dataRange[i] = readValueUnsafe<T>(pos + i);
            return;
        }
        catch( python::error_already_set const & )
        {}

        throw kdlib::DbgException("python accessor error");
    }

    template<typename T>
    void writeValues( const std::vector<T>&  dataRange, size_t pos) 
    {
        AutoSavePythonState  pystate(&m_pystate);
        try 
        {
            for ( size_t  i = 0; i < dataRange.size(); ++i )
                writeValueUnsafe(dataRange[i], pos + i );
            return;
        }
        catch( python::error_already_set const & )
        {}

        throw kdlib::DbgException("python accessor error");
    }
};


///////////////////////////////////////////////////////////////////////////////


}

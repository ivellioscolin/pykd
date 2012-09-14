
#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "typeinfo.h"
#include "win\dbgeng.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class CustomStruct : public UdtFieldColl
{
public:
    static TypeInfoPtr create(const std::string &name, ULONG align = 0);

protected:
    CustomStruct(const std::string &name, ULONG align)
        : UdtFieldColl(name), m_name(name), m_align(align) 
    {
    }

    virtual std::string getName() override {
        return m_name;
    }

    virtual ULONG getSize() override;

    virtual void appendField(const std::string &fieldName, TypeInfoPtr fieldType) override;

    virtual std::string getTypeString() const override {
        return "custom struct";
    }

private:
    std::string m_name;
    ULONG m_align;
};

////////////////////////////////////////////////////////////////////////////////

class CustomUnion : public UdtFieldColl
{
public:
    static TypeInfoPtr create(const std::string &name);

protected:
    CustomUnion(const std::string &name) : UdtFieldColl(name) {}

    virtual ULONG getSize() override;

    virtual void appendField(const std::string &fieldName, TypeInfoPtr fieldType) override;

    virtual std::string getTypeString() const override {
        return "custom union";
    }

private:
    std::string m_name;
};

////////////////////////////////////////////////////////////////////////////////

TypeInfoPtr PtrToVoid();

////////////////////////////////////////////////////////////////////////////////

}   // namespace pykd

////////////////////////////////////////////////////////////////////////////////

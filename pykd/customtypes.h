
#pragma once

////////////////////////////////////////////////////////////////////////////////

#include "typeinfo.h"
#include "win\dbgeng.h"

////////////////////////////////////////////////////////////////////////////////

namespace pykd {

////////////////////////////////////////////////////////////////////////////////

class CustomTypeBase : public UdtFieldColl
{
    typedef UdtFieldColl Base;
protected:
    CustomTypeBase(const std::string &name) : UdtFieldColl(name) {}

    void throwIfFiledExist(const std::string &fieldName);
    void throwIfTypeRecursive(TypeInfoPtr type);

private:
    void throwIfTypeRecursiveImpl(TypeInfoPtr type);
};

////////////////////////////////////////////////////////////////////////////////

class CustomStruct : public CustomTypeBase
{
    typedef CustomTypeBase Base;
public:
    static TypeInfoPtr create(const std::string &name, ULONG align = 0);

protected:
    CustomStruct(const std::string &name, ULONG align)
        : Base(name), m_name(name), m_align(align) 
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

class CustomUnion : public CustomTypeBase
{
    typedef CustomTypeBase Base;
public:
    static TypeInfoPtr create(const std::string &name);

protected:
    CustomUnion(const std::string &name) : Base(name) {}

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

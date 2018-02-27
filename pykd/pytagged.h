
#pragma once

namespace python = boost::python;

namespace pykd {

python::list enumTagged();

python::list loadTaggedBuffer(const std::wstring &id_str);

}   // namespace pykd

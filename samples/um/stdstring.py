
import pykd

#it's exact name of the std::string class. We need it later.
stringClassName = "basic_string<char,std::char_traits<char>,std::allocator<char> >"
fullStringClassName = "std::" + stringClassName

# get a malloc function. May be we have not its prototype in pdb file, so we need to define prototype manually
PVoid = pykd.typeInfo("Void*")
size_t = pykd.typeInfo("Int8B") if pykd.getCPUMode ==  pykd.CPUType.AMD64 else pykd.typeInfo("Int4B")
mallocProto = pykd.defineFunction( PVoid, pykd.callingConvention.NearC )
mallocProto.append("size", size_t)

malloc = pykd.typedVar(mallocProto, pykd.getOffset("malloc") ) #getOffset("malloc") may take a long time

# get a type of a std::string
stringClass = pykd.typeInfo(fullStringClassName)
 
# allocate memory
buffer = malloc( stringClass.size() )

# get a typed access to memory. As you may see the instance of the std::string is not initialized
stringVar = pykd.typedVar( stringClass, buffer )

# set up parameters for a constructor call.
param = pykd.stackAlloc(100)

pykd.writeCStr(param, "hello")

# call ctor for initalizing. std::string has some form of constructor so we need note prototype
ctor = stringVar.method( stringClassName, "Void(__thiscall)(Char*)" )
ctor(param)

#check result:
print( pykd.loadCStr( stringVar.c_str() ) )

pykd.stackFree(100)

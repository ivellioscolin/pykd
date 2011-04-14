#
#

from pykd import *
import sys


def loadSymbols():

   global nt
   nt = loadModule( "nt" )


def getObjectInDir( dirObj, objName ):


    if objName.find( "\\" ) != -1:
        ( dirSubName, objSubName ) =  objName.split("\\", 1)
    else:
        dirSubName = objName
 
    for i in range( 0, 37 ):

       if dirObj.HashBuckets[i] != 0:
          dirEntry = typedVar( "nt", "_OBJECT_DIRECTORY_ENTRY", dirObj.HashBuckets[i] )

          while dirEntry != 0:

              objHeader = containingRecord( dirEntry.Object, "nt", "_OBJECT_HEADER", "Body" )

              objName = typedVar( "nt", "_OBJECT_HEADER_NAME_INFO",  objHeader.getAddress() - objHeader.NameInfoOffset )

              name = loadUnicodeString( objName.Name.getAddress() )

              if name.lower() == dirSubName.lower():

                if objHeader.Type == ptrPtr( nt.ObpDirectoryObjectType ):
                    return getObjectInDir( typedVar( "nt", "_OBJECT_DIRECTORY", dirEntry.Object), objSubName )			 
                else:
                    return  dirEntry.Object

              if dirEntry.ChainLink != 0:
                  dirEntry = typedVar( "nt", "_OBJECT_DIRECTORY_ENTRY", dirEntry.ChainLink )
              else:
                  dirEntry = 0    



                                                                          
def getObjectByName( objName ):

    if len(objName)==0: 
	return

    if objName[0] != '\\':
	return

    rootDir = typedVar( "nt", "_OBJECT_DIRECTORY", ptrPtr( nt.ObpRootDirectoryObject ) )
   
    return getObjectInDir( rootDir, objName[1:] )



def printDrvMajorTable( drvName ):

     objName = "\\Driver\\" + drvName
     drvObjPtr = getObjectByName( objName )
    
     if drvObjPtr == None:
        dprintln( "object not found" )
        return
 
     drvObj = typedVar( "nt", "_DRIVER_OBJECT", drvObjPtr )

     
     for i in xrange( 0, len( drvObj.MajorFunction ) ):
       dprintln( "MajorFunction[%d] = %s" % ( i, findSymbol( drvObj.MajorFunction[i] ) ) )



if __name__ == "__main__":

   if not isWindbgExt():
      loadDump( sys.argv[1] )

   if isKernelDebugging():
       loadSymbols();
       printDrvMajorTable( "afd" )
   else:
       dprintln( "not a kernel debugging" )

   
    

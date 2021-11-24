# ======================================================================================
#	Copyright (c) 1999-2001 Palm, Inc. or its subsidiaries.
#	All rights reserved.
# ======================================================================================
#
# File:    Test.py
# Author:  David Creemer
# Created: Thu Jul 29 17:05:33 PDT 1999

# Test of the Python Poser RPC mechanism
#
# This file includes a number of PalmOS sample function wrappers. Each
# wraps a PalmOS trap or hostcontrol function, and calls the lower
# level Poser RPC layer. Each function takes 1 or more parameters. The 
# first parameter is a required Poser Socket (an object representing a 
# connection to a running Poser). All other parameters are optional
# and specific to each function.
#

import sys
import Poser
from SysTraps import *

def MemReadMemory( poserSocket, addr, len ):
    msg = Poser.SysPacketReadMem( addr, len )
    poserSocket.call( msg )
    return msg.getMemory()
    
def MemWriteMemory( poserSocket, addr, data, len ):
    msg = Poser.SysPacketWriteMem( addr, data, len )
    poserSocket.call( msg )
    
def MemPtrNew( poserSocket, size ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapMemPtrNew'] )
    msg[ 'size' ] = Poser.RPCParam( 0, 'L', size )
    poserSocket.call( msg )
    return msg[ 'A0' ]
    
def SysTicksPerSecond( poserSocket ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapSysTicksPerSecond'] )
    poserSocket.call( msg )
    return msg[ 'D0' ]
    
def DmNumDatabases( poserSocket, cardNo ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapDmNumDatabases'] )
    msg[ 'cardNo' ] = Poser.RPCParam( 0, 'H', cardNo )
    poserSocket.call( msg )
    return msg[ 'D0' ]

def DmGetDatabase( poserSocket, cardNo, index ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapDmGetDatabase'] )
    msg[ 'cardNo' ] = Poser.RPCParam( 0, 'H', cardNo )
    msg[ 'index' ] = Poser.RPCParam( 1, 'H', index )
    poserSocket.call( msg )
    return msg[ 'D0' ]

def DmFindDatabase( poserSocket, cardNo, name ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapDmFindDatabase'] )
    msg[ 'cardNo' ] = Poser.RPCParam( 0, 'H', cardNo )
    msg[ 'name' ] = Poser.RPCParam( 1, '32s', name )
    poserSocket.call( msg )
    return msg[ 'D0' ]

def DmDatabaseInfo( poserSocket, cardNo, dbId ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapDmDatabaseInfo'] )
    msg[ 'cardNo' ] = Poser.RPCParam( 0, 'H', cardNo )
    msg[ 'dbId' ] = Poser.RPCParam( 0, 'L', dbId )
    msg[ 'name' ] = Poser.RPCParam( 1, '32s', "" )
    msg[ 'attributes' ] = Poser.RPCParam( 1, 'H', 0 )
    msg[ 'version' ] = Poser.RPCParam( 1, 'H', 0 )
    msg[ 'crDate' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'modDate' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'bckUpDate' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'modNum' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'appInfoID' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'sortInfoID' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'type' ] = Poser.RPCParam( 1, 'L', 0 )
    msg[ 'creator' ] = Poser.RPCParam( 1, 'L', 0 )
    poserSocket.call( msg )
    return msg[ 'D0' ]

def DmGetLastError( poserSocket ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapDmGetLastErr'] )
    poserSocket.call( msg )
    return msg[ 'D0' ]

def EvtEnqueueKey( poserSocket, ascii, keycode=0, modifiers=0 ):
    msg = Poser.SysPacketRPC2( sysTraps['sysTrapEvtEnqueueKey'] )
    msg[ 'ascii' ] = Poser.RPCParam( 0, 'H', ascii )
    msg[ 'keycode' ] = Poser.RPCParam( 0, 'H', keycode )
    msg[ 'modifiers' ] = Poser.RPCParam( 0, 'H', modifiers )
    poserSocket.call( msg )
    return msg[ 'D0' ]

#-----------------------------------------------------------------------------
	
def test():
    ps = Poser.Socket()
    ps.connect()
    
    print "Enqueuing 'Hello'..."
    EvtEnqueueKey( ps, ord('H') )
    EvtEnqueueKey( ps, ord('e') )
    EvtEnqueueKey( ps, ord('l') )
    EvtEnqueueKey( ps, ord('l') )
    EvtEnqueueKey( ps, ord('o') )
    print "SysTicksPerSecond() =", SysTicksPerSecond( ps )
    
    addr = MemPtrNew( ps, 10 )
    print "MemPtrNew(10) =", addr
    print "MemWriteMemory(",addr,",'123456789',10)"
    MemWriteMemory( ps, addr, "123456789\0", 10 )
    
    data = MemReadMemory( ps, addr, 10 )
    print "MemReadMemory(",addr,",10)= ", data
    
    print "DmNumDatabase(0) =", DmNumDatabases( ps, 0 )
    
    print "DmFindDatabase(0, 'AddressDB') =", DmFindDatabase( ps, 0, "AddressDB" )
    localid = DmFindDatabase( ps, 0, "MemoDB" )
    print "DmFindDatabase(0, 'MemoDB') =", localid
    print "DmDatabaseInfo(0,", localid,") =", DmDatabaseInfo( ps, 0, localid )
    print "DmGetLastError() =", DmGetLastError( ps )
    
## close the socket
    ps.close()
    sys.exit(1)

#-----------------------------------------------------------------------------
        
if __name__ == '__main__':
    test()

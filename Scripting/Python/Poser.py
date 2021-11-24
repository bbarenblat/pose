# ======================================================================================
#	Copyright (c) 1999-2001 Palm, Inc. or its subsidiaries.
#	All rights reserved.
# ======================================================================================
#
# File:    Poser.py
# Author:  David Creemer
# Created: Thu Jul 29 17:05:33 PDT 1999

""" This module implements a connection to the PalmOS Emulator
POSER. It provides a socket for client - poser communications, and an
hierarchy of classes for formatting poser RPC and other SysPackets
calls. """

import sys
import socket
from struct import pack, unpack
from string import upper,atoi

# -----------------------------------------------------------------------------
# Utility functions

def _memdump( baseaddr, len, data ):
    """return a string that is a nicely formatted hex and ascii dump of a range of bytes"""

    base = "Addr=0x%08X " % (baseaddr)
    base = base + "Len=0x%04X (%d) " % (len, len)
    for i in range(len):
	if (i % 8) == 0:
	    base = base +  "\n  %08X " % (baseaddr + i)
	base = base + " %02X" % ( ord(data[i]) )
    return base

# -----------------------------------------------------------------------------
class ProtocolException:

    def __init__( self, msg ):
        self._message = msg

# -----------------------------------------------------------------------------

class Socket:
    """defines the interface by which an application talks with a running poser"""

    # packet header signatures
    _HeaderSignature1 = 0xBEEF
    _HeaderSignature2 = 0xED

    def __init__( self ):
	self._sock = socket.socket( socket.AF_INET, socket.SOCK_STREAM )
	self._transid = 1
        self._src = 1
        self._type = 0

    # ----------------------------------------------------------------
    # public API
    # ----------------------------------------------------------------

    def connect( self, addr="localhost", port=6414 ):
	"""connect to the running poser at the given host and port"""
	self._sock.connect( addr, port )

    def close( self ):
	"""terminate communication with the connected poser"""
	self._sock.close()

    def call( self, pkt ):
	"""call the connected poser with the given sysCall packet"""
	# get the raw packet data
	pkt._marshal()
	# prepare the header
	shdr = pack( ">HBBBBHB", Socket._HeaderSignature1, Socket._HeaderSignature2,
                     pkt._dest, self._src, self._type, len( pkt._data ), self._transid );
	shdr = shdr + pack( ">B", self._calcHeaderChecksum( shdr ) )
	# send the packet pieces
	spkt = shdr + pkt._data + pack( ">H", self._calcFooterChecksum( pkt._data ) )
	self._sock.send( spkt )
	# read the header
	rhdr = self._sock.recv( 10 )
	( h1, b1, rsrc, rdest, rtype, rlen, rtid, rhcs ) = unpack( ">HBBBBHBB", rhdr )
	# read in the body
	pkt._data = self._sock.recv( rlen )
	# read in the footer
	rftr = self._sock.recv( 2 )
	(rbcs) = unpack( ">H", rftr )
	# bump the transaction id
	self._transid = self._transid + 1
	pkt._unmarshal()

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def _calcHeaderChecksum( self, bytes ):
	"""calculate the checksum for the packet header"""
	cs = 0
	for c in bytes:
	    cs = cs + ord( c )
	    if ( cs > 255 ):
		cs = cs - 256
	return cs

    def _calcFooterChecksum( self, body ):
	"""calculate the checksum for the packet body"""
	# TBD
	return 0

    def __repr__( self ):
	return "<poser socket, tid=" + str( self._transid ) + ", sock=" + \
	       str( self._sock ) + ">"

#-----------------------------------------------------------------------------
# Abstract base class for all SysPackets
#-----------------------------------------------------------------------------

class SysPacket:
    """abstract base class for all poser SysPacket messages"""

    def __init__( self ):
	self._data = None
	self._command = 0x00
        self._dest = 0

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def _marshal( self ):
	"""pack the packet data into a binary stream"""
	return pack( ">BB", self._command, 0 )

    def _unmarshal( self ):
	"""pull the packet data from the binary stream"""
	( self._command, dummy ) = unpack( ">BB", self._data[0:2] )
	self._data = self._data[2:]

    def __repr__( self ):
	return "cmd=0x%02X" % ( self._command )



#-----------------------------------------------------------------------------
# Memory Reading and Writing SysPackets
#-----------------------------------------------------------------------------

class SysPacketMem( SysPacket ):
    """ abstract parent class of mem read/write packet classes"""

    def __init__( self, addr, length ):
	"""instantiate generic memory sys packet"""
	SysPacket.__init__( self )
        self._dest = 1
	self._memAddr = addr
	self._memLength = length
	self._memory = None

    # ----------------------------------------------------------------
    # public API
    # ----------------------------------------------------------------

    def getMemory( self ):
	"""return the memory read"""
	return self._memory

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def __repr__( self ):
	return SysPacket.__repr__( self ) + ", " + _memdump( self._memAddr, self._memLength, self._memory )

#-----------------------------------------------------------------------------

class SysPacketReadMem( SysPacketMem ):
    """A poser sysPacket which reads PalmOS memory"""

    def __init__( self, addr, length ):
	"""instantiate read memory packet"""
	SysPacketMem.__init__( self, addr, length )
	self._command = 0x01

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def _marshal( self ):
	"""marshal the RPC information & parameters into a flat byte stream"""
	self._data = SysPacket._marshal( self ) + pack( ">LH",
							 self._memAddr,
							 self._memLength )

    def _unmarshal( self ):
	"""unmarshal the RPC information & parameters from the flat byte stream"""
	SysPacket._unmarshal( self )
	self._memory = self._data[0:self._memLength]

#-----------------------------------------------------------------------------

class SysPacketWriteMem( SysPacketMem ):
    """A poser sysPacket which writes PalmOS memory"""

    def __init__( self, addr, data, length ):
	"""instantiate write memory"""
	SysPacketMem.__init__( self, addr, length )
	self._command = 0x02
	self._memory = data

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def _marshal( self ):
	"""marshal the RPC information & parameters into a flat byte stream"""
	fmt = ">LH" + str(self._memLength) + "s"
	self._data = SysPacket._marshal( self ) + pack( fmt,
							 self._memAddr,
							 self._memLength,
							 self._memory )

#-----------------------------------------------------------------------------
# OS Trap RPC SysPackets
#-----------------------------------------------------------------------------

class SysPacketRPC( SysPacket ):
    """A poser sysPacket which implements a RPC call/respone PalmOS Trap call"""

    def __init__( self, trap ):
	"""instantiate new RPC call with the trap word to be called"""
	SysPacket.__init__( self )
        self._dest = 1
	self._command = 0x0A
	self._trap = trap
	self._params = {}
        self._paramnames = [] # used to preserver order
	self._a0 = 0
	self._d0 = 0

    # ----------------------------------------------------------------
    # public API
    # ----------------------------------------------------------------

    def __len__( self ):
        # return # of params + A0 and D0 registers
        return len( self._paramnames ) + 2

    def keys( self ):
        return self._paramnames + [ 'A0' , 'D0' ]
    
    def __setitem__( self, key, param ):
        if upper( key ) == 'A0':
            self._a0 = param
        elif upper( key ) == 'D0':
            self._d0 = param
        else:
            self._params[ key ] = param
            self._paramnames.insert( 0, key )
        
    def __getitem__( self, key ):
        if upper( key ) == 'A0':
            return self._a0
        elif upper( key ) == 'D0':
            return self._d0
        else:
            return self._params[ key ]._getvalue()
        
    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def _marshal( self ):
	"""marshal the RPC information & parameters into a flat byte stream"""
	self._data = SysPacket._marshal( self ) + pack( ">HLLH",
							self._trap,
							self._d0,
							self._a0,
							len( self._params ) )
	for pname in self._paramnames:
	    self._data = self._data + self._params[ pname ]._data

    def _unmarshal( self ):
	"""unmarshal the RPC information & parameters from the flat byte stream"""
	SysPacket._unmarshal( self )

	( self._trap, self._d0, self._a0, pcount ) = unpack( ">HLLH", self._data[0:12] )
	self._data = self._data[12:]

        if pcount != len( self._paramnames ):
            raise ProtocolException, "Unexpected number of return parameters"
        
	for i in range( pcount ):
            pname = self._paramnames[i]
	    ( dummy, size ) = unpack( ">BB", self._data[0:2] )
	    self._params[ pname ]._data = self._data[0:size+2]
	    self._data = self._data[size+2:]
	    self._params[ pname ]._unmarshal()

    def __repr__( self ):
	base = "<sysPacketRPC, " + SysPacket.__repr__( self )
	base = base + ", trap=0x%04X, " % ( self._trap )
	base = base + "a0=0x%08X, " % (self._a0)
	base = base + "d0=0x%08X(%d), " % (self._d0, self._d0)
        base = base + "Params("
	for pname in self._paramnames:
	    base = base + " " + pname + "=" + str( self._params[ pname ] )
	return base + " ) >"

#-----------------------------------------------------------------------------

class SysPacketRPC2( SysPacketRPC ):

    def __init__( self, trap ):
	"""instantiate a new RPC2 call with the trap word to be called"""
	SysPacketRPC.__init__( self, trap )
        self._dest = 14
	self._command = 0x70
        self._registers = {} # for A0,A1...,D0,D1...
        self._exception = 0

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    # register list. order is important: don't change it
    _reglist = [ 'D7','D6','D5','D4','D3','D2','D1','D0','A7','A6','A5','A4','A3','A2','A1','A0' ]
    _reglistrev = [ 'A0','A1','A2','A3','A4','A5','A6','A7','D0','D1','D2','D3','D4','D5','D6','D7' ]
    
    def _isRegister( self, key ):
        return upper( key ) in SysPacketRPC2._reglist

    def _makeRegMask( self ):
        mask = 0
        for r in SysPacketRPC2._reglist:
            mask = mask << 1
            if self._registers.has_key( r ):
                mask = mask | 1
        return mask
    
    def __setitem__( self, key, param ):
        if self._isRegister( key ):
            self._registers[ upper( key ) ] = param
        else:
            self._params[ key ] = param
            self._paramnames.insert( 0, key )
    
    def __getitem__( self, key ):
        if upper( key ) == 'A0':
            return self._a0
        elif upper( key ) == 'D0':
            return self._d0
        elif self._isRegister( key ):
            # this may throw an exception. That's ok.
            return self._registers[ upper( key ) ]
        elif upper( key ) == 'exception':
            return self._exception
        else:
            return self._params[ key ]._getvalue()

    def _marshal( self ):
	"""marshal the RPC information & parameters into a flat byte stream"""
	self._data = SysPacket._marshal( self ) + pack( ">HLLHH",
							self._trap,
							self._d0,
							self._a0,
                                                        self._exception,
                                                        self._makeRegMask() )
        # add registers
        for r in self._registers.keys():
            self._data = self._data + pack( ">L", self._registers[ r ] )
            
        # add parameters
        self._data = self._data + pack( ">H", len( self._params ) )
	for pname in self._paramnames:
	    self._data = self._data + self._params[ pname ]._data

    def _unmarshal( self ):
	"""unmarshal the RPC information & parameters from the flat byte stream"""
        SysPacket._unmarshal( self )
        ( self._trap, self._d0, self._a0, self._exception, regmask ) = unpack( ">HLLHH",
                                                                               self._data[0:14] )
	self._data = self._data[14:]

        # extract the registers ( if any )
        for r in SysPacketRPC2._reglistrev:
            # if the mask bit is set
            if regmask & 0x0001:
                # extract register value & trim unmarshalled data
                ( self._registers[ r ], ) = unpack( ">L", self._data )
                self._data = self._data[4:]
            # go onto next bit in mask
            regmask = regmask >> 1
        
        # extract parameter count
        ( pcount, ) = unpack( ">H", self._data[0:2] )
        self._data = self._data[2:]

        # extract paramters
        if pcount != len( self._paramnames ):
            raise ProtocolException, "Unexpected number of return parameters"
        
	for i in range( pcount ):
            pname = self._paramnames[i]
	    ( dummy, size ) = unpack( ">BB", self._data[0:2] )
	    self._params[ pname ]._data = self._data[0:size+2]
	    self._data = self._data[size+2:]
	    self._params[ pname ]._unmarshal()

    def __repr__( self ):
	base = "<sysPacketRPC2, " + SysPacket.__repr__( self )
	base = base + ", trap=0x%04X, " % ( self._trap )
	base = base + "a0=0x%08X, " % (self._a0)
	base = base + "d0=0x%08X(%d), " % (self._d0, self._d0)
	base = base + "exception=0x%04X, " % (self._exception)
	base = base + "registers=" + str(self._registers)
        base = base + "Params("
	for pname in self._paramnames:
	    base = base + " " + pname + "=" + str( self._params[ pname ] )
	return base + " ) >"

#-----------------------------------------------------------------------------

class RPCParam:
    """a parameter to a poser SysPacketRPC(2) call"""

    def __init__( self, byref, type, value ):
	"""instantiate new RPC parameter of a given type and value"""
	self._type = type
	self._value = value
	if byref:
	    self._byref = 1
	else:
	    self._byref = 0
	self._data = None
	self._calcSize()
	self._marshal()

    # ----------------------------------------------------------------
    # implementation methods
    # ----------------------------------------------------------------

    def _getvalue( self ):
	"""return the value of the param as received from the wire"""
	self._unmarshal()
	return self._value

    def _calcSize( self ):
	"""calculate the size of the parameter based on its type"""
	t = upper( self._type[ -1 ] )
	if t == 'B':
	    self._size = 1
	elif t == 'H':
	    self._size = 2
	elif t == 'L':
	    self._size = 4	
	elif t == 'S':
	    # string with specified size ('32s')
	    self._size = atoi( self._type[:-1] )

    def _marshal( self ):
	"""flatten the RPC param into a binary stream"""
	self._data = pack( ">BB" + self._type,
			   self._byref, self._size, self._value )

    def _unmarshal( self ):
	"""pull the RPC param data from the binary stream"""
	( self._byref, self._size ) = unpack( ">BB", self._data[0:2] )
	( self._value, ) = unpack( ">" + self._type, self._data[2:] )

    def __repr__( self ):
	return "<sysPacketRPC param, byref=" + str( self._byref ) + ", type=" + \
	       str( self._type ) + ", size=" + str( self._size ) + ", value=" + \
	       str( self._value ) + ">"

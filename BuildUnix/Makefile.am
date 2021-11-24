## -*- mode: Makefile; tab-width: 4; -*-
## ======================================================================================
##	Copyright (c) 1998-2001 Palm, Inc. or its subsidiaries.
##	All rights reserved.
## ======================================================================================

SUBDIRS			=	Gzip jpeg espws-2.0

SUFFIXES		=	.cpp

VPATH			=	$(srcdir)/../SrcUnix:					\
					$(srcdir)/../SrcShared:					\
					$(srcdir)/../SrcShared/Hardware:		\
					$(srcdir)/../SrcShared/Hardware/TRG:	\
					$(srcdir)/../SrcShared/omnithread:		\
					$(srcdir)/../SrcShared/Palm/Platform/Core/System/Src:	\
					$(srcdir)/../SrcShared/Patches:			\
					$(srcdir)/../SrcShared/UAE:


## POSER_CFLAGS and POSER_CXXFLAGS defined by configure.in.

CFLAGS			=   $(POSER_CFLAGS) $(LOCAL_CFLAGS) $(FLAGS) $(THREAD_FLAGS) $(X_CFLAGS)
CXXFLAGS		= 	$(POSER_CXXFLAGS) $(LOCAL_CFLAGS) $(FLAGS) $(THREAD_FLAGS) $(X_CFLAGS)

INCLUDES		=	-I$(srcdir)/../SrcShared
INCLUDES		+=	-I$(srcdir)/../SrcShared/Hardware
INCLUDES		+=	-I$(srcdir)/../SrcShared/Hardware/TRG
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Device
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Core/Hardware/IncsPrv
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Core/System/IncsPrv
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Incs
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Incs/Core
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Incs/Core/Hardware
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Incs/Core/System
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Incs/Core/UI
INCLUDES		+=	-I$(srcdir)/../SrcShared/Palm/Platform/Incs/Libraries
INCLUDES		+=	-I$(srcdir)/../SrcShared/Gzip
INCLUDES		+=	-I$(srcdir)/../SrcShared/jpeg
INCLUDES		+=	-I$(srcdir)/../SrcShared/omnithread
INCLUDES		+=	-I$(srcdir)/../SrcShared/Patches
INCLUDES		+=	-I$(srcdir)/../SrcShared/UAE
INCLUDES		+=	-I$(srcdir)/../SrcUnix
INCLUDES		+=	-I$(srcdir)/../SrcUnix/espws-2.0

SRC_UNIX		= 	EmApplicationFltk.cpp			\
					EmApplicationFltk.h				\
					EmCommonUnix.h					\
					EmDirRefUnix.cpp				\
					EmDirRefUnix.h					\
					EmDlgFltk.cpp					\
					EmDlgFltk.h						\
					EmDocumentUnix.cpp				\
					EmDocumentUnix.h				\
					EmFileRefUnix.cpp				\
					EmFileRefUnix.h					\
					EmMenusFltk.cpp					\
					EmMenusFltk.h					\
					EmPixMapUnix.cpp				\
					EmPixMapUnix.h					\
					EmTransportSerialUnix.cpp		\
					EmTransportSerialUnix.h			\
					EmTransportUSBUnix.cpp			\
					EmTransportUSBUnix.h			\
					EmWindowFltk.cpp				\
					EmWindowFltk.h					\
					Platform_Unix.cpp				\
					jconfig.h

SRC_UNIX_GEN	=	ResStrings.cpp					\
					EmDlgFltkFactory.h				\
					EmDlgFltkFactory.cpp

SRC_SHARED		=	ATraps.cpp						\
					ATraps.h						\
					Byteswapping.cpp				\
					Byteswapping.h					\
					CGremlins.cpp					\
					CGremlins.h						\
					CGremlinsStubs.cpp				\
					CGremlinsStubs.h				\
					ChunkFile.cpp					\
					ChunkFile.h						\
					DebugMgr.cpp					\
					DebugMgr.h						\
					EcmIf.h							\
					EcmObject.h						\
					EmAction.cpp					\
					EmAction.h						\
					EmApplication.cpp				\
					EmApplication.h					\
					EmCommands.h					\
					EmCommon.cpp					\
					EmCommon.h						\
					EmDevice.cpp					\
					EmDevice.h						\
					EmDirRef.cpp					\
					EmDirRef.h						\
					EmDlg.cpp						\
					EmDlg.h							\
					EmDocument.cpp					\
					EmDocument.h					\
					EmErrCodes.h					\
					EmEventOutput.cpp				\
					EmEventOutput.h					\
					EmEventPlayback.cpp				\
					EmEventPlayback.h				\
					EmException.cpp					\
					EmException.h					\
					EmExgMgr.cpp					\
					EmExgMgr.h						\
					EmFileImport.cpp				\
					EmFileImport.h					\
					EmFileRef.cpp					\
					EmFileRef.h						\
					EmJPEG.cpp						\
					EmJPEG.h						\
					EmLowMem.cpp					\
					EmLowMem.h						\
					EmMapFile.cpp					\
					EmMapFile.h						\
					EmMenus.cpp						\
					EmMenus.h						\
					EmMinimize.cpp					\
					EmMinimize.h					\
					EmPalmFunction.cpp				\
					EmPalmFunction.h				\
					EmPalmHeap.cpp					\
					EmPalmHeap.h					\
					EmPalmOS.cpp					\
					EmPalmOS.h						\
					EmPalmStructs.cpp				\
					EmPalmStructs.h					\
					EmPalmStructs.i					\
					EmPixMap.cpp					\
					EmPixMap.h						\
					EmPoint.cpp						\
					EmPoint.h						\
					EmQuantizer.cpp					\
					EmQuantizer.h					\
					EmRect.cpp						\
					EmRect.h						\
					EmRefCounted.cpp				\
					EmRefCounted.h					\
					EmRegion.cpp					\
					EmRegion.h						\
					EmROMReader.cpp					\
					EmROMReader.h					\
					EmROMTransfer.cpp				\
					EmROMTransfer.h					\
					EmRPC.cpp						\
					EmRPC.h							\
					EmScreen.cpp					\
					EmScreen.h						\
					EmSession.cpp					\
					EmSession.h						\
					EmStream.cpp					\
					EmStream.h						\
					EmStreamFile.cpp				\
					EmStreamFile.h					\
					EmStructs.h						\
					EmSubroutine.cpp				\
					EmSubroutine.h					\
					EmThreadSafeQueue.cpp			\
					EmThreadSafeQueue.h				\
					EmTransport.cpp					\
					EmTransport.h					\
					EmTransportSerial.cpp			\
					EmTransportSerial.h				\
					EmTransportSocket.cpp			\
					EmTransportSocket.h				\
					EmTransportUSB.cpp				\
					EmTransportUSB.h				\
					EmTypes.h						\
					EmWindow.cpp					\
					EmWindow.h						\
					ErrorHandling.cpp				\
					ErrorHandling.h					\
					Hordes.cpp						\
					Hordes.h						\
					HostControl.cpp					\
					HostControl.h					\
					HostControlPrv.h				\
					LoadApplication.cpp				\
					LoadApplication.h				\
					Logging.cpp						\
					Logging.h						\
					Marshal.cpp						\
					Marshal.h						\
					MetaMemory.cpp					\
					MetaMemory.h					\
					Miscellaneous.cpp				\
					Miscellaneous.h					\
					Palm.h							\
					PalmOptErrorCheckLevel.h		\
					PalmPack.h						\
					PalmPackPop.h					\
					Platform.h						\
					Platform_NetLib.h				\
					Platform_NetLib_Sck.cpp			\
					PreferenceMgr.cpp				\
					PreferenceMgr.h					\
					Profiling.cpp					\
					Profiling.h						\
					ROMStubs.cpp					\
					ROMStubs.h						\
					SLP.cpp							\
					SLP.h							\
					SessionFile.cpp					\
					SessionFile.h					\
					Skins.cpp						\
					Skins.h							\
					SocketMessaging.cpp				\
					SocketMessaging.h				\
					Startup.cpp						\
					Startup.h						\
					StringConversions.cpp			\
					StringConversions.h				\
					StringData.cpp					\
					StringData.h					\
					SystemPacket.cpp				\
					SystemPacket.h


SRC_SHARED_HARDWARE =								\
					EmBankDRAM.cpp					\
					EmBankDRAM.h					\
					EmBankDummy.cpp					\
					EmBankDummy.h					\
					EmBankMapped.cpp				\
					EmBankMapped.h					\
					EmBankROM.cpp					\
					EmBankROM.h						\
					EmBankRegs.cpp					\
					EmBankRegs.h					\
					EmBankSRAM.cpp					\
					EmBankSRAM.h					\
					EmCPU.cpp						\
					EmCPU.h							\
					EmCPU68K.cpp					\
					EmCPU68K.h						\
					EmCPUARM.cpp					\
					EmCPUARM.h						\
					EmHAL.cpp						\
					EmHAL.h							\
					EmMemory.cpp					\
					EmMemory.h						\
					EmRegs.cpp						\
					EmRegs.h						\
					EmRegs328.cpp					\
					EmRegs328.h						\
					EmRegs328PalmIII.h				\
					EmRegs328PalmPilot.cpp			\
					EmRegs328PalmPilot.h			\
					EmRegs328PalmVII.h				\
					EmRegs328Pilot.h				\
					EmRegs328Prv.h					\
					EmRegs328Symbol1700.cpp			\
					EmRegs328Symbol1700.h			\
					EmRegsASICSymbol1700.cpp		\
					EmRegsASICSymbol1700.h			\
					EmRegsEZ.cpp					\
					EmRegsEZ.h						\
					EmRegsEZPalmIIIc.cpp			\
					EmRegsEZPalmIIIc.h				\
					EmRegsEZPalmIIIe.h				\
					EmRegsEZPalmIIIx.h				\
					EmRegsEZPalmM100.cpp			\
					EmRegsEZPalmM100.h				\
					EmRegsEZPalmV.cpp				\
					EmRegsEZPalmV.h					\
					EmRegsEZPalmVIIx.cpp			\
					EmRegsEZPalmVIIx.h				\
					EmRegsEZPalmVII.cpp				\
					EmRegsEZPalmVII.h				\
					EmRegsEZPalmVx.h				\
					EmRegsEZPrv.h					\
					EmRegsEZTemp.cpp				\
					EmRegsEZTemp.h					\
					EmRegsEZTRGpro.cpp				\
					EmRegsEZTRGpro.h				\
					EmRegsEZVisor.cpp				\
					EmRegsEZVisor.h					\
					EmRegsFrameBuffer.cpp			\
					EmRegsFrameBuffer.h				\
					EmRegsMediaQ11xx.cpp			\
					EmRegsMediaQ11xx.h				\
					EmRegsPLDPalmVIIEZ.cpp			\
					EmRegsPLDPalmVIIEZ.h			\
					EmRegsPrv.h						\
					EmRegsSED1375.cpp				\
					EmRegsSED1375.h					\
					EmRegsSED1376.cpp				\
					EmRegsSED1376.h					\
					EmRegsSZ.cpp					\
					EmRegsSZ.h						\
					EmRegsSZPrv.h					\
					EmRegsSZTemp.cpp				\
					EmRegsSZTemp.h					\
					EmRegsUSBPhilipsPDIUSBD12.cpp	\
					EmRegsUSBPhilipsPDIUSBD12.h		\
					EmRegsUSBVisor.cpp				\
					EmRegsUSBVisor.h				\
					EmRegsVZ.cpp					\
					EmRegsVZ.h						\
					EmRegsVZHandEra330.cpp			\
					EmRegsVZHandEra330.h			\
					EmRegsVZPalmM500.cpp			\
					EmRegsVZPalmM500.h				\
					EmRegsVZPalmM505.cpp			\
					EmRegsVZPalmM505.h				\
					EmRegsVZPrv.h					\
					EmRegsVZTemp.cpp				\
					EmRegsVZTemp.h					\
					EmRegsVZVisorEdge.cpp			\
					EmRegsVZVisorEdge.h				\
					EmRegsVZVisorPlatinum.cpp		\
					EmRegsVZVisorPlatinum.h			\
					EmRegsVZVisorPrism.cpp			\
					EmRegsVZVisorPrism.h			\
					EmSPISlave.cpp					\
					EmSPISlave.h					\
					EmSPISlaveADS784x.cpp			\
					EmSPISlaveADS784x.h				\
					EmUAEGlue.cpp					\
					EmUAEGlue.h						\
					EmUARTDragonball.cpp			\
					EmUARTDragonball.h

SRC_PATCHES		=	EmPatchIf.h						\
					EmPatchLoader.cpp				\
					EmPatchLoader.h					\
					EmPatchMgr.cpp					\
					EmPatchMgr.h					\
					EmPatchModule.cpp				\
					EmPatchModule.h					\
					EmPatchModuleHtal.cpp			\
					EmPatchModuleHtal.h				\
					EmPatchModuleMap.cpp			\
					EmPatchModuleMap.h				\
					EmPatchModuleMemMgr.cpp			\
					EmPatchModuleNetLib.cpp			\
					EmPatchModuleNetLib.h			\
					EmPatchModuleSys.cpp			\
					EmPatchModuleSys.h				\
					EmPatchModuleTypes.h			\
					EmPatchState.cpp				\
					EmPatchState.h

SRC_TRG			=	EmHandEra330Defs.h				\
					EmHandEraCFBus.h				\
					EmHandEraSDBus.h				\
					EmRegs330CPLD.cpp				\
					EmRegs330CPLD.h					\
					EmSPISlave330Current.cpp		\
					EmSPISlave330Current.h			\
					EmTRG.cpp						\
					EmTRG.h							\
					EmTRGATA.cpp					\
					EmTRGATA.h						\
					EmTRGCF.cpp						\
					EmTRGCF.h						\
					EmTRGCFDefs.h					\
					EmTRGCFIO.cpp					\
					EmTRGCFIO.h						\
					EmTRGCFMem.cpp					\
					EmTRGCFMem.h					\
					EmTRGDiskIO.cpp					\
					EmTRGDiskIO.h					\
					EmTRGDiskType.cpp				\
					EmTRGDiskType.h					\
					EmTRGSD.cpp						\
					EmTRGSD.h

SRC_UAE			=	UAE.h							\
					compiler.h						\
					config.h						\
					cpudefs.c						\
					cpuemu.c						\
					cpustbl.c						\
					cputbl.h						\
					custom.h						\
					machdep_m68k.h					\
					machdep_maccess.h				\
					memory_cpu.h					\
					newcpu.h						\
					options.h						\
					readcpu.cpp						\
					readcpu.h						\
					sysconfig.h						\
					sysdeps.h						\
					target.h

SRC_PALM		=	Crc.c

if SOLARIS
SRC_THREAD		=	solaris.cpp
else
SRC_THREAD		=	posix.cpp
endif

bin_PROGRAMS	=	pose

pose_LDADD		=	$(srcdir)/Gzip/libposergzip.a
pose_LDADD		+=	$(srcdir)/jpeg/libposerjpeg.a
pose_LDADD		+=	$(srcdir)/espws-2.0/libposerespws.a
pose_LDADD		+=	-lfltk
pose_LDADD		+=	-L/usr/local/lib $(X_LIBS) -lXext -lX11 $(THREAD_LIBS) $(GLLIB) -lm

pose_SOURCES	=	$(SRC_UNIX) $(SRC_UNIX_GEN) $(SRC_SHARED) $(SRC_SHARED_HARDWARE)
pose_SOURCES	+=	$(SRC_PATCHES) $(SRC_TRG) $(SRC_UAE) $(SRC_PALM) $(SRC_THREAD)

ResStrings.cpp: $(srcdir)/../SrcShared/Strings.txt
	perl -x $(srcdir)/../SrcShared/Strings.txt

EmDlgFltkFactory.h EmDlgFltkFactory.cpp : $(srcdir)/../SrcUnix/EmDlgFltkFactory.fl
	fluid -c $(srcdir)/../SrcUnix/EmDlgFltkFactory.fl

EmDlgFltk.o : EmDlgFltkFactory.h

mytags:
	etags -R $(srcdir)/../SrcUnix/* $(srcdir)/../SrcShared/*

CLEANFILES		=		ResStrings.cpp
CLEANFILES		+=		EmDlgFltkFactory.h
CLEANFILES		+=		EmDlgFltkFactory.cpp
CLEANFILES		+=		config.cache
CLEANFILES		+=		config.log

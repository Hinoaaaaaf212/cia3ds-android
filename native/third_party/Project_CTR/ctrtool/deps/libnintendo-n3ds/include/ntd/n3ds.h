#pragma once

// definitions
#include <ntd/n3ds/cci.h>
#include <ntd/n3ds/cia.h>
#include <ntd/n3ds/cro.h>
#include <ntd/n3ds/crr.h>
#include <ntd/n3ds/exefs.h>
#include <ntd/n3ds/exheader.h>
#include <ntd/n3ds/firm.h>
#include <ntd/n3ds/ivfc.h>
#include <ntd/n3ds/ncch.h>
#include <ntd/n3ds/romfs.h>
#include <ntd/n3ds/smdh.h>

// Wrapped IStream
#include <ntd/n3ds/IvfcStream.h>

// Utilities
#include <ntd/n3ds/CtrKeyGenerator.h>

// VirtualFileSystem metadata generators
#include <ntd/n3ds/ExeFsMetaGenerator.h>
#include <ntd/n3ds/RomFsMetaGenerator.h>
#include <ntd/n3ds/CciFsMetaGenerator.h>
#include <ntd/n3ds/CiaFsMetaGenerator.h>
# CTR Tool (CTRTool)
General purpose reading/extraction tool for Nintendo 3DS file formats.

## Supported File Formats
* ExeFs (.exefs)
* RomFs (.romfs) (and RomFS wrapped in IVFC)
* NCCH Format Variants:
  * CTR Executable Image (.cxi)
  * CTR File Archive (.cfa)
  * CIP (.cip) (These are the processes bundled with the kernel image)
* NCCH ExtendedHeader (.exhdr)
* CTR Importable Archive (.cia)
* NCSD Format Variants:
  * CTR Card Image (.cci/.3ds/.3dz)
  * CTR System Update (.csu)
* ES TitleMetaData (.tmd)
* ES eTicket (.tik)
* Firmware Images (.firm)
* CRR (.crr)


# Usage
```
Usage: ctrtool [options... ] <file>
Options:
  -i, --info         Show file info.
                          This is the default action.
  -p, --plain        Extract data without decrypting.
  -r, --raw          Keep raw data, don't unpack.
  -v, --verbose      Give verbose output.
  -y, --verify       Verify hashes and signatures.
  -d, --dev          Decrypt with development keys instead of retail.
  --titlekey=key     Set tik title key.
  --seeddb=file      Set seeddb for ncch seed crypto.
  --seed=key         Set specific seed for ncch seed crypto.
  --showsyscalls     Show system call names instead of numbers.
  -t, --intype=type  Specify input file type. [cia, tik, tmd, ncsd, ncch, exheader, exefs, romfs, firm, lzss]
                     (only needed when file type isn't detected automatically)
CCI options:
  -n, --ncch=index   Specify NCCH partition index.
  --contents=dir     Specify Contents directory path.
CIA options:
  -n, --ncch=index   Specify NCCH partition index.
  --contents=dir     Specify Contents directory path.
  --certs=file       Specify Certificate chain file path.
  --tik=file         Specify Ticket file path.
  --tmd=file         Specify TMD file path.
  --footer=file      Specify Footer file path.
NCCH options:
  --exheader=file    Specify Extended Header file path.
  --logo=file        Specify Logo file path.
  --plainrgn=file    Specify Plain region file path
  --exefs=file       Specify ExeFS file path.
  --romfs=file       Specify RomFS file path.
EXEFS options:
  --exefsdir=dir     Specify ExeFS directory path.
  --listexefs        List files in ExeFS.
  --decompresscode   Decompress .code section
                     (only needed when using raw ExeFS file)
ROMFS options:
  --romfsdir=dir     Specify RomFS directory path.
  --listromfs        List files in RomFS.
FIRM options:
  --firmdir=dir      Specify Firm directory path.
  --firmtype=type    Specify Firm location type, this determines encryption/signing.
                       - nand: (default) FIRM images installed to internal NAND,
                       - ngc: FIRM images loaded from NTR game card at boot,
                       - nor: FIRM images loaded from WiFi board NOR at boot,
                       - sdmc: FIRM images installed from SD card by FIRM installers (internal dev tool).
LZSS options:
  --lzssout=file     Specify lzss output file
```

# Building
See [BUILDING.md](/ctrtool/BUILDING.md).
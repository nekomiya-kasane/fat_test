#ifndef __NYU__FAT_32_H__
#define __NYU__FAT_32_H__

#define INDEX(i, j, length) ((i) * (length) + (j))

#pragma pack(push, 1)
typedef struct BootEntry
{
  unsigned char BS_jmpBoot[3];    // Assembly instruction to jump to boot code
  unsigned char BS_OEMName[8];    // OEM Name in ASCII
  unsigned short BPB_BytsPerSec;  // Bytes per sector. Allowed values include 512, 1024, 2048, and 4096
  unsigned char BPB_SecPerClus;   // Sectors per cluster (data unit). Allowed values are powers of 2, but the cluster size must be 32KB or smaller
  unsigned short BPB_RsvdSecCnt;  // Size in sectors of the reserved area
  unsigned char BPB_NumFATs;      // Number of FATs
  unsigned short BPB_RootEntCnt;  // Maximum number of files in the root directory for FAT12 and FAT16. This is 0 for FAT32
  unsigned short BPB_TotSec16;    // 16-bit value of number of sectors in file system
  unsigned char BPB_Media;        // Media type
  unsigned short BPB_FATSz16;     // 16-bit size in sectors of each FAT for FAT12 and FAT16. For FAT32, this field is 0
  unsigned short BPB_SecPerTrk;   // Sectors per track of storage device
  unsigned short BPB_NumHeads;    // Number of heads in storage device
  unsigned int BPB_HiddSec;       // Number of sectors before the start of partition
  unsigned int BPB_TotSec32;      // 32-bit value of number of sectors in file system. Either this value or the 16-bit value above must be 0
  unsigned int BPB_FATSz32;       // 32-bit size in sectors of one FAT
  unsigned short BPB_ExtFlags;    // A flag for FAT
  unsigned short BPB_FSVer;       // The major and minor version number
  unsigned int BPB_RootClus;      // Cluster where the root directory can be found
  unsigned short BPB_FSInfo;      // Sector where FSINFO structure can be found
  unsigned short BPB_BkBootSec;   // Sector where backup copy of boot sector is located
  unsigned char BPB_Reserved[12]; // Reserved
  unsigned char BS_DrvNum;        // BIOS INT13h drive number
  unsigned char BS_Reserved1;     // Not used
  unsigned char BS_BootSig;       // Extended boot signature to identify if the next three values are valid
  unsigned int BS_VolID;          // Volume serial number
  unsigned char BS_VolLab[11];    // Volume label in ASCII. User defines when creating the file system
  unsigned char BS_FilSysType[8]; // File system type label in ASCII
} BootEntry;
#pragma pack(pop)

enum Attribute
{
  ATTR_READ_AND_WRITE = 0x00,
  ATTR_READ_ONLY = 0x01,
  ATTR_HIDDEN = 0x02,
  ATTR_SYSTEM = 0x04,
  ATTR_VOLUMN = 0x08,
  ATTR_SUBDIRECTORY = 0x10,
  ATTR_ARCHIEVED = 0x20
};

#pragma pack(push, 1)
typedef struct DirEntry
{
  unsigned char DIR_Name[8];     // File name
  unsigned char DIR_Ext[3];
#define	SLOT_EMPTY	(char)0x00		/* slot has never been used */
#define	SLOT_E5		(char)0x05		/* the real value is 0xe5 */
#define	SLOT_DELETED	(char)0xe5		/* file in this slot deleted */
#define SLOT_SPECIAL (char)0x2e /* special dir */
#define SLOT_NEXT_VALID (char)0x41 /* next entry is valid */
#define SLOT_LONG_ENTRY (char)0x0f /* long entry */
  unsigned char DIR_Attr;         // File attributes
#define	ATTR_NORMAL	0x00		/* normal file */
#define	ATTR_READONLY	0x01		/* file is readonly */
#define	ATTR_HIDDEN	0x02		/* file is hidden */
#define	ATTR_SYSTEM	0x04		/* file is a system file */
#define	ATTR_VOLUME	0x08		/* entry is a volume label */
#define	ATTR_DIRECTORY	0x10		/* entry is a directory name */
#define	ATTR_ARCHIVE	0x20		/* file is new or modified */
  unsigned char DIR_NTRes;        // Reserved
  unsigned char DIR_CrtTimeTenth; // Created time (tenths of second)
  unsigned short DIR_CrtTime;     // Created time (hours, minutes, seconds)
  unsigned short DIR_CrtDate;     // Created day
  unsigned short DIR_LstAccDate;  // Accessed day
  unsigned short DIR_FstClusHI;   // High 2 bytes of the first cluster address
  unsigned short DIR_WrtTime;     // Written time (hours, minutes, seconds
  unsigned short DIR_WrtDate;     // Written day
  unsigned short DIR_FstClusLO;   // Low 2 bytes of the first cluster address
  unsigned int DIR_FileSize;      // File size in bytes. (0 for directories)
} DirEntry;
#pragma pack(pop)

#pragma pack(push, 1)
typedef struct DirLongEntry
{
#define LONG_SLOT_ID_MAST 0x1F
#define LONG_SLOT_END_MASK 0x40
  unsigned char id;             /* sequence number for slot */
  unsigned char name0_4[10];    /* first 5 characters in name */
  unsigned char attr;           /* attribute byte */
  unsigned char reserved;       /* always 0 */
  unsigned char alias_checksum; /* checksum for 8.3 alias */
  unsigned char name5_10[12];   /* 6 more characters in name */
  unsigned short start;         /* starting cluster number, 0 in long slots */
  unsigned char name11_12[4];   /* last 2 characters in name */
} DirLongEntry;
#pragma pack(pop)

#define CLUSTER_END 0x0FFFFFFF
#define CLUSTER_EOC 0x0FFFFFF8
#define CLUSTER_BAD 0x0FFFFFF7
#define CLUSTER_MAX 0x0FFFFFEF
#define CLUSTER_MIN 0x00000002
#define CLUSTER_RESERVED 0x00000001
#define CLUSTER_IDLE 0x00000000

#endif

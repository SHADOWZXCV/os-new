#include "types/primitives.h"

#ifndef MEM_MNGR_H_
#define MEM_MNGR_H_
#define SIZE_1_KB 1024
#define DEFAULT_BLOCK_SIZE_B 4096
#define BLOCKS_IN_BYTE 8
#define ALIGN_UP(address, upto) (((address) + ((upto) - 1)) & ~((upto) - 1))

extern dword kernel_end;
extern dword kernel_start;

enum MMAP_ENTRY_TYPE {
    AVAILABLE = 1,
    RESERVED
};

typedef struct __attribute__((packed)) {
	unsigned long long base_address;
	unsigned long long length;
	enum MMAP_ENTRY_TYPE type;
	unsigned int acpi_null;
} mmap_entry;

typedef struct {
    mmap_entry *mmap_entries;
    word mmap_entries_count;
    dword memory_size_kb;
} memory_state;

typedef struct {
  dword blocks_count;
  dword *blocks;
  dword used_blocks;
} memory_manager;

extern memory_state memory;
extern memory_manager manager;

mmap_entry* get_memory_map_entry_details(byte index);
void mmanager_init_memory();

void mmanager_dealloc_region_block(dword address, dword bytes);
void mmanager_alloc_region_block(dword address, dword bytes);
sdword mmanager_find_first_free_block();
void mmanager_use_block(dword index);
void *alloc_block();
void mmanager_free_block(dword index);
byte mmanager_is_block_free(dword index);

#endif
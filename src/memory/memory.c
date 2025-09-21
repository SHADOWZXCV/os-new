#include "memory/memory.h"
#include "shared/os_state.h"

dword kernel_end_addr;
dword kernel_start_addr;

memory_state memory;
memory_manager manager;

void *memset(void *ptr, int value, size size_to_cover) {
	byte *p = (byte *) ptr;

	while(size_to_cover--) {
		*p++ = (byte)value;
	}

    return ptr;
}

void init_memory_map() {
    kernel_start_addr = (dword) &kernel_start;
    kernel_end_addr = (dword) &kernel_end;

    // Old, does not capture the entire memory size
    // unsigned int size = ((boot_info_arr->memory_high * 64) + boot_info_arr->memory_low + SIZE_1_KB);
    dword size = 0;
    dword base_mmap_addr = (boot_info_arr->mmap_addr << 4) | boot_info_arr->mmap_offset;

    memory.mmap_entries = (mmap_entry *) base_mmap_addr;
    memory.mmap_entries_count = boot_info_arr->mmap_length;

    for (short i = 0; i < memory.mmap_entries_count; i++) {
        mmap_entry *entry = get_memory_map_entry_details(i);

        size += entry->length / 1024;
    }

    memory.memory_size_kb = size;
}

mmap_entry* get_memory_map_entry_details(byte index) {
    if (index >= memory.mmap_entries_count)
        return NULL;

    return (mmap_entry *) memory.mmap_entries + index;
}

void mmanager_init_memory() {
    init_memory_map();

    // OLD, Does not capture the true memory size
    // manager.blocks_count = memory.memory_size_kb / (DEFAULT_BLOCK_SIZE_B / 1024);
    // Modern, but still does not capture the true memory's size
    manager.blocks_count = memory.memory_size_kb / (DEFAULT_BLOCK_SIZE_B / 1024);
    manager.blocks = (dword *) ALIGN_UP(kernel_end_addr, sizeof(dword));

    kernel_end_addr = (dword) (manager.blocks + (manager.blocks_count + 31) / 32);
    
    // set all first
    manager.used_blocks = manager.blocks_count;
    memset(manager.blocks, 0xFF, (manager.blocks_count + 31) / 32 * sizeof(dword));

    // skip the reserved memory
    for (short i = 0; i < memory.mmap_entries_count; i++) {
        mmap_entry *entry = get_memory_map_entry_details(i);

        if (entry->type == AVAILABLE) {
            mmanager_dealloc_region_block(entry->base_address, entry->length);
        }
    }

    // set the first 1MB as used ( everything bootloader + anything else related )
    mmanager_alloc_region_block(0, 0x100000);
    // set the kernel's place in memory ( 0x12000 + its size ) as used
    mmanager_alloc_region_block(kernel_start_addr, kernel_end_addr - kernel_start_addr);

    // leave the first block used, to use for null references
    mmanager_use_block(0);
}

void mmanager_dealloc_region_block(dword address, dword bytes) {
    dword block_index = address / DEFAULT_BLOCK_SIZE_B;
    dword blocks = (bytes + DEFAULT_BLOCK_SIZE_B - 1) / DEFAULT_BLOCK_SIZE_B;

    while(blocks--) {
        if (!mmanager_is_block_free(block_index)) {
            mmanager_free_block(block_index);
        }
        block_index++;
    }
}

void mmanager_alloc_region_block(dword address, dword bytes) {
    dword block_index = address / DEFAULT_BLOCK_SIZE_B;
    dword blocks = (bytes + DEFAULT_BLOCK_SIZE_B - 1) / DEFAULT_BLOCK_SIZE_B;

    while(blocks--) {
        if (mmanager_is_block_free(block_index)) {
            mmanager_use_block(block_index);
        }
        block_index++;
    }
}

// TODO: make functions to alloc and dealloc a region based on a specific address

sdword mmanager_find_first_free_block() {
   for (dword i = 0; i < (manager.blocks_count + 31) / 32; i++) {
        // access a block
        dword *block_cluster = &manager.blocks[i];

        if (*block_cluster == 0xFFFFFFFF)
            continue;

        for (dword j = 0; j < 32; j++) {
            if (!(*block_cluster & 1 << j)) {
                return i * sizeof(dword) * 8 + j;
            }
        }
   }

   return -1;
}

void mmanager_use_block(dword index) {
    manager.blocks[index / (sizeof(dword) * 8)] |= 1 << (index % (sizeof(dword) * 8));
    manager.used_blocks++;
}

void *alloc_block() {
    if (manager.used_blocks == manager.blocks_count)
        return NULL;

    sdword frame = mmanager_find_first_free_block();

    // frame 0 is always reserved (IVT + BDA), never allocatable
    if (frame) {
        mmanager_use_block(frame);

        return (void *) (frame * DEFAULT_BLOCK_SIZE_B);
    }

    return NULL;
}

void mmanager_free_block(dword index) {
    manager.blocks[index / (sizeof(dword) * 8)] &= ~(1 << (index % (sizeof(dword) * 8)));
    manager.used_blocks--;
}

byte mmanager_is_block_free(dword index) {
    return !(manager.blocks[index / (sizeof(dword) * 8)] & (1 << (index % (sizeof(dword) * 8))));
}

#ifndef PAGES_H
#define PAGES_H

#include "typedef.h"

#define PAGE_SIZE 4096
#define PAGE_SHIFT 12 // number of bits to represent page offset

#define PAGE_ROUND_UP(pa) (((pa)+PAGE_SIZE-1) & ~(PAGE_SIZE-1)) // round up an address to page-byte boundary
#define PAGE_ROUND_DOWN(pa) (((pa)) & ~(PAGE_SIZE-1)) // round down an address to page-byte boundary

#define PTE_V (1L << 0) // valid
#define PTE_R (1L << 1)  
#define PTE_W (1L << 2)
#define PTE_X (1L << 3)
#define PTE_U (1L << 4) // user access

// 44 bits of physical addresses + 10 bits of permission bits to be stored in PTEs
#define PA2PTE(pa) ((((uint64)pa) >> 12) << 10) // convert physical addresses to a format which PTEs store

#define PTE2PA(pte) (((pte) >> 10) << 12) // convert the format which PTEs store to physical addresses

// extract 9-bit indices for each page table level from virtual address
#define PAGE_INDEX_MASK 0x1FF // 9 bits
#define PAGE_INDEX_SHIFT(level) (PAGE_SHIFT + (9*(level)))
#define PAGE_INDEX(level, va) ((((uint64)va) >> PAGE_INDEX_SHIFT(level)) & PAGE_INDEX_MASK)

// MAX_VA is one bit less than the max allowed by Sv39
// to avoid having to sign-extend virtual addresses
// that have the high bit set.
// MAX_VA is the highest possible virtual address + 1,
// basically a limit which cannot be used.
#define MAX_VA (1L << (9 + 9 + 9 + 12 - 1))

#endif

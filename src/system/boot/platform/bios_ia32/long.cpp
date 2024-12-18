/*
 * Copyright 2012, Alex Smith, alex@alex-smith.me.uk.
 * Distributed under the terms of the MIT License.
 */


#include "long.h"

#include <algorithm>

#include <KernelExport.h>

// Include the x86_64 version of descriptors.h
#define __x86_64__
#include <arch/x86/descriptors.h>
#undef __x86_64__

#include <arch_system_info.h>
#include <boot/platform.h>
#include <boot/heap.h>
#include <boot/stage2.h>
#include <boot/stdio.h>
#include <kernel.h>
#include <safemode.h>

#include "debug.h"
#include "mmu.h"
#include "smp.h"


static const uint64 kTableMappingFlags = 0x7;
static const uint64 kLargePageMappingFlags = 0x183;
static const uint64 kPageMappingFlags = 0x103;
	// Global, R/W, Present

extern "C" void long_enter_kernel(int currentCPU, uint64 stackTop);

extern uint64 gLongGDT;
extern uint32 gLongPhysicalPMLTop;
extern bool gLongLA57;
extern uint64 gLongKernelEntry;


/*! Convert a 32-bit address to a 64-bit address. */
static inline uint64
fix_address(uint64 address)
{
	if (address >= KERNEL_LOAD_BASE)
		return address + KERNEL_FIXUP_FOR_LONG_MODE;
	else
		return address;
}


template<typename Type>
inline void
fix_address(FixedWidthPointer<Type>& p)
{
	if (p != NULL)
		p.SetTo(fix_address(p.Get()));
}


static void
long_gdt_init()
{
	STATIC_ASSERT(BOOT_GDT_SEGMENT_COUNT > KERNEL_CODE_SEGMENT
		&& BOOT_GDT_SEGMENT_COUNT > KERNEL_DATA_SEGMENT
		&& BOOT_GDT_SEGMENT_COUNT > USER_CODE_SEGMENT
		&& BOOT_GDT_SEGMENT_COUNT > USER_DATA_SEGMENT);

	clear_segment_descriptor(&gBootGDT[0]);

	// Set up code/data segments (TSS segments set up later in the kernel).
	set_segment_descriptor(&gBootGDT[KERNEL_CODE_SEGMENT], DT_CODE_EXECUTE_ONLY,
		DPL_KERNEL);
	set_segment_descriptor(&gBootGDT[KERNEL_DATA_SEGMENT], DT_DATA_WRITEABLE,
		DPL_KERNEL);
	set_segment_descriptor(&gBootGDT[USER_CODE_SEGMENT], DT_CODE_EXECUTE_ONLY,
		DPL_USER);
	set_segment_descriptor(&gBootGDT[USER_DATA_SEGMENT], DT_DATA_WRITEABLE,
		DPL_USER);

	// Used by long_enter_kernel().
	gLongGDT = fix_address((addr_t)gBootGDT);
	dprintf("GDT at 0x%llx\n", gLongGDT);
}


static void
long_mmu_init()
{
	uint64* pmlTop;
	// Allocate the top level PMLTop.
	pmlTop = (uint64*)mmu_allocate_page((addr_t*)&gKernelArgs.arch_args.phys_pgdir);
	memset(pmlTop, 0, B_PAGE_SIZE);
	gKernelArgs.arch_args.vir_pgdir = fix_address((uint64)(addr_t)pmlTop);

	// Store the virtual memory usage information.
	gKernelArgs.virtual_allocated_range[0].start = KERNEL_LOAD_BASE_64_BIT;
	gKernelArgs.virtual_allocated_range[0].size = mmu_get_virtual_usage();
	gKernelArgs.num_virtual_allocated_ranges = 1;
	gKernelArgs.arch_args.virtual_end = ROUNDUP(KERNEL_LOAD_BASE_64_BIT
		+ gKernelArgs.virtual_allocated_range[0].size, 0x200000);

	// Find the highest physical memory address. We map all physical memory
	// into the kernel address space, so we want to make sure we map everything
	// we have available.
	uint64 maxAddress = 0;
	for (uint32 i = 0; i < gKernelArgs.num_physical_memory_ranges; i++) {
		maxAddress = std::max(maxAddress,
			gKernelArgs.physical_memory_range[i].start
				+ gKernelArgs.physical_memory_range[i].size);
	}

	// Want to map at least 4GB, there may be stuff other than usable RAM that
	// could be in the first 4GB of physical address space.
	maxAddress = std::max(maxAddress, (uint64)0x100000000ll);
	maxAddress = ROUNDUP(maxAddress, 0x40000000);

	// Currently only use 1 PDPT (512GB). This will need to change if someone
	// wants to use Haiku on a box with more than 512GB of RAM but that's
	// probably not going to happen any time soon.
	if (maxAddress / 0x40000000 > 512)
		panic("Can't currently support more than 512GB of RAM!");

	uint64* pml4 = pmlTop;
	addr_t physicalAddress;
	cpuid_info info;
	if (get_current_cpuid(&info, 7, 0) == B_OK
		&& (info.regs.ecx & IA32_FEATURE_LA57) != 0) {

		if (get_safemode_boolean(B_SAFEMODE_256_TB_MEMORY_LIMIT, false)) {
			// LA57 has been disabled!
			dprintf("la57 disabled per safemode setting\n");
		} else {
			dprintf("la57 enabled\n");
			gLongLA57 = true;
			pml4 = (uint64*)mmu_allocate_page(&physicalAddress);
			memset(pml4, 0, B_PAGE_SIZE);
			pmlTop[511] = physicalAddress | kTableMappingFlags;
			pmlTop[0] = physicalAddress | kTableMappingFlags;
		}
	}

	uint64* pdpt;
	uint64* pageDir;
	uint64* pageTable;

	// Create page tables for the physical map area. Also map this PDPT
	// temporarily at the bottom of the address space so that we are identity
	// mapped.

	pdpt = (uint64*)mmu_allocate_page(&physicalAddress);
	memset(pdpt, 0, B_PAGE_SIZE);
	pml4[510] = physicalAddress | kTableMappingFlags;
	pml4[0] = physicalAddress | kTableMappingFlags;

	for (uint64 i = 0; i < maxAddress; i += 0x40000000) {
		pageDir = (uint64*)mmu_allocate_page(&physicalAddress);
		memset(pageDir, 0, B_PAGE_SIZE);
		pdpt[i / 0x40000000] = physicalAddress | kTableMappingFlags;

		for (uint64 j = 0; j < 0x40000000; j += 0x200000) {
			pageDir[j / 0x200000] = (i + j) | kLargePageMappingFlags;
		}

		mmu_free(pageDir, B_PAGE_SIZE);
	}

	mmu_free(pdpt, B_PAGE_SIZE);

	// Allocate tables for the kernel mappings.
	pdpt = (uint64*)mmu_allocate_page(&physicalAddress);
	memset(pdpt, 0, B_PAGE_SIZE);
	pml4[511] = physicalAddress | kTableMappingFlags;

	pageDir = (uint64*)mmu_allocate_page(&physicalAddress);
	memset(pageDir, 0, B_PAGE_SIZE);
	pdpt[510] = physicalAddress | kTableMappingFlags;

	// We can now allocate page tables and duplicate the mappings across from
	// the 32-bit address space to them.
	pageTable = NULL;
	for (uint32 i = 0; i < gKernelArgs.virtual_allocated_range[0].size
			/ B_PAGE_SIZE; i++) {
		if ((i % 512) == 0) {
			if (pageTable)
				mmu_free(pageTable, B_PAGE_SIZE);

			pageTable = (uint64*)mmu_allocate_page(&physicalAddress);
			memset(pageTable, 0, B_PAGE_SIZE);
			pageDir[i / 512] = physicalAddress | kTableMappingFlags;
		}

		// Get the physical address to map.
		if (!mmu_get_virtual_mapping(KERNEL_LOAD_BASE + (i * B_PAGE_SIZE),
				&physicalAddress))
			continue;

		pageTable[i % 512] = physicalAddress | kPageMappingFlags;
	}

	if (pageTable)
		mmu_free(pageTable, B_PAGE_SIZE);
	mmu_free(pageDir, B_PAGE_SIZE);
	mmu_free(pdpt, B_PAGE_SIZE);
	if (pml4 != pmlTop)
		mmu_free(pml4, B_PAGE_SIZE);

	// Sort the address ranges.
	sort_address_ranges(gKernelArgs.physical_memory_range,
		gKernelArgs.num_physical_memory_ranges);
	sort_address_ranges(gKernelArgs.physical_allocated_range,
		gKernelArgs.num_physical_allocated_ranges);
	sort_address_ranges(gKernelArgs.virtual_allocated_range,
		gKernelArgs.num_virtual_allocated_ranges);

	dprintf("phys memory ranges:\n");
	for (uint32 i = 0; i < gKernelArgs.num_physical_memory_ranges; i++) {
		dprintf("    base %#018" B_PRIx64 ", length %#018" B_PRIx64 "\n",
			gKernelArgs.physical_memory_range[i].start,
			gKernelArgs.physical_memory_range[i].size);
	}

	dprintf("allocated phys memory ranges:\n");
	for (uint32 i = 0; i < gKernelArgs.num_physical_allocated_ranges; i++) {
		dprintf("    base %#018" B_PRIx64 ", length %#018" B_PRIx64 "\n",
			gKernelArgs.physical_allocated_range[i].start,
			gKernelArgs.physical_allocated_range[i].size);
	}

	dprintf("allocated virt memory ranges:\n");
	for (uint32 i = 0; i < gKernelArgs.num_virtual_allocated_ranges; i++) {
		dprintf("    base %#018" B_PRIx64 ", length %#018" B_PRIx64 "\n",
			gKernelArgs.virtual_allocated_range[i].start,
			gKernelArgs.virtual_allocated_range[i].size);
	}

	gLongPhysicalPMLTop = gKernelArgs.arch_args.phys_pgdir;
}


static void
convert_preloaded_image(preloaded_elf64_image* image)
{
	fix_address(image->next);
	fix_address(image->name);
	fix_address(image->debug_string_table);
	fix_address(image->syms);
	fix_address(image->rel);
	fix_address(image->rela);
	fix_address(image->pltrel);
	fix_address(image->debug_symbols);
}


/*!	Convert all addresses in kernel_args to 64-bit addresses. */
static void
convert_kernel_args()
{
	fix_address(gKernelArgs.boot_volume);
	fix_address(gKernelArgs.vesa_modes);
	fix_address(gKernelArgs.edid_info);
	fix_address(gKernelArgs.debug_output);
	fix_address(gKernelArgs.previous_debug_output);
	fix_address(gKernelArgs.boot_splash);
	fix_address(gKernelArgs.ucode_data);
	fix_address(gKernelArgs.arch_args.apic);
	fix_address(gKernelArgs.arch_args.hpet);

	convert_preloaded_image(static_cast<preloaded_elf64_image*>(
		gKernelArgs.kernel_image.Pointer()));
	fix_address(gKernelArgs.kernel_image);

	// Iterate over the preloaded images. Must save the next address before
	// converting, as the next pointer will be converted.
	preloaded_image* image = gKernelArgs.preloaded_images;
	fix_address(gKernelArgs.preloaded_images);
	while (image != NULL) {
		preloaded_image* next = image->next;
		convert_preloaded_image(static_cast<preloaded_elf64_image*>(image));
		image = next;
	}

	// Set correct kernel args range addresses.
	dprintf("kernel args ranges:\n");
	for (uint32 i = 0; i < gKernelArgs.num_kernel_args_ranges; i++) {
		gKernelArgs.kernel_args_range[i].start = fix_address(
			gKernelArgs.kernel_args_range[i].start);
		dprintf("    base %#018" B_PRIx64 ", length %#018" B_PRIx64 "\n",
			gKernelArgs.kernel_args_range[i].start,
			gKernelArgs.kernel_args_range[i].size);
	}

	// Fix driver settings files.
	driver_settings_file* file = gKernelArgs.driver_settings;
	fix_address(gKernelArgs.driver_settings);
	while (file != NULL) {
		driver_settings_file* next = file->next;
		fix_address(file->next);
		fix_address(file->buffer);
		file = next;
	}
}


static void
enable_sse()
{
	x86_write_cr4(x86_read_cr4() | CR4_OS_FXSR | CR4_OS_XMM_EXCEPTION);
	x86_write_cr0(x86_read_cr0() & ~(CR0_FPU_EMULATION | CR0_MONITOR_FPU));
}


static void
long_smp_start_kernel(void)
{
	uint32 cpu = smp_get_current_cpu();

	// Important.  Make sure supervisor threads can fault on read only pages...
	asm("movl %%eax, %%cr0" : : "a" ((1 << 31) | (1 << 16) | (1 << 5) | 1));
	asm("cld");
	asm("fninit");
	enable_sse();

	// Fix our kernel stack address.
	gKernelArgs.cpu_kstack[cpu].start
		= fix_address(gKernelArgs.cpu_kstack[cpu].start);

	long_enter_kernel(cpu, gKernelArgs.cpu_kstack[cpu].start
		+ gKernelArgs.cpu_kstack[cpu].size);

	panic("Shouldn't get here");
}


void
long_start_kernel()
{
	// Check whether long mode is supported.
	cpuid_info info;
	get_current_cpuid(&info, 0x80000001, 0);
	if ((info.regs.edx & (1 << 29)) == 0)
		panic("64-bit kernel requires a 64-bit CPU");

	enable_sse();

	preloaded_elf64_image *image = static_cast<preloaded_elf64_image *>(
		gKernelArgs.kernel_image.Pointer());

	smp_init_other_cpus();

	long_gdt_init();
	debug_cleanup();
	long_mmu_init();
	heap_release();

	convert_kernel_args();

	// Save the kernel entry point address.
	gLongKernelEntry = image->elf_header.e_entry;
	dprintf("kernel entry at %#llx\n", gLongKernelEntry);

	// Fix our kernel stack address.
	gKernelArgs.cpu_kstack[0].start
		= fix_address(gKernelArgs.cpu_kstack[0].start);

	// We're about to enter the kernel -- disable console output.
	stdout = NULL;

	smp_boot_other_cpus(long_smp_start_kernel);

	// Enter the kernel!
	long_enter_kernel(0, gKernelArgs.cpu_kstack[0].start
		+ gKernelArgs.cpu_kstack[0].size);

	panic("Shouldn't get here");
}

/*
 * Copyright 2004-2009, Axel Dörfler, axeld@pinc-software.de.
 * Copyright 2008, Stephan Aßmus <superstippi@gmx.de>
 * Copyright 2008, Philippe Saint-Pierre <stpere@gmail.com>
 * Distributed under the terms of the MIT License.
 */


#include <arch/cpu.h>
#include <boot/stage2.h>
#include <boot/platform.h>
#include <boot/menu.h>
#include <boot/kernel_args.h>
#include <boot/platform/generic/video.h>
#include <boot/platform/generic/video_blitter.h>

#include <boot/images.h>
#include <boot/platform/generic/video_splash.h>

#include <stdio.h>
#include <stdlib.h>

#include <zlib.h>


//#define TRACE_VIDEO
#ifdef TRACE_VIDEO
#	define TRACE(x) dprintf x
#else
#	define TRACE(x) ;
#endif


static status_t
uncompress(const uint8 compressed[], unsigned int compressedSize,
	uint8* uncompressed, unsigned int uncompressedSize)
{
	if (compressedSize == 0 || uncompressedSize == 0)
		return B_BAD_VALUE;

	// prepare zlib stream
	z_stream zStream = {
		(Bytef*)compressed,		// next_in
		compressedSize,			// avail_in
		0,						// total_in
		(Bytef*)uncompressed,	// next_out
		uncompressedSize,		// avail_out
		0,						// total_out
		0,						// msg
		0,						// state
		Z_NULL,					// zalloc (kernel_args_malloc?)
		Z_NULL,					// zfree (kernel_args_free?)
		Z_NULL,					// opaque
		0,						// data_type
		0,						// adler
		0						// reserved
	};

	int zlibError = inflateInit(&zStream);
	if (zlibError != Z_OK)
		return B_ERROR;	// TODO: translate zlibError

	// inflate
	status_t status = B_OK;
	zlibError = inflate(&zStream, Z_FINISH);
	if (zlibError != Z_STREAM_END) {
		if (zlibError == Z_OK)
			status = B_BUFFER_OVERFLOW;
		else
			status = B_ERROR;	// TODO: translate zlibError
	}

	// clean up
	zlibError = inflateEnd(&zStream);
	if (zlibError != Z_OK && status == B_OK)
		status = B_ERROR;	// TODO: translate zlibError

	if (status == B_OK && zStream.total_out != uncompressedSize)
		status = B_ERROR;

	return status;
}


extern "C" void
video_blit_image(addr_t frameBuffer, const uint8 *data,
	uint16 width, uint16 height, uint16 imageWidth, uint16 left, uint16 top)
{
	if (gKernelArgs.frame_buffer.depth == 4) {
		// call platform specific code since it's really platform-specific.
		platform_blit4(frameBuffer, data, width, height,
			imageWidth, left, top);
	} else {
		BlitParameters params;
		params.from = data;
		params.fromWidth = imageWidth;
		params.fromLeft = params.fromTop = 0;
		params.fromRight = width;
		params.fromBottom = height;
		params.to = (uint8*)frameBuffer;
		params.toBytesPerRow = gKernelArgs.frame_buffer.bytes_per_row;
		params.toLeft = left;
		params.toTop = top;
		blit(params, gKernelArgs.frame_buffer.depth);
	}
}


extern "C" status_t
video_display_splash(addr_t frameBuffer)
{
	if (!gKernelArgs.frame_buffer.enabled)
		return B_NO_INIT;

	addr_t pos = 0;
	// Limit area to clear to estimated screen area
	// UEFI can happily report a >256M framebuffer
	addr_t size = min_c(gKernelArgs.frame_buffer.width
			* gKernelArgs.frame_buffer.height * 4u,
		gKernelArgs.frame_buffer.physical_buffer.size);

	if (size >= 64) {
		// Align writes
		for (addr_t align = (8 - (frameBuffer & 7)) & 7; pos < align; pos++)
			*(char*)(frameBuffer + pos) = 0;
		// Write eight bytes, many many times, but not too many
		for (addr_t alignSize = size - 8; pos < alignSize; pos +=8) {
			*(uint32*)(frameBuffer + pos) = 0;
			*(uint32*)(frameBuffer + pos + 4) = 0;
		}
	}
	// Write a few bytes more
	for (; pos < size; pos++)
		*(char*)(frameBuffer + pos) = 0;

	uint8* uncompressedLogo = NULL;
	unsigned int uncompressedSize = kSplashLogoWidth * kSplashLogoHeight;
	switch (gKernelArgs.frame_buffer.depth) {
		case 8:
			platform_set_palette(k8BitPalette);
			uncompressedLogo = (uint8*)kernel_args_malloc(uncompressedSize);
			if (uncompressedLogo == NULL)
				return B_NO_MEMORY;

			uncompress(kSplashLogo8BitCompressedImage,
				sizeof(kSplashLogo8BitCompressedImage), uncompressedLogo,
				uncompressedSize);
		break;
		default: // 24 bits is assumed here
			uncompressedSize *= 3;
			uncompressedLogo = (uint8*)kernel_args_malloc(uncompressedSize);
			if (uncompressedLogo == NULL)
				return B_NO_MEMORY;

			uncompress(kSplashLogo24BitCompressedImage,
				sizeof(kSplashLogo24BitCompressedImage), uncompressedLogo,
				uncompressedSize);
		break;
	}

	// TODO: support 4-bit indexed version of the images!

	// render splash logo
	int width, height, x, y;
	compute_splash_logo_placement(gKernelArgs.frame_buffer.width, gKernelArgs.frame_buffer.height,
		width, height, x, y);
	video_blit_image(frameBuffer, uncompressedLogo, width, height,
		kSplashLogoWidth, x, y);

	kernel_args_free(uncompressedLogo);

	const uint8* lowerHalfIconImage;
	uncompressedSize = kSplashIconsWidth * kSplashIconsHeight;
	const uint16 iconsHalfHeight = kSplashIconsHeight / 2;
	switch (gKernelArgs.frame_buffer.depth) {
		case 8:
			// pointer into the lower half of the icons image data
			gKernelArgs.boot_splash
				= (uint8*)kernel_args_malloc(uncompressedSize);
			if (gKernelArgs.boot_splash == NULL)
				return B_NO_MEMORY;
			uncompress(kSplashIcons8BitCompressedImage,
				sizeof(kSplashIcons8BitCompressedImage),
				gKernelArgs.boot_splash, uncompressedSize);
			lowerHalfIconImage = (uint8 *)gKernelArgs.boot_splash
				+ (kSplashIconsWidth * iconsHalfHeight);
		break;
		default:	// 24bits is assumed here
			uncompressedSize *= 3;
			// pointer into the lower half of the icons image data
			gKernelArgs.boot_splash
				= (uint8*)kernel_args_malloc(uncompressedSize);
			if (gKernelArgs.boot_splash == NULL)
				return B_NO_MEMORY;
			uncompress(kSplashIcons24BitCompressedImage,
				sizeof(kSplashIcons24BitCompressedImage),
				gKernelArgs.boot_splash, uncompressedSize);
			lowerHalfIconImage = (uint8 *)gKernelArgs.boot_splash
				+ (kSplashIconsWidth * iconsHalfHeight) * 3;
		break;
	}

	// render initial (grayed out) icons
	// the grayed out version is the lower half of the icons image
	compute_splash_icons_placement(gKernelArgs.frame_buffer.width, gKernelArgs.frame_buffer.height,
		width, height, x, y);

	video_blit_image(frameBuffer, lowerHalfIconImage, width, height,
		kSplashIconsWidth, x, y);
	return B_OK;
}

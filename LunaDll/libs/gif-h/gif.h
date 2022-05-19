//
// gif.h
// by Charlie Tangora
// Public domain.
// Email me : ctangora -at- gmail -dot- com
//
// This file offers a simple, very limited way to create animated GIFs directly in code.
//
// Those looking for particular cleverness are likely to be disappointed; it's pretty 
// much a straight-ahead implementation of the GIF format with optional Floyd-Steinberg 
// dithering. (It does at least use delta encoding - only the changed portions of each 
// frame are saved.) 
//
// So resulting files are often quite large. The hope is that it will be handy nonetheless
// as a quick and easily-integrated way for programs to spit out animations.
//
// Only RGBA8 is currently supported as an input format. (The alpha is ignored.)
//
// USAGE:
// Create a GifWriter struct. Pass it to GifBegin() to initialize and write the header.
// Pass subsequent frames to GifWriteFrame().
// Finally, call GifEnd() to close the file handle and free memory.
//

#ifndef gif_h
#define gif_h

#include <stdio.h>   // for FILE*
#include <string.h>  // for memcpy and bzero
#include <stdint.h>  // for integer typedefs
#include <vector>
#include <algorithm>

// Define these macros to hook into a custom memory allocator.
// TEMP_MALLOC and TEMP_FREE will only be called in stack fashion - frees in the reverse order of mallocs
// and any temp memory allocated by a function will be freed before it exits.
// MALLOC and FREE are used only by GifBegin and GifEnd respectively (to allocate a buffer the size of the image, which
// is used to find changed pixels for delta-encoding.)

#ifndef GIF_TEMP_MALLOC
#include <stdlib.h>
#define GIF_TEMP_MALLOC malloc
#endif

#ifndef GIF_TEMP_FREE
#include <stdlib.h>
#define GIF_TEMP_FREE free
#endif

#ifndef GIF_MALLOC
#include <stdlib.h>
#define GIF_MALLOC malloc
#endif

#ifndef GIF_FREE
#include <stdlib.h>
#define GIF_FREE free
#endif

#include "gif_writer.h"

namespace GIF_H
{

	static const int kGifTransIndex = 0;

	// max, min, and abs functions
	static int GifIMax(int l, int r) { return l>r ? l : r; }
	static int GifIMin(int l, int r) { return l<r ? l : r; }
	static int GifIAbs(int i) { return i<0 ? -i : i; }

	static const int colorDimScales[3] = { 14, 20, 17 };

	// walks the k-d tree to pick the palette entry for a desired color.
	// Takes as in/out parameters the current best color and its error -
	// only changes them if it finds a better color in its subtree.
	// this is the major hotspot in the code at the moment.
	static void GifGetClosestPaletteColor(GifPalette* pPal, int r, int g, int b, int& bestInd, int& bestDiff, int treeRoot = 1)
	{
		// base case, reached the bottom of the tree
		if (treeRoot > (1 << pPal->bitDepth) - 1)
		{
			int ind = treeRoot - (1 << pPal->bitDepth);
			if (ind == kGifTransIndex) return;

			// check whether this color is better than the current winner
			int r_err = r - ((int32_t)pPal->entries[ind].r);
			int g_err = g - ((int32_t)pPal->entries[ind].g);
			int b_err = b - ((int32_t)pPal->entries[ind].b);
			// RED: Increase green importance
			int diff = colorDimScales[0] * GifIAbs(r_err) + colorDimScales[1] * GifIAbs(g_err) + colorDimScales[2] * GifIAbs(b_err);

			if (diff < bestDiff)
			{
				bestInd = ind;
				bestDiff = diff;
			}

			return;
		}

		if (pPal->nodes[treeRoot].splitRange == 0)
		{
			// Degenerate case, no split
			GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2);
		}
		else
		{
			// take the appropriate color (r, g, or b) for this node of the k-d tree
			int comps[3]; comps[0] = r; comps[1] = g; comps[2] = b;
			int splitComp = comps[pPal->nodes[treeRoot].treeSplitElt];
			int dimScale = colorDimScales[pPal->nodes[treeRoot].treeSplitElt];

			int splitPos = pPal->nodes[treeRoot].treeSplit;

			if (splitPos >= splitComp)
			{
				// check the left subtree
				GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2);
				if (bestDiff > dimScale*(splitPos - splitComp))
				{
					// cannot prove there's not a better value in the right subtree, check that too
					GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2 + 1);
				}
			}
			else
			{
				GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2 + 1);
				if (bestDiff > dimScale*(splitComp - splitPos))
				{
					GifGetClosestPaletteColor(pPal, r, g, b, bestInd, bestDiff, treeRoot * 2);
				}
			}
		}
	}

	static void GifSwapPixels(uint8_t* image, int pixA, int pixB)
	{
		uint8_t rA = image[pixA * 4];
		uint8_t gA = image[pixA * 4 + 1];
		uint8_t bA = image[pixA * 4 + 2];
		uint8_t aA = image[pixA * 4 + 3];

		uint8_t rB = image[pixB * 4];
		uint8_t gB = image[pixB * 4 + 1];
		uint8_t bB = image[pixB * 4 + 2];
		uint8_t aB = image[pixA * 4 + 3];

		image[pixA * 4] = rB;
		image[pixA * 4 + 1] = gB;
		image[pixA * 4 + 2] = bB;
		image[pixA * 4 + 3] = aB;

		image[pixB * 4] = rA;
		image[pixB * 4 + 1] = gA;
		image[pixB * 4 + 2] = bA;
		image[pixB * 4 + 3] = aA;
	}

	// just the partition operation from quicksort
	static int GifPartition(uint8_t* image, const int left, const int right, const int elt, int pivotIndex)
	{
		const int pivotValue = image[(pivotIndex) * 4 + elt];
		GifSwapPixels(image, pivotIndex, right - 1);
		int storeIndex = left;
		bool split = 0;
		for (int ii = left; ii<right - 1; ++ii)
		{
			int arrayVal = image[ii * 4 + elt];
			if (arrayVal < pivotValue)
			{
				GifSwapPixels(image, ii, storeIndex);
				++storeIndex;
			}
			else if (arrayVal == pivotValue)
			{
				if (split)
				{
					GifSwapPixels(image, ii, storeIndex);
					++storeIndex;
				}
				split = !split;
			}
		}
		GifSwapPixels(image, storeIndex, right - 1);
		return storeIndex;
	}

	// Perform an incomplete sort, finding all elements above and below the desired median
	static void GifPartitionByMedian(uint8_t* image, int left, int right, int com, int neededCenter)
	{
		if (left < right - 1)
		{
			int pivotIndex = left + (right - left) / 2;

			pivotIndex = GifPartition(image, left, right, com, pivotIndex);

			// Only "sort" the section of the array that contains the median
			if (pivotIndex > neededCenter)
				GifPartitionByMedian(image, left, pivotIndex, com, neededCenter);

			if (pivotIndex < neededCenter)
				GifPartitionByMedian(image, pivotIndex + 1, right, com, neededCenter);
		}
	}

	// Builds a palette by creating a balanced k-d tree of all pixels in the image
	static void GifSplitPalette(uint8_t* image, int numPixels, int firstElt, int lastElt, int splitElt, int splitDist, int treeNode, bool buildForDither, GifPalette* pal)
	{
		if (lastElt <= firstElt || numPixels == 0)
			return;

		// base case, bottom of the tree
		if (lastElt == firstElt + 1)
		{
			if (buildForDither)
			{
				// Dithering needs at least one color as dark as anything
				// in the image and at least one brightest color -
				// otherwise it builds up error and produces strange artifacts
				if (firstElt == 1)
				{
					// special case: the darkest color in the image
					uint32_t r = 255, g = 255, b = 255;
					for (int ii = 0; ii<numPixels; ++ii)
					{
						r = GifIMin(r, image[ii * 4 + 0]);
						g = GifIMin(g, image[ii * 4 + 1]);
						b = GifIMin(b, image[ii * 4 + 2]);
					}

					pal->entries[firstElt].r = r;
					pal->entries[firstElt].g = g;
					pal->entries[firstElt].b = b;

					return;
				}

				if (firstElt == (1 << pal->bitDepth) - 1)
				{
					// special case: the lightest color in the image
					uint32_t r = 0, g = 0, b = 0;
					for (int ii = 0; ii<numPixels; ++ii)
					{
						r = GifIMax(r, image[ii * 4 + 0]);
						g = GifIMax(g, image[ii * 4 + 1]);
						b = GifIMax(b, image[ii * 4 + 2]);
					}

					pal->entries[firstElt].r = r;
					pal->entries[firstElt].g = g;
					pal->entries[firstElt].b = b;

					return;
				}
			}

			// otherwise, take the average of all colors in this subcube
			uint64_t r = 0, g = 0, b = 0;
			for (int ii = 0; ii<numPixels; ++ii)
			{
				r += image[ii * 4 + 0];
				g += image[ii * 4 + 1];
				b += image[ii * 4 + 2];
			}

			r += numPixels / 2;  // round to nearest
			g += numPixels / 2;
			b += numPixels / 2;

			r /= numPixels;
			g /= numPixels;
			b /= numPixels;

			pal->entries[firstElt].r = (uint8_t)r;
			pal->entries[firstElt].g = (uint8_t)g;
			pal->entries[firstElt].b = (uint8_t)b;

			return;
		}

		// Find the axis with the largest range
		int minR = 255, maxR = 0;
		int minG = 255, maxG = 0;
		int minB = 255, maxB = 0;
		for (int ii = 0; ii<numPixels; ++ii)
		{
			int r = image[ii * 4 + 0];
			int g = image[ii * 4 + 1];
			int b = image[ii * 4 + 2];

			if (r > maxR) maxR = r;
			if (r < minR) minR = r;

			if (g > maxG) maxG = g;
			if (g < minG) minG = g;

			if (b > maxB) maxB = b;
			if (b < minB) minB = b;
		}

		int ranges[3];
		ranges[0] = (maxR - minR) * colorDimScales[0];
		ranges[1] = (maxG - minG) * colorDimScales[1];
		ranges[2] = (maxB - minB) * colorDimScales[2];

		// and split along that axis. (incidentally, this means this isn't a "proper" k-d tree but I don't know what else to call it)
		int splitCom = 0;
		for (int i = 1; i < 3; i++)
		{
			if (ranges[i] > ranges[splitCom])
			{
				splitCom = i;
			}
		}

		int subPixelsA = numPixels * (splitElt - firstElt) / (lastElt - firstElt);
		int subPixelsB = numPixels - subPixelsA;

		if (ranges[splitCom] == 0)
		{
			// If it's all the same, stop averaging more than we need to
			subPixelsA = 1;
			subPixelsB = 0;
		}
		else
		{
			// Partition the pixels across the median
			GifPartitionByMedian(image, 0, numPixels, splitCom, subPixelsA);

			// Shift ties to the left when it's trivial
			uint8_t splitValue = image[subPixelsA * 4 + splitCom];
			for (; subPixelsA < numPixels; subPixelsB--, subPixelsA++)
			{
				if (splitValue != image[subPixelsA * 4 + splitCom])
				{
					break;
				}
			}
		}

		pal->nodes[treeNode].treeSplitElt = splitCom;
		pal->nodes[treeNode].treeSplit = image[subPixelsA * 4 + splitCom];
		pal->nodes[treeNode].splitRange = ranges[splitCom];

		GifSplitPalette(image, subPixelsA, firstElt, splitElt, splitElt - splitDist, splitDist / 2, treeNode * 2, buildForDither, pal);
		GifSplitPalette(image + subPixelsA * 4, subPixelsB, splitElt, lastElt, splitElt + splitDist, splitDist / 2, treeNode * 2 + 1, buildForDither, pal);
	}

	// Finds all pixels that have changed from the previous image and
	// makes a new buffer with them at the front.
	// This allows us to build a palette optimized for the colors of the
	// changed pixels only.
	static int GifPickChangedPixels(const uint8_t* lastFrame, uint8_t* out, const uint8_t* nextFrame, int numPixels)
	{
		int numChanged = 0;
		uint8_t* writeIter = out;

		for (int ii = 0; ii<numPixels; ++ii)
		{
			if (lastFrame[0] != nextFrame[0] ||
				lastFrame[1] != nextFrame[1] ||
				lastFrame[2] != nextFrame[2])
			{
				writeIter[0] = nextFrame[0];
				writeIter[1] = nextFrame[1];
				writeIter[2] = nextFrame[2];
				++numChanged;
				writeIter += 4;
			}
			lastFrame += 4;
			nextFrame += 4;
		}

		return numChanged;
	}

	struct DistanceComparableRGB
	{
		uint16_t dist;
		uint8_t r;
		uint8_t g;
		uint8_t b;

		DistanceComparableRGB(int _dist, uint8_t _r, uint8_t _g, uint8_t _b) :
			dist(_dist), r(_r), g(_g), b(_b)
		{
		}

		bool operator <(const DistanceComparableRGB& o)
		{
			return dist < o.dist;
		}
	};

	// Creates a palette by placing all the image pixels in a k-d tree and then averaging the blocks at the bottom.
	// This is known as the "modified median split" technique
	static void GifMakePalette(const GifWriter* writer, const uint8_t* lastFrame, const uint8_t* nextFrame, uint32_t width, uint32_t height, int bitDepth, bool buildForDither, GifPalette* pPal)
	{

		// SplitPalette is destructive (it sorts the pixels by color) so
		// we must create a copy of the image for it to destroy
		int imageSize = width*height * 4 * sizeof(uint8_t);
		uint8_t* destroyableImage = writer->tempImage;

		int numPixels = width*height;
		if (lastFrame)
		{
			// Get best 1024 different-from-last colors to pass on to tree-generation
			std::vector<DistanceComparableRGB> queue;
			queue.reserve(numPixels);
			for (int i = 0; i < numPixels; i++)
			{
				uint8_t r = nextFrame[i * 4 + 0];
				uint8_t g = nextFrame[i * 4 + 1];
				uint8_t b = nextFrame[i * 4 + 2];
				uint8_t or = lastFrame[i * 4 + 0];
				uint8_t og = lastFrame[i * 4 + 1];
				uint8_t ob = lastFrame[i * 4 + 2];
				uint16_t dist = (
					colorDimScales[0] * GifIAbs(r - or) +
					colorDimScales[1] * GifIAbs(g - og) +
					colorDimScales[2] * GifIAbs(b - ob));
				if (dist > 0)
				{
					// Check if the exact color was in the last palette? If so amplify distance
					// This encourages consistent colors
					int bestDiff = 1;
					int bestInd = kGifTransIndex;
					GifGetClosestPaletteColor(pPal, r, g, b, bestInd/*modified*/, bestDiff/*modified*/);
					if (bestDiff == 0)
					{
						dist *= 4;
						dist += 1000;
					}

					queue.emplace_back(dist, r, g, b);
				}
			}
			std::make_heap(queue.begin(), queue.end());
			uint8_t used[32][32][32] = {0};
			//memset(used, 0, 32 * 32 * 32);
			numPixels = 0;
			for (numPixels = 0; numPixels < 1024;)
			{
				DistanceComparableRGB& elem = queue.front();
				if (used[elem.r/8][elem.g/8][elem.b/8] < 4)
				{
					destroyableImage[numPixels * 4 + 0] = elem.r;
					destroyableImage[numPixels * 4 + 1] = elem.g;
					destroyableImage[numPixels * 4 + 2] = elem.b;
					used[elem.r/8][elem.g/8][elem.b/8]++;
					numPixels++;
				}
				std::pop_heap(queue.begin(), queue.end()); queue.pop_back();
			}

			//numPixels = GifPickChangedPixels(lastFrame, destroyableImage, nextFrame, numPixels);
		}
		else
		{
			memcpy(destroyableImage, nextFrame, imageSize);
		}

		const int lastElt = 1 << bitDepth;
		const int splitElt = lastElt / 2;
		const int splitDist = splitElt / 2;
		pPal->bitDepth = bitDepth;
		GifSplitPalette(destroyableImage, numPixels, 1, lastElt, splitElt, splitDist, 1, buildForDither, pPal);

		// now done with destroyableImage

		// add the bottom node for the transparency index
		pPal->nodes[1 << (bitDepth - 1)].treeSplit = 0;
		pPal->nodes[1 << (bitDepth - 1)].treeSplitElt = 0;
		pPal->nodes[1 << (bitDepth - 1)].splitRange = 0;
		pPal->entries[0].r = 0;
		pPal->entries[0].g = 0;
		pPal->entries[0].b = 0;
	}

	// Implements Floyd-Steinberg dithering, writes palette value to alpha
	static void GifDitherImage(const uint8_t* lastFrame, const uint8_t* nextFrame, uint8_t* outFrame, uint32_t width, uint32_t height, GifPalette* pPal)
	{
		int numPixels = width*height;

		// quantPixels initially holds color*256 for all pixels
		// The extra 8 bits of precision allow for sub-single-color error values
		// to be propagated
		int32_t* quantPixels = (int32_t*)GIF_TEMP_MALLOC(sizeof(int32_t)*numPixels * 4);

		for (int ii = 0; ii<numPixels * 4; ++ii)
		{
			uint8_t pix = nextFrame[ii];
			int32_t pix16 = int32_t(pix) * 256;
			quantPixels[ii] = pix16;
		}

		for (uint32_t yy = 0; yy<height; ++yy)
		{
			for (uint32_t xx = 0; xx<width; ++xx)
			{
				int32_t* nextPix = quantPixels + 4 * (yy*width + xx);
				const uint8_t* lastPix = lastFrame ? lastFrame + 4 * (yy*width + xx) : NULL;

				// Compute the colors we want (rounding to nearest)
				int32_t rr = (nextPix[0] + 127) / 256;
				int32_t gg = (nextPix[1] + 127) / 256;
				int32_t bb = (nextPix[2] + 127) / 256;

				// if it happens that we want the color from last frame, then just write out
				// a transparent pixel
				if (lastFrame &&
					lastPix[0] == rr &&
					lastPix[1] == gg &&
					lastPix[2] == bb)
				{
					nextPix[0] = rr;
					nextPix[1] = gg;
					nextPix[2] = bb;
					nextPix[3] = kGifTransIndex;
					continue;
				}

				int32_t bestDiff = 1000000;
				int32_t bestInd = kGifTransIndex;

				// Search the palete
				GifGetClosestPaletteColor(pPal, rr, gg, bb, bestInd, bestDiff);

				// Write the result to the temp buffer
				int32_t r_err = nextPix[0] - int32_t(pPal->entries[bestInd].r) * 256;
				int32_t g_err = nextPix[1] - int32_t(pPal->entries[bestInd].g) * 256;
				int32_t b_err = nextPix[2] - int32_t(pPal->entries[bestInd].b) * 256;

				nextPix[0] = pPal->entries[bestInd].r;
				nextPix[1] = pPal->entries[bestInd].g;
				nextPix[2] = pPal->entries[bestInd].b;
				nextPix[3] = bestInd;

				// Propagate the error to the four adjacent locations
				// that we haven't touched yet
				int quantloc_7 = (yy*width + xx + 1);
				int quantloc_3 = (yy*width + width + xx - 1);
				int quantloc_5 = (yy*width + width + xx);
				int quantloc_1 = (yy*width + width + xx + 1);

				if (quantloc_7 < numPixels)
				{
					int32_t* pix7 = quantPixels + 4 * quantloc_7;
					pix7[0] += GifIMax(-pix7[0], r_err * 7 / 16);
					pix7[1] += GifIMax(-pix7[1], g_err * 7 / 16);
					pix7[2] += GifIMax(-pix7[2], b_err * 7 / 16);
				}

				if (quantloc_3 < numPixels)
				{
					int32_t* pix3 = quantPixels + 4 * quantloc_3;
					pix3[0] += GifIMax(-pix3[0], r_err * 3 / 16);
					pix3[1] += GifIMax(-pix3[1], g_err * 3 / 16);
					pix3[2] += GifIMax(-pix3[2], b_err * 3 / 16);
				}

				if (quantloc_5 < numPixels)
				{
					int32_t* pix5 = quantPixels + 4 * quantloc_5;
					pix5[0] += GifIMax(-pix5[0], r_err * 5 / 16);
					pix5[1] += GifIMax(-pix5[1], g_err * 5 / 16);
					pix5[2] += GifIMax(-pix5[2], b_err * 5 / 16);
				}

				if (quantloc_1 < numPixels)
				{
					int32_t* pix1 = quantPixels + 4 * quantloc_1;
					pix1[0] += GifIMax(-pix1[0], r_err / 16);
					pix1[1] += GifIMax(-pix1[1], g_err / 16);
					pix1[2] += GifIMax(-pix1[2], b_err / 16);
				}
			}
		}

		// Copy the palettized result to the output buffer
		for (int ii = 0; ii<numPixels * 4; ++ii)
		{
			outFrame[ii] = quantPixels[ii];
		}

		GIF_TEMP_FREE(quantPixels);
	}

	// Picks palette colors for the image using simple thresholding, no dithering
	static void GifThresholdImage(const uint8_t* lastFrame, const uint8_t* nextFrame, uint8_t* outFrame, uint32_t width, uint32_t height, GifPalette* pPal)
	{
		uint32_t numPixels = width*height;
		for (uint32_t ii = 0; ii<numPixels; ++ii)
		{
			// if a previous color is available, and it matches the current color,
			// set the pixel to transparent
			if (lastFrame &&
				lastFrame[0] == nextFrame[0] &&
				lastFrame[1] == nextFrame[1] &&
				lastFrame[2] == nextFrame[2])
			{
				outFrame[0] = lastFrame[0];
				outFrame[1] = lastFrame[1];
				outFrame[2] = lastFrame[2];
				outFrame[3] = kGifTransIndex;
			}
			else
			{
				// palettize the pixel
				int32_t bestDiff = 1000000;
				int32_t bestInd = 1;
				GifGetClosestPaletteColor(pPal, nextFrame[0], nextFrame[1], nextFrame[2], bestInd, bestDiff);

				bool usedOld = false;
				if (lastFrame)
				{
					// RED: If the chosen one is worse than the old one, don't go with it
					int r_err = (int)lastFrame[0] - (int)nextFrame[0];
					int g_err = (int)lastFrame[1] - (int)nextFrame[1];
					int b_err = (int)lastFrame[2] - (int)nextFrame[2];
					int oldDiff = colorDimScales[0] * GifIAbs(r_err) + colorDimScales[1] * GifIAbs(g_err) + colorDimScales[2] * GifIAbs(b_err);
					if (oldDiff <= bestDiff)
					{
						outFrame[0] = lastFrame[0];
						outFrame[1] = lastFrame[1];
						outFrame[2] = lastFrame[2];
						outFrame[3] = kGifTransIndex;
						usedOld = true;
					}
				}

				if (!usedOld)
				{
					// Write the resulting color to the output buffer
					outFrame[0] = pPal->entries[bestInd].r;
					outFrame[1] = pPal->entries[bestInd].g;
					outFrame[2] = pPal->entries[bestInd].b;
					outFrame[3] = bestInd;
				}
			}

			if (lastFrame) lastFrame += 4;
			outFrame += 4;
			nextFrame += 4;
		}
	}

	// Simple structure to write out the LZW-compressed portion of the image
	// one bit at a time
	struct GifBitStatus
	{
		uint8_t bitIndex;  // how many bits in the partial byte written so far
		uint8_t byte;      // current partial byte

		uint32_t chunkIndex;
		uint8_t chunk[256];   // bytes are written in here until we have 256 of them, then written to the file
	};

	// insert a single bit
	static void GifWriteBit(GifBitStatus& stat, uint32_t bit)
	{
		bit = bit & 1;
		bit = bit << stat.bitIndex;
		stat.byte |= bit;

		++stat.bitIndex;
		if (stat.bitIndex > 7)
		{
			// move the newly-finished byte to the chunk buffer 
			stat.chunk[stat.chunkIndex++] = stat.byte;
			// and start a new byte
			stat.bitIndex = 0;
			stat.byte = 0;
		}
	}

	// write all bytes so far to the file
	static void GifWriteChunk(FILE* f, GifBitStatus& stat)
	{
		fputc(stat.chunkIndex, f);
		fwrite(stat.chunk, 1, stat.chunkIndex, f);

		stat.bitIndex = 0;
		stat.byte = 0;
		stat.chunkIndex = 0;
	}

	static void GifWriteCode(FILE* f, GifBitStatus& stat, uint32_t code, uint32_t length)
	{
		for (uint32_t ii = 0; ii<length; ++ii)
		{
			GifWriteBit(stat, code);
			code = code >> 1;

			if (stat.chunkIndex == 255)
			{
				GifWriteChunk(f, stat);
			}
		}
	}

	// The LZW dictionary is a 256-ary tree constructed as the file is encoded,
	// this is one node
	struct GifLzwNode
	{

		uint16_t m_next[256];
	};

	// Code tree length to allocate
	static const int kGifLzwNodeCount = 4096;

	// write a 256-color (8-bit) image palette to the file
	static void GifWritePalette(const GifPalette* pPal, FILE* f)
	{
		fputc(0, f);  // first color: transparency
		fputc(0, f);
		fputc(0, f);

		for (int ii = 1; ii<(1 << pPal->bitDepth); ++ii)
		{
			uint32_t r = pPal->entries[ii].r;
			uint32_t g = pPal->entries[ii].g;
			uint32_t b = pPal->entries[ii].b;

			fputc(r, f);
			fputc(g, f);
			fputc(b, f);
		}
	}

	// write the image header, LZW-compress and write out the image
	static void GifWriteLzwImage(GifWriter* writer, uint32_t left, uint32_t top, uint32_t width, uint32_t height, uint32_t delay, GifPalette* pPal)
	{
		FILE * const          f     = writer->f;
		const uint8_t * const image = writer->oldImage; // Note: not actually old currently, just the image with palette applied

		// graphics control extension
		fputc(0x21, f);
		fputc(0xf9, f);
		fputc(0x04, f);
		fputc(0x05, f); // leave prev frame in place, this frame has transparency
		writer->delaypos = ftell(f);
		fputc(delay & 0xff, f);
		fputc((delay >> 8) & 0xff, f);
		fputc(kGifTransIndex, f); // transparent color index
		fputc(0, f);

		fputc(0x2c, f); // image descriptor block

		fputc(left & 0xff, f);           // corner of image in canvas space
		fputc((left >> 8) & 0xff, f);
		fputc(top & 0xff, f);
		fputc((top >> 8) & 0xff, f);

		fputc(width & 0xff, f);          // width and height of image
		fputc((width >> 8) & 0xff, f);
		fputc(height & 0xff, f);
		fputc((height >> 8) & 0xff, f);

		//fputc(0, f); // no local color table, no transparency
		//fputc(0x80, f); // no local color table, but transparency

		fputc(0x80 + pPal->bitDepth - 1, f); // local color table present, 2 ^ bitDepth entries
		GifWritePalette(pPal, f);

		const int minCodeSize = pPal->bitDepth;
		const uint32_t clearCode = 1 << pPal->bitDepth;

		fputc(minCodeSize, f); // min code size 8 bits

		GifLzwNode* codetree = writer->tempCodetree;

		memset(codetree, 0, sizeof(GifLzwNode) * kGifLzwNodeCount);
		int32_t curCode = -1;
		uint32_t codeSize = minCodeSize + 1;
		uint32_t maxCode = clearCode + 1;

		GifBitStatus stat;
		stat.byte = 0;
		stat.bitIndex = 0;
		stat.chunkIndex = 0;

		GifWriteCode(f, stat, clearCode, codeSize);  // start with a fresh LZW dictionary

		for (uint32_t yy = 0; yy<height; ++yy)
		{
			for (uint32_t xx = 0; xx<width; ++xx)
			{
				uint8_t nextValue = image[(yy*width + xx) * 4 + 3];

				// "loser mode" - no compression, every single code is followed immediately by a clear
				//WriteCode( f, stat, nextValue, codeSize );
				//WriteCode( f, stat, 256, codeSize );

				if (curCode < 0)
				{
					// first value in a new run
					curCode = nextValue;
				}
				else if (codetree[curCode].m_next[nextValue])
				{
					// current run already in the dictionary
					curCode = codetree[curCode].m_next[nextValue];
				}
				else
				{
					// finish the current run, write a code
					GifWriteCode(f, stat, curCode, codeSize);

					// insert the new run into the dictionary
					codetree[curCode].m_next[nextValue] = ++maxCode;

					if (maxCode >= (1ul << codeSize))
					{
						// dictionary entry count has broken a size barrier,
						// we need more bits for codes
						codeSize++;
					}
					if (maxCode == (kGifLzwNodeCount-1))
					{
						// the dictionary is full, clear it out and begin anew
						GifWriteCode(f, stat, clearCode, codeSize); // clear tree

						memset(codetree, 0, sizeof(GifLzwNode) * kGifLzwNodeCount);
						curCode = -1;
						codeSize = minCodeSize + 1;
						maxCode = clearCode + 1;
					}

					curCode = nextValue;
				}
			}
		}

		// compression footer
		GifWriteCode(f, stat, curCode, codeSize);
		GifWriteCode(f, stat, clearCode, codeSize);
		GifWriteCode(f, stat, clearCode + 1, minCodeSize + 1);

		// write out the last partial chunk
		while (stat.bitIndex) GifWriteBit(stat, 0);
		if (stat.chunkIndex) GifWriteChunk(f, stat);

		fputc(0, f); // image block terminator
	}


	// GifWriter moved into the own header

	// Creates a gif file.
	// The input GIFWriter is assumed to be uninitialized.
	// The delay value is the time between frames in hundredths of a second - note that not all viewers pay much attention to this value.
	static bool GifBegin(GifWriter* writer, FILE *file, uint32_t width, uint32_t height, uint32_t delay, int32_t bitDepth = 8, bool dither = false)
	{
		(void)bitDepth;
		(void)dither;
		writer->f = file;
		if (!writer->f) return false;

		writer->height = height;
		writer->width = width;
		writer->firstFrame = true;
		writer->delaypos = -1;

		// allocate 
		writer->oldImage = (uint8_t*)GIF_MALLOC(width*height * 4);
		writer->tempImage = (uint8_t*)GIF_MALLOC(width*height * 4);
		writer->tempCodetree = (GifLzwNode*)GIF_MALLOC(sizeof(GifLzwNode) * kGifLzwNodeCount);

		fputs("GIF89a", writer->f);

		// screen descriptor
		fputc(width & 0xff, writer->f);
		fputc((width >> 8) & 0xff, writer->f);
		fputc(height & 0xff, writer->f);
		fputc((height >> 8) & 0xff, writer->f);

		fputc(0xf0, writer->f);  // there is an unsorted global color table of 2 entries
		fputc(0, writer->f);     // background color
		fputc(0, writer->f);     // pixels are square (we need to specify this because it's 1989)

								 // now the "global" palette (really just a dummy palette)
								 // color 0: black
		fputc(0, writer->f);
		fputc(0, writer->f);
		fputc(0, writer->f);
		// color 1: also black
		fputc(0, writer->f);
		fputc(0, writer->f);
		fputc(0, writer->f);

		if (delay != 0)
		{
			// animation header
			fputc(0x21, writer->f); // extension
			fputc(0xff, writer->f); // application specific
			fputc(11, writer->f); // length 11
			fputs("NETSCAPE2.0", writer->f); // yes, really
			fputc(3, writer->f); // 3 bytes of NETSCAPE2.0 data

			fputc(1, writer->f); // JUST BECAUSE
			fputc(0, writer->f); // loop infinitely (byte 0)
			fputc(0, writer->f); // loop infinitely (byte 1)

			fputc(0, writer->f); // block terminator
		}

		return true;
	}

	// Writes out a new frame to a GIF in progress.
	// The GIFWriter should have been created by GIFBegin.
	// AFAIK, it is legal to use different bit depths for different frames of an image -
	// this may be handy to save bits in animations that don't change much.
	static bool GifWriteFrame(GifWriter* writer, const uint8_t* image, uint32_t width, uint32_t height, uint32_t delay, int bitDepth = 8, bool dither = false)
	{
		if (!writer->f) return false;

		// If size differs, reallocate and don't use transparency? This would make a weird gif but let's not crash.
		if ((width != writer->width) || (height != writer->height))
		{
			writer->firstFrame = true;
			writer->width = width;
			writer->height = height;
			GIF_FREE(writer->oldImage);
			writer->oldImage = (uint8_t*)GIF_MALLOC(width*height * 4);
			GIF_FREE(writer->tempImage);
			writer->tempImage = (uint8_t*)GIF_MALLOC(width*height * 4);
		}

		const uint8_t* oldImage = writer->firstFrame ? NULL : writer->oldImage;
		writer->firstFrame = false;

		GifMakePalette(writer, (dither ? NULL : oldImage), image, width, height, bitDepth, dither, &writer->pal);

		if (dither)
			GifDitherImage(oldImage, image, writer->oldImage, width, height, &writer->pal);
		else
			GifThresholdImage(oldImage, image, writer->oldImage, width, height, &writer->pal);

		// NOTE: For the following call, writer->oldImage is no longer 'old', it now refers to the image after applying the palette
		GifWriteLzwImage(writer, 0, 0, width, height, delay, &writer->pal);

		return true;
	}

	static void GifOverwriteLastDelay(GifWriter* writer, uint32_t delay)
	{
		if (writer->delaypos == -1) return;
		FILE* f = writer->f;

		long int pos = ftell(f);
		fseek(f, writer->delaypos, SEEK_SET);
		fputc(delay & 0xff, f);
		fputc((delay >> 8) & 0xff, f);
		fseek(f, pos, SEEK_SET);
	}

	// Writes the EOF code, closes the file handle, and frees temp memory used by a GIF.
	// Many if not most viewers will still display a GIF properly if the EOF code is missing,
	// but it's still a good idea to write it out.
	static bool GifEnd(GifWriter* writer)
	{
		if (!writer->f) return false;

		fputc(0x3b, writer->f); // end of file
		fclose(writer->f);
		GIF_FREE(writer->oldImage);
		GIF_FREE(writer->tempImage);
		GIF_FREE(writer->tempCodetree);

		writer->f = NULL;
		writer->oldImage = NULL;
		writer->delaypos = -1;

		return true;
	}

} // namespace

#endif

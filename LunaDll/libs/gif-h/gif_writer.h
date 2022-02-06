#ifndef GIF_WRITER_HHHH
#define GIF_WRITER_HHHH

#include <stdio.h>   // for FILE*
#include <stdint.h>  // for integer typedefs

namespace GIF_H
{

	struct GifLzwNode;

	struct GifPalette
	{
		// k-d tree over RGB space, organized in heap fashion
		// i.e. left child of node i is node i*2, right child is node i*2+1
		// nodes 256-511 are implicitly the leaves, containing a color
		struct Node
		{
			uint8_t treeSplitElt;
			uint8_t treeSplit;
			int     splitRange;
		};
		Node nodes[256];

		struct Entry
		{
			uint8_t r;
			uint8_t g;
			uint8_t b;
		};
		Entry entries[256];

		int bitDepth;
	};

	struct GifWriter
	{
		int height;
		int width;
		FILE* f;
		uint8_t* oldImage;
		uint8_t* tempImage;
		GifLzwNode* tempCodetree;
		GifPalette pal;
		bool firstFrame;
		long int delaypos;
	};


}

#endif // GIF_WRITER_HHHH

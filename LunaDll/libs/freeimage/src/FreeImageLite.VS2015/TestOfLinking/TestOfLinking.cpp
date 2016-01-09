// TestOfLinking.cpp: определяет точку входа для консольного приложения.
//

#include <stdio.h>
#define FREEIMAGE_LIB
#include <FreeImageLite.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
	FreeImage_Initialise();

	FREE_IMAGE_FORMAT fm = FreeImage_GetFileType("test.bmp");
	printf("format is: %i\n", fm);
	FIBITMAP*meow = FreeImage_Load(fm, "test.bmp");
	if (!meow) {
		printf("image not opened!\n");
	}else {
		printf("image opened!\n");

		if(FreeImage_Save(FIF_PNG, meow, "out_test.png"))
			printf("saved PNG!\n");

		FIBITMAP* meow8bit = FreeImage_ConvertTo8Bits(meow);
		if (FreeImage_Save(FIF_GIF, meow8bit, "out_test.gif"))
			printf("saved GIF!\n");

		FreeImage_Unload(meow);
		FreeImage_Unload(meow8bit);
	}

	//system("pause");
    return 0;
}



#define STB_IMAGE_IMPLEMENTATION

#include "PixelScene.h"
#include "PixelRenderer.h"

int main()
{

	PixelRenderer pixRenderer;

	if (pixRenderer.initRenderer() == EXIT_FAILURE)
	{
		return EXIT_FAILURE;
	}

    pixRenderer.run();


	pixRenderer.cleanup();

	return 0;
}
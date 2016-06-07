#include "bmp.h"

#define kernSize 9
#define kernLen	4
#define numInput 3
#define numFilter 2

int filters[numFilter][kernSize][kernSize] = {
   8,  18,  34,  50,  56,  50,  34,  18,  8,
  18,  44,  82, 119, 135, 119,  82,  44, 18,
  34,  82, 153, 223, 253, 223, 153,  82, 34,
  50, 119, 223, 325, 368, 325, 223, 119, 50,
  56, 135, 253, 368, 417, 368, 253, 135, 56,
  50, 119, 223, 325, 368, 325, 223, 119, 50,
  34,  82, 153, 223, 253, 223, 153,  82, 34,
  18,  44,  82, 119, 135, 119,  82,  44, 18,
   8,  18,  34,  50,  56,  50,  34,  18,  8,

	 0,   1,   1,   2,   2,   2,   1,   1,  0,
	 1,   2,   4,   5,   5,   5,   4,   2,  1,
	 1,   4,   5,   3,   0,   3,   5,   4,  1,
	 2,   5,   3, -20, -36, -20,   3,   5,  2,
	 2,   5,   0, -36, -88, -36,   0,   5,  2,
	 2,   5,   3, -20, -36, -20,   3,   5,  2,
	 1,   4,   5,   3,   0,   3,   5,   4,  1,
	 1,   2,   4,   5,   5,   5,   4,   2,  1,
	 0,   1,   1,   2,   2,   2,   1,   1,  0
};

char *inputNames[numInput] = {
	"Images/lake.bmp",
	"Images/maple.bmp",
	"Images/tree.bmp"
};

char *outputNames[numFilter][numInput] = {
	"Output/blurLake.bmp",
	"Output/blurMaple.bmp",
	"Output/blurTree.bmp",
	"Output/sharpLake.bmp",
	"Output/sharpMaple.bmp",
	"Output/sharpTree.bmp"
};

BMP bmpFiles[numInput];
BYTE *data[numInput];
BYTE *results[numFilter][numInput];

int imgHeight, imgWidth, imgSize;

void setImageInfo()
{
	imgHeight = bmpFiles[0].height;
	imgWidth = bmpFiles[0].width;
	imgSize = imgHeight * imgWidth * 3;
}

void loadImages()
{
	int i;
	
	for(i = 0; i < numInput; i++){
		bmpLoad(&bmpFiles[i], inputNames[i]);
	}
}

void saveImages()
{
	BMP bmpTemp;
	int i, j;

	bmpTemp = bmpFiles[0];

	for(i = 0; i < numFilter; i++){
		for(j = 0; j < numInput; j++){
			bmpTemp.data = results[i][j];	
			bmpSave(&bmpTemp, outputNames[i][j]);
		}
	}
}

void initData()
{
	int i;

	for(i = 0; i < numInput; i++){
		data[i] = malloc(imgSize * sizeof(BYTE));
		memcpy(data[i], bmpFiles[i].data, imgSize * sizeof(BYTE));
	}
}

void initResults()
{
	int i, j;

	for(i = 0; i < numFilter; i++)
		for(j = 0; j < numInput; j++)
			results[i][j] = malloc(imgSize * sizeof(BYTE));
}

void freeMem()
{
	int i, j;

	for(i = 0; i < numInput; i++)
		free(data[i]);

	for(i = 0; i < numFilter; i++)
		for(j = 0; j < numInput; j++)
			free(results[i][j]);
}

void checkPixelValue(int *arr, int weight)
{
	if(weight != 0){
		arr[0] = arr[0] / weight;
		arr[1] = arr[1] / weight;
		arr[2] = arr[2] / weight;
	}
		
	if(arr[0] < 0)
		arr[0] = 0;
	else if(arr[0] > 255)
		arr[0] = 255;
	if(arr[1] < 0)
		arr[1] = 0;
	else if(arr[1] > 255)
		arr[1] = 255;
	if(arr[2] < 0)
		arr[2] = 0;
	else if(arr[2] > 255)
		arr[2] = 255;

}

void setPixel(BYTE *data, int idx, int *rgb)
{
	Pixel *pixelPtr;

	pixelPtr = (Pixel *) &data[idx*sizeof(Pixel)];
	pixelPtr->R = rgb[0]; 
	pixelPtr->G = rgb[1]; 
	pixelPtr->B = rgb[2];
}

void cross2DConv()
{
	int i, j, k, l;
	int filIdx, inIdx;
	int weight;
	int rgb[3], idx[3];
	int filterPix;
	int idy;
	Pixel *pixelPtr;

	for(filIdx = 0; filIdx < numFilter; filIdx++){
		for(inIdx = 0; inIdx < numInput; inIdx++){
			for(i = 0; i < imgWidth; i++){
				for(j = 0; j < imgHeight; j++){
					rgb[0] = 0; rgb[1] = 0; rgb[2] = 0;
					weight = filIdx ? -128 : 9993;
					idx[0] = j*imgWidth + i - j;
					for(k = 0; k < kernSize; k++){
						idx[2] = idx[0]/imgWidth - kernLen + k;
						idx[1] = idx[0]%imgWidth - kernLen;
						idy = idx[2]*imgWidth + idx[1];
						for(l = 0; l < kernSize; l=l+3){
							filterPix=filters[filIdx][k][l];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][idy
																									* sizeof(Pixel)];
								rgb[0] += filterPix * pixelPtr->R;
								rgb[1] += filterPix * pixelPtr->G;
								rgb[2] += filterPix * pixelPtr->B;
							}                               
							else                            
								weight -= filterPix;
							idy++;
							idx[1]++;
							filterPix=filters[filIdx][k][l+1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][idy
																									* sizeof(Pixel)];
								rgb[0] += filterPix * pixelPtr->R;
								rgb[1] += filterPix * pixelPtr->G;
								rgb[2] += filterPix * pixelPtr->B;
							}                               
							else                            
								weight -= filterPix;
							idy++;
							idx[1]++;
							filterPix=filters[filIdx][k][l+2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][idy
																									* sizeof(Pixel)];
								rgb[0] += filterPix * pixelPtr->R;
								rgb[1] += filterPix * pixelPtr->G;
								rgb[2] += filterPix * pixelPtr->B;
							}                               
							else                            
								weight -= filterPix;
							idy++;
							idx[1]++;
						}
					}		
					checkPixelValue(rgb, weight);
					setPixel(results[filIdx][inIdx], idx[0], rgb);
				}
			}
		}
	}

}

int main() {
	loadImages();
	setImageInfo();
	initData();
	initResults();
	cross2DConv();
	saveImages();
	freeMem();

  return 0;
}

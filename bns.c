#include "bmp.h"

#define kernSize 9
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

int weights[numFilter] = {9993, -128};

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
	bmpLoad(&bmpFiles[0], inputNames[0]);
	bmpLoad(&bmpFiles[1], inputNames[1]);
	bmpLoad(&bmpFiles[2], inputNames[2]);

}

void saveImages()
{
	BMP bmpTemp;
	bmpTemp = bmpFiles[0];
	bmpTemp.data = results[0][0];	
	bmpSave(&bmpTemp, outputNames[0][0]);
	bmpTemp.data = results[0][1];	
	bmpSave(&bmpTemp, outputNames[0][1]);
	bmpTemp.data = results[0][2];	
	bmpSave(&bmpTemp, outputNames[0][2]);
	bmpTemp.data = results[1][0];	
	bmpSave(&bmpTemp, outputNames[1][0]);
	bmpTemp.data = results[1][1];	
	bmpSave(&bmpTemp, outputNames[1][1]);
	bmpTemp.data = results[1][2];	
	bmpSave(&bmpTemp, outputNames[1][2]);
}

void initData()
{
	data[0] = malloc(imgSize * sizeof(BYTE));
	memcpy(data[0], bmpFiles[0].data, imgSize * sizeof(BYTE));
	data[1] = malloc(imgSize * sizeof(BYTE));
	memcpy(data[1], bmpFiles[1].data, imgSize * sizeof(BYTE));
	data[2] = malloc(imgSize * sizeof(BYTE));
	memcpy(data[2], bmpFiles[2].data, imgSize * sizeof(BYTE));

}

void initResults()
{
	results[0][0] = malloc(imgSize * sizeof(BYTE));
	results[0][1] = malloc(imgSize * sizeof(BYTE));
	results[0][2] = malloc(imgSize * sizeof(BYTE));
	results[1][0] = malloc(imgSize * sizeof(BYTE));
	results[1][1] = malloc(imgSize * sizeof(BYTE));
	results[1][2] = malloc(imgSize * sizeof(BYTE));
}

void freeMem()
{
	free(data[0]);
	free(data[1]);
	free(data[2]);
	free(results[0][0]);
	free(results[0][1]);
	free(results[0][2]);
	free(results[1][0]);
	free(results[1][1]);
	free(results[1][2]);
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

int getIdx(int y, int x)
{
	return y*imgWidth + x;
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
	int i, j, k;
	int filIdx, inIdx;
	int weight, kernLen;
	int rgb[3], idx[3];
	int filterPix;
	Pixel *pixelPtr;

	kernLen = kernSize/2;
	for(inIdx = 0; inIdx < numInput; inIdx++){
	for(filIdx = 0; filIdx < numFilter; filIdx++){
	for(i = 0; i < imgWidth; i++){
		for(j = 0; j < imgHeight; j++){
			
					rgb[0] = 0; rgb[1] = 0; rgb[2] = 0;
					weight = weights[filIdx];
					//for(k = 0; k < kernSize; k++){
					idx[0] = getIdx(j, i - j);
							k=0;
							
							idx[2] = idx[0]/imgWidth - kernLen ;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=1;

							
							idx[2]++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=2;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=3;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=4;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=5;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=6;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen ;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=7;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
							k=8;

							
							idx[2] ++;

							idx[1] = idx[0]%imgWidth - kernLen;
							filterPix=filters[filIdx][k][0];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][1];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][2];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][3];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][4];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][5];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][6];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][7];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;

							idx[1]++;
							filterPix=filters[filIdx][k][8];
							if((idx[1] >= 0) && (idx[2] >= 0) && 
								 (idx[1] < imgWidth) && (idx[2] < imgHeight)){
								pixelPtr = (Pixel *) &data[inIdx][getIdx(idx[2], idx[1])* sizeof(Pixel)];
								
								rgb[0] += filterPix * (pixelPtr->R - 0);
								rgb[1] += filterPix * (pixelPtr->G - 0);
								rgb[2] += filterPix * (pixelPtr->B - 0);
							}                               
							else                            
								weight -= filterPix;
						
							
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

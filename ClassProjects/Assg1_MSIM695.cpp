// Assg1_MSIM695_SeanLeo.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <math.h> //for mathmatical operations such as sqrt
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>

//Load Lorenz volume flow data from "LorenzVolumeFlow.vec"
float * LoadVolumeVectorData
		( char * fileName, int * vol_xRes,
		  int  * vol_yRes, int * vol_zRes)
{
	//*Open file
	FILE *fn = fopen(fileName, "rb"); 
	
	//*load the xRes, yRes, zRes
	fread(vol_xRes, sizeof(int),1, fn);
	fread(vol_yRes, sizeof(int),1, fn);
	fread(vol_zRes, sizeof(int),1, fn);

	//*load volume flow data into a 1D array this array is located in main as Vol_VecA. 
	int vec_field = (*vol_xRes) * (*vol_yRes) * (*vol_zRes) * 3;
	
	float * vec_data = new float[vec_field];
	
	fread( vec_data, sizeof( float ), (*vol_xRes) * (*vol_yRes) * (*vol_zRes) * 3, fn ); fclose( fn ); // ��omitted
	 		
	//*print xRes, yRes, zRes to console window as single values

	printf("Resolution: %d %d %d\n\n\n", *vol_xRes, *vol_yRes, *vol_zRes);

	return vec_data; //Returns a 1D dymanic array 
}

//This function finds the vector magnitude at each grid point then organizes that data
//into min, max, mean, and Standard Deviation
void    AnalyzeVectorMagnitu
        ( int     vol_xRes, int    vol_yRes, int vol_zRes, 
		  float * vec_data)
{
	int res = vol_xRes * vol_yRes * vol_zRes;

	float *  mag = new float[res];
	
	//*find the magnitude of each vector at each grid point 
	for (int k1 = 0; k1 < vol_zRes; k1++)
	{
		for( int k2 = 0; k2 < vol_yRes; k2++)
		{
			for( int k3 = 0; k3 < vol_xRes; k3++)
			{
				int idx1 = 3 * (k3 + vol_xRes*k2 + vol_xRes*vol_yRes*k1);
				int idx2 = k3 + vol_xRes*k2 + vol_xRes*vol_yRes*k1;

				float x = vec_data[idx1];
				float y = vec_data[idx1 + 1];
				float z = vec_data[idx1 + 2];
				 
				mag[idx2] =  (float)sqrt((double)((x*x)+(y*y)+(z*z)));
			}
		}
	}
	//*Search Array
	//fake values so that the min and max intial valuse will be covering all of the values
	float min = 99999.9;  
	float max =     0.0;

	for(int i = 0; i < res; i++)
	{
		if (mag[i] < min)
		{

			float q = mag[i];
			min = q;
		}
		if(mag[i] > max)
		{
			float g = mag[i];

			max = g;
		}
	}
	//*find the mean ( mean = (sum of all values)/ number of data points )
	float sum = 0.0;

	for(int i = 0; i < res; i++)
	{
		float g = mag[i];

	    sum += g;
	}
	float mean = sum / res;

	//*find the Standard Deviation (SD = sqrt( (sigma pow(a value in the data set - mean, 2)) /res ) 
	sum = 0.0; 
	
	for(int i = 0; i < res; i++)
	{
		float g = mag[i];

		sum += (g - mean) * (g - mean);
	}
	float std_dev = sqrt(sum/res);

	//*Display min, max, mean, and Standard Deviation
	printf(" Min= %f \n Max= %f \n Mean= %f \n Standard Deviation= %f\n", min, max, mean, std_dev);

	///*Check Benchmark
	//FILE *fn = fopen("VecMag.dat", "wb");
	//fwrite(&vol_xRes, sizeof(int),1, fn);
	//fwrite(&vol_yRes, sizeof(int),1, fn);
	//fwrite(&vol_zRes, sizeof(int),1, fn);
	//fwrite(&min,     sizeof(float),1, fn);
	//fwrite(&max,     sizeof(float),1, fn);
	//fwrite(&mean,    sizeof(float),1, fn);
	//fwrite(&std_dev, sizeof(float),1, fn);
	//fwrite(mag, sizeof(float),res, fn);
	//fclose(fn); 
	//fn   = NULL;
	delete mag;
	mag  = NULL;
}

//This function extracts the volume vector data in the region of interest(ROI)--based on a Cartesian grid--that has been specfied using I,J,K indices 
//This function is called by main with specfic values
//for ROI indices(roi_xPos, roi_yPos, roi_zPos) with I = J = K = 21
//for ROI size information(roi_xRes, roi_yRes, roi_zRes) with 86, 86, 86
float * Get_ROI_VolumeVector
//			   input		 input		   input			 input
		(  int vol_xRes, int vol_yRes, int vol_zRes, float * vec_data, 
//			   input		 input		   input
		   int roi_xPos, int roi_yPos, int roi_zPos,
//			   input		 input		   input
		   int roi_xRes, int roi_yRes, int roi_zRes	)
{
	// Read the volume data from within the region of interest into a 1D array
	int roi_Area = ( roi_xRes * roi_yRes * roi_zRes ) * 3;  // Component index for ROI

	float *roi_data = new float[ roi_Area ];

	// Find values in a range (first num in range - last num in range) + 1
	int idx1 = 0;
	int idx2 = 0;
	int sliceVol  = vol_xRes * vol_yRes ;  // slice for volume data
	int sliceROI  = roi_xRes * roi_yRes ;  // slice for region of interest data

		for(int r3 = 0; r3 < roi_zRes; r3++ )     // z
		{
	      for(int r2 = 0; r2 < roi_yRes; r2++ )	  // y
		  {
		    for(int r1 = 0; r1 < roi_xRes; r1++ ) // x 
			{	
					int idx2 = ( r1 + roi_xRes * r2 + sliceROI * r3 ) * 3;												// Component index for ROI
					int idx1 = ( ( roi_zPos + r3 ) * sliceVol + ( roi_yPos + r2 ) * vol_xRes + ( roi_xPos + r1 )) * 3;  // Component index for volume data

					roi_data [ idx2     ] = vec_data [ idx1     ];  // x-component
					roi_data [ idx2 + 1 ] = vec_data [ idx1 + 1 ];  // y-component
					roi_data [ idx2 + 2 ] = vec_data [ idx1 + 2 ];  // z-component
			}
		  }
		}
	//FILE *fn = fopen("ROI_Data.vec", "wb");

	//fwrite(&roi_xRes, sizeof(int),1, fn);
	//fwrite(&roi_yRes, sizeof(int),1, fn);
	//fwrite(&roi_zRes, sizeof(int),1, fn);

	//fwrite(roi_data, sizeof(float), roi_Area, fn);

	//fclose(fn);

	return roi_data;
}//*/

	float Interpolate  
//			input	   input	input
		( float r , float s, float t,
//			input	   input	input	  input
		  float rs, float rt,float st, float rst, 
//			input	   input     input     input     input     input     input     input
		  float x0, float x1, float x2, float x3, float x4, float x5, float x6, float x7 )
	{
		// Tri-Linear Interpolation of One Component
		//
		// result = [  ( x0 * ( 1 - r ) + x4 * r ) * ( 1 - s ) 
		//           + ( x1 * ( 1 - r ) + x5 * r ) *       s  
		//			]       * ( 1 - t ) 
		//           + 
		//          [  ( x3 * ( 1 - r ) + x7 * r ) * ( 1 - s ) 
		//           + ( x2 * ( 1 - r ) + x6 * r ) *       s
		//          ]       *       t
		//
		//---------------------------------------------------------------

		return			 x0 + ( x1 - x0 ) * r + 
							  ( x3 - x0 ) * s +
							  ( x4 - x0 ) * t + 
					( x0 + x2 - x1 - x3 ) * rs + 
					( x0 + x5 - x1 - x4 ) * rt +
					( x0 + x7 - x3 - x4 ) * st +
		  ( x1 + x3 + x4 + x6 - x0 - x2 - x5 - x7 ) * rst;
	}

	void Interpolate 
//				input		input		input		input
		( float    r, float    s, float    t, float *  v,
//				input	    input       input       input
		  float * v0, float * v1, float * v2, float * v3,
//				input       input		input		input
		  float * v4, float * v5, float * v6, float * v7 )
	{
		float rs  =  r * s; 
		float rt  =  r * t; 
		float st  =  s * t;
		float rst = rs * t;

	 v[ 0 ] = Interpolate ( r, s, t, rs, rt, st, rst, v0[ 0 ], v1[ 0 ], v2[ 0 ], v3[ 0 ] ,   // vx-component
													  v4[ 0 ], v5[ 0 ], v6[ 0 ], v7[ 0 ] ); 

	 v[ 1 ] = Interpolate ( r, s, t, rs, rt, st, rst, v0[ 1 ], v1[ 1 ], v2[ 1 ], v3[ 1 ] ,   // vy-component
												      v4[ 1 ], v5[ 1 ], v6[ 1 ], v7[ 1 ] ); 

	 v[ 2 ] = Interpolate ( r, s, t, rs, rt, st, rst, v0[ 2 ], v1[ 2 ], v2[ 2 ], v3[ 2 ] ,   // vz-component
												      v4[ 2 ], v5[ 2 ], v6[ 2 ], v7[ 2 ] ); 
	}

	void Interpolate  
//		        input       input       input       input
		( int   xRes, int   yRes, int   zRes, float * A, 
//			input	  input		input
		  int   i, int   j, int   k, 
//			input     input    input     input
		  float r, float s, float t, float * v )
	{
		int slice =   xRes * yRes; 
		//Passed the indices of a base point of a cube that we want to use to find the interpolated points

		//Point index
		int index0 = ( i + j * xRes + k * slice ); 

		int index1 = index0 + 1     ;   // x + 1   		
		int index3 = index0 + xRes  ; 	// y + 1	
		int index2 = index3 + 1     ;   // x + 1, y + 1     		
		int index4 = index0 + slice ;   // z + 1         		
		int index5 = index1 + slice ;   // x + 1, z + 1
		int index6 = index2 + slice ;   // y + 1, z + 1
		int index7 = index3 + slice ;   // x + 1, y + 1, z + 1
		
		// Component index Multiply by 3
		index0 += ( index0 << 1 );		index4 += ( index4 << 1 );
		index1 += ( index1 << 1 );		index5 += ( index5 << 1 );
		index2 += ( index2 << 1 ); 	    index6 += ( index6 << 1 );
		index3 += ( index3 << 1 );		index7 += ( index7 << 1 );

		// Allocate all the vectors into static arrays 
		float v0[ 3 ];					float v1[ 3 ];					float v2[ 3 ];					float v3[ 3 ];
		v0[ 0 ] = A[ index0     ];		v1[ 0 ] = A[ index1     ];		v2[ 0 ] = A[ index2     ];		v3[ 0 ] = A[ index3     ];
		v0[ 1 ] = A[ index0 + 1 ];		v1[ 1 ] = A[ index1 + 1 ];		v2[ 1 ] = A[ index2 + 1 ];		v3[ 1 ] = A[ index3 + 1 ];
		v0[ 2 ] = A[ index0 + 2 ];		v1[ 2 ] = A[ index1 + 2 ];		v2[ 2 ] = A[ index2 + 2 ];		v3[ 2 ] = A[ index3 + 2 ];

		float v4[ 3 ];					float v5[ 3 ];					 float v6[ 3 ];				    float v7[ 3 ];
		v4[ 0 ] = A[ index4     ];		v5[ 0 ] = A[ index5     ];		 v6[ 0 ] = A[ index6     ];	    v7[ 0 ] = A[ index7     ];
		v4[ 1 ] = A[ index4 + 1 ];      v5[ 1 ] = A[ index5 + 1 ];       v6[ 1 ] = A[ index6 + 1 ];     v7[ 1 ] = A[ index7 + 1 ];
		v4[ 2 ] = A[ index4 + 2 ];      v5[ 2 ] = A[ index5 + 2 ];       v6[ 2 ] = A[ index6 + 2 ];     v7[ 2 ] = A[ index7 + 2 ];

		Interpolate ( r, s, t, v, v0, v1, v2, v3, 
								  v4, v5, v6, v7 );
	}

//Uses data supersampling in the ROI by a factor scaling factor of 3
/*void    SuperSampleVolumeVectorData
		( int   *  old_xRes,  int * old_yRes, int * old_zRes, 
		  float * &vec_data,  int   scaleNum)
{

	int new_xRes = ( *old_xRes - 1 ) * ( scaleNum + 1 );
	int new_yRes = ( *old_yRes - 1 ) * ( scaleNum + 1 );
	int new_zRes = ( *old_zRes - 1 ) * ( scaleNum + 1 );

	//new resolution of the grid would be N = (res - 1) * n + 1;
	int NewRes = (((( *old_xRes ) * ( *old_yRes ) * ( *old_zRes ) ) - 1 ) * scaleNum + 1 ) * 3;

	float * ROISupSamp = new float [ NewRes         ];
	
	float * ratioVals  = new float [ scaleNum - 1   ];

	//This is setting the locations of the supersampled points i.e. if i = 0 the (i + 1) / 3 = 1/3 
	for( int i = 0; i < scaleNum - 1; i++ )
	{
		ratioVals[ i ] = float( i + 1 ) / scaleNum;
	}

	//float r = (  -  ) / (  -  ); 
	//float s = 
	//float t = 

	//int I = 
	//int J = 
	//int K = 

	for( int i = 0; i < new_zRes; i++ )
	{
		for( int j = 0; j < new_yRes; j++ )
		{
			for( int k = 0; k < new_xRes; k++ )
			{
				int oldIDX = ( k + j * ( *old_xRes ) + i * ( *old_yRes ) * ( *old_xRes )) * 3;

				int newIDX = ( K + J *    new_xRes   + I *    new_yRes   *    new_xRes )  * 3; //Fix this index
				
				ROISupSamp[ newIDX ] = vec_data[ oldIDX ];
			}
		}
	}	
}*/

//Writes the super-sampled data to a binary file with the xRes, yRes, zRes and the vector data the same to how Dr.Liu formats his files
void    SaveVectorData3D
		( int     vec_xRes, int    vec_yRes, int vec_zRes, 
	      float * vec_data, char * fileName)
{
	FILE *fn = fopen(fileName, "wb");

	int Area = ( vec_xRes * vec_yRes * vec_zRes ) * 3;

	fwrite( &vec_xRes, sizeof(int)  ,    1, fn );
	fwrite( &vec_yRes, sizeof(int)  ,    1, fn );
	fwrite( &vec_zRes, sizeof(int)  ,    1, fn );
	fwrite(  vec_data, sizeof(float), Area, fn );

	fclose( fn );
}

//This function is called in the main function three times to get the 2D vector data of the three slices
//Called with 192 as the position of all of the slices
//returns a 1D array
/*float *  ExtractSlice
		 (int     vol_xRes, int  vol_yRes, int vol_zRes,
		  float * vec_data, char axisChar, int sliceIdx)
{

}//*/

//Writes to a binary file in Dr. Liu's format with two resolutions instead of three
void      SaveVectorData2D
		  (int     vec_xRes, int  vec_yRes,
		   float * vec_data, char * fileName)
{
	FILE * fn = fopen(fileName, "wb");

	int Area = ( vec_xRes * vec_yRes ) * 3;
	
	fwrite( &vec_xRes, sizeof(int)  ,    1, fn );
	fwrite( &vec_yRes, sizeof(int)  ,    1, fn );
	fwrite(  vec_data, sizeof(float), Area, fn );

	fclose( fn );
}

//called for each slice
//reads from a BMP file into a 1D array
//file name is that of the BMP image file
/*void      SetTexture(char * fileName, GLuint texIndex )

{


}//*/

int _tmain(int argc, _TCHAR* argv[])
{
	char *fileVol = "LorenzVolumeFlow.vec";//Change name to use a different data set
	
	int vol_xRes =  0, vol_yRes =  0, vol_zRes =  0; 
	int roi_xPos = 21, roi_yPos = 21, roi_zPos = 21; 
	int roi_xRes = 86, roi_yRes = 86, roi_zRes = 86; 
	int scaleNum =  3;

	int vol_size = (vol_xRes) * (vol_yRes) * (vol_zRes) * 3;
	int roi_size = (roi_xRes) * (roi_yRes) * (roi_zRes) * 3;

	float * Vol_Vec = new float[vol_size];
	float * ROI_Vec = new float[roi_size];

	Vol_Vec = LoadVolumeVectorData
				( fileVol, &vol_xRes, &vol_yRes, &vol_zRes);
			  AnalyzeVectorMagnitu
				( vol_xRes, vol_yRes, vol_zRes, Vol_Vec);
	ROI_Vec = Get_ROI_VolumeVector
				(   vol_xRes, vol_yRes, vol_zRes, Vol_Vec, 
				    roi_xPos, roi_yPos, roi_zPos,
				    roi_xRes, roi_yRes, roi_zRes           );
    //        SuperSampVolVectData
	//  ( &roi_xRes, &roi_yRes, &roi_zRes, 
	//	  &ROI_Vec,  scaleNum);

	delete (Vol_Vec); Vol_Vec  = NULL;
	delete (ROI_Vec); ROI_Vec  = NULL;
					  fileVol  = NULL;
					  
   system("pause");
	return 0;
}


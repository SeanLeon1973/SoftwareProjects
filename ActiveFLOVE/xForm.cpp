
/// xForm Class -----------------------------------------------------------

	



/// -----------------------------------------------------------------------

#include "stdafx.h"

#include "windows.h"
#include <math.h>      //for Mathmatical operations such as sqrt
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <time.h>
#include <string>
#include <fstream>
#include <iostream>

#include <GL/glut.h>

#include "xForm.h"

//-------------------------------------------------------------------------------------------------------------------------------- Function Prototypes 

float  *  XtractSlice   ( int    xRes    ,  int    yRes   ,  VECTOR3D * vec_data, int   sliceIdx  );

void      SetTextureS   ( VECTOR2D * vec_data,  int    nxres  ,  int        nyres             );

void      CrossProductS ( float  U[]     ,  float  V[]    ,  float  N[]                           );

// 2D Interpolation functions

float      Interpolate  ( float r , float s , float rs , float x0 , float x1, float x2, float x3 );
 
void       Interpolate  ( float   r , float    s , float *   v , 
				          float * v0 , float * v1 , float *  v2 , float * v3 );

void       Interpolate  ( int   xRes, int   yRes, float * A, int   i, int   j, 
				          float r   , float s   , float * v  );

// 3D Interpolation Functions

void       Interpolate  ( int    xRes , int    yRes , int    zRes, VECTOR3D * A ,  
				          int    i    , int    j    , int    k   ,  
		                  float  r    , float  s    , float  t   , VECTOR3D   v );

VECTOR3D   Interpolate  ( float      r,  float      s,  float      t,   							
						  float     rs,  float     rt,  float     st,  float     rst ,	
						  VECTOR3D  v0,  VECTOR3D  v1,  VECTOR3D  v2,  VECTOR3D   v3 ,  
						  VECTOR3D  v4,  VECTOR3D  v5,  VECTOR3D  v6,  VECTOR3D   v7 );

// Supersampling Function

void    SuperSampleVolumeVectorData2D	( int   *  vol_xRes,  int * vol_yRes , 
									      float * &vec_data,  int   scaleNum );

//  --------------------------------------------------------------------------------------------------------------------------- Constructors and Deconstructors

xForm::xForm()
{
	Distance2User   = 0.0;
	ArbitraryPoint.x  = 0.0;
	ArbitraryPoint.y  = 0.0;
	ArbitraryPoint.z  = 0.0;

	XResolution = 0;
	yres = 0;
	zres = 0;
	
	plane_xResolution = 7;
	plane_yResolution = 7;

	GlobalVectorData = NULL;

	viewDirectin(); 
	userPosition();
	distanceUser();  
	getUpVector ();

	Init();
}

xForm::xForm( VECTOR3D viewDirect ,  POINT3D   userPost ,     float   distUser, VECTOR3D upwrd, VECTOR3D * vec_data, 
			  int  xreso, int    yreso , int zreso, 
			  int planeX, int   planeY )
{
	Distance2User = distUser;

	ViewDirectionVector.vx = viewDirect.vx;
	ViewDirectionVector.vy = viewDirect.vy;
	ViewDirectionVector.vz = viewDirect.vz;

	UserPosition.x  = userPost.x;
	UserPosition.y  = userPost.y;
	UserPosition.z  = userPost.z;

	UpVector.vx = upwrd.vx;
	UpVector.vy = upwrd.vy;
	UpVector.vz = upwrd.vz;

	GlobalVectorData = vec_data;

	XResolution = xreso;
	yres = yreso;
	zres = zreso;

	plane_xResolution = planeX;
	plane_yResolution = planeY;

	Init();
}

xForm::~xForm()
{
	GlobalVectorData = NULL;
}

//  ----------------------------------------------------------------------------------------------------------------------------------- Member Functions

void xForm::Init()
{
	getOriginofLocalSystem();
	TMatrix();
	RMatrix();
	MMatrix();
}

void xForm::setCuttingPlane()
{
	int     slice = plane_xResolution * plane_yResolution;
    int compslice = ( slice << 1 );              // Multiply by 2

	   float * temp = NULL;

	   float * img_data = ( float* ) malloc ( sizeof( float ) * compslice );

	    temp = img_data;

    for ( int j = 0; j < plane_xResolution; j++            ) 
	for ( int i = 0; i < plane_yResolution; i++, temp += 2 )
		{			
			ArbitraryPoint.x = LocalSystemOrigin.x + i;   // based on the Local system's location in the global system
			ArbitraryPoint.y = LocalSystemOrigin.y + j;
			ArbitraryPoint.z = LocalSystemOrigin.z    ;

			VECTOR3D ArbitraryGlobalVector = GetVector();

			VECTOR3D ArbitraryLocalVector = transferGlb2LoclVectr( ArbitraryGlobalVector );  

			temp[ 0 ] = ArbitraryLocalVector.vx;
			temp[ 1 ] = ArbitraryLocalVector.vy;
		}			  

	temp = NULL;

	float XresDiv2 = ( plane_xResolution >> 1 );
	float YresDiv2 = ( plane_yResolution >> 1 );

	// p ------ s
	//   |    | 
	//   |    | 
	// q ------ r

	p.x = LocalSystemOrigin.x             ; //( 0, 1 )
	p.y = LocalSystemOrigin.y + Distance2User   ;
	p.z = LocalSystemOrigin.z + plane_yResolution;

	q.x = LocalSystemOrigin.x            ; //( 0, 0 )
	q.y = LocalSystemOrigin.y + Distance2User   ;
	q.z = LocalSystemOrigin.z             ;

	r.x = LocalSystemOrigin.x + plane_xResolution; //( 1, 0 )
	r.y = LocalSystemOrigin.y + Distance2User   ;
	r.z = LocalSystemOrigin.z             ;

	s.x = LocalSystemOrigin.x + plane_xResolution; //( 1, 1 )
	s.y = LocalSystemOrigin.y + Distance2User   ;
	s.z = LocalSystemOrigin.z + plane_yResolution;

	int scaleNum = 10;  

	//SuperSampleVolumeVectorData2D              //supersamples vec data to allow it to show more detail with less resolution
	//  	( &plane_xRes , &plane_yRes ,
	//	     img_data ,  scaleNum   );         //AUTOMATE: make the scale number something that reduces more the higher the plane resolution, but remains at 10 at plane res 49 and increas the lower the resolution

	texture = ( unsigned char * ) malloc( sizeof( unsigned char ) * plane_xResolution * plane_yResolution * 3 );

	InvokeLICWrapper( img_data );
	//LICWRAPPER licWrapper;
	//licWrapper.SetVectorField( plane_xRes, plane_yRes, img_data ); // giving the LIC engine our vector data
	//licWrapper.SetVectorType ( RL_DATA ); // Real Data
	//licWrapper.EnhanceLIC();              // EnhancedLIC
	//licWrapper.LeaveData2User( 0 );       // Leaves the user's data
	//licWrapper.TransferRGB2User( 1 );

	/*texture = licWrapper.GetClrLICimage();*/

	if( img_data )  free( img_data ); img_data = NULL;
}

void xForm::InvokeLICWrapper( float * img_data )
{
	LICWRAPPER licWrapper;
	licWrapper.SetVectorField( plane_xResolution, plane_yResolution, img_data ); // giving the LIC engine our vector data
	licWrapper.SetVectorType ( RL_DATA ); // Real Data
	licWrapper.EnhanceLIC();              // EnhancedLIC
	licWrapper.LeaveData2User( 0 );       // Leaves the user's data
	licWrapper.TransferRGB2User( 1 );

	texture = licWrapper.GetClrLICimage();
}

// Transfers a point in a local system to a point in a global system

//   Forward Matrix     *   Local Point   =  Global Point
//   /--------------\          /---\             /---\
//   | Ux Vx Wx  Tx |          | x |             | x |
//   | Uy Vy Wy  Ty |   X      | y |      =      | y |
//   | Uz Vz Wz  Tz |          | z |             | z |
//   | 0  0  0   1  |          | 1 |             \---/
//   \--------------/          \---/    

POINT3D xForm::transferLocl2GlbPoint( POINT3D LocPoint )
{
	POINT3D GlbPoint;

	GlbPoint.x = ( ForwardMatrix[  0 ] * LocPoint.x ) +
				 ( ForwardMatrix[  1 ] * LocPoint.y ) +
			     ( ForwardMatrix[  2 ] * LocPoint.z ) + ForwardMatrix[  3 ];

	GlbPoint.y = ( ForwardMatrix[  4 ] * LocPoint.x ) +
		         ( ForwardMatrix[  5 ] * LocPoint.y ) + 
			     ( ForwardMatrix[  6 ] * LocPoint.z ) + ForwardMatrix[  7 ];

	GlbPoint.z = ( ForwardMatrix[  8 ] * LocPoint.x ) +
			     ( ForwardMatrix[  9 ] * LocPoint.y ) +
			     ( ForwardMatrix[ 10 ] * LocPoint.z ) + ForwardMatrix[ 11 ];

	return GlbPoint;
}

// Transfers a point in a global system to a point in a local system

//   Backward Matrix   *   Global Point   =   Local Point
//   /--------------\       /---\             /---\
//   | Ux Uy Uz -Tx |       | x |             | x |
//   | Vx Vy Vz -Ty |  X    | y |         =   | y |
//   | Wx Wy Wz -Tz |       | z |             | z |
//   | 0  0  0   1  |       | 1 |             \---/
//   \--------------/       \---/    

POINT3D xForm::transferGlb2LoclPoint( POINT3D GlbPoint )
{
	POINT3D LocPoint;

	LocPoint.x = ( BackwardMatrix[  0 ] * GlbPoint.x ) +
				 ( BackwardMatrix[  1 ] * GlbPoint.y ) +
			     ( BackwardMatrix[  2 ] * GlbPoint.z ) + BackwardMatrix[  3 ];

	LocPoint.y = ( BackwardMatrix[  4 ] * GlbPoint.x ) +
		         ( BackwardMatrix[  5 ] * GlbPoint.y ) + 
			     ( BackwardMatrix[  6 ] * GlbPoint.z ) + BackwardMatrix[  7 ];

	LocPoint.z = ( BackwardMatrix[  8 ] * GlbPoint.x ) +
			     ( BackwardMatrix[  9 ] * GlbPoint.y ) +
			     ( BackwardMatrix[ 10 ] * GlbPoint.z ) + BackwardMatrix[ 11 ];

	return LocPoint;
}

//Transfers a vector in a local system to a vector in a global system

//         Rotational Matrix  *  Local Vector    =  Global Vector
//         /----------\             /----\			 /----\
//         | Ux Vx Wx |             | vx |			 | vx |
//         | Uy Vy Wy |       X     | vy |       =	 | vy |
//         | Uz Vz Wz |             | vz |			 | vz |
//         \----------/             \----/			 \----/
// 
VECTOR3D xForm::transferLocl2GlbVectr( VECTOR3D LocVectr )
{
	VECTOR3D GlbVectr;

    GlbVectr.vx = ( RotationMatrix[ 0 ] * LocVectr.vx ) +	    
				  ( RotationMatrix[ 3 ] * LocVectr.vy ) +	    
				  ( RotationMatrix[ 6 ] * LocVectr.vz ) ;      
							    
	GlbVectr.vy = ( RotationMatrix[ 1 ] * LocVectr.vx ) +      
				  ( RotationMatrix[ 4 ] * LocVectr.vy ) +      
				  ( RotationMatrix[ 7 ] * LocVectr.vz ) ;      
						    
	GlbVectr.vz = ( RotationMatrix[ 2 ] * LocVectr.vx ) +
				  ( RotationMatrix[ 5 ] * LocVectr.vy ) +
				  ( RotationMatrix[ 8 ] * LocVectr.vz ) ;

	return GlbVectr;
}

//Transfers a vector in a global system to a vector in a local system

// Inverse Rotational Matrix  *  Global Vector   =  Local Vector
//         /----------\             /----\			 /----\
//         | Ux Uy Uz |             | vx |			 | vx |
//         | Vx Vy Vz |       X     | vy |       =	 | vy |
//         | Wx Wy Wz |             | vz |			 | vz |
//         \----------/             \----/			 \----/
// 
VECTOR3D xForm::transferGlb2LoclVectr( VECTOR3D GlbVectr )
{
	VECTOR3D LocVectr;

    LocVectr.vx = ( InverseRotationMatrix[ 0 ] * GlbVectr.vx ) +	    
				  ( InverseRotationMatrix[ 3 ] * GlbVectr.vy ) +	    
				  ( InverseRotationMatrix[ 6 ] * GlbVectr.vz ) ;      
								    
	LocVectr.vy = ( InverseRotationMatrix[ 1 ] * GlbVectr.vx ) +      
				  ( InverseRotationMatrix[ 4 ] * GlbVectr.vy ) +      
				  ( InverseRotationMatrix[ 7 ] * GlbVectr.vz ) ;      
							    
	LocVectr.vz = ( InverseRotationMatrix[ 2 ] * GlbVectr.vx ) +
			      ( InverseRotationMatrix[ 5 ] * GlbVectr.vy ) +
		  		  ( InverseRotationMatrix[ 8 ] * GlbVectr.vz ) ;

	return LocVectr;
}

//Function takes in two vectors: View, Up; and it uses those vectors to form a coordinate system
// It sets a float array holding nine values with the vectors stored in this order
//    /--------\
//	  |Ux Vx Wx|
//	  |Uy Vy Wy|
//	  |Uz Vz Wz|
//	  \--------/
//
//First  vector found with View(-U) and Up vectors 
//Second vector found with the U and  result of the previous equation
//
//							V 
//							|
//							|   
//							|  
//							|   
//							|
//					^	    + -------------U
//				   /	   /
//				  /		  / 
//				 /		 /								
//				/		/
//			   /       /
//	View Direction	  W	
//
//-------------------------------------------------------------------

void xForm::ConstructAFrameRef()
{	                            
	float W[ 3 ]; 
	float V[ 3 ]; 
	float U[ 3 ]; 

	W[ 0 ] = -ViewDirectionVector.vx;
	W[ 1 ] = -ViewDirectionVector.vy;
	W[ 2 ] = -ViewDirectionVector.vz; 

	V[ 0 ] =  UpVector.vx;
	V[ 1 ] =  UpVector.vy;
	V[ 2 ] =  UpVector.vz;

	CrossProductS( V, W, U );   // W = Up * U
	CrossProductS( W, U, V );   // V = U  * W
	
	//Normalize
	////------------Vector U------------------------
	float mag = sqrt( double( U[ 0 ] * U[ 0 ] + U[ 1 ] * U[ 1 ] + U[ 2 ] * U[ 2 ]));

	  float s = 1.0f / mag;   U[ 0 ] *= s;      U[ 1 ] *= s;      U[ 2 ] *= s;
	////------------Vector V------------------------
	mag = sqrt( double( V[ 0 ] * V[ 0 ] + V[ 1 ] * V[ 1 ] + V[ 2 ] * V[ 2 ]));
	
	  s = 1.0f / mag;   V[ 0 ] *= s;      V[ 1 ] *= s;      V[ 2 ] *= s;

	////-----------Vector W--------------------------	                              		// W does not need to be normalized because the vector
	//float mag = sqrt( double( W[ 0 ] *  W[ 0 ] + W[ 1 ] * W[ 1 ] + W[ 2 ] * W[ 2 ]));     // used for it is already normalized 

	//float s = 1.0f / mag;   W[ 0 ] *= s;       W[ 1 ] *= s;      W[ 2 ] *= s;
	////----------------------------------------------

	RotationMatrix[ 0 ] = U[ 0 ]; RotationMatrix[ 1 ] = V[ 0 ]; RotationMatrix[ 2 ] = W[ 0 ];    ////Rotational Matrix
	RotationMatrix[ 3 ] = U[ 1 ]; RotationMatrix[ 4 ] = V[ 1 ]; RotationMatrix[ 5 ] = W[ 1 ];    //	/--------\      
	RotationMatrix[ 6 ] = U[ 2 ]; RotationMatrix[ 7 ] = V[ 2 ]; RotationMatrix[ 8 ] = W[ 2 ];    //	|Ux Vx Wx|
																		 //	|Uy Vy Wy|
																		 //	|Uz Vz Wz|
}																		 // \--------/

// Transformation Matrix    Inverse Transformation Matrix
//  /----------\                  /-----------\
//  | 1 0 0 Tx |                  | 1 0 0 -Tx |
//  | 0 1 0 Ty |                  | 0 1 0 -Ty |
//  | 0 0 1 Tz |                  | 0 0 1 -Tz |
//  | 0 0 0 1  |                  | 0 0 0  1  |
//  \----------/                  \-----------/
void xForm::TMatrix()
{  
	//Transitional Matrix
	TransformationMatrix[  0 ] = 1.0;  TransformationMatrix[  1 ] = 0.0;  TransformationMatrix[  2 ] = 0.0;  TransformationMatrix[  3 ] =   LocalSystemOrigin.x;
	TransformationMatrix[  4 ] = 0.0;  TransformationMatrix[  5 ] = 1.0;  TransformationMatrix[  6 ] = 0.0;  TransformationMatrix[  7 ] =   LocalSystemOrigin.y;
	TransformationMatrix[  8 ] = 0.0;  TransformationMatrix[  9 ] = 0.0;  TransformationMatrix[ 10 ] = 1.0;  TransformationMatrix[ 11 ] =   LocalSystemOrigin.z;
	TransformationMatrix[ 12 ] = 0.0;  TransformationMatrix[ 13 ] = 0.0;  TransformationMatrix[ 14 ] = 0.0;  TransformationMatrix[ 15 ] =         1.0;

	//Inverse Transformation Matrix
	InverseTransformationMatrix[  0 ] = 1.0;  InverseTransformationMatrix[  1 ] = 0.0;  InverseTransformationMatrix[  2 ] = 0.0;  InverseTransformationMatrix[  3 ] =  -LocalSystemOrigin.x;
	InverseTransformationMatrix[  4 ] = 0.0;  InverseTransformationMatrix[  5 ] = 1.0;  InverseTransformationMatrix[  6 ] = 0.0;  InverseTransformationMatrix[  7 ] =  -LocalSystemOrigin.y;
	InverseTransformationMatrix[  8 ] = 0.0;  InverseTransformationMatrix[  9 ] = 0.0;  InverseTransformationMatrix[ 10 ] = 1.0;  InverseTransformationMatrix[ 11 ] =  -LocalSystemOrigin.z;
	InverseTransformationMatrix[ 12 ] = 0.0;	 InverseTransformationMatrix[ 13 ] = 0.0;  InverseTransformationMatrix[ 14 ] = 0.0;  InverseTransformationMatrix[ 15 ] =         1.0;
}

//  Rotationl Matrix   Inverse Rotational Matrix
//  /----------\	    /----------\
//  | Ux Vx Wx |		| Ux Uy Uz |
//  | Uy Vy Wy |		| Vx Vy Vz |
//  | Uz Vz Wz |		| Wx Wy Wz |
//  \----------/		\----------/
void xForm::RMatrix()
{   
	//Rotational Matrix
	ConstructAFrameRef();

	//Inverse Rotational Matrix
	InverseRotationMatrix[ 0 ] = RotationMatrix[ 0 ]; InverseRotationMatrix[ 1 ] = RotationMatrix[ 3 ]; InverseRotationMatrix[ 2 ] = RotationMatrix[ 6 ];
	InverseRotationMatrix[ 3 ] = RotationMatrix[ 1 ]; InverseRotationMatrix[ 4 ] = RotationMatrix[ 4 ]; InverseRotationMatrix[ 5 ] = RotationMatrix[ 7 ];
	InverseRotationMatrix[ 6 ] = RotationMatrix[ 2 ]; InverseRotationMatrix[ 7 ] = RotationMatrix[ 5 ]; InverseRotationMatrix[ 8 ] = RotationMatrix[ 8 ];
}

//  Forward Matrix        Backward Matrix
//  /-------------\	      /--------------\
//  | Ux Vx Wx Tx |		  | Ux Uy Uz -Tx |
//  | Uy Vy Wy Ty |		  | Vx Vy Vz -Ty |
//  | Uz Vz Wz Tz |		  | Wx Wy Wz -Tz |
//  |  0  0  0  1 |		  |  0  0  0  1  |
//  \-------------/		  \--------------/
void xForm::MMatrix()
{  
	//Forward Matrix
	ForwardMatrix[  0 ] = RotationMatrix[ 0 ];  ForwardMatrix[  1 ] = RotationMatrix[ 1 ];  ForwardMatrix[  2 ] =  RotationMatrix[ 2 ];  ForwardMatrix[  3 ] = LocalSystemOrigin.x; 
	ForwardMatrix[  4 ] = RotationMatrix[ 3 ];  ForwardMatrix[  5 ] = RotationMatrix[ 4 ];  ForwardMatrix[  6 ] =  RotationMatrix[ 5 ];  ForwardMatrix[  7 ] = LocalSystemOrigin.y; 
	ForwardMatrix[  8 ] = RotationMatrix[ 6 ];  ForwardMatrix[  9 ] = RotationMatrix[ 7 ];  ForwardMatrix[ 10 ] =  RotationMatrix[ 8 ];  ForwardMatrix[ 11 ] = LocalSystemOrigin.z;
	ForwardMatrix[ 12 ] =         0.0;  ForwardMatrix[ 13 ] =         0.0;  ForwardMatrix[ 14 ] =          0.0;  ForwardMatrix[ 15 ] =       1.0;

	//Backward Matrix
	BackwardMatrix[  0 ] = InverseRotationMatrix[ 0 ];  BackwardMatrix[  1 ] = InverseRotationMatrix[ 1 ];  BackwardMatrix[  2 ] =  InverseRotationMatrix[ 2 ];  BackwardMatrix[  3 ] =  -LocalSystemOrigin.x;
    BackwardMatrix[  4 ] = InverseRotationMatrix[ 3 ];  BackwardMatrix[  5 ] = InverseRotationMatrix[ 4 ];  BackwardMatrix[  6 ] =  InverseRotationMatrix[ 5 ];  BackwardMatrix[  7 ] =  -LocalSystemOrigin.y;
	BackwardMatrix[  8 ] = InverseRotationMatrix[ 6 ];  BackwardMatrix[  9 ] = InverseRotationMatrix[ 7 ];  BackwardMatrix[ 10 ] =  InverseRotationMatrix[ 7 ];  BackwardMatrix[ 11 ] =  -LocalSystemOrigin.z;
	BackwardMatrix[ 12 ] =         0.0;  BackwardMatrix[ 13 ] =         0.0;  BackwardMatrix[ 14 ] =          0.0;  BackwardMatrix[ 15 ] =         1.0;
}

// 3D Interpolation Function

///					 ^ y (s)
///				   3 |				  2
///				     +--------------+ 
///				    /|			   /|
///				  /  |			 /  |
///			    /	 |		   /	|
///			  /		 |		 /	    |
///		  7	 +-------+------+  6    |
///			 |		 |		|		|
///			 |		 |		|		|	
///			 |	   0 |		|		| 1
///			 |	     +------+-------+-----> x (r)
///			 |		/       |      /
///			 |	  /		    |    /
///          |  /           |  /
///       4  +/-------------+/ 5
///         /
///       / z (t)
///		
///		Get  the  vector  at  a  given  point  WITHIN a feild	 ///

VECTOR3D	xForm::GetVector()	
{
	static float XRES_TINY = XResolution - 1 - 0.000004; // 0.000004 used because TINY was to small for the computer to register the difference 
	static float YRES_TINY = yres - 1 - 0.000004; // between for example, 0.999999 and 0.000000 
	static float ZRES_TINY = zres - 1 - 0.000004;

	int slice = XResolution * yres;

			float     r = 0.0;
			float     s = 0.0;
			float     t = 0.0;
			int       x = 0;
			int       y = 0;
			int       z = 0;
			int inCellX = 0;
			int inCellY = 0;
			int inCellZ = 0;

			x = int ( ArbitraryPoint.x );
			y = int ( ArbitraryPoint.y );
			z = int ( ArbitraryPoint.z );

			if( ArbitraryPoint.x > x )  //if the point is rounded down
			{
				r = ArbitraryPoint.x - x;
			}
			else // if the point is rounded up or not at all
			{
				r = 1 - ( ArbitraryPoint.x - x ) ;
			}

			inCellX = int ( ArbitraryPoint.x - r );

			if( ArbitraryPoint.y > y )  //if the point is rounded down
			{
				s = ArbitraryPoint.y - y;
			} 
			else // if the point is rounded up or not at all
			{
				s = 1 - ( ArbitraryPoint.y - y ) ;
			}
			
			inCellY = int ( ArbitraryPoint.y - s ) ;

			if( ArbitraryPoint.z > z )  //if the point is rounded down
			{
				t = ArbitraryPoint.z - z;
			}
			else // if the point is rounded up or not at all
			{
			 	t = 1 - ( ArbitraryPoint.z - z ) ;
			}
			
			inCellZ = int ( ArbitraryPoint.z - t ) ;

			if ( inCellX > XResolution - 1   ) return VECTOR3D( 0.0f, 0.0f, 0.0f );
			if ( inCellX < 0          ) return VECTOR3D( 0.0f, 0.0f, 0.0f );
			if ( inCellY > yres - 1   ) return VECTOR3D( 0.0f, 0.0f, 0.0f );
			if ( inCellY < 0          ) return VECTOR3D( 0.0f, 0.0f, 0.0f );
			if ( inCellZ > zres - 1   ) return VECTOR3D( 0.0f, 0.0f, 0.0f );
			if ( inCellZ < 0          ) return VECTOR3D( 0.0f, 0.0f, 0.0f );

			if ( ArbitraryPoint.x > XRES_TINY ) { ArbitraryPoint.x = XRES_TINY; inCellX = int( ArbitraryPoint.x ); }
			if ( ArbitraryPoint.y > YRES_TINY ) { ArbitraryPoint.y = YRES_TINY; inCellY = int( ArbitraryPoint.y ); }
			if ( ArbitraryPoint.z > ZRES_TINY ) { ArbitraryPoint.z = ZRES_TINY; inCellZ = int( ArbitraryPoint.z ); }

		float st = s * t;

		int		index0 = inCellZ * slice + inCellY * XResolution + inCellX;
		int		index1 = index0 + 1;
		int		index3 = index0 + XResolution;
		int		index2 = index3 + 1;
		int		index4 = index0 + slice;
		int		index5 = index1 + slice;
		int		index6 = index2 + slice;
		int		index7 = index3 + slice;

		return	Interpolate(  r , s , t , r * s, r * t , st , r * st , 
							 GlobalVectorData[ index0 ] , GlobalVectorData[ index1 ] , GlobalVectorData[ index2 ] , GlobalVectorData[ index3 ] ,
							 GlobalVectorData[ index4 ] , GlobalVectorData[ index5 ] , GlobalVectorData[ index6 ] , GlobalVectorData[ index7 ] );  
} 

// ---------------------------------------------------------------------------------------------------------------------------- Support Functions

VECTOR3D	Interpolate( float      r ,  float      s ,  float      t ,   							
						 float     rs ,  float     rt ,  float     st ,  float     rst ,	
						 VECTOR3D  v0 ,  VECTOR3D  v1 ,  VECTOR3D  v2 ,  VECTOR3D   v3 ,  
						 VECTOR3D  v4 ,  VECTOR3D  v5 ,  VECTOR3D  v6 ,  VECTOR3D   v7 )
{	
	return  v0 + ( v1 - v0 ) * r + 
				 ( v3 - v0 ) * s + 
				 ( v4 - v0 ) * t +
                 ( v0 + v2 - v1 - v3 ) * rs +
				 ( v0 + v5 - v1 - v4 ) * rt +
				 ( v0 + v7 - v3 - v4 ) * st +
				 ( v1 + v3 + v4 + v6 - v0 - v2 - v5 - v7 ) * rst;
}	

//--Cross Product---------------------------------------
//
//					   /-----------------\      /--\  
//					   |Uy * Vz - Uz * Vy|		|Wx|
//	U * V =	W    or    |Uz * Vx - Ux * Vz|  =   |Wy|
//					   |Ux * Vy - Uy * Vx|      |Wz|
//				       \-----------------/      \--/
//
//------------------------------------------------------
//                 INPUT   , INPUT    , OUTPUT
void CrossProductS(float U[], float V[], float N[])
{
	N[ 0 ]	= U[ 1 ] * V[ 2 ] -
		      U[ 2 ] * V[ 1 ] ;
			
	N[ 1 ]	= U[ 2 ] * V[ 0 ] - 
			  U[ 0 ] * V[ 2 ] ;

	N[ 2 ]	= U[ 0 ] * V[ 1 ] - 
			  U[ 1 ] * V[ 0 ] ;
}	

//Uses data supersampling 
void    SuperSampleVolumeVectorData2D
		//         In/Out           In/Out      
		( int   *  vol_xRes,  int * vol_yRes,
		//         In/Out           In
		  float * &vec_data,  int   scaleNum)
{
	// reassign values to a local variable to remove complexity of pointer in operations
	int old_xRes = *vol_xRes;
	int old_yRes = *vol_yRes;

	// generate xRes, yRes, zRes for ROI and assign it to the pointer previously holding the old xRes, yRes, zRes
	int new_xRes = ( old_xRes - 1 ) * scaleNum + 1;				//
	   *vol_xRes = ( old_xRes - 1 ) * scaleNum + 1;				//  1        2        3         4	o = old grid point
	int new_yRes = ( old_yRes - 1 ) * scaleNum + 1;				//	o--------x--------x---------o	x = new grid point ( based on scale number )
	   *vol_yRes = ( old_yRes - 1 ) * scaleNum + 1;				//	|		 |		  |         |


	int newSize = new_xRes * new_yRes; // Component index of the total size of ROI data
	newSize = ( newSize << 1 );

	int oldSize = old_xRes * old_yRes; // Component index of the total size of volume data
	oldSize = ( oldSize << 1 );

	float * oldtemp = (float*) malloc( sizeof(float) * oldSize );

	memcpy( oldtemp, vec_data, sizeof(float) * oldSize );  // copys old vector ROI data into a temporary array

    vec_data = (float*) realloc( vec_data, sizeof(float) * newSize );
	
	float * ratioVals  = new float [ scaleNum + 1 ];      // array to handle the ratios required to implement the new grid points

	//Set the ratios of the on cell interpolated points
	//Note!!	This is because floating point values may be saved as 0.9999 or 1.0000001 when using division
	ratioVals[ 0        ] = 0.0; 
	ratioVals[ scaleNum ] = 1.0;

	//Find the ratios of the in cell interpolated points
	for( int d = 1; d <= scaleNum - 1; d++ )		
	{
		ratioVals[ d ] = float( d ) / scaleNum;		// sets the interior ratios in the case of scale = 3 the ratios would be .33333 and .666666
	} 
	// Base poinT index  // These are used to navigate the the old array, and get the values from the orginial grid points
	int bt_i = 0;
	int bt_j = 0;

	int BT_I = 0;  // Because bt_i,j,k are manipulated this is nessecary to 
	int BT_J = 0;

	// In Cell index
	int ic_i = 0;
	int ic_j = 0;

		for ( int j = 0; j < new_yRes; j++)
		{
			bt_j = BT_J = j / scaleNum;
			ic_j =		  j % scaleNum;

			// Check j boundary
			if ( j == ( new_yRes - 1 ) )   // When j reaches the y boundary the move the index back one base point, and set the ratio to 1.0000
			{
				bt_j --;
				ic_j = scaleNum;
			}
			for ( int i = 0; i < new_xRes; i++ )
			{
				bt_i = BT_I = i / scaleNum;
				ic_i =		  i % scaleNum;
			
				//Check i boundary
				if ( i == ( new_xRes - 1 ))  // When x reaches the x boundary the move the index back one base point, and set the ratio to 1.0000
				{
					bt_i --;
					ic_i = scaleNum; 
				}

				int newIdx = ( i + j * new_xRes ) * 2;   // New index to navigate supersampled array

				if ( ic_i == 0  && ic_j == 0 )				// Checks the ratio value is at the base point of the cell
				{
					int oldIdx = ( BT_I + BT_J * old_xRes ) * 2;  // Old index to navigate the old array points

					vec_data[ newIdx     ] = oldtemp[ oldIdx     ];		// x-component of base point 
					vec_data[ newIdx + 1 ] = oldtemp[ oldIdx + 1 ];		// y-component of base point
				}
				else  // when the index is not at the base point
				{
					 Interpolate ( old_xRes, old_yRes, oldtemp, 
								   bt_i, bt_j, 
								   ratioVals[ ic_i ], ratioVals[ ic_j ], vec_data + newIdx );
				}
			}
		}
	free( oldtemp   );  oldtemp  = NULL;	
	free( ratioVals ); ratioVals = NULL;
}

/// 2D Interpolation functions

// Find a point within a 2D plane
//    
//     ^ y+
//     |					( 1 , 1 )	
//     +------------------------+
//     |            |           |
//     |            | a - 1     |
//     |            |           |
//     |   1 - b    |     b     |
//     |------------+-----------|
//     |            |           |
//     |            |a          |
//     |            |           |
//     +------------------------+-> x+
//  ( 0 , 0 )
//  ( a , b )

void Interpolate  
//		        input       input       input       input
		( int   xRes, int   yRes, float * A, 
//			input	  input		input
		  int   i, int   j, 
//			input     input    input     input
		  float r, float s, float * v )
	{
    	//Point index
		int index0 = i + j * xRes ; 

		int index1 = index0 + 1    ;   // x + 1   		
		int index3 = index0 + xRes ;   // y + 1	
		int index2 = index3 + 1    ;   // x + 1, y + 1     		
		
		// Component index Multiply by 2
		index0 = ( index0 << 1 );		
		index1 = ( index1 << 1 );		
		index2 = ( index2 << 1 ); 	    
		index3 = ( index3 << 1 );		

		// Allocate all the vectors into static arrays 
		float v0[ 2 ];				float v1[ 2 ];
		v0[ 0 ] = A[ index0     ];  	  v1[ 0 ] = A[ index1     ];
		v0[ 1 ] = A[ index0 + 1 ];		  v1[ 1 ] = A[ index1 + 1 ];


		float v2[ 2 ];				float v3[ 2 ];
		v2[ 0 ] = A[ index2     ];	      v3[ 0 ] = A[ index3     ];
		v2[ 1 ] = A[ index2 + 1 ];        v3[ 1 ] = A[ index3 + 1 ];

		Interpolate ( r, s, v, v0, v1, v2, v3 );
	}

	void Interpolate 
//				input		input		input		input
		( float    r, float    s,  float *  v,
//				input	    input       input       input
		  float * v0, float * v1, float * v2, float * v3 )
	{
	   float rs = r * s;

	   v[ 0 ] = Interpolate ( r, s, rs, v0[ 0 ], v1[ 0 ], v2[ 0 ], v3[ 0 ] ); 

	   v[ 1 ] = Interpolate ( r, s, rs, v0[ 1 ], v1[ 1 ], v2[ 1 ], v3[ 1 ] ); 
	}

	float Interpolate  
//			input	   input     input
		( float r , float s,  float rs,  
//			input	   input     input     input  
		  float x0, float x1, float x2, float x3 )
	{
		// Bi-Linear Interpolation of One Component
		//
		//   result = ( 1 - s ) * (( 1 - r ) * x0 + r * x1 ) +
		//		            s   * (( 1 - r ) * x3 + r * x2 ) ;
		//
		//---------------------------------------------------------------

		return     x0 + ( x1 - x0           ) * r  +
					    ( x3 - x0           ) * s  + 
					    ( x0 + x2 - x3 - x1 ) * rs ;
	}


void xForm::SetTexture( unsigned int texIndex )
( 
	glBindTexture( GL_TEXTURE_2D, texIndex );  // source: https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glBindTexture.xhtml
											   // source http://docs.gl/es2/glBindTexture
	//Set texture parameters:				   // source: OpenGL_SurfaceRendering_Ext____with_Texture_Mapping
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,     GL_REPEAT );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

	//We will next multiply the texture color with the surface color
    //glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	/// !!!MUST!!! /// to void conflict with the streamline texture
	
	//Pass the image pixels to OpenGL
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, plane_xRes, plane_yRes, 0, GL_RGB, GL_UNSIGNED_BYTE, texture );

	if( texture ) free( texture ); texture = NULL; // if this line
}
#ifndef XFORM
#define XFORM

#include "GLView.h"     // For boilerplate includes
#include "LicWrapper.h" // For the LIC functions

// Dependencies ---------------------------------
//
// GLView.h:
// Included to connect this class to ActiveFLOVE
// Along with a declaration of an xForm object, and the calling of setCoordinatePlane function
//
// LicWrapper.h:
// Used in setCoordinatePlane function
//
//----------------------------------------------- 
		

#ifndef _VECTOR2D_
#define _VECTOR2D_
typedef struct	tagVECTOR2D
{	
		float	vx;
		float	vy;

		// Constructors
		tagVECTOR2D()  {}
		tagVECTOR2D(int    vx0,  int    vy0)  {  vx = float(vx0);  vy = float(vy0);  }
		tagVECTOR2D(float  vx0,  float  vy0)  {  vx = vx0;         vy = vy0;         }

		// Magnitude
		double	Mag2()      {  return  double(vx * vx + vy * vy);  }
		float	Mag ()      {  return  float (   sqrt(  double(vx * vx + vy * vy)  )   );  }
		float	Fabs()		{  return  float ( fabs(vx) + fabs(vy) );  }

		// Normalization
		float	Normalize()	{  double  s;	 NORMINF   normInf = (  ( s = Mag2() ) < TINY2  ) ? 
											 NORMINF(0.0f, 0.0f) : NORMINF(   float(  sqrt(s)  )   );
							   vx  *=  normInf.nel;	   vy  *=  normInf.nel;   return  normInf.len;
							}
		void	Normalize (float  mag)  {  float  s = 1.0f / mag;  vx *= s;  vy *= s;  }
		void	Normalize2(double mg2)  {  float  s = 1.0f / float( sqrt(mg2) );  vx *= s;  vy *= s;  }
		void	Normalize2(double mg2,  int  dir)
										{  float  s = dir  / float( sqrt(mg2) );  vx *= s;  vy *= s;  }

		// Member functions
		int		IsZero()             {  return  ( (vx != 0.0f) || (vy != 0.0f) ) ? 0 : 1;  }
		void	Add(tagVECTOR2D  v)  {  vx += v.vx;  vy += v.vy;  }
		void	Sub(tagVECTOR2D  v)  {  vx -= v.vx;  vy -= v.vy;  }
		void	Mult(int    i)		 {  vx *= i;     vy *= i;     }
		void	Mult(float  f)	 	 {  vx *= f;     vy *= f;     }
		void	Div(int     i)		 {  float  f = 1.0f / i;  vx *= f;  vy *= f;  }
		void	Div(float   f)		 {         f = 1.0f / f;  vx *= f;  vy *= f;  }
		void	Zero()               {  vx = vy = 0.0f;           }
		float	xMult(tagVECTOR2D v) {  return  vx * v.vy - vy * v.vx; }
		tagVECTOR2D     Xpal()		 {  return  tagVECTOR2D(-vy, vx);  }

		// Operator overrides
		tagVECTOR2D	operator +  (tagVECTOR2D  v)  {  return  tagVECTOR2D(vx + v.vx, vy + v.vy);  }
		tagVECTOR2D	operator -  (tagVECTOR2D  v)  {  return  tagVECTOR2D(vx - v.vx, vy - v.vy);  }
		float		operator *  (tagVECTOR2D  v)  {  return  vx * v.vx + vy * v.vy;              }
		tagVECTOR2D	operator *  (int          i)  {  return  tagVECTOR2D(vx * i,    vy * i);     }
		tagVECTOR2D	operator *  (float        f)  {  return  tagVECTOR2D(vx * f,    vy * f);     }
		tagVECTOR2D	operator /  (int          i)  {  float   f = 1.0f / i;    return  tagVECTOR2D(vx * f, vy * f);     }
		tagVECTOR2D	operator /  (float        f)  {          f = 1.0f / f;    return  tagVECTOR2D(vx * f, vy * f);     }
		int			operator == (tagVECTOR2D  v)  {  return  ( (vx == v.vx) && (vy == v.vy) ) ? 1 : 0;  }
		int			operator != (tagVECTOR2D  v)  {  return  ( (vx != v.vx) || (vy != v.vy) ) ? 1 : 0;  }
		int			operator >  (tagVECTOR2D  v)  {  return  ( (vx >  v.vx) || (vy >  v.vy) ) ? 1 : 0;  }
		int			operator <  (tagVECTOR2D  v)  {  return  ( (vx <  v.vx) || (vy <  v.vy) ) ? 1 : 0;  }
		int			operator >> (tagVECTOR2D  v)  {  return  ( (vx >  v.vx) && (vy >  v.vy) ) ? 1 : 0;  }
		int			operator << (tagVECTOR2D  v)  {  return  ( (vx <  v.vx) && (vy <  v.vy) ) ? 1 : 0;  }
} 		VECTOR2D;
#endif

#ifndef _VECTOR3D_
#define _VECTOR3D_
typedef struct  tagVECTOR3D
{	
		float	vx;
		float	vy;
		float	vz;

		// Constructors
		tagVECTOR3D()   {  vx = vy = vz = 0.0f;  }
		tagVECTOR3D(int    vx0,  int    vy0,  int    vz0)   {  vx = float(vx0);  vy = float(vy0);  vz = float(vz0);  }
		tagVECTOR3D(float  vx0,  float  vy0,  float  vz0)   {  vx = vx0;         vy = vy0;         vz = vz0;  }

		// Magnitude
		double	Mag2()      {  return  double(vx * vx + vy * vy + vz * vz);  }
		float	Mag ()      {  return  float (  sqrt( double(vx * vx + vy * vy + vz * vz) )  );  }
		float	Fabs()		{  return  float ( fabs(vx) + fabs(vy) + fabs(vz) );  }

		// Normalization
		float	Normalize()	{  double  s;	 NORMINF   normInf = (  ( s = Mag2() ) < TINY2  ) ? 
											 NORMINF(0.0f, 0.0f) : NORMINF(   float(  sqrt(s)  )   );
							   vx  *=  normInf.nel;	   vy  *=  normInf.nel;   vz  *=  normInf.nel;	  return  normInf.len;
							}
		void	Normalize (float  mag)  {  float  s = 1.0f / mag;  vx *= s;  vy *= s;  vz *= s;  }
		void	Normalize2(double mg2)  {  float  s = 1.0f / float( sqrt(mg2) );  vx *= s;  vy *= s;  vz *= s;  }
		void	Normalize2(double mg2,  int  dir)
										{  float  s = dir  / float( sqrt(mg2) );  vx *= s;  vy *= s;  vz *= s;  }
		void	NegateVec() {  vx = -vx;  vy = -vy;  vz = -vz;  }

		// Member functions
		void	Add(tagVECTOR3D  v)  {  vx += v.vx;  vy += v.vy;  vz += v.vz;  }
		void	Sub(tagVECTOR3D  v)  {  vx -= v.vx;  vy -= v.vy;  vz -= v.vz;  }
		void	Mul(int          i)  {  vx *= i;     vy *= i;     vz *= i;     }
		void	Mul(float        f)  {  vx *= f;	 vy *= f;     vz *= f;     }
		void	Div(int          i)  {  float  f = 1.0f / i;  vx *= f;  vy *= f;  vz *= f;  }
		void	Div(float        f)  {         f = 1.0f / f;  vx *= f;  vy *= f;  vz *= f;  }
		void	Zero()               {  vx = vy = vz = 0.0f;                   }


		double	    dMult(tagVECTOR3D  v)  {  return  double(vx) * double(v.vx ) + double(vy) * double(v.vy ) + double(vz) * double(v.vz );  } 
		double		dMult(tagVECTOR3D* v)  {  return  double(vx) * double(v->vx) + double(vy) * double(v->vy) + double(vz) * double(v->vz);  }
		tagVECTOR3D xMult(tagVECTOR3D  v)
					{  return  tagVECTOR3D(vy * v.vz - vz * v.vy, vz * v.vx - vx * v.vz, vx * v.vy - vy * v.vx);  }
		tagVECTOR3D xMult(float  vx0,  float  vy0,  float  vz0)
					{  return
					tagVECTOR3D(vy * vz0  - vz * vy0,  vz * vx0  - vx * vz0,  vx * vy0  - vy * vx0 );  }

		// Operator overrides
		tagVECTOR3D	operator +  (tagVECTOR3D  v)  {  return  tagVECTOR3D(vx + v.vx, vy + v.vy, vz + v.vz);  }
		tagVECTOR3D	operator -  (tagVECTOR3D  v)  {  return  tagVECTOR3D(vx - v.vx, vy - v.vy, vz - v.vz);  }
		float		operator *  (tagVECTOR3D  v)  {  return  vx * v.vx + vy * v.vy + vz * v.vz;             }
		tagVECTOR3D	operator *  (int          i)  {  return  tagVECTOR3D(vx * i,    vy * i,    vz * i   );  }
		tagVECTOR3D	operator *  (float        f)  {  return  tagVECTOR3D(vx * f,    vy * f,    vz * f   );  }
		tagVECTOR3D	operator /  (int          i)  {  float   f = 1.0f / i;  return  tagVECTOR3D(vx * f, vy * f, vz * f);  }
		tagVECTOR3D	operator /  (float        f)  {          f = 1.0f / f;  return  tagVECTOR3D(vx * f, vy * f, vz * f);  }
		int			operator == (tagVECTOR3D  v)  {  return  ( (vx == v.vx) && (vy == v.vy) && (vz == v.vz) ) ? 1 : 0;  }
		int			operator != (tagVECTOR3D  v)  {  return  ( (vx != v.vx) || (vy != v.vy) || (vz != v.vz) ) ? 1 : 0;  }
		int			operator >  (tagVECTOR3D  v)  {  return  ( (vx >  v.vx) || (vy >  v.vy) || (vz >  v.vz) ) ? 1 : 0;  }
		int			operator <  (tagVECTOR3D  v)  {  return  ( (vx <  v.vx) || (vy <  v.vy) || (vz <  v.vz) ) ? 1 : 0;  }
		int			operator >> (tagVECTOR3D  v)  {  return  ( (vx >  v.vx) && (vy >  v.vy) && (vz >  v.vz) ) ? 1 : 0;  }
		int			operator << (tagVECTOR3D  v)  {  return  ( (vx <  v.vx) && (vy <  v.vy) && (vz <  v.vz) ) ? 1 : 0;  }
}		VECTOR3D;
#endif

#ifndef _POINT3D_
#define _POINT3D_
typedef struct  tagPOINT3D
{	
		float	x;
		float	y;
		float	z;

		// Constructors
		tagPOINT3D()  {  x = y = z = 0.0f;  }
		tagPOINT3D(int    i0,  int    j0,  int    k0)   {  x = float(i0);    y = float(j0);    z = float(k0);  }
		tagPOINT3D(float  x0,  float  y0,  float  z0)   {  x = x0;           y = y0;           z = z0;         }
		tagPOINT3D(float  x0,  float  y0)               {  x = x0;           y = y0;           z = 0.0f;       }
		tagPOINT3D(POINT2D   p)                         {  x = p.x;          y = p.y;          z = 0.0f;       }
		tagPOINT3D(VECTOR3D  v)                         {  x = v.vx;         y = v.vy;         z = v.vz;       }

		// Member functions
		void		GetMin(tagPOINT3D* p)  {  x = (x <= p->x) ? x : p->x;  y = (y <= p->y) ? y : p->y;  z = (z <= p->z) ? z : p->z;  }
		void		GetMax(tagPOINT3D* p)  {  x = (x >= p->x) ? x : p->x;  y = (y >= p->y) ? y : p->y;  z = (z >= p->z) ? z : p->z;  }
		void		GetMin(tagPOINT3D  p)  {  x = (x <= p.x ) ? x : p.x;   y = (y <= p.y ) ? y : p.y;   z = (z <= p.z ) ? z : p.z;   }
		void		GetMax(tagPOINT3D  p)  {  x = (x >= p.x ) ? x : p.x;   y = (y >= p.y ) ? y : p.y;   z = (z >= p.z ) ? z : p.z;   }
		void		Add(tagPOINT3D  p)  {  x += p.x;  y += p.y;  z += p.z;  }
		void		Add(VECTOR3D    v)  {  x += v.vx; y += v.vy; z += v.vz; }
		VECTOR3D	Sub(tagPOINT3D  p)  {  return     VECTOR3D(x - p.x, y - p.y, z - p.z);  }
		void		Sub(VECTOR3D    v)  {  x -= v.vx; y -= v.vy; z -= v.vz; }
		void		Mul(int		    i)  {  x *= i;    y *= i;    z *= i;    }
		void		Mul(float  	    f)  {  x *= f;    y *= f;    z *= f;    }
		void		Div(int         i)  {  float  f = 1.0f / i;  x *= f;  y *= f;  z *= f;  }
		void		Div(float       f)  {         f = 1.0f / f;  x *= f;  y *= f;  z *= f;  }
		VECTOR3D	vector()			{  return VECTOR3D(x, y, z);        }
		float		Dist2(tagPOINT3D  p){  float  vx = x - p.x;   float  vy = y - p.y;   float  vz = z - p.z;
										   return vx * vx + vy * vy + vz * vz;              }
		float		Dist2(tagPOINT3D* p){  float  vx = x - p->x;  float  vy = y - p->y;  float  vz = z - p->z;
										   return vx * vx + vy * vy + vz * vz;              }

		// Operator overrides
		tagPOINT3D  operator +  (tagPOINT3D  p)  {  return  tagPOINT3D(x + p.x,  y + p.y,  z + p.z );  }
		tagPOINT3D  operator +  (VECTOR3D    v)  {  return  tagPOINT3D(x + v.vx, y + v.vy, z + v.vz);  }
		VECTOR3D    operator -  (tagPOINT3D  p)  {  return  VECTOR3D  (x - p.x,  y - p.y,  z - p.z );  }
		tagPOINT3D  operator -  (VECTOR3D    v)  {  return  tagPOINT3D(x - v.vx, y - v.vy, z - v.vz);  }
		tagPOINT3D  operator *  (int         i)  {  return  tagPOINT3D(x * i,    y * i,    z * i   );  }
		tagPOINT3D  operator *  (float       f)  {  return  tagPOINT3D(x * f,    y * f,    z * f   );  }
		tagPOINT3D  operator /  (int         i)  {  float   f = 1.0f / i;  return  tagPOINT3D(x * f,  y * f,  z * f);  }
		tagPOINT3D  operator /  (float       f)  {          f = 1.0f / f;  return  tagPOINT3D(x * f,  y * f,  z * f);  }
		int			operator == (tagPOINT3D  p)  {  return  ( (x == p.x) && (y == p.y) && (z == p.z) ) ? 1 : 0;  }
		int			operator != (tagPOINT3D  p)  {  return  ( (x != p.x) || (y != p.y) || (z != p.z) ) ? 1 : 0;  }
		int			operator >  (tagPOINT3D  p)  {  return  ( (x >  p.x) || (y >  p.y) || (z >  p.z) ) ? 1 : 0;  }
		int			operator <  (tagPOINT3D  p)  {  return  ( (x <  p.x) || (y <  p.y) || (z <  p.z) ) ? 1 : 0;  }
		int			operator >> (tagPOINT3D  p)  {  return  ( (x >  p.x) && (y >  p.y) && (z >  p.z) ) ? 1 : 0;  }
		int			operator << (tagPOINT3D  p)  {  return  ( (x <  p.x) && (y <  p.y) && (z <  p.z) ) ? 1 : 0;  }
}		POINT3D;
#endif


class xForm
{
public:

	 xForm(); // Defualt Constructor
	 xForm( VECTOR3D viewDir, 
	         POINT3D userPos, 
			   float distUsr, 
		      VECTOR3D upwrd, 
		 VECTOR3D * vec_data, 
		           int xreso, 
				   int yreso, 
				   int zreso, 
				   int u, 
				   int v );  // NON-Default Constructor 
	
	~xForm();  // Deconstructor

private:

	// Cutting plane dimensions
     int plane_xResolution; 
	 int plane_yResolution;

	float RotationMatrix[ 9 ];              // Rotational Matrix
	float InverseRotationMatrix[ 9 ];       // Inverse Rotatinal Matrix
	float TransformationMatrix[ 9 ];	    // Transformation Matrix
	float InverseTransformationMatrix[ 9 ]; // Inverse Transformation Matrix
	float ForwardMatrix[ 16 ];              // Forward Matrix
	float BackwardMatrix[ 16 ];             // Backward Matrix
  
	// Corner points for the cutting plane

	// Cutting Plane
	// p +------+ s
	//   |      |
	//   |      |
	// q +------+ r
	POINT3D p; // (0,1)
	POINT3D q; // (0,0)
	POINT3D r; // (1,0)
	POINT3D s; // (1,1)

	POINT3D    LocalSystemOrigin;   // coordinates for the Origin of the LOCal SYStem

	VECTOR3D   UpVector;            // UP VECToR
	VECTOR3D   ViewDirectionVector; // VIEWing DIRection
	float      Distance2User;       // DISTance between USeR and cutting plane

	POINT3D    UserPosition;       // USER POSition
	POINT3D    ArbitraryPoint;     // Arbitrary POiNT within the cutting plane

	VECTOR3D * GlobalVectorData;   // Global VECtor DATa ( pVector )

	// global system dimensions
	int XResolution;  
	int YResolution;
	int ZResolution;
	
	unsigned char * texture; 

public:
	
	//Getter functions for each corner
	 POINT3D GetCornerP() { return p; }
	 POINT3D GetCornerQ() { return q; }
	 POINT3D GetCornerR() { return r; }
	 POINT3D GetCornerS() { return s; }

	//Transformation functions
	 POINT3D transferLocl2GlbPoint  (  POINT3D  LocPoint ); 
	 POINT3D transferGlb2LoclPoint  (  POINT3D  GlbPoint );
	VECTOR3D transferLocl2GlbVectr  ( VECTOR3D  LocVectr );
	VECTOR3D transferGlb2LoclVectr  ( VECTOR3D  GlbVectr );

	// Sets the cutting plane 
	void setCuttingPlane(); 

	void SetTexture( unsigned int texIndex );

private:

	void Init();

	void TMatrix();
	void RMatrix();
	void MMatrix();

	void viewDirectin() { ViewDirectionVector.vx = 0.0000;
						  ViewDirectionVector.vy = 0.0000; 
						  ViewDirectionVector.vz = 0.0000; }  //viewDir.vx = v.vx; viewDir.vy = v.vy; viewDir.vz = v.vz;
	void userPosition() { UserPosition.x  = 0.0000; 
					      UserPosition.y  = 0.0000; 
						  UserPosition.z  = 0.0000; }
	void distanceUser() { Distance2User    = 5.0000;                                      }
	void getUpVector () { UpVector.vx = 0.0000; 
						  UpVector.vy = 1.0000; 
						  UpVector.vz = 0.0000; }  //UpVectr.vx = upwrd.vx; UpVectr.vy = upwrd.vy; UpVectr.vz = upwrd.vz; 

	void getOriginofLocalSystem() { ViewDirectionVector.Normalize();
									LocalSystemOrigin.x = UserPosition.x + ( Distance2User * ViewDirectionVector.vx );
									LocalSystemOrigin.y = UserPosition.y + ( Distance2User * ViewDirectionVector.vy );
									LocalSystemOrigin.z = UserPosition.z + ( Distance2User * ViewDirectionVector.vz ); }		
	void ConstructAFrameRef();

	VECTOR3D GetVector();

	void  InvokeLICWrapper( float * img_data ); // contains all nessecary functions for using the vector data
};

#endif
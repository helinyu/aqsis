
// Aqsis
// Copyright � 1997 - 2001, Paul C. Gregory
//
// Contact: pgregory@aqsis.com
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public
// License as published by the Free Software Foundation; either
// version 2 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


/** \file
		\brief Implements the classes and support structures for handling RenderMan patch primitives.
		\author Paul C. Gregory (pgregory@aqsis.com)
*/

#include	<math.h>

#include	"aqsis.h"
#include	"imagebuffer.h"
#include	"micropolygon.h"
#include	"renderer.h"
#include	"patch.h"
#include	"vector2d.h"

START_NAMESPACE( Aqsis )


//---------------------------------------------------------------------
/** Constructor both u and vbasis matrices default to bezier.
 */

CqSurfacePatchBicubic::CqSurfacePatchBicubic() : CqSurface()
{}


//---------------------------------------------------------------------
/** Copy constructor.
 */

CqSurfacePatchBicubic::CqSurfacePatchBicubic( const CqSurfacePatchBicubic& From ) :
		CqSurface( From )
{
	*this = From;
}


//---------------------------------------------------------------------
/** Destructor.
 */

CqSurfacePatchBicubic::~CqSurfacePatchBicubic()
{}


//---------------------------------------------------------------------
/** Evaluate a bicubic spline patch at the specified intervals.
 * \param s Float interval.
 * \param t Float interval.
 * \return CqVector4D surface position.
 */

CqVector4D CqSurfacePatchBicubic::Evaluate( TqFloat s, TqFloat t ) const
{
	// Set up the geometry vector.
	CqMatrix	Gx;
	CqMatrix	Gy;
	CqMatrix	Gz;

	GetGeometryMatrices( s, t, Gx, Gy, Gz );

	return ( EvaluateMatrix( s, t, Gx, Gy, Gz ) );
}

//---------------------------------------------------------------------
/** Build the geometry matrices Gx,Gy,Gz for the patch.
 */

void CqSurfacePatchBicubic::GetGeometryMatrices( TqFloat& s, TqFloat &t, CqMatrix& Gx, CqMatrix& Gy, CqMatrix& Gz ) const
{
	const CqVector4D & P11 = P() [ 0 ];
	const CqVector4D&	P21 = P() [ 1 ];
	const CqVector4D&	P31 = P() [ 2 ];
	const CqVector4D&	P41 = P() [ 3 ];
	const CqVector4D&	P12 = P() [ 4 ];
	const CqVector4D&	P22 = P() [ 5 ];
	const CqVector4D&	P32 = P() [ 6 ];
	const CqVector4D&	P42 = P() [ 7 ];
	const CqVector4D&	P13 = P() [ 8 ];
	const CqVector4D&	P23 = P() [ 9 ];
	const CqVector4D&	P33 = P() [ 10 ];
	const CqVector4D&	P43 = P() [ 11 ];
	const CqVector4D&	P14 = P() [ 12 ];
	const CqVector4D&	P24 = P() [ 13 ];
	const CqVector4D&	P34 = P() [ 14 ];
	const CqVector4D&	P44 = P() [ 15 ];

	Gx[ 0 ][ 0 ] = P11.x();	Gx[ 0 ][ 1 ] = P12.x();	Gx[ 0 ][ 2 ] = P13.x();	Gx[ 0 ][ 3 ] = P14.x();
	Gx[ 1 ][ 0 ] = P21.x();	Gx[ 1 ][ 1 ] = P22.x();	Gx[ 1 ][ 2 ] = P23.x();	Gx[ 1 ][ 3 ] = P24.x();
	Gx[ 2 ][ 0 ] = P31.x();	Gx[ 2 ][ 1 ] = P32.x();	Gx[ 2 ][ 2 ] = P33.x();	Gx[ 2 ][ 3 ] = P34.x();
	Gx[ 3 ][ 0 ] = P41.x();	Gx[ 3 ][ 1 ] = P42.x();	Gx[ 3 ][ 2 ] = P43.x();	Gx[ 3 ][ 3 ] = P44.x();

	Gy[ 0 ][ 0 ] = P11.y();	Gy[ 0 ][ 1 ] = P12.y();	Gy[ 0 ][ 2 ] = P13.y();	Gy[ 0 ][ 3 ] = P14.y();
	Gy[ 1 ][ 0 ] = P21.y();	Gy[ 1 ][ 1 ] = P22.y();	Gy[ 1 ][ 2 ] = P23.y();	Gy[ 1 ][ 3 ] = P24.y();
	Gy[ 2 ][ 0 ] = P31.y();	Gy[ 2 ][ 1 ] = P32.y();	Gy[ 2 ][ 2 ] = P33.y();	Gy[ 2 ][ 3 ] = P34.y();
	Gy[ 3 ][ 0 ] = P41.y();	Gy[ 3 ][ 1 ] = P42.y();	Gy[ 3 ][ 2 ] = P43.y();	Gy[ 3 ][ 3 ] = P44.y();

	Gz[ 0 ][ 0 ] = P11.z();	Gz[ 0 ][ 1 ] = P12.z();	Gz[ 0 ][ 2 ] = P13.z();	Gz[ 0 ][ 3 ] = P14.z();
	Gz[ 1 ][ 0 ] = P21.z();	Gz[ 1 ][ 1 ] = P22.z();	Gz[ 1 ][ 2 ] = P23.z();	Gz[ 1 ][ 3 ] = P24.z();
	Gz[ 2 ][ 0 ] = P31.z();	Gz[ 2 ][ 1 ] = P32.z();	Gz[ 2 ][ 2 ] = P33.z();	Gz[ 2 ][ 3 ] = P34.z();
	Gz[ 3 ][ 0 ] = P41.z();	Gz[ 3 ][ 1 ] = P42.z();	Gz[ 3 ][ 2 ] = P43.z();	Gz[ 3 ][ 3 ] = P44.z();
}

//---------------------------------------------------------------------
/** Evaluate a bicubic spline patch at the specified intervals, given the geometry matrices.
 * \param s Float interval.
 * \param t Float interval.
 * \param Gx Geometry matrix for x.
 * \param Gy Geometry matrix for y.
 * \param Gz Geometry matrix for z.
 * \return CqVector4D surface position.
 */

CqVector4D CqSurfacePatchBicubic::EvaluateMatrix( TqFloat s, TqFloat t, CqMatrix& Gx, CqMatrix& Gy, CqMatrix& Gz ) const
{
	TqFloat t2 = t * t;
	TqFloat t3 = t2 * t;
	TqFloat s2 = s * s;
	TqFloat s3 = s2 * s;

	CqVector4D	T;						// T Transpose
	T[ 0 ] = t3;	T[ 1 ] = t2;	T[ 2 ] = t;	T[ 3 ] = 1;
	CqVector4D	S;						// S
	S[ 0 ] = s3;	S[ 1 ] = s2;	S[ 2 ] = s;	S[ 3 ] = 1;

	CqMatrix	matvBasisT;
	matvBasisT = pAttributes() ->GetMatrixAttribute("System", "Basis")[1].Transpose();

	CqVector4D	vecResult;

	vecResult = T * matvBasisT * Gx * pAttributes() ->GetMatrixAttribute("System", "Basis")[0] * S;

	return ( vecResult );
}


//---------------------------------------------------------------------
/** Initialise the forward differencing variables given the current geometry matrix.
 */

void CqSurfacePatchBicubic::InitFD( TqInt cu, TqInt cv,
                                    CqMatrix&	matDDx,
                                    CqMatrix&	matDDy,
                                    CqMatrix&	matDDz,
                                    CqVector4D&	DDxA,
                                    CqVector4D&	DDyA,
                                    CqVector4D&	DDzA )
{
	CqMatrix	Gx;
	CqMatrix	Gy;
	CqMatrix	Gz;

	TqFloat	s, t;
	GetGeometryMatrices( s, t, Gx, Gy, Gz );
	Gx.SetfIdentity( TqFalse );
	Gy.SetfIdentity( TqFalse );
	Gz.SetfIdentity( TqFalse );

	TqFloat ud = 1.0 / static_cast<TqFloat>( cu );
	TqFloat	ud2 = ud * ud;
	TqFloat	ud3 = ud2 * ud;

	TqFloat vd = 1.0 / static_cast<TqFloat>( cv );
	TqFloat	vd2 = vd * vd;
	TqFloat	vd3 = vd2 * vd;

	TqFloat	EuValues[ 16 ] = {
	                             0, 0, 0, 1,
	                             ud3, ud2, ud, 0,
	                             6 * ud3, 2 * ud2, 0, 0,
	                             6 * ud3, 0, 0, 0
	                         };
	CqMatrix	matEu( EuValues );

	TqFloat	EvValues[ 16 ] = {
	                             0, vd3, 6 * vd3, 6 * vd3,
	                             0, vd2, 2 * vd2, 0,
	                             0, vd, 0, 0,
	                             1, 0, 0, 0
	                         };
	CqMatrix	matEvT( EvValues );

	CqMatrix	matvBasisT;
	matvBasisT = pAttributes() ->GetMatrixAttribute("System", "Basis")[1].Transpose();

	const CqMatrix& matuBasis = pAttributes() ->GetMatrixAttribute("System", "Basis")[0];
	Gx = matvBasisT * Gx * matuBasis;
	Gy = matvBasisT * Gy * matuBasis;
	Gz = matvBasisT * Gz * matuBasis;

	matDDx = matEvT * Gx * matEu;
	matDDy = matEvT * Gy * matEu;
	matDDz = matEvT * Gz * matEu;

	DDxA[ 0 ] = matDDx[ 0 ][ 0 ]; DDxA[ 1 ] = matDDx[ 1 ][ 0 ]; DDxA[ 2 ] = matDDx[ 2 ][ 0 ]; DDxA[ 3 ] = matDDx[ 3 ][ 0 ];
	DDyA[ 0 ] = matDDy[ 0 ][ 0 ]; DDyA[ 1 ] = matDDy[ 1 ][ 0 ]; DDyA[ 2 ] = matDDy[ 2 ][ 0 ]; DDyA[ 3 ] = matDDy[ 3 ][ 0 ];
	DDzA[ 0 ] = matDDz[ 0 ][ 0 ]; DDzA[ 1 ] = matDDz[ 1 ][ 0 ]; DDzA[ 2 ] = matDDz[ 2 ][ 0 ]; DDzA[ 3 ] = matDDz[ 3 ][ 0 ];
}


//---------------------------------------------------------------------
/** Evaluate the mext iteration of the forward difference variables.
 */

CqVector4D	CqSurfacePatchBicubic::EvaluateFD( CqMatrix&	matDDx,
        CqMatrix&	matDDy,
        CqMatrix&	matDDz,
        CqVector4D&	DDxA,
        CqVector4D&	DDyA,
        CqVector4D&	DDzA )
{
	CqVector4D	vecResult( DDxA[ 0 ], DDyA[ 0 ], DDzA[ 0 ], 1 );

	DDxA[ 0 ] += DDxA[ 1 ]; DDxA[ 1 ] += DDxA[ 2 ]; DDxA[ 2 ] += DDxA[ 3 ];
	DDyA[ 0 ] += DDyA[ 1 ]; DDyA[ 1 ] += DDyA[ 2 ]; DDyA[ 2 ] += DDyA[ 3 ];
	DDzA[ 0 ] += DDzA[ 1 ]; DDzA[ 1 ] += DDzA[ 2 ]; DDzA[ 2 ] += DDzA[ 3 ];

	return ( vecResult );
}


//---------------------------------------------------------------------
/** Evaluate the mext iteration of the forward difference variables.
 */

void	CqSurfacePatchBicubic::AdvanceFD( CqMatrix&	matDDx,
                                       CqMatrix&	matDDy,
                                       CqMatrix&	matDDz,
                                       CqVector4D&	DDxA,
                                       CqVector4D&	DDyA,
                                       CqVector4D&	DDzA )
{
	// Row1 = Row1 + Row2
	matDDx[ 0 ][ 0 ] += matDDx[ 0 ][ 1 ]; matDDx[ 1 ][ 0 ] += matDDx[ 1 ][ 1 ]; matDDx[ 2 ][ 0 ] += matDDx[ 2 ][ 1 ]; matDDx[ 3 ][ 0 ] += matDDx[ 3 ][ 1 ];
	matDDy[ 0 ][ 0 ] += matDDy[ 0 ][ 1 ]; matDDy[ 1 ][ 0 ] += matDDy[ 1 ][ 1 ]; matDDy[ 2 ][ 0 ] += matDDy[ 2 ][ 1 ]; matDDy[ 3 ][ 0 ] += matDDy[ 3 ][ 1 ];
	matDDz[ 0 ][ 0 ] += matDDz[ 0 ][ 1 ]; matDDz[ 1 ][ 0 ] += matDDz[ 1 ][ 1 ]; matDDz[ 2 ][ 0 ] += matDDz[ 2 ][ 1 ]; matDDz[ 3 ][ 0 ] += matDDz[ 3 ][ 1 ];

	// Row2 = Row2 + Row3
	matDDx[ 0 ][ 1 ] += matDDx[ 0 ][ 2 ]; matDDx[ 1 ][ 1 ] += matDDx[ 1 ][ 2 ]; matDDx[ 2 ][ 1 ] += matDDx[ 2 ][ 2 ]; matDDx[ 3 ][ 1 ] += matDDx[ 3 ][ 2 ];
	matDDy[ 0 ][ 1 ] += matDDy[ 0 ][ 2 ]; matDDy[ 1 ][ 1 ] += matDDy[ 1 ][ 2 ]; matDDy[ 2 ][ 1 ] += matDDy[ 2 ][ 2 ]; matDDy[ 3 ][ 1 ] += matDDy[ 3 ][ 2 ];
	matDDz[ 0 ][ 1 ] += matDDz[ 0 ][ 2 ]; matDDz[ 1 ][ 1 ] += matDDz[ 1 ][ 2 ]; matDDz[ 2 ][ 1 ] += matDDz[ 2 ][ 2 ]; matDDz[ 3 ][ 1 ] += matDDz[ 3 ][ 2 ];

	// Row3 = Row3 + Row4
	matDDx[ 0 ][ 2 ] += matDDx[ 0 ][ 3 ]; matDDx[ 1 ][ 2 ] += matDDx[ 1 ][ 3 ]; matDDx[ 2 ][ 2 ] += matDDx[ 2 ][ 3 ]; matDDx[ 3 ][ 2 ] += matDDx[ 3 ][ 3 ];
	matDDy[ 0 ][ 2 ] += matDDy[ 0 ][ 3 ]; matDDy[ 1 ][ 2 ] += matDDy[ 1 ][ 3 ]; matDDy[ 2 ][ 2 ] += matDDy[ 2 ][ 3 ]; matDDy[ 3 ][ 2 ] += matDDy[ 3 ][ 3 ];
	matDDz[ 0 ][ 2 ] += matDDz[ 0 ][ 3 ]; matDDz[ 1 ][ 2 ] += matDDz[ 1 ][ 3 ]; matDDz[ 2 ][ 2 ] += matDDz[ 2 ][ 3 ]; matDDz[ 3 ][ 2 ] += matDDz[ 3 ][ 3 ];

	DDxA[ 0 ] = matDDx[ 0 ][ 0 ]; DDxA[ 1 ] = matDDx[ 1 ][ 0 ]; DDxA[ 2 ] = matDDx[ 2 ][ 0 ]; DDxA[ 3 ] = matDDx[ 3 ][ 0 ];
	DDyA[ 0 ] = matDDy[ 0 ][ 0 ]; DDyA[ 1 ] = matDDy[ 1 ][ 0 ]; DDyA[ 2 ] = matDDy[ 2 ][ 0 ]; DDyA[ 3 ] = matDDy[ 3 ][ 0 ];
	DDzA[ 0 ] = matDDz[ 0 ][ 0 ]; DDzA[ 1 ] = matDDz[ 1 ][ 0 ]; DDzA[ 2 ] = matDDz[ 2 ][ 0 ]; DDzA[ 3 ] = matDDz[ 3 ][ 0 ];
}


//---------------------------------------------------------------------
/** Assignment operator.
 */

CqSurfacePatchBicubic& CqSurfacePatchBicubic::operator=( const CqSurfacePatchBicubic& From )
{
	// Perform per surface copy function
	CqSurface::operator=( From );

	//	TqInt i;
	//	for(i=0; i<16; i++)
	//		P()[i]=From.P()[i];

	return ( *this );
}


//---------------------------------------------------------------------
/** Subdivide a bicubic patch in the u direction, return the left side.
 */

void CqSurfacePatchBicubic::uSubdivide( CqSurfacePatchBicubic* pNew1, CqSurfacePatchBicubic* pNew2 )
{
	TqFloat	aDlb[ 16 ] = {
	                         8, 0, 0, 0,
	                         4, 4, 0, 0,
	                         2, 4, 2, 0,
	                         1, 3, 3, 1
	                     };

	TqFloat	aDrb[ 16 ] = {
	                         1, 3, 3, 1,
	                         0, 2, 4, 2,
	                         0, 0, 4, 4,
	                         0, 0, 0, 8
	                     };

	CqMatrix	Dlb( aDlb );
	CqMatrix	Drb( aDrb );
	Dlb = ( 1.0f / 8.0f ) * Dlb;
	Drb = ( 1.0f / 8.0f ) * Drb;

	pNew1->P().SetSize( cVertex() );
	pNew2->P().SetSize( cVertex() );

	// Create the geometry matrix and storage area for the left and right split matrices.
	CqMatrix	G;
	CqMatrix	Gl;
	CqMatrix	Gr;

	TqInt i;
	for ( i = 0; i < 4; i++ )
	{
		G[ 0 ][ 0 ] = CP( i, 0 ).x();	G[ 0 ][ 1 ] = CP( i, 0 ).y();	G[ 0 ][ 2 ] = CP( i, 0 ).z();	G[ 0 ][ 3 ] = CP( i, 0 ).h();
		G[ 1 ][ 0 ] = CP( i, 1 ).x();	G[ 1 ][ 1 ] = CP( i, 1 ).y();	G[ 1 ][ 2 ] = CP( i, 1 ).z();	G[ 1 ][ 3 ] = CP( i, 1 ).h();
		G[ 2 ][ 0 ] = CP( i, 2 ).x();	G[ 2 ][ 1 ] = CP( i, 2 ).y();	G[ 2 ][ 2 ] = CP( i, 2 ).z();	G[ 2 ][ 3 ] = CP( i, 2 ).h();
		G[ 3 ][ 0 ] = CP( i, 3 ).x();	G[ 3 ][ 1 ] = CP( i, 3 ).y();	G[ 3 ][ 2 ] = CP( i, 3 ).z();	G[ 3 ][ 3 ] = CP( i, 3 ).h();
		G.SetfIdentity( TqFalse );

		Gl = ( G * Dlb );
		Gr = ( G * Drb );

		pNew1->CP( i, 0 ) = CqVector4D( Gl[ 0 ][ 0 ], Gl[ 0 ][ 1 ], Gl[ 0 ][ 2 ], 1 );
		pNew1->CP( i, 1 ) = CqVector4D( Gl[ 1 ][ 0 ], Gl[ 1 ][ 1 ], Gl[ 1 ][ 2 ], 1 );
		pNew1->CP( i, 2 ) = CqVector4D( Gl[ 2 ][ 0 ], Gl[ 2 ][ 1 ], Gl[ 2 ][ 2 ], 1 );
		pNew1->CP( i, 3 ) = CqVector4D( Gl[ 3 ][ 0 ], Gl[ 3 ][ 1 ], Gl[ 3 ][ 2 ], 1 );

		pNew2->CP( i, 0 ) = CqVector4D( Gr[ 0 ][ 0 ], Gr[ 0 ][ 1 ], Gr[ 0 ][ 2 ], 1 );
		pNew2->CP( i, 1 ) = CqVector4D( Gr[ 1 ][ 0 ], Gr[ 1 ][ 1 ], Gr[ 1 ][ 2 ], 1 );
		pNew2->CP( i, 2 ) = CqVector4D( Gr[ 2 ][ 0 ], Gr[ 2 ][ 1 ], Gr[ 2 ][ 2 ], 1 );
		pNew2->CP( i, 3 ) = CqVector4D( Gr[ 3 ][ 0 ], Gr[ 3 ][ 1 ], Gr[ 3 ][ 2 ], 1 );
	}

	// Subdivide the normals
	if ( USES( Uses(), EnvVars_N ) && bHasN() ) 
	{
		pNew1->N() = N();
		pNew1->N().uSubdivide( &pNew2->N() );
	}

	uSubdivideUserParameters( pNew1, pNew2 );
}


//---------------------------------------------------------------------
/** Subdivide a bicubic patch in the v direction, return the top side.
 */

void CqSurfacePatchBicubic::vSubdivide( CqSurfacePatchBicubic* pNew1, CqSurfacePatchBicubic* pNew2 )
{
	TqFloat	aDlb[ 16 ] = {
	                         8, 0, 0, 0,
	                         4, 4, 0, 0,
	                         2, 4, 2, 0,
	                         1, 3, 3, 1
	                     };

	TqFloat	aDrb[ 16 ] = {
	                         1, 3, 3, 1,
	                         0, 2, 4, 2,
	                         0, 0, 4, 4,
	                         0, 0, 0, 8
	                     };

	CqMatrix	Dlb( aDlb );
	CqMatrix	Drb( aDrb );
	Dlb = ( 1.0f / 8.0f ) * Dlb;
	Drb = ( 1.0f / 8.0f ) * Drb;

	// Create the geometry matrix and storage area for the left and right split matrices.
	CqMatrix	G;
	CqMatrix	Gl;
	CqMatrix	Gr;

	pNew1->P().SetSize( cVertex() );
	pNew2->P().SetSize( cVertex() );

	TqInt i;
	for ( i = 0; i < 4; i++ )
	{
		G[ 0 ][ 0 ] = CP( 0, i ).x();	G[ 0 ][ 1 ] = CP( 0, i ).y();	G[ 0 ][ 2 ] = CP( 0, i ).z();	G[ 0 ][ 3 ] = CP( 0, i ).h();
		G[ 1 ][ 0 ] = CP( 1, i ).x();	G[ 1 ][ 1 ] = CP( 1, i ).y();	G[ 1 ][ 2 ] = CP( 1, i ).z();	G[ 1 ][ 3 ] = CP( 1, i ).h();
		G[ 2 ][ 0 ] = CP( 2, i ).x();	G[ 2 ][ 1 ] = CP( 2, i ).y();	G[ 2 ][ 2 ] = CP( 2, i ).z();	G[ 2 ][ 3 ] = CP( 2, i ).h();
		G[ 3 ][ 0 ] = CP( 3, i ).x();	G[ 3 ][ 1 ] = CP( 3, i ).y();	G[ 3 ][ 2 ] = CP( 3, i ).z();	G[ 3 ][ 3 ] = CP( 3, i ).h();
		G.SetfIdentity( TqFalse );

		Gl = ( G * Dlb );
		Gr = ( G * Drb );

		pNew1->CP( 0, i ) = CqVector4D( Gl[ 0 ][ 0 ], Gl[ 0 ][ 1 ], Gl[ 0 ][ 2 ], 1 );
		pNew1->CP( 1, i ) = CqVector4D( Gl[ 1 ][ 0 ], Gl[ 1 ][ 1 ], Gl[ 1 ][ 2 ], 1 );
		pNew1->CP( 2, i ) = CqVector4D( Gl[ 2 ][ 0 ], Gl[ 2 ][ 1 ], Gl[ 2 ][ 2 ], 1 );
		pNew1->CP( 3, i ) = CqVector4D( Gl[ 3 ][ 0 ], Gl[ 3 ][ 1 ], Gl[ 3 ][ 2 ], 1 );

		pNew2->CP( 0, i ) = CqVector4D( Gr[ 0 ][ 0 ], Gr[ 0 ][ 1 ], Gr[ 0 ][ 2 ], 1 );
		pNew2->CP( 1, i ) = CqVector4D( Gr[ 1 ][ 0 ], Gr[ 1 ][ 1 ], Gr[ 1 ][ 2 ], 1 );
		pNew2->CP( 2, i ) = CqVector4D( Gr[ 2 ][ 0 ], Gr[ 2 ][ 1 ], Gr[ 2 ][ 2 ], 1 );
		pNew2->CP( 3, i ) = CqVector4D( Gr[ 3 ][ 0 ], Gr[ 3 ][ 1 ], Gr[ 3 ][ 2 ], 1 );
	}

	// Subdivide the normals
	if ( USES( Uses(), EnvVars_N ) && bHasN() ) 
	{
		pNew1->N() = N();
		pNew1->N().vSubdivide( &pNew2->N() );
	}

	vSubdivideUserParameters( pNew1, pNew2 );
}


//---------------------------------------------------------------------
/** Get the boundary extents in camera space of the surface patch
 */

CqBound CqSurfacePatchBicubic::Bound() const
{
	// Get the boundary in camera space.
	CqVector3D	vecA( FLT_MAX, FLT_MAX, FLT_MAX );
	CqVector3D	vecB( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	TqInt i;
	for ( i = 0; i < 16; i++ )
	{
		CqVector3D	vecV = P() [ i ];
		if ( vecV.x() < vecA.x() ) vecA.x( vecV.x() );
		if ( vecV.y() < vecA.y() ) vecA.y( vecV.y() );
		if ( vecV.x() > vecB.x() ) vecB.x( vecV.x() );
		if ( vecV.y() > vecB.y() ) vecB.y( vecV.y() );
		if ( vecV.z() < vecA.z() ) vecA.z( vecV.z() );
		if ( vecV.z() > vecB.z() ) vecB.z( vecV.z() );
	}
	CqBound	B;
	B.vecMin() = vecA;
	B.vecMax() = vecB;
	return ( B );
}


//---------------------------------------------------------------------
/** Dice the patch into a mesh of micropolygons.
 */

void CqSurfacePatchBicubic::NaturalInterpolate(CqParameter* pParameter, TqInt uDiceSize, TqInt vDiceSize, IqShaderData* pData)
{
	// \note: Only to avoid problems with not used warnings, remove when proper implementation in place.
	pParameter = pParameter;

	// NOTE: This violates thread safety, look into this.
	CqMatrix	matDDx;
	CqMatrix	matDDy;
	CqMatrix	matDDz;
	CqVector4D	DDxA;
	CqVector4D	DDyA;
	CqVector4D	DDzA;

	// Initialise the forward difference variables.
	InitFD( uDiceSize, vDiceSize, matDDx, matDDy, matDDz, DDxA, DDyA, DDzA );

	TqInt iv, iu;
	for ( iv = 0; iv <= vDiceSize; iv++ )
	{
		for ( iu = 0; iu <= uDiceSize; iu++ )
		{
			TqInt igrid = ( iv * ( uDiceSize + 1 ) ) + iu;
			pData->SetPoint( static_cast<CqVector3D>( EvaluateFD( matDDx, matDDy, matDDz, DDxA, DDyA, DDzA ) ), igrid );
		}
		AdvanceFD( matDDx, matDDy, matDDz, DDxA, DDyA, DDzA );
	}
}

//---------------------------------------------------------------------
/** Split the patch into smaller patches.
 */

TqInt CqSurfacePatchBicubic::Split( std::vector<CqBasicSurface*>& aSplits )
{
	TqInt cSplits = 0;

	// Split the surface in u or v
	CqSurfacePatchBicubic * pNew1 = new CqSurfacePatchBicubic;
	CqSurfacePatchBicubic * pNew2 = new CqSurfacePatchBicubic;

	// If this primitive is being split because it spans the e and hither planes, then
	// we should split in both directions to ensure we overcome the crossing.
	if ( m_SplitDir == SplitDir_U )
		uSubdivide(pNew1, pNew2);
	else
		vSubdivide(pNew1, pNew2);

	pNew1->SetSurfaceParameters( *this );
	pNew2->SetSurfaceParameters( *this );
	pNew1->m_fDiceable = TqTrue;
	pNew2->m_fDiceable = TqTrue;
	pNew1->m_SplitDir = (m_SplitDir == SplitDir_U)? SplitDir_V:SplitDir_U;
	pNew2->m_SplitDir = (m_SplitDir == SplitDir_U)? SplitDir_V:SplitDir_U;
	pNew1->m_EyeSplitCount = m_EyeSplitCount;
	pNew2->m_EyeSplitCount = m_EyeSplitCount;
	pNew1->AddRef();
	pNew2->AddRef();

	if ( !m_fDiceable)
	{
		cSplits += pNew1->Split( aSplits );
		cSplits += pNew2->Split( aSplits );
		pNew1->Release();
		pNew2->Release();
	}
	else
	{
		aSplits.push_back( pNew1 );
		aSplits.push_back( pNew2 );

		cSplits += 2;
	}
	return ( cSplits );
}

//---------------------------------------------------------------------
/** Determine whether or not the patch is diceable
 */

TqBool	CqSurfacePatchBicubic::Diceable()
{
	// If the cull check showed that the primitive cannot be diced due to crossing the e and hither planes,
	// then we can return immediately.
	if ( !m_fDiceable )
		return ( TqFalse );

	// Otherwise we should continue to try to find the most advantageous split direction, OR the dice size.
	const CqMatrix & matCtoR = QGetRenderContext() ->matSpaceToSpace( "camera", "raster" );

	// Convert the control hull to raster space.
	CqVector2D	avecHull[ 16 ];
	TqInt i;
	TqInt gridsize;

	const TqInt* poptGridSize = QGetRenderContext() ->optCurrent().GetIntegerOption( "limits", "gridsize" );
	TqInt m_XBucketSize = 16;
	TqInt m_YBucketSize = 16;
	const TqInt* poptBucketSize = QGetRenderContext() ->optCurrent().GetIntegerOption( "limits", "bucketsize" );
	if ( poptBucketSize != 0 )
	{
		m_XBucketSize = poptBucketSize[ 0 ];
		m_YBucketSize = poptBucketSize[ 1 ];
	}
	TqFloat ShadingRate = pAttributes() ->GetFloatAttribute("System", "ShadingRate")[0];
	if ( poptGridSize )
		gridsize = poptGridSize[ 0 ];
	else
		gridsize = static_cast<TqInt>( m_XBucketSize * m_XBucketSize / ShadingRate );
	for ( i = 0; i < 16; i++ )
		avecHull[ i ] = matCtoR * P() [ i ];

	// First check flatness, a curve which is too far off flat will
	// produce unreliable results when the length is approximated below.
	m_SplitDir = SplitDir_U;
	TqInt u;
	for ( u = 0; u < 16; u += 4 )
	{
		// Find an initial line
		TqFloat Len = 0;
		CqVector2D	vec0 = avecHull[ u ];
		CqVector2D	vecL;
		TqInt i = 4;
		while ( i-- > 0 && Len < FLT_EPSILON )
		{
			vecL = avecHull[ u + i ] - vec0;
			Len = vecL.Magnitude();
		}
		vecL /= Len;	// Normalise

		i = 0;
		while ( i++ < 4 )
		{
			// Get the distance to the line for each point
			CqVector3D	vec = avecHull[ u + i ] - vec0;
			vec.Unit();
			vec %= vecL;
			if ( vec.Magnitude() > 1 ) return ( TqFalse );
		}
	}
	m_SplitDir = SplitDir_V;
	TqInt v;
	for ( v = 0; v < 4; v++ )
	{
		// Find an initial line
		TqFloat Len = 0;
		CqVector2D	vec0 = avecHull[ v ];
		CqVector2D	vecL;
		TqInt i = 4;
		while ( i-- > 0 && Len < FLT_EPSILON )
		{
			vecL = avecHull[ v + ( i * 4 ) ] - vec0;
			Len = vecL.Magnitude();
		}
		vecL /= Len;	// Normalise

		i = 0;
		while ( i++ < 4 )
		{
			// Get the distance to the line for each point
			CqVector3D	vec = avecHull[ v + ( i * 4 ) ] - vec0;
			vec.Unit();
			vec %= vecL;
			if ( vec.Magnitude() > 1 ) return ( TqFalse );
		}
	}


	TqFloat uLen = 0;
	TqFloat vLen = 0;

	for ( u = 0; u < 16; u += 4 )
	{
		CqVector2D	Vec1 = avecHull[ u + 1 ] - avecHull[ u ];
		CqVector2D	Vec2 = avecHull[ u + 2 ] - avecHull[ u + 1 ];
		CqVector2D	Vec3 = avecHull[ u + 3 ] - avecHull[ u + 2 ];
		if ( Vec1.Magnitude2() > uLen ) uLen = Vec1.Magnitude2();
		if ( Vec2.Magnitude2() > uLen ) uLen = Vec2.Magnitude2();
		if ( Vec3.Magnitude2() > uLen ) uLen = Vec3.Magnitude2();
	}
	for ( v = 0; v < 4; v++ )
	{
		CqVector2D	Vec1 = avecHull[ v + 4 ] - avecHull[ v ];
		CqVector2D	Vec2 = avecHull[ v + 8 ] - avecHull[ v + 4 ];
		CqVector2D	Vec3 = avecHull[ v + 12 ] - avecHull[ v + 8 ];
		if ( Vec1.Magnitude2() > vLen ) vLen = Vec1.Magnitude2();
		if ( Vec2.Magnitude2() > vLen ) vLen = Vec2.Magnitude2();
		if ( Vec3.Magnitude2() > vLen ) vLen = Vec3.Magnitude2();
	}

	ShadingRate = static_cast<float>( sqrt( ShadingRate ) );
	uLen = sqrt( uLen ) / ShadingRate;
	vLen = sqrt( vLen ) / ShadingRate;

	m_SplitDir = ( uLen > vLen ) ? SplitDir_U : SplitDir_V;
	// TODO: Should ensure powers of half to prevent cracking.
	uLen *= 3;
	vLen *= 3;
	m_uDiceSize = static_cast<TqInt>( MAX( ROUND( uLen ), 1 ) );
	m_vDiceSize = static_cast<TqInt>( MAX( ROUND( vLen ), 1 ) );

	// Ensure power of 2 to avoid cracking
	m_uDiceSize = CEIL_POW2(m_uDiceSize);
	m_vDiceSize = CEIL_POW2(m_vDiceSize);

	TqFloat Area = m_uDiceSize * m_vDiceSize;

	if ( uLen < FLT_EPSILON || vLen < FLT_EPSILON )
	{
		m_fDiscard = TqTrue;
		return ( TqFalse );
	}

	if ( fabs( Area ) > gridsize )
		return ( TqFalse );

	return ( TqTrue );
}


//---------------------------------------------------------------------
/** Transform the patch by the specified matrix.
 */

void	CqSurfacePatchBicubic::Transform( const CqMatrix& matTx, const CqMatrix& matITTx, const CqMatrix& matRTx )
{
	// Tansform the control hull by the specified matrix.
	TqInt i;
	for ( i = 0; i < 16; i++ )
		P() [ i ] = matTx * P() [ i ];
}


//---------------------------------------------------------------------
/** Constructor.
 */

CqSurfacePatchBilinear::CqSurfacePatchBilinear() : CqSurface()
{}


//---------------------------------------------------------------------
/** Copy constructor.
 */

CqSurfacePatchBilinear::CqSurfacePatchBilinear( const CqSurfacePatchBilinear& From ) :
		CqSurface( From )
{
	*this = From;
}


//---------------------------------------------------------------------
/** Destructor.
 */

CqSurfacePatchBilinear::~CqSurfacePatchBilinear()
{}


//---------------------------------------------------------------------
/** Evaluate a bilinear spline patch normal at the specified intervals.
 */

CqVector4D CqSurfacePatchBilinear::EvaluateNormal( TqFloat s, TqFloat t ) const
{
	CqVector3D vecNAB, vecNCD;
	// Work out where the u points are first, then linear interpolate the v value.
	if ( s <= 0.0 )
	{
		vecNAB = N() [ 0 ];
		vecNCD = N() [ 2 ];
	}
	else
	{
		if ( s >= 1.0 )
		{
			vecNAB = N() [ 1 ];
			vecNCD = N() [ 3 ];
		}
		else
		{
			vecNAB = ( N() [ 1 ] * s ) + ( N() [ 0 ] * ( 1.0 - s ) );
			vecNCD = ( N() [ 3 ] * s ) + ( N() [ 2 ] * ( 1.0 - s ) );
		}
	}

	CqVector3D vecN;
	if ( t <= 0.0 )
		vecN = vecNAB;
	else
	{
		if ( t >= 1.0 )
			vecN = vecNCD;
		else
			vecN = ( vecNCD * t ) + ( vecNAB * ( 1.0 - t ) );
	}

	return ( vecN );
}


//---------------------------------------------------------------------
/** Assignment operator.
 */

CqSurfacePatchBilinear& CqSurfacePatchBilinear::operator=( const CqSurfacePatchBilinear& From )
{
	CqSurface::operator=( From );

	return ( *this );
}


//---------------------------------------------------------------------
/** Generate the vertex normals if not specified.
 */

void CqSurfacePatchBilinear::GenerateGeometricNormals( TqInt uDiceSize, TqInt vDiceSize, IqShaderData* pNormals )
{
	assert( P().Size() == 4 );
	N().SetSize( 4 );

	// Get the handedness of the coordinate system (at the time of creation) and
	// the coordinate system specified, to check for normal flipping.
	TqInt O = pAttributes() ->GetIntegerAttribute("System", "Orientation")[0];

	// For each of the four points, calculate the normal as the cross product of its
	// two vectors.
	N() [ 0 ] = ( P() [ 1 ] - P() [ 0 ] ) % ( P() [ 2 ] - P() [ 0 ] );
	N() [ 1 ] = ( P() [ 3 ] - P() [ 1 ] ) % ( P() [ 0 ] - P() [ 1 ] );
	N() [ 2 ] = ( P() [ 0 ] - P() [ 2 ] ) % ( P() [ 3 ] - P() [ 2 ] );
	N() [ 3 ] = ( P() [ 2 ] - P() [ 3 ] ) % ( P() [ 1 ] - P() [ 3 ] );

	CqVector3D	N;
	TqInt v, u;
	for ( v = 0; v <= vDiceSize; v++ )
	{
		for ( u = 0; u <= uDiceSize; u++ )
		{
			TqInt igrid = ( v * ( uDiceSize + 1 ) ) + u;
			N = EvaluateNormal( u, v );
			N = ( O == OrientationLH )? N : -N;
			pNormals->SetNormal( N, igrid );
		}
	}
}


//---------------------------------------------------------------------
/** Subdivide a bicubic patch in the u direction, return the left side.
 */

void CqSurfacePatchBilinear::uSubdivide( CqSurfacePatchBilinear* pNew1, CqSurfacePatchBilinear* pNew2 )
{
	pNew1->P().SetSize( cVertex() );
	pNew2->P().SetSize( cVertex() );
	// Subdivide the vertices
	pNew1->P() = P();
	pNew2->P() = P();
	pNew1->P().uSubdivide( &pNew2->P() );

	// Subdivide the normals
	if ( USES( Uses(), EnvVars_N ) && bHasN() ) 
	{
		pNew1->N() = N();
		pNew1->N().uSubdivide( &pNew2->N() );
	}

	uSubdivideUserParameters( pNew1, pNew2 );
}


//---------------------------------------------------------------------
/** Subdivide a bicubic patch in the v direction, return the top side.
 */

void CqSurfacePatchBilinear::vSubdivide( CqSurfacePatchBilinear* pNew1, CqSurfacePatchBilinear* pNew2 )
{
	pNew1->P().SetSize( cVertex() );
	pNew2->P().SetSize( cVertex() );
	// Subdivide the vertices.
	pNew1->P() = P();
	pNew2->P() = P();
	pNew1->P().vSubdivide( &pNew2->P() );

	// Subdivide the normals
	if ( USES( Uses(), EnvVars_N ) && bHasN() ) 
	{
		pNew1->N() = N();
		pNew1->N().vSubdivide( &pNew2->N() );
	}

	vSubdivideUserParameters( pNew1, pNew2 );
}


//---------------------------------------------------------------------
/** Return the boundary extents in camera space of the surface patch
 */

CqBound CqSurfacePatchBilinear::Bound() const
{
	// Get the boundary in camera space.
	CqVector3D	vecA( FLT_MAX, FLT_MAX, FLT_MAX );
	CqVector3D	vecB( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	TqInt i;
	for ( i = 0; i < 4; i++ )
	{
		CqVector3D	vecV = P() [ i ];
		if ( vecV.x() < vecA.x() ) vecA.x( vecV.x() );
		if ( vecV.y() < vecA.y() ) vecA.y( vecV.y() );
		if ( vecV.x() > vecB.x() ) vecB.x( vecV.x() );
		if ( vecV.y() > vecB.y() ) vecB.y( vecV.y() );
		if ( vecV.z() < vecA.z() ) vecA.z( vecV.z() );
		if ( vecV.z() > vecB.z() ) vecB.z( vecV.z() );
	}
	CqBound	B;
	B.vecMin() = vecA;
	B.vecMax() = vecB;
	return ( B );
}


//---------------------------------------------------------------------
/** Dice the patch into a mesh of micropolygons.
 */

void CqSurfacePatchBilinear::NaturalInterpolate(CqParameter* pParameter, TqInt uDiceSize, TqInt vDiceSize, IqShaderData* pData)
{
	// \note: Only to avoid problems with not used warnings, remove when proper implementation in place.
	pParameter = pParameter;

	TqFloat diu = 1.0 / uDiceSize;
	TqFloat div = 1.0 / vDiceSize;

	TqInt iv, iu;
	for ( iv = 0; iv <= vDiceSize; iv++ )
	{
		for ( iu = 0; iu <= uDiceSize; iu++ )
		{
			TqInt igrid = ( iv * ( uDiceSize + 1 ) ) + iu;
			pData->SetPoint( static_cast<CqVector3D>( BilinearEvaluate<CqVector4D>( P() [ 0 ], P() [ 1 ], P() [ 2 ], P() [ 3 ], iu * diu, iv * div ) ), igrid );
		}
	}
}


//---------------------------------------------------------------------
/** Split the patch into smaller patches.
 */

TqInt CqSurfacePatchBilinear::Split( std::vector<CqBasicSurface*>& aSplits )
{
	TqInt cSplits = 0;

	// Split the surface in u or v
	CqSurfacePatchBilinear* pNew1 = new CqSurfacePatchBilinear;
	CqSurfacePatchBilinear* pNew2 = new CqSurfacePatchBilinear;

	if ( m_SplitDir == SplitDir_U )
		uSubdivide(pNew1, pNew2);
	else
		vSubdivide(pNew1, pNew2);

	pNew1->SetSurfaceParameters( *this );
	pNew2->SetSurfaceParameters( *this );
	pNew1->m_fDiceable = TqTrue;
	pNew2->m_fDiceable = TqTrue;
	pNew1->m_SplitDir = (m_SplitDir == SplitDir_U)? SplitDir_V:SplitDir_U;
	pNew2->m_SplitDir = (m_SplitDir == SplitDir_U)? SplitDir_V:SplitDir_U;
	pNew1->m_EyeSplitCount = m_EyeSplitCount;
	pNew2->m_EyeSplitCount = m_EyeSplitCount;
	pNew1->AddRef();
	pNew2->AddRef();

	if( !m_fDiceable )
	{
		cSplits += pNew1->Split( aSplits );
		cSplits += pNew2->Split( aSplits );
		pNew1->Release();
		pNew2->Release();
	}
	else
	{
		aSplits.push_back( pNew1 );
		aSplits.push_back( pNew2 );

		cSplits += 2;
	}

	return ( cSplits );
}


//---------------------------------------------------------------------
/** Determine whether or not the patch is diceable
 */

TqBool	CqSurfacePatchBilinear::Diceable()
{
	// If the cull check showed that the primitive cannot be diced due to crossing the e and hither planes,
	// then we can return immediately.
	if ( !m_fDiceable )
		return ( TqFalse );

	// Otherwise we should continue to try to find the most advantageous split direction, OR the dice size.
	const CqMatrix & matCtoR = QGetRenderContext() ->matSpaceToSpace( "camera", "raster" );

	// Convert the control hull to raster space.
	CqVector2D	avecHull[ 4 ];
	TqInt i;
	TqInt gridsize;

	const TqInt* poptGridSize = QGetRenderContext() ->optCurrent().GetIntegerOption( "limits", "gridsize" );
	TqInt m_XBucketSize = 16;
	TqInt m_YBucketSize = 16;
	const TqInt* poptBucketSize = QGetRenderContext() ->optCurrent().GetIntegerOption( "limits", "bucketsize" );
	if ( poptBucketSize != 0 )
	{
		m_XBucketSize = poptBucketSize[ 0 ];
		m_YBucketSize = poptBucketSize[ 1 ];
	}
	TqFloat ShadingRate = pAttributes() ->GetFloatAttribute("System", "ShadingRate")[0];
	if ( poptGridSize )
		gridsize = poptGridSize[ 0 ];
	else
		gridsize = static_cast<TqInt>( m_XBucketSize * m_XBucketSize / ShadingRate );
	for ( i = 0; i < 4; i++ )
		avecHull[ i ] = matCtoR * P() [ i ];

	TqFloat uLen = 0;
	TqFloat vLen = 0;

	CqVector2D	Vec1 = avecHull[ 1 ] - avecHull[ 0 ];
	CqVector2D	Vec2 = avecHull[ 3 ] - avecHull[ 2 ];
	uLen = ( Vec1.Magnitude2() > Vec2.Magnitude2() ) ? Vec1.Magnitude2() : Vec2.Magnitude2();

	Vec1 = avecHull[ 2 ] - avecHull[ 0 ];
	Vec2 = avecHull[ 3 ] - avecHull[ 1 ];
	vLen = ( Vec1.Magnitude2() > Vec2.Magnitude2() ) ? Vec1.Magnitude2() : Vec2.Magnitude2();

	ShadingRate = static_cast<float>( sqrt( ShadingRate ) );
	uLen = sqrt( uLen ) / ShadingRate;
	vLen = sqrt( vLen ) / ShadingRate;

	m_SplitDir = ( uLen > vLen ) ? SplitDir_U : SplitDir_V;

	// TODO: Should ensure powers of half to prevent cracking.
	uLen = MAX( ROUND( uLen ), 1 );
	vLen = MAX( ROUND( vLen ), 1 );
	TqFloat Area = uLen * vLen;
	m_uDiceSize = static_cast<TqInt>( uLen );
	m_vDiceSize = static_cast<TqInt>( vLen );

	// Ensure power of 2 to avoid cracking
	m_uDiceSize = CEIL_POW2(m_uDiceSize);
	m_vDiceSize = CEIL_POW2(m_vDiceSize);

	if ( uLen < FLT_EPSILON || vLen < FLT_EPSILON )
	{
		m_fDiscard = TqTrue;
		return ( TqFalse );
	}

	if ( fabs( Area ) > gridsize )
		return ( TqFalse );

	return ( TqTrue );
}


//---------------------------------------------------------------------
/** Transform the patch by the specified matrix.
 */

void	CqSurfacePatchBilinear::Transform( const CqMatrix& matTx, const CqMatrix& matITTx, const CqMatrix& matRTx )
{
	// Tansform the control hull by the specified matrix.
	TqInt i;
	for ( i = 0; i < 4; i++ )
	{
		P() [ i ] = matTx * P() [ i ];
		if ( N().Size() == 4 ) N() [ i ] = matITTx * N() [ i ];
	}
}


//---------------------------------------------------------------------
/** Copy constructor.
 */

CqSurfacePatchMeshBicubic::CqSurfacePatchMeshBicubic( const CqSurfacePatchMeshBicubic& From ) :
		CqSurface( From )
{
	*this = From;
}


//---------------------------------------------------------------------
/** Destructor.
 */

CqSurfacePatchMeshBicubic::~CqSurfacePatchMeshBicubic()
{}


//---------------------------------------------------------------------
/** Assignment operator.
 */

CqSurfacePatchMeshBicubic& CqSurfacePatchMeshBicubic::operator=( const CqSurfacePatchMeshBicubic& From )
{
	// Perform per surface copy function
	CqSurface::operator=( From );

	m_uPatches = From.m_uPatches;
	m_vPatches = From.m_vPatches;
	m_nu = From.m_nu;
	m_nv = From.m_nv;
	m_uPeriodic = From.m_uPeriodic;
	m_vPeriodic = From.m_vPeriodic;

	return ( *this );
}


//---------------------------------------------------------------------
/** Get the boundary extents in camera space of the surface patch mesh
 */

CqBound CqSurfacePatchMeshBicubic::Bound() const
{
	// Get the boundary in camera space.
	CqVector3D	vecA( FLT_MAX, FLT_MAX, FLT_MAX );
	CqVector3D	vecB( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	TqUint i;
	for ( i = 0; i < P().Size(); i++ )
	{
		CqVector3D	vecV = P() [ i ];
		if ( vecV.x() < vecA.x() ) vecA.x( vecV.x() );
		if ( vecV.y() < vecA.y() ) vecA.y( vecV.y() );
		if ( vecV.x() > vecB.x() ) vecB.x( vecV.x() );
		if ( vecV.y() > vecB.y() ) vecB.y( vecV.y() );
		if ( vecV.z() < vecA.z() ) vecA.z( vecV.z() );
		if ( vecV.z() > vecB.z() ) vecB.z( vecV.z() );
	}
	CqBound	B;
	B.vecMin() = vecA;
	B.vecMax() = vecB;
	return ( B );
}


//---------------------------------------------------------------------
/** Transform the patch by the specified matrix.
 */

void CqSurfacePatchMeshBicubic::Transform( const CqMatrix& matTx, const CqMatrix& matITTx, const CqMatrix& matRTx )
{
	// Tansform the control hull by the specified matrix.
	TqUint i;
	for ( i = 0; i < P().Size(); i++ )
		P() [ i ] = matTx * P() [ i ];
}


//---------------------------------------------------------------------
/** Split the patch mesh into individual patches and post them.
 */

#define	PatchCoord(v,u)	((((v)%m_nv)*m_nu)+((u)%m_nu))
#define	PatchCorner(v,u)	((((v)%nvaryingv)*nvaryingu)+((u)%nvaryingu));

TqInt CqSurfacePatchMeshBicubic::Split( std::vector<CqBasicSurface*>& aSplits )
{
	TqInt cSplits = 0;

	CqVector4D vecPoint;
	RtInt iP = 0;
	RtInt uStep = pAttributes() ->GetIntegerAttribute("System", "BasisStep")[0];
	RtInt vStep = pAttributes() ->GetIntegerAttribute("System", "BasisStep")[1];

	RtInt nvaryingu = ( m_uPeriodic ) ? m_uPatches : m_uPatches + 1;
	RtInt nvaryingv = ( m_vPeriodic ) ? m_vPatches : m_vPatches + 1;

	RtFloat up, vp;
	RtFloat du = 1.0 / m_uPatches;
	RtFloat dv = 1.0 / m_vPatches;

	TqInt MyUses = Uses();

	const TqFloat* pTC = pAttributes() ->GetFloatAttribute("System", "TextureCoordinates");
	CqVector2D st1( pTC[0], pTC[1]);
	CqVector2D st2( pTC[2], pTC[3]);
	CqVector2D st3( pTC[4], pTC[5]);
	CqVector2D st4( pTC[6], pTC[7]);

	vp = 0;
	// Fill in the points
	TqInt i;
	for ( i = 0; i < m_vPatches; i++ )
	{
		up = 0;
		// vRow is the coordinate row of the mesh.
		RtInt	vRow = i * vStep;
		RtInt j;
		for ( j = 0; j < m_uPatches; j++ )
		{
			// uCol is the coordinate column of the mesh.
			RtInt uCol = j * uStep;
			CqSurfacePatchBicubic*	pSurface = new CqSurfacePatchBicubic();
			pSurface->AddRef();
			pSurface->SetSurfaceParameters( *this );
//			pSurface->SetDefaultPrimitiveVariables();
			pSurface->P().SetSize( pSurface->cVertex() );
			RtInt v;
			for ( v = 0; v < 4; v++ )
			{
				iP = PatchCoord( vRow + v, uCol );
				pSurface->P() [ ( v * 4 ) ] = P() [ iP ];
				iP = PatchCoord( vRow + v, uCol + 1 );
				pSurface->P() [ ( v * 4 ) + 1 ] = P() [ iP ];
				iP = PatchCoord( vRow + v, uCol + 2 );
				pSurface->P() [ ( v * 4 ) + 2 ] = P() [ iP ];
				iP = PatchCoord( vRow + v, uCol + 3 );
				pSurface->P() [ ( v * 4 ) + 3 ] = P() [ iP ];
			}

			RtInt iTa = PatchCorner( i, j );
			RtInt iTb = PatchCorner( i, j + 1 );
			RtInt iTc = PatchCorner( i + 1, j );
			RtInt iTd = PatchCorner( i + 1, j + 1 );

			// Copy any user specified primitive variables.
			std::vector<CqParameter*>::iterator iUP;
			for( iUP = aUserParams().begin(); iUP != aUserParams().end(); iUP++ )
			{
				CqParameter* pNewUP = (*iUP)->Clone();
				pNewUP->Clear();
				pNewUP->SetSize(4);
				switch( (*iUP)->Type() )
				{
					case type_float:
					{
						CqParameterTyped<TqFloat, TqFloat>* pTParam = static_cast<CqParameterTyped<TqFloat, TqFloat>*>(*iUP);
						CqParameterTyped<TqFloat, TqFloat>* pTNewUP = static_cast<CqParameterTyped<TqFloat, TqFloat>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_integer:
					{
						CqParameterTyped<TqInt, TqFloat>* pTParam = static_cast<CqParameterTyped<TqInt, TqFloat>*>(*iUP);
						CqParameterTyped<TqInt, TqFloat>* pTNewUP = static_cast<CqParameterTyped<TqInt, TqFloat>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_point:
					case type_normal:
					case type_vector:
					{
						CqParameterTyped<CqVector3D, CqVector3D>* pTParam = static_cast<CqParameterTyped<CqVector3D, CqVector3D>*>(*iUP);
						CqParameterTyped<CqVector3D, CqVector3D>* pTNewUP = static_cast<CqParameterTyped<CqVector3D, CqVector3D>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_hpoint:
					{
						CqParameterTyped<CqVector4D, CqVector3D>* pTParam = static_cast<CqParameterTyped<CqVector4D, CqVector3D>*>(*iUP);
						CqParameterTyped<CqVector4D, CqVector3D>* pTNewUP = static_cast<CqParameterTyped<CqVector4D, CqVector3D>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_color:
					{
						CqParameterTyped<CqColor, CqColor>* pTParam = static_cast<CqParameterTyped<CqColor, CqColor>*>(*iUP);
						CqParameterTyped<CqColor, CqColor>* pTNewUP = static_cast<CqParameterTyped<CqColor, CqColor>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_string:
					{
						CqParameterTyped<CqString, CqString>* pTParam = static_cast<CqParameterTyped<CqString, CqString>*>(*iUP);
						CqParameterTyped<CqString, CqString>* pTNewUP = static_cast<CqParameterTyped<CqString, CqString>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_matrix:
					{
						CqParameterTyped<CqMatrix, CqMatrix>* pTParam = static_cast<CqParameterTyped<CqMatrix, CqMatrix>*>(*iUP);
						CqParameterTyped<CqMatrix, CqMatrix>* pTNewUP = static_cast<CqParameterTyped<CqMatrix, CqMatrix>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;
				}
				pSurface->AddPrimitiveVariable(pNewUP);
			}

			// Fill in the surface parameters for the mesh.
			if ( USES( MyUses, EnvVars_u ) && bHasu() )
			{
				pSurface->u()->SetSize( 4 );
				(*pSurface->u())[ 0 ] = up;
				(*pSurface->u())[ 1 ] = up + du;
				(*pSurface->u())[ 2 ] = up;
				(*pSurface->u())[ 3 ] = up + du;
			}

			if ( USES( MyUses, EnvVars_v ) && bHasv() )
			{
 				pSurface->v()->SetSize( 4 );
				(*pSurface->v())[ 0 ] = vp;
				(*pSurface->v())[ 1 ] = vp;
				(*pSurface->v())[ 2 ] = vp + dv;
				(*pSurface->v())[ 3 ] = vp + dv;
			}

			up += du;

			aSplits.push_back( pSurface );
			cSplits++;
		}
		vp += dv;
	}
	return ( cSplits );
}


//---------------------------------------------------------------------
/** Copy constructor.
 */

CqSurfacePatchMeshBilinear::CqSurfacePatchMeshBilinear( const CqSurfacePatchMeshBilinear& From ) :
		CqSurface( From )
{
	*this = From;
}


//---------------------------------------------------------------------
/** Destructor.
 */

CqSurfacePatchMeshBilinear::~CqSurfacePatchMeshBilinear()
{}


//---------------------------------------------------------------------
/** Assignment operator.
 */

CqSurfacePatchMeshBilinear& CqSurfacePatchMeshBilinear::operator=( const CqSurfacePatchMeshBilinear& From )
{
	// Perform per surface copy function
	CqSurface::operator=( From );

	m_uPatches = From.m_uPatches;
	m_vPatches = From.m_vPatches;
	m_nu = From.m_nu;
	m_nv = From.m_nv;
	m_uPeriodic = From.m_uPeriodic;
	m_vPeriodic = From.m_vPeriodic;

	return ( *this );
}


//---------------------------------------------------------------------
/** Get the boundary extents in camera space of the surface patch mesh
 */

CqBound CqSurfacePatchMeshBilinear::Bound() const
{
	// Get the boundary in camera space.
	CqVector3D	vecA( FLT_MAX, FLT_MAX, FLT_MAX );
	CqVector3D	vecB( -FLT_MAX, -FLT_MAX, -FLT_MAX );
	TqUint i;
	for ( i = 0; i < P().Size(); i++ )
	{
		CqVector3D	vecV = P() [ i ];
		if ( vecV.x() < vecA.x() ) vecA.x( vecV.x() );
		if ( vecV.y() < vecA.y() ) vecA.y( vecV.y() );
		if ( vecV.x() > vecB.x() ) vecB.x( vecV.x() );
		if ( vecV.y() > vecB.y() ) vecB.y( vecV.y() );
		if ( vecV.z() < vecA.z() ) vecA.z( vecV.z() );
		if ( vecV.z() > vecB.z() ) vecB.z( vecV.z() );
	}
	CqBound	B;
	B.vecMin() = vecA;
	B.vecMax() = vecB;
	return ( B );
}


//---------------------------------------------------------------------
/** Transform the patch by the specified matrix.
 */

void CqSurfacePatchMeshBilinear::Transform( const CqMatrix& matTx, const CqMatrix& matITTx, const CqMatrix& matRTx )
{
	// Tansform the control hull by the specified matrix.
	TqUint i;
	for ( i = 0; i < P().Size(); i++ )
		P() [ i ] = matTx * P() [ i ];
}


//---------------------------------------------------------------------
/** Split the patch mesh into individual patches and post them.
 */

#define	PatchCoord(v,u)	((((v)%m_nv)*m_nu)+((u)%m_nu))
#define	PatchCorner(v,u)	((((v)%nvaryingv)*nvaryingu)+((u)%nvaryingu));

TqInt CqSurfacePatchMeshBilinear::Split( std::vector<CqBasicSurface*>& aSplits )
{
	TqInt cSplits = 0;

	// Create a surface patch
	RtInt iP = 0;

	RtFloat up, vp;
	RtFloat du = 1.0 / m_uPatches;
	RtFloat dv = 1.0 / m_vPatches;

	TqInt MyUses = Uses();

	const TqFloat* pTC = pAttributes() ->GetFloatAttribute("System", "TextureCoordinates");
	CqVector2D st1( pTC[0], pTC[1]);
	CqVector2D st2( pTC[2], pTC[3]);
	CqVector2D st3( pTC[4], pTC[5]);
	CqVector2D st4( pTC[6], pTC[7]);

	vp = 0;
	TqInt i;
	for ( i = 0; i < m_vPatches; i++ )      	// Fill in the points
	{
		up = 0;
		RtInt j;
		for ( j = 0; j < m_uPatches; j++ )
		{
			CqSurfacePatchBilinear*	pSurface = new CqSurfacePatchBilinear;
			pSurface->AddRef();
			pSurface->SetSurfaceParameters( *this );
			pSurface->SetDefaultPrimitiveVariables();
			pSurface->P().SetSize( 4 );

			// Calculate the position in the point table for u taking into account
			// periodic patches.
			iP = PatchCoord( i, j );
			pSurface->P() [ 0 ] = P() [ iP ];
			iP = PatchCoord( i, j + 1 );
			pSurface->P() [ 1 ] = P() [ iP ];
			iP = PatchCoord( i + 1, j );
			pSurface->P() [ 2 ] = P() [ iP ];
			iP = PatchCoord( i + 1, j + 1 );
			pSurface->P() [ 3 ] = P() [ iP ];

			RtInt iTa = PatchCoord( i, j );
			RtInt iTb = PatchCoord( i, j + 1 );
			RtInt iTc = PatchCoord( i + 1, j );
			RtInt iTd = PatchCoord( i + 1, j + 1 );

			// Copy any user specified primitive variables.
			std::vector<CqParameter*>::iterator iUP;
			for( iUP = aUserParams().begin(); iUP != aUserParams().end(); iUP++ )
			{
				CqParameter* pNewUP = (*iUP)->Clone();
				pNewUP->Clear();
				pNewUP->SetSize(4);
				switch( (*iUP)->Type() )
				{
					case type_float:
					{
						CqParameterTyped<TqFloat, TqFloat>* pTParam = static_cast<CqParameterTyped<TqFloat, TqFloat>*>(*iUP);
						CqParameterTyped<TqFloat, TqFloat>* pTNewUP = static_cast<CqParameterTyped<TqFloat, TqFloat>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_integer:
					{
						CqParameterTyped<TqInt, TqFloat>* pTParam = static_cast<CqParameterTyped<TqInt, TqFloat>*>(*iUP);
						CqParameterTyped<TqInt, TqFloat>* pTNewUP = static_cast<CqParameterTyped<TqInt, TqFloat>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_point:
					case type_normal:
					case type_vector:
					{
						CqParameterTyped<CqVector3D, CqVector3D>* pTParam = static_cast<CqParameterTyped<CqVector3D, CqVector3D>*>(*iUP);
						CqParameterTyped<CqVector3D, CqVector3D>* pTNewUP = static_cast<CqParameterTyped<CqVector3D, CqVector3D>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_hpoint:
					{
						CqParameterTyped<CqVector4D, CqVector3D>* pTParam = static_cast<CqParameterTyped<CqVector4D, CqVector3D>*>(*iUP);
						CqParameterTyped<CqVector4D, CqVector3D>* pTNewUP = static_cast<CqParameterTyped<CqVector4D, CqVector3D>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_color:
					{
						CqParameterTyped<CqColor, CqColor>* pTParam = static_cast<CqParameterTyped<CqColor, CqColor>*>(*iUP);
						CqParameterTyped<CqColor, CqColor>* pTNewUP = static_cast<CqParameterTyped<CqColor, CqColor>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_string:
					{
						CqParameterTyped<CqString, CqString>* pTParam = static_cast<CqParameterTyped<CqString, CqString>*>(*iUP);
						CqParameterTyped<CqString, CqString>* pTNewUP = static_cast<CqParameterTyped<CqString, CqString>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;

					case type_matrix:
					{
						CqParameterTyped<CqMatrix, CqMatrix>* pTParam = static_cast<CqParameterTyped<CqMatrix, CqMatrix>*>(*iUP);
						CqParameterTyped<CqMatrix, CqMatrix>* pTNewUP = static_cast<CqParameterTyped<CqMatrix, CqMatrix>*>(pNewUP);
						pTNewUP->pValue(0)[0] = pTParam->pValue(iTa)[0];
						pTNewUP->pValue(1)[0] = pTParam->pValue(iTb)[0];
						pTNewUP->pValue(2)[0] = pTParam->pValue(iTc)[0];
						pTNewUP->pValue(3)[0] = pTParam->pValue(iTd)[0];
					}
					break;
				}
				pSurface->AddPrimitiveVariable(pNewUP);
			}

			// Fill in the surface parameters for the mesh.
			if ( USES( MyUses, EnvVars_u ) && bHasu() )
			{
				pSurface->u()->SetSize( 4 );
				(*pSurface->u())[ 0 ] = up;
				(*pSurface->u())[ 1 ] = up + du;
				(*pSurface->u())[ 2 ] = up;
				(*pSurface->u())[ 3 ] = up + du;
			}

			if ( USES( MyUses, EnvVars_v ) && bHasv() )
			{
				pSurface->v()->SetSize( 4 );
				(*pSurface->v())[ 0 ] = vp;
				(*pSurface->v())[ 1 ] = vp;
				(*pSurface->v())[ 2 ] = vp + dv;
				(*pSurface->v())[ 3 ] = vp + dv;
			}

			up += du;

			aSplits.push_back( pSurface );
			cSplits++;
		}
		vp += dv;
	}
	return ( cSplits );
}


END_NAMESPACE( Aqsis )

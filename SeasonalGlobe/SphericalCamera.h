#ifndef __SPHERICAL_CAMERA_H__
#define __SPHERICAL_CAMERA_H__

#include "ctypes.h"

enum SphericalCoordIndex
{
	RADIUS=0,
	THETA=1,
	AZIMUTH=2,
};

// A data structure describing spherical coordinates. By default, all variables are set to 0, but you
// can pass r (radius), theta (inclination angle), and azimuth (x-y plane) seperately. The structure functions
// assume Z=UP. No conversions may occur within constructors.
// To avoid function naming conflicts, all static functions end in _s
struct SphericalCamera
{
public:
	SphericalCamera(); // sets all to 0
	SphericalCamera(f32,f32,f32); // sets specified r,theta,azimuth

	// Converts the objects spherical coordinates to cartesian coordinates, and stores them in output vector.
	// Output vec needs to be provided, so we dont have to grab memory from the heap within the function
	// (returning a local array is invalid, as it is on the stack)
	void SphericalToCartesian(f32*) const;

	// Converts a set of Cartesian coordinates (X,Y,Z) to spherical coordinates
	// The result is in the objects variables
	void CartesianToSpherical(f32,f32,f32);

	// pass r,theta and azimuth directly. Use this to provide conversion without creating SphericalCamera object in
	// memory
	static void SphericalToCartesian_s(f32,f32,f32,f32*);

	// Converts a set of Cartesian coordinates (X,Y,Z) to spherical coordinates
	// (3 element array, 0=r,1=theta,2=azimuth)
	static void CartesianToSpherical_s(f32,f32,f32, f32*);

	// Creates a new SphericalCamera object with the converted results from (X,Y,Z) Cartesian coordinates
	// to the spherical coordinates
	static SphericalCamera CartesianToSpherical_s(f32,f32,f32);

	// In the array, element 0 = r, element 1 = theta, element 2 = azimuth
	// Index the element with a value from SphericalCoordIndex enum for ease and to reduce bugs, as the order
	// in the array could change. 
	f32 spherical_coords[3];
};

#endif
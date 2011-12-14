#ifndef __SPHERICAL_CAMERA_H__
#define __SPHERICAL_CAMERA_H__

#include "ctypes.h"
#include "float3.h"
#include <GXBase.h>

class SphericalCamera
{
private:
	f32 radius, azimuth, theta;
	float3 pos, dir;

	// convert the spherical coordinates into cartesian coordinates
	void RecalculatePos()
	{
		pos.set (
			radius*sin(theta)*cos(azimuth),
			radius*sin(theta)*sin(azimuth),
			radius*cos(theta));
		//dir = -pos;
		dir.set(-pos.x(), -pos.y(), -pos.z());
	};

public:

	SphericalCamera() : radius(0), azimuth(0), theta(0) { };
	~SphericalCamera() { };

	const f32 GetRadius() const { return radius; };
	const f32 GetAzimuth() const { return azimuth; };
	const f32 GetTheta() const { return theta; };

	void SetRadius(f32 _radius) { radius = _radius; RecalculatePos(); };
	void SetTheta(f32 _theta) { theta = _theta; RecalculatePos(); };
	void SetAzimuth(f32 _azimuth) { azimuth = _azimuth; RecalculatePos(); };

	const float3& GetPosition() const { return pos; }
	const float3& GetDirection() const { return dir; };

	void Update() const
	{
		gluLookAt(pos.x(), pos.y(), pos.z(), dir.x(), dir.y(), dir.z(), 0,1,0);
	};
};


/*
enum SphericalCoordIndex
{
	RADIUS=0,
	THETA=1,
	AZIMUTH=2,
};
*/

/*
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
*/

#endif
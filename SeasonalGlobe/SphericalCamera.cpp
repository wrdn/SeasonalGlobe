#include "SphericalCamera.h"
#include <math.h>

SphericalCamera::SphericalCamera()
{
	spherical_coords[RADIUS] = 0;
	spherical_coords[THETA] = 0;
	spherical_coords[AZIMUTH] = 0;
};

SphericalCamera::SphericalCamera(f32 r, f32 theta, f32 azimuth)
{
	spherical_coords[RADIUS] = r;
	spherical_coords[THETA] = theta;
	spherical_coords[AZIMUTH] = azimuth;
};

void SphericalCamera::SphericalToCartesian(f32* output_vec)
{
	if(output_vec)
	{
		f32 r = spherical_coords[RADIUS];
		f32 t = spherical_coords[THETA];
		f32 a = spherical_coords[AZIMUTH];

		output_vec[0] = r * sin(t) * cos(a); //x
		output_vec[1] = r * sin(t) * sin(a); //y
		output_vec[2] = r * cos(t);          //z
	}
};

//static
void SphericalCamera::SphericalToCartesian_s(f32 r,f32 theta,f32 azimuth,f32* output_vec)
{
	if(output_vec)
	{
		output_vec[0] = r * sin(theta) * cos(azimuth); //x
		output_vec[1] = r * sin(theta) * sin(azimuth); //y
		output_vec[2] = r * cos(theta);                //z
	}
};

//static
void SphericalCamera::CartesianToSpherical_s(f32 x,f32 y,f32 z, f32* output_vec)
{
	if(output_vec)
	{
		output_vec[RADIUS] = sqrt((x*x)+(y*y)+(z*z));
		output_vec[THETA] = acos(z/output_vec[RADIUS]);
		output_vec[AZIMUTH] = atan(y/x);
	}

};

void SphericalCamera::CartesianToSpherical(f32 x,f32 y,f32 z)
{
	spherical_coords[RADIUS] = sqrt((x*x)+(y*y)+(z*z));
	spherical_coords[THETA] = acos(z/spherical_coords[RADIUS]);
	spherical_coords[AZIMUTH] = atan(y/x);
};

// static
SphericalCamera SphericalCamera::CartesianToSpherical_s(f32 x,f32 y,f32 z)
{
	f32 r = sqrt((x*x)+(y*y)+(z*z));
	return SphericalCamera(r, acos(z/r), atan(y/x));
};
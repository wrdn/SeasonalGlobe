#include "Sphere.h"
#include "util.h"
#include "ResourceManager.h"
#include <math.h>

Sphere::Sphere() : radius(0), slices(0), stacks(0) { };

Sphere::~Sphere() { };

bool Sphere::Create(f32 _radius, u32 _slices, u32 _stacks, bool createHemisphere)
{
	if( ( _stacks < 2 ) && ( _slices < 2 ) ) return false;

	radius = _radius;
	slices = _slices;
	stacks = _stacks;

	std::vector<float> sinI, cosI, sinJ, cosJ;

	for (u32 i = 0; i < slices; i++)
	{
		f32 inp = 2.0f * PI * (f32)i / (f32)slices;
		sinI.push_back(sin(inp));
		cosI.push_back(cos(inp));
	}
	for (u32 j = 0; j < stacks; j++)
	{
		f32 inp = PI * (f32)j / (f32)stacks;

		sinJ.push_back(sin(inp));
		cosJ.push_back(cos(inp));
	}

	std::vector<VERTEX> vertices;

	VERTEX topVertex;
	topVertex.position.set(0,0,radius);
	topVertex.uv.set(0,1);

	//topVertex.position.set(0,radius,0);
	vertices.push_back(topVertex);

	for(u32 j=1;j<stacks;++j)
	{
		for(u32 i=0;i<slices;++i)
		{
			VERTEX v;
			v.normal.set(
				sinI[i] * sinJ[j],
				cosI[i] * sinJ[j],
				cosJ[j]
			);

			v.normal.normalize();

			v.position.set(
				v.normal.x() * radius,
				v.normal.y() * radius,
				v.normal.z() * radius);

			v.uv.set((f32)i/(f32)slices, 1.0f - (PI/(f32)stacks) * (f32)j);

			vertices.push_back(v);
		}
	}

	VERTEX bottomVertex;
	bottomVertex.position.set(0,0,-radius);
	//bottomVertex.position.set(0,-radius,0);
	vertices.push_back(bottomVertex);

	int rowA = 0;
	int rowB = 1;

	std::vector<u32> indices;

	u32 i=0;
	for(i = 0; i < slices - 1; i++)
	{
		indices.push_back(rowA);
		indices.push_back(rowB + i + 1);
		indices.push_back(rowB + i);
	}

	indices.push_back(rowA);
	indices.push_back(rowB);
	indices.push_back(rowB+i);

	i32 less = createHemisphere ? stacks/2-1 : 0;

	// middle stacks
	for (u32 j = 1; j < stacks - 1 - less; j++)
	{
		rowA = 1 + (j - 1) * slices;
		rowB = rowA + slices;

		for (i = 0; i < slices - 1; i++)
		{
			indices.push_back(rowA+i);
			indices.push_back(rowA+i+1);
			indices.push_back(rowB+i);

			indices.push_back(rowA+i+1);
			indices.push_back(rowB+i+1);
			indices.push_back(rowB+i);
		}

		indices.push_back(rowA+i);
		indices.push_back(rowA);
		indices.push_back(rowB+i);

		indices.push_back(rowA);
		indices.push_back(rowB);
		indices.push_back(rowB+i);
	}

	rowA = 1 + (stacks - 2) * slices;
	rowB = rowA + slices;

	if(!createHemisphere)
	{
		for (i = 0; i < slices - 1; i++)
		{
			indices.push_back(rowA+i);
			indices.push_back(rowA+i+1);
			indices.push_back(rowB);
		}

		indices.push_back(rowA+i);
		indices.push_back(rowA);
		indices.push_back(rowB);
	}

	MeshHandle mh = CreateMesh("sphere");
	bool ret = mh->BuildVBO(&vertices[0], vertices.size(),  &indices[0], indices.size());
	if(ret) { SetMesh(mh); }
	else { ResourceManager::get().RemoveResource(mh->GetResourceID()); }

	return ret;
};
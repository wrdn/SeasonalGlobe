#include "TerrainLoader.h"
#include "Collision.h"
using namespace gxbase;

TerrainLoader::TerrainLoader(void) : width(0), height(0)
{
}

TerrainLoader::~TerrainLoader(void)
{
}

bool TerrainLoader::Load(char *filename)
{
	unsigned char *rowpointer;
	i32 vertex_index;

	Image heightfield;
	if(!heightfield.Load(filename)) { return false; }

	height = heightfield.Height();
	width = heightfield.Width();
	i32 bitsperpixel = heightfield.GetPixelSize();

	const u32 vertexArraySize = width*height;
	VERTEX *verts = new VERTEX[vertexArraySize]; // this can be passed straight to Model class

	// Calculate vertices (and texture coordinates) for each pixel
	vertex_index = 0;
	for(u32 j=0;j<height;j++)
	{
		rowpointer =heightfield.GetRowData(j);
		for(u32 i = 0; i <width; i++)
		{
			VERTEX v;
			v.uv.set ( (f32)i/(width-1), (f32)j/(height-1) );

			v.position.set(
				(-2.0f + 4.0f* v.uv.x()),
				(-2.0f + 4.0f*v.uv.y()),
				-((float)rowpointer[i*bitsperpixel]/255.0f));

			verts[vertex_index] = v;
			++vertex_index;
		}
	}

	// Calculate the normals (indices will be calculated later, to create a circle)
	// First row
	vertex_index=0;
	verts[0].normal.set ( // first pixel (row 0)
		-verts[vertex_index].position.z(),
		-verts[vertex_index+width].position.z(), 8.0f/(width-1) );
	++vertex_index;
	for(u32 i=1;i<width-1;++i)
	{
		verts[vertex_index].normal.set ( // mid pixels (row 0)
			verts[vertex_index-1].position.z() - verts[vertex_index+1].position.z(),
			-verts[vertex_index+width].position.z(),
			8.0f/(width-1) );
		++vertex_index;
	}
	verts[vertex_index].position.set( // last pixel (row 0)
		verts[vertex_index-1].position.z(),
		-verts[vertex_index+width].position.z(), 8.0f / (width-1) );
	++vertex_index;

	// "Middle" rows
	for(u32 j=1;j<height-1;++j)
	{
		verts[vertex_index].normal.set( // First pixel on row j
			-verts[vertex_index].position.z(),
			-verts[vertex_index+width].position.z(),
			8.0f / (width-1));
		++vertex_index;

		for(u32 i=1;i<width-1;++i) // "Middle" pixels on row j
		{
			verts[vertex_index].normal.set (
				verts[vertex_index-1].position.z() - verts[vertex_index+1].position.z(),
				verts[vertex_index-width].position.z() - verts[vertex_index+width].position.z(),
				8.0f / (width-1));
			++vertex_index;
		}

		verts[vertex_index].normal.set( // Last pixel on row j
			verts[vertex_index-1].position.z(),
			-verts[vertex_index+width].position.z(),
			8.0f / (width-1) );
		++vertex_index;
	}

	// Last row
	verts[vertex_index].normal.set( // First pixel on last row
		-verts[vertex_index].position.z(),
		verts[vertex_index-width].position.z(),
		8.0f / (width - 1) );
	++vertex_index;

	for(u32 i=1;i<width-1;++i) // "Middle" pixels on last row
	{
		verts[vertex_index].normal.set(
			verts[vertex_index-1].position.z() - verts[vertex_index+1].position.z(),
			verts[vertex_index-width].position.z(),
			8.0f / (width-1) );
		++vertex_index;
	}

	verts[vertex_index].normal.set( // Last pixel on last row
		verts[vertex_index-1].position.z(),
		-verts[vertex_index-width].position.z(),
		8.0f / (width-1) );

	for(u32 i=0;i<vertexArraySize;++i) // normalize all the normals
		verts[i].normal.normalize();

	#pragma region Rectangular Terrain Triangle Generation
	// Calculate triangles (rectangular)
	/*for(int j=0;j<height-1;++j)
	{
		for(int i=0;i<width-2;++i)
		{
			int k = (width * j) + i; // current pixel
			
			{
				Face f;
				f.v1 = verts[k+1];
				f.v2 = verts[k];
				f.v3 = verts[k+width];

				faces.push_back(f);
			}
			{
				Face f;
				f.v1 = verts[k+1];
				f.v2 = verts[k+width];
				f.v3 = verts[k+width+1];

				faces.push_back(f);
			}
		}
	}*/
	#pragma endregion

	#pragma region Circular Terrain
	float2 centre((f32)width/2, (f32)height/2);
	Circle circ(centre, min(centre.x(), centre.y()));

	// Calculate the collision points (where the circle exists) on each row
	i32 loop_start = ((i32)(circ.pos.y()-circ.radius));
	i32 loop_end = ((i32)(circ.pos.y()+circ.radius));
	for(i32 i = loop_start; i < loop_end; ++i)
	{
		CollisionPoint2D cp;
		Line2D ln( float2(0, (f32)i), float2(circ.pos.x(), (f32)i) );
		if(LineCircleCollision(ln, circ, cp))
		{
			ImageRowInfo irow;
			irow.pixelStart = Vec2i( (i32)cp.p1.x(), i);
			irow.pixelEnd = Vec2i( (i32)cp.p2.x(), i);
			irow.pixelCount = irow.pixelEnd.x - irow.pixelStart.x;
			irow.firstVertexIndex = (width * i) + irow.pixelStart.x; // ?????
			irow.lastVertexIndex = irow.firstVertexIndex + irow.pixelCount;
			rows.push_back(irow);
		}
	}

	// Rough approximation for index array size is 2*CircleArea+300, where CircleArea=PI*(radius^2)
	vector<u32> indices;

	for(u32 i=0;i<rows.size()-2;++i) // for each row of pixels in the circle
	{
		ImageRowInfo &currentRow = rows[i], &nextRow = rows[i+1];

		i32 D, OverlapStart, OverlapEnd, OverlapLength;
		if(currentRow.pixelStart.x > nextRow.pixelStart.x)
		{
			D = currentRow.pixelStart.x - nextRow.pixelStart.x;
			OverlapStart = nextRow.pixelStart.x + D;
		}
		else
		{
			D = nextRow.pixelStart.x - currentRow.pixelStart.x;
			OverlapStart = currentRow.pixelStart.x + D;
		}
		OverlapEnd = OverlapStart + min(currentRow.pixelCount, nextRow.pixelCount);
		OverlapLength = OverlapEnd - OverlapStart;

		// Smooth left side of vertices
		if(currentRow.pixelCount >= nextRow.pixelCount)
		{
			indices.push_back(currentRow.firstVertexIndex+D);
			indices.push_back(currentRow.firstVertexIndex);
			indices.push_back(nextRow.firstVertexIndex);
		}
		else
		{
			indices.push_back(currentRow.firstVertexIndex);
			indices.push_back(nextRow.firstVertexIndex);
			indices.push_back(nextRow.firstVertexIndex+D);
		}

		// Calculate faces for overlapping pixels
		for(i32 j=0;j<OverlapLength;++j)
		{
			u32 _indices[4];

			if(currentRow.pixelCount > nextRow.pixelCount)
			{
				_indices[0] = currentRow.firstVertexIndex + D + j;
				_indices[1] = currentRow.firstVertexIndex + D + j + 1;
				_indices[2] = nextRow.firstVertexIndex + j;
				_indices[3] = nextRow.firstVertexIndex + j + 1;
			}
			else
			{
				_indices[0] = currentRow.firstVertexIndex + j;
				_indices[1] = currentRow.firstVertexIndex + j + 1;
				_indices[2] = nextRow.firstVertexIndex + D + j;
				_indices[3] = nextRow.firstVertexIndex + D + j + 1;
			}
			
			indices.push_back(_indices[1]);
			indices.push_back(_indices[0]);
			indices.push_back(_indices[2]);

			indices.push_back(_indices[1]);
			indices.push_back(_indices[2]);
			indices.push_back(_indices[3]);
		}

		// Smooth right side of vertices
		if(currentRow.pixelCount >= nextRow.pixelCount)
		{
			indices.push_back(currentRow.lastVertexIndex-1);
			indices.push_back(currentRow.lastVertexIndex-D-1);
			indices.push_back(nextRow.lastVertexIndex-1);
		}
		else
		{
			indices.push_back(currentRow.lastVertexIndex-1);
			indices.push_back(nextRow.lastVertexIndex-D-1);
			indices.push_back(nextRow.lastVertexIndex-1);
		}
	}
	#pragma endregion

	u32 *indicesArray = new u32[indices.size()];
	memcpy(indicesArray, &indices[0], sizeof(u32)*indices.size());

	::Model &m = ((::Model&)this->GetModel());
	m.SetVertexArray(verts, vertexArraySize);
	m.SetIndicesArray(indicesArray, indices.size());
	return m.BuildVBO();
};
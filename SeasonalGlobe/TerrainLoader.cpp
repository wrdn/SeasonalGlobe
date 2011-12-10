#include "TerrainLoader.h"
#include "Collision.h"
using namespace gxbase;

TerrainLoader::TerrainLoader(void)
{
}

TerrainLoader::~TerrainLoader(void)
{
}

void TerrainLoader::Load(char *filename)
{
	unsigned char *rowpointer;
	int vertex_index, current_vertex;

	Image heightfield;
	if(!heightfield.Load(filename)) return;

	height = heightfield.Height();
	width = heightfield.Width();
	int bitsperpixel = heightfield.GetPixelSize();

	// Calculate vertices (and texture coordinates) for each pixel
	vertex_index = 0;
	for(int j=0;j<height;j++)
	{
		rowpointer =heightfield.GetRowData(j);
		for(int i = 0; i <width; i++)
		{
			VERTEX v;
			v.uvs.set ( (float)i/(width-1), (float)j/(height-1) );

			v.pos.set(
				-2.0f + 4.0f* v.uvs.x(),
				(-2.0f + 4.0f*v.uvs.y()),
				(float)rowpointer[i*bitsperpixel]/255.0f);

			verts.push_back(v);
		};
	}

	// Calculate the normals (indices will be calculated later, to create a circle)
	// First row
	vertex_index=0;
	verts[0].normal.set ( // first pixel (row 0)
		-verts[vertex_index].pos.z(),
		-verts[vertex_index+width].pos.z(), 8.0f/(width-1) );
	++vertex_index;
	for(int i=1;i<width-1;++i)
	{
		verts[vertex_index].normal.set ( // mid pixels (row 0)
			verts[vertex_index-1].pos.z() - verts[vertex_index+1].pos.z(),
			-verts[vertex_index+width].pos.z(),
			8.0f/(width-1) );
		++vertex_index;
	}
	verts[vertex_index].pos.set( // last pixel (row 0)
		verts[vertex_index-1].pos.z(),
		-verts[vertex_index+width].pos.z(), 8.0f / (width-1) );
	++vertex_index;

	// "Middle" rows
	for(int j=1;j<height-1;++j)
	{
		verts[vertex_index].normal.set( // First pixel on row j
			-verts[vertex_index].pos.z(),
			-verts[vertex_index+width].pos.z(),
			8.0f / (width-1));
		++vertex_index;

		for(int i=1;i<width-1;++i) // "Middle" pixels on row j
		{
			verts[vertex_index].normal.set (
				verts[vertex_index-1].pos.z() - verts[vertex_index+1].pos.z(),
				verts[vertex_index-width].pos.z() - verts[vertex_index+width].pos.z(),
				8.0f / (width-1));
			++vertex_index;
		}

		verts[vertex_index].normal.set( // Last pixel on row j
			verts[vertex_index-1].pos.z(),
			-verts[vertex_index+width].pos.z(),
			8.0f / (width-1) );
		++vertex_index;
	}

	// Last row
	verts[vertex_index].normal.set( // First pixel on last row
		-verts[vertex_index].pos.z(),
		verts[vertex_index-width].pos.z(),
		8.0f / (width - 1) );
	++vertex_index;

	for(int i=1;i<width-1;++i) // "Middle" pixels on last row
	{
		verts[vertex_index].normal.set(
			verts[vertex_index-1].pos.z() - verts[vertex_index+1].pos.z(),
			verts[vertex_index-width].pos.z(),
			8.0f / (width-1) );
		++vertex_index;
	}

	verts[vertex_index].normal.set( // Last pixel on last row
		verts[vertex_index-1].pos.z(),
		-verts[vertex_index-width].pos.z(),
		8.0f / (width-1) );

	for(int i=0;i<verts.size();++i) // normalize all the normals
		verts[i].normal.normalize();


	#pragma region Rectangular Terrain (Triangle Generation
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
	float2 centre(width/2, height/2);
	Circle circ(centre, min(centre.x(), centre.y()));
	
	// Calculate the collision points (where the circle exists) on each row
	for(u32 i = (u32)(circ.pos.y()-circ.radius); i < (u32)(circ.pos.y()+circ.radius); ++i)
	{
		CollisionPoint2D cp;
		Line2D ln( float2(0, i), float2(circ.pos.x(), i) );
		if(LineCircleCollision(ln, circ, cp))
		{
			ImageRowInfo irow;
			irow.pixelStart = Vec2i( cp.p1.x(), i);
			irow.pixelEnd = Vec2i( cp.p2.x(), i);
			irow.pixelCount = irow.pixelEnd.x - irow.pixelStart.x;
			irow.firstVertexIndex = (width * i) + irow.pixelStart.x; // ?????
			irow.lastVertexIndex = irow.firstVertexIndex + irow.pixelCount;
			rows.push_back(irow);
		}
	}

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
			VERTEX v0,v1,v2;
			v0 = verts[currentRow.firstVertexIndex+D];
			v1 = verts[currentRow.firstVertexIndex];
			v2 = verts[nextRow.firstVertexIndex];
			faces.push_back(Face(v0,v1,v2));
		}
		else
		{
			VERTEX v0,v1,v2;
			v0 = verts[currentRow.firstVertexIndex];
			v1 = verts[nextRow.firstVertexIndex];
			v2 = verts[nextRow.firstVertexIndex+D];
			faces.push_back(Face(v0,v1,v2));
		}

		// Calculate faces for overlapping pixels
		for(i32 j=0;j<OverlapLength;++j)
		{
			VERTEX v0, v1, v2, v3;

			if(currentRow.pixelCount > nextRow.pixelCount)
			{
				v0 = verts[ currentRow.firstVertexIndex + D + j ];
				v1 = verts[ currentRow.firstVertexIndex + D + j + 1 ];
				v2 = verts[ nextRow.firstVertexIndex + j ];
				v3 = verts[ nextRow.firstVertexIndex + j + 1 ];
			}
			else
			{
				v0 = verts[ currentRow.firstVertexIndex + j ];
				v1 = verts[ currentRow.firstVertexIndex + j + 1 ];
				v2 = verts[ nextRow.firstVertexIndex + D + j ];
				v3 = verts[ nextRow.firstVertexIndex + D + j + 1];
			}
			faces.push_back(Face(v1, v0, v2));
			faces.push_back(Face(v1, v2, v3));
		}

		// Smooth right side of vertices
		if(currentRow.pixelCount >= nextRow.pixelCount)
		{
			VERTEX v0,v1,v2;
			v0 = verts[currentRow.lastVertexIndex-1];
			v1 = verts[currentRow.lastVertexIndex-D-1];
			v2 = verts[nextRow.lastVertexIndex-1];
			faces.push_back(Face(v0,v1,v2));
		}
		else
		{
			VERTEX v0,v1,v2;
			v0 = verts[currentRow.lastVertexIndex-1];
			v1 = verts[nextRow.lastVertexIndex-D-1];
			v2 = verts[nextRow.lastVertexIndex-1];
			faces.push_back(Face(v0,v1,v2));
		}
	}

	#pragma endregion
};

void TerrainLoader::Draw()
{
	/*glBegin(GL_POINTS);
	for(int i=0;i<rows.size();++i)
	{
		int h = rows[i].pixelStart.y; // height position
		int k = (width * h); // first pixel on the row
		int FirstPoint = k + rows[i].pixelStart.x; // first pixel in the circle

		for(int j=FirstPoint;j<FirstPoint+rows[i].pixelCount;++j)
		{
			glNormal3fv(verts[j].normal.GetVec());
			glVertex3fv(verts[j].pos.GetVec());
		}
	}
	glEnd();*/

	glBegin(GL_TRIANGLES);
	for(int i=0;i<faces.size();++i)
	{
		glNormal3fv(faces[i].v1.normal.GetVec());
		glTexCoord2fv(faces[i].v1.uvs.GetVec());
		glVertex3fv(faces[i].v1.pos.GetVec());

		glNormal3fv(faces[i].v2.normal.GetVec());
		glTexCoord2fv(faces[i].v2.uvs.GetVec());
		glVertex3fv(faces[i].v2.pos.GetVec());

		glNormal3fv(faces[i].v3.normal.GetVec());
		glTexCoord2fv(faces[i].v3.uvs.GetVec());
		glVertex3fv(faces[i].v3.pos.GetVec());
	}
	glEnd();

	/*glBegin(GL_POINTS);
	for(int i=0;i<verts.size();++i)
		glVertex3fv(verts[i].pos.GetVec());
	glEnd();*/
};
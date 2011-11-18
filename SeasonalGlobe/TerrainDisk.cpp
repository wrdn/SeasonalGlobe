#include "TerrainDisk.h"
#include "Collision.h"

#include <GXBase.h>
using namespace gxbase;

TerrainDisk::TerrainDisk() { };

TerrainDisk::~TerrainDisk() { };


// holds information about rows of pixels in the heightmap
// only needed by TerrainDisk so hidden from rest of code base
struct RowInfo
{
public:
	Vec2i pixstart, pixend; // where the row starts and ends in the image
	i32 pixcount; // pixels in this row (pixend-pixstart)
	i32 prevertexcount; // count before we added vertices for this row
	i32 postvertexcount; // count after we added vertices for this row
};

void TerrainDisk::Draw(const bool drawPoints)
{
	if(drawPoints)
	{
		glColor3f(1,1,1);
		GLboolean cullingEnabled = glIsEnabled(GL_CULL_FACE);
		glDisable(GL_CULL_FACE);
		glBegin(GL_POINTS);
		for(u32 i=0;i<_vertices.size();++i)
		{
			glTexCoord2fv(_vertices[i].uvs.vec);
			glVertex3fv(_vertices[i].pos.vec);
		}
		glEnd();
		if(cullingEnabled)
			glEnable(GL_CULL_FACE);
	}
	else
	{
		// draw faces
		glColor3f(1,1,1);
		//GLboolean cullingEnabled = glIsEnabled(GL_CULL_FACE);
		//glDisable(GL_CULL_FACE);
		glBegin(GL_TRIANGLES);
		for(u32 i=0;i<_faces.size();++i)
		{
			glTexCoord2fv(_faces[i].pos1.uvs.vec);
			glVertex3fv(_faces[i].pos1.pos.vec);

			glTexCoord2fv(_faces[i].pos2.uvs.vec);
			glVertex3fv(_faces[i].pos2.pos.vec);

			glTexCoord2fv(_faces[i].pos3.uvs.vec);
			glVertex3fv(_faces[i].pos3.pos.vec);
		}
		glEnd();
		//if(cullingEnabled)
		//	glEnable(GL_CULL_FACE);
	}
};

bool TerrainDisk::CreateTerrainDisk(const c8 * const heightmap_filename)
{
	#pragma region Setup
	Image sourceImage;
	if(!sourceImage.Load(heightmap_filename)) { return false; };

	float2 CircleCentre_t((f32)(sourceImage.Width()/2), (f32)(sourceImage.Height()/2));
	const Circle circle = Circle(CircleCentre_t, min(CircleCentre_t.x(), CircleCentre_t.y()));

	u32 FirstYPixel = (u32)(circle.pos.y() - circle.radius);
	u32 LastYPixel  = (u32)(circle.pos.y() + circle.radius);
	//u32 CirclePixelHeight = LastYPixel - FirstYPixel;

	uc8* rowptr=0; // pointer to the row of pixels
	u32 bpp = sourceImage.GetPixelSize(); // bits per pixel

	std::vector<RowInfo> rows;

	#pragma endregion

	#pragma region Read Pixels
	for(u32 i = FirstYPixel; i < LastYPixel; ++i)
	{
		rowptr = sourceImage.GetRowData(i);
		float2 rowStart(0, (f32)i), rowEnd(circle.pos.x(), (f32)i);

		CollisionPoint cp;
		const Line2D line(rowStart, rowEnd);
		if(LineCircleCollision(line, circle, cp))
		{
			i32 p1x = (i32)cp.p1.x();
			const i32 p2x = (i32)cp.p2.x();
			RowInfo r;
			r.pixstart = Vec2i(p1x, i);
			r.pixend   = Vec2i(p2x, i);
			r.pixcount = p2x - p1x;

			// Read vertices data from heightmap
			r.prevertexcount = _vertices.size();
			while(p1x <= p2x)
			{
				// UV
				float2 tx((f32)p1x / (f32)(sourceImage.Width()-1),
					(f32)i / (f32)(sourceImage.Height()-1));

				// Vertex
				float3 vpos(
					-2.0f + 4.0f * tx.x(),
					-2.0f + 4.0f * tx.y(),
					(f32)rowptr[p1x * bpp]/255.0f);

				VERTEX v(vpos, float3(), tx);
				_vertices.push_back(v);

				++p1x;
			}
			r.postvertexcount = _vertices.size();
			rows.push_back(r);
		}
	} // End of pixel reading for loop
	#pragma endregion

	#pragma region Build Faces

	for( u32 i=0; i < rows.size()-2; ++i)
	{
		RowInfo *currentRow = &rows[i]; // A
		RowInfo *nextRow = &rows[i+1];  // B

		// Find overlapping pixels
		i32 D, OverlapStart;
		if(max(currentRow->pixstart.x, nextRow->pixstart.x) == currentRow->pixstart.x)
		{
			D = currentRow->pixstart.x - nextRow->pixstart.x;
			OverlapStart = nextRow->pixstart.x + D;
		}
		else
		{
			D = nextRow->pixstart.x - currentRow->pixstart.x;
			OverlapStart = currentRow->pixstart.x + D;
		}
		i32 OverlapEnd = OverlapStart + min(currentRow->pixcount, nextRow->pixcount);
		i32 OverlapLength = OverlapEnd - OverlapStart;


		// Finish left face (on "left" side of disk)
		Face leftFace;
		if(currentRow->pixcount >= nextRow->pixcount) // A Length >= B Length
		{
			leftFace.pos1 = _vertices[currentRow->prevertexcount+D];
			leftFace.pos2 = _vertices[currentRow->prevertexcount];
			leftFace.pos3 = _vertices[nextRow->prevertexcount];
		}
		else // B Length > A Length
		{
			Face leftFace;
			leftFace.pos1 = _vertices[currentRow->prevertexcount];
			leftFace.pos2 = _vertices[nextRow->prevertexcount];
			leftFace.pos3 = _vertices[nextRow->prevertexcount+D];
		}
		//_faces.push_back(leftFace);

		// Faces for Overlapping pixels
		Face f1, f2;
		for(int j=0;j<OverlapLength;++j)
		{
			/*VERTEX v1 = _vertices[currentRow->prevertexcount + j + 1];
			VERTEX v2 = _vertices[currentRow->prevertexcount + j];
			VERTEX v3 = _vertices[nextRow->prevertexcount + j];
			VERTEX v4 = _vertices[nextRow->prevertexcount + j + 1];*/

			if(currentRow->pixcount > nextRow->pixcount)
			{
				VERTEX v1 = _vertices[currentRow->prevertexcount + D + j];
				VERTEX v2 = _vertices[currentRow->prevertexcount + D + j + 1];
				VERTEX v3 = _vertices[nextRow->prevertexcount + j];
				VERTEX v4 = _vertices[nextRow->prevertexcount + j + 1];

				f1.pos1 = v2;
				f1.pos2 = v1;
				f1.pos3 = v3;

				f2.pos1 = v2;
				f2.pos2 = v3;
				f2.pos3 = v4;

				_faces.push_back(f1);
				_faces.push_back(f2);
			}
			else
			{
				VERTEX v1 = _vertices[currentRow->prevertexcount + j];
				VERTEX v2 = _vertices[currentRow->prevertexcount + j + 1];
				VERTEX v3 = _vertices[nextRow->prevertexcount + D + j];
				VERTEX v4 = _vertices[nextRow->prevertexcount + D + j + 1];

				f1.pos1 = v2;
				f1.pos2 = v1;
				f1.pos3 = v3;

				f2.pos1 = v2;
				f2.pos2 = v3;
				f2.pos3 = v4;

				_faces.push_back(f1);
				_faces.push_back(f2);
			}
		}

		Face rightFace;
		if(currentRow->pixcount >= nextRow->pixcount) // A Length >= B Length
		{
			rightFace.pos1 = _vertices[currentRow->postvertexcount-1];
			rightFace.pos3 = _vertices[nextRow->postvertexcount-1];
			rightFace.pos2 = _vertices[currentRow->postvertexcount-D-1];
			//_faces.push_back(rightFace);
		}
		else
		{
			rightFace.pos2 = _vertices[currentRow->postvertexcount-1];
			rightFace.pos1 = _vertices[nextRow->postvertexcount-1];
			rightFace.pos3 = _vertices[nextRow->postvertexcount-D-1];
			//_faces.push_back(rightFace);
		}
		
	}

	#pragma endregion

	return false;
};
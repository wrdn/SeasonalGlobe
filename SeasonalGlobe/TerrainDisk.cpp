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

bool TerrainDisk::CreateTerrainDisk(const c8 * const heightmap_filename)
{
	Image sourceImage;
	if(!sourceImage.Load(heightmap_filename)) { return false; };

	float2 CircleCentre_t((f32)(sourceImage.Width()/2), (f32)(sourceImage.Height()/2));
	const Circle circle = Circle(CircleCentre_t, min(CircleCentre_t.x(), CircleCentre_t.y()));

	u32 FirstYPixel = (u32)(circle.pos.y() - circle.radius);
	u32 LastYPixel  = (u32)(circle.pos.y() + circle.radius);
	
	uc8* rowptr=0; // pointer to the row of pixels
	u32 bpp = sourceImage.GetPixelSize(); // bits per pixel

	std::vector<RowInfo> rows;

	for(u32 i = FirstYPixel; i < LastYPixel; ++i)
	{
		rowptr = sourceImage.GetRowData(i);
		float2 rowStart(0, (f32)i), rowEnd(circle.pos.x(), (f32)i);

		CollisionPoint2D cp;
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

				VERTEX v(vpos, (float3&)float3::ZERO, tx);
				_vertices.push_back(v);

				++p1x;
			}
			r.postvertexcount = _vertices.size();
			rows.push_back(r);
		}
	} // End of pixel reading for loop

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
		
		if(currentRow->pixcount >= nextRow->pixcount) // A Length >= B Length
		{
			_indices.push_back(currentRow->prevertexcount+D);
			_indices.push_back(currentRow->prevertexcount);
			_indices.push_back(nextRow->prevertexcount);
		}
		else // B Length > A Length
		{
			_indices.push_back(currentRow->prevertexcount);
			_indices.push_back(nextRow->prevertexcount);
			_indices.push_back(nextRow->prevertexcount+D);
		}
		
		// Faces for Overlapping pixels
		for(i32 j=0;j<OverlapLength;++j)
		{
			u32 indices[4];

			if(currentRow->pixcount > nextRow->pixcount)
			{
				indices[0] = currentRow->prevertexcount + D + j;
				indices[1] = currentRow->prevertexcount + D + j + 1;
				indices[2] = nextRow->prevertexcount + j;
				indices[3] = nextRow->prevertexcount + j + 1;
			}
			else
			{
				indices[0] = currentRow->prevertexcount + j;
				indices[1] = currentRow->prevertexcount + j + 1;
				indices[2] = nextRow->prevertexcount + D + j;
				indices[3] = nextRow->prevertexcount + D + j + 1;
			}
			_indices.push_back(indices[1]);
			_indices.push_back(indices[0]);
			_indices.push_back(indices[2]);

			_indices.push_back(indices[1]);
			_indices.push_back(indices[2]);
			_indices.push_back(indices[3]);
		}

		if(currentRow->pixcount >= nextRow->pixcount) // A Length >= B Length
		{
			_indices.push_back(currentRow->postvertexcount-1);
			_indices.push_back(currentRow->postvertexcount-D-1);
			_indices.push_back(nextRow->postvertexcount-1);
		}
		else
		{
			_indices.push_back(currentRow->postvertexcount-1);
			_indices.push_back(nextRow->postvertexcount-1-D);
			_indices.push_back(nextRow->postvertexcount-1);
		}	
	}

	SetVertexArray((VERTEX*)&_vertices[0], _vertices.size());
	SetIndicesArray((u32*)&_indices[0], _indices.size());

	return BuildVBO();
};
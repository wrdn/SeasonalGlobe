#include "Terrain.h"
#include <math.h>
#include <fstream>
#include <iostream>
#include <vector>
using namespace std;

#define PI 3.14159265

terrain::terrain() {
}

terrain::~terrain() {
	delete [] _vertexarray;
	delete [] _normalarray;
	delete [] _indexarray;
	delete [] _texarray;
}

bool LineCircleCollision(Line &line, Circle &c, CollisionPoint &cp)
{
	Vec2f lineDir = line.end - line.start;

	float A = lineDir.dot(lineDir);
	float B = 2.0 * (lineDir.x * (line.start.x - c.pos.x) + lineDir.y * (line.start.y - c.pos.y));
	float C = (line.start.x - c.pos.x) * (line.start.x - c.pos.x) +
		(line.start.y - c.pos.y) * (line.start.y - c.pos.y) -
		c.radius * c.radius;

	float det = B*B - 4 * A * C;
	if(det < 0 || A < 0.0001f) { return false; }
	else if(det < 0)
	{
		// 1 solution
		cp.t1 = -B / (2 * A);
		cp.t2 = -1;
		cp.p1.x =  line.start.x + cp.t1 * lineDir.x;
		cp.p1.y =  line.start.y + cp.t1 * lineDir.y;
		return true;
	}
	else
	{
		// 2 solutions
		cp.t1 = (-B + sqrtf(det)) / (2*A);
		cp.t2 = (-B - sqrtf(det)) / (2*A);
		if(cp.t1 > cp.t2)
		{
			float tmp = cp.t1;
			cp.t1 = cp.t2;
			cp.t2 = tmp;
		}

		cp.p1.x =  line.start.x + cp.t1 * lineDir.x;
		cp.p1.y =  line.start.y + cp.t1 * lineDir.y;

		cp.p2.x =  line.start.x + cp.t2 * lineDir.x;
		cp.p2.y =  line.start.y + cp.t2 * lineDir.y;

		return true;
	}

	return false;
};

struct RowInfo
{
public:
	Vec2i pixstart, pixend;
	int pixcount; int currentVertexCount;
};

void terrain::OnCreate() {

	const char *HEIGHTMAP_FILENAME = "ground.bmp";
	
	Image src;
	src.Load(HEIGHTMAP_FILENAME);
	
	Vec2f circle_centre( src.Width()/2, src.Height()/2 );
	float radius = min(src.Width(), src.Height()) / 2;
	int FirstY = circle_centre.y - radius;
	int LastY = circle_centre.y + radius;
	int total_height = LastY - FirstY;
	unsigned char *rowptr;
	long bits_per_pixel = src.GetPixelSize();

	std::vector<RowInfo> _rows;

	// For all the pixels that make up the circle
	for(int i=FirstY; i < LastY; ++i)
	{
		rowptr = src.GetRowData(i);

		Vec2f p1(0, i); // start of row, line start
		Vec2f p2(circle_centre.x+radius*2, i);

		CollisionPoint collisionPoint;
		Line cpLine; cpLine.start = p1; cpLine.end = p2;
		Circle c; c.pos = circle_centre; c.radius = radius;
		if(LineCircleCollision(cpLine, c, collisionPoint))
		{
			p1 = collisionPoint.p1;
			int pt = p1.x; int pq=0;

			RowInfo r;
			r.pixstart.x = pt; r.pixstart.y = i;
			r.pixend.x = collisionPoint.p2.x; r.pixend.y = i;
			r.pixcount = r.pixend.x - r.pixstart.x;
			r.currentVertexCount = _vertices.size();
			_rows.push_back(r);

			while(pt <= collisionPoint.p2.x)
			{
				Vec2f tx;
				tx.x = (float)pt / (src.Width()-1);
				tx.y = (float)i / (src.Height()-1);
				_tex.push_back(tx);

				Vec3f v;
				v.x = (-2.0f + 4.0f * tx.x);
				v.y = (-2.0f + 4.0f * tx.y);
				v.z = ((float)rowptr[pt * bits_per_pixel]/255.0f);
				_vertices.push_back(v);

				++pt;
			}
		}
	}
	
	int i=0;

	// For each row
	for ( std::vector<RowInfo>::iterator it = _rows.begin(); it < _rows.end()-1; ++it )
	{
		printf("%d\n",i);
		++i;

		RowInfo currentRow = *it;
		RowInfo nextRow   = *(it+1);

		RowInfo rowSet[] = { *it, *(it+1) };
		
		int _max_index, _min_index;
		if(rowSet[0].pixcount > rowSet[1].pixcount)
		{
			_max_index = 0;
			_min_index = 1;
		}
		else
		{
			_max_index = 1;
			_min_index = 0;
		}

		int overLapStart = rowSet[_max_index].pixcount - rowSet[_min_index].pixcount;
		int t = 2+2;

		//int startOfOverlap = max(currentRow.pixcount, nextRow.pixcount) - min(currentRow.pixcount, nextRow.pixcount);


		//// overlapping pixels
		//int PixelOverlap = currentRow.pixend.x  - rowBelow.pixstart.x;
		//int FirstOverlappingPixel = currentRow.pixend.x - PixelOverlap;

		//int _overlap = 


		//// Create a set of faces for all the overlapping pixels
		//for(int i = FirstOverlappingPixel; i < (FirstOverlappingPixel + PixelOverlap); ++i)
		//{
		//	Vec3f v1 = _vertices[ currentRow.currentVertexCount + i + 1 ]; // add 1 for CCW winding
		//	Vec3f v2 = _vertices[ currentRow.currentVertexCount + i ];
		//	Vec3f v3 = _vertices[ rowBelow.currentVertexCount + i ];
		//	Vec3f v4 = _vertices[ rowBelow.currentVertexCount + i + 1 ];

		//	Face f1;
		//	f1.pos1 = v1;
		//	f1.pos2 = v2;
		//	f1.pos3 = v3;

		//	Face f2;
		//	f2.pos1 = v1;
		//	f2.pos2 = v3;
		//	f2.pos3 = v4;

		//	faces.push_back(f1);
		//	faces.push_back(f2);
		//}
	}

	int bitsperpixel;
	unsigned char* rowpointer;
	int vertex_index;
	int current_vertex;

	_heightfield.Load("ground.bmp");


	// now need to build the vertex array
	// first some basic information about the image
	_height =_heightfield.Height();
	_width = _heightfield.Width();
	bitsperpixel = _heightfield.GetPixelSize();
	_vertexarray = new float[_width*_height][3];
	_normalarray = new float[_width*_height][3];
	_indexarray = new GLuint[_width*_height*2];
	_texarray = new float[_width*_height][2];

	// now need to produce the vertexarray from this
	vertex_index = 0;
	for(int j=0;j<_height;j++) {
		rowpointer =_heightfield.GetRowData(j);
		for(int i = 0; i <_width; i++) {
			_texarray[vertex_index][0] = (float)i/(_width-1);
			_texarray[vertex_index][1] = (float)j/(_height-1);
			_vertexarray[vertex_index][0] = (-2.0f + 4.0f*_texarray[vertex_index][0]);
			_vertexarray[vertex_index][1] =  (-2.0f + 4.0f*_texarray[vertex_index][1]);
			_vertexarray[vertex_index][2] =  ((float)rowpointer[i*bitsperpixel]/255.0f * 2.0f);
			vertex_index++;

		};
	};

	// need to generate the index array
	vertex_index = 0;
	for(int j=0;j<_height-1;j++) {
		for(int i = 0; i<_width;i++) {
			current_vertex = j*_width+i;
			_indexarray[vertex_index++] = current_vertex + _width;
			_indexarray[vertex_index++] = current_vertex;
		};
	};

	// now need to generate normals for the terrain
	// this is not that simple
	_gen_normals();
	_heightfield.Free();
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	glTexCoordPointer(2, GL_FLOAT, 0, _texarray);
	glEnableClientState(GL_NORMAL_ARRAY);
	glNormalPointer(GL_FLOAT, 0, _normalarray);
	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer(3, GL_FLOAT, 0, _vertexarray);
}

void terrain::draw(bool texon) const {

	/*
	if(texon) 
		glEnable(GL_TEXTURE_2D);
	for(int j=0;j<_height-1;j++) {
		glDrawElements(GL_TRIANGLE_STRIP, _width*2, GL_UNSIGNED_INT, &_indexarray[j*_width*2]);
	}
	glDisable(GL_TEXTURE_2D);
	*/
	glDisable(GL_LIGHTING);
	//glEnable(GL_TEXTURE_2D);
	
	/*glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glBegin(GL_POINTS);
	for(int i=0;i<_vertices.size();++i)
	{
		glVertex3f(_vertices[i].x, _vertices[i].y, _vertices[i].z);
	}
	glEnd();*/


	for(int i=0;i<faces.size();++i)
	{
		glBegin(GL_TRIANGLES);

		Face f = faces[i];

		glVertex3f(f.pos1.x, f.pos1.y, f.pos1.z);
		glVertex3f(f.pos2.x, f.pos2.y, f.pos2.z);
		glVertex3f(f.pos3.x, f.pos3.y, f.pos3.z);

		glEnd();
	}

}

void terrain::normal(bool normon) const {
	if (normon) {
		for(int j=0;j<_height*_width;j++) {
			glBegin(GL_LINES);
				glVertex3f(_vertexarray[j][0],_vertexarray[j][1],_vertexarray[j][2]);
				glVertex3f(_vertexarray[j][0]+0.1f*_normalarray[j][0],_vertexarray[j][1]+0.1f*_normalarray[j][1],_vertexarray[j][2]+0.1f*_normalarray[j][2]);
			glEnd();
		};
	};
}

void terrain::_gen_normals() {
	int vertex_index;

	vertex_index = 0;
	_normalarray[vertex_index][0] = -_vertexarray[vertex_index][2];
	_normalarray[vertex_index][1] = -_vertexarray[vertex_index+_width][2];
	_normalarray[vertex_index][2] = 8.0f/(_width-1);
	vertex_index++;
	for(int i = 1; i<_width-1;i++) {
		_normalarray[vertex_index][0] = _vertexarray[vertex_index-1][2]-_vertexarray[vertex_index+1][2];
		_normalarray[vertex_index][1] = -_vertexarray[vertex_index+_width][2];
		_normalarray[vertex_index][2] = 8.0f/(_width-1);
		vertex_index++;
	};
	_normalarray[vertex_index][0] = _vertexarray[vertex_index-1][2];
	_normalarray[vertex_index][1] = -_vertexarray[vertex_index+_width][2];
	_normalarray[vertex_index][2] = 8.0f/(_width-1);
	vertex_index++;

	for(int j=1;j<_height-1;j++) {
		_normalarray[vertex_index][0] = -_vertexarray[vertex_index][2];
		_normalarray[vertex_index][1] = -_vertexarray[vertex_index+_width][2];
		_normalarray[vertex_index][2] = 8.0f/(_width-1);
		vertex_index++;
		for(int i=1;i<_width-1;i++) {
			_normalarray[vertex_index][0] = _vertexarray[vertex_index-1][2]-_vertexarray[vertex_index+1][2];
			_normalarray[vertex_index][1] = _vertexarray[vertex_index-_width][2]-_vertexarray[vertex_index+_width][2];
			_normalarray[vertex_index][2] = 8.0f/(_width-1);
			vertex_index++;
		}
		_normalarray[vertex_index][0] = _vertexarray[vertex_index-1][2];
		_normalarray[vertex_index][1] = -_vertexarray[vertex_index+_width][2];
		_normalarray[vertex_index][2] = 8.0f/(_width-1);
		vertex_index++;
	}
	_normalarray[vertex_index][0] = -_vertexarray[vertex_index][2];
	_normalarray[vertex_index][1] = _vertexarray[vertex_index-_width][2];
	_normalarray[vertex_index][2] = 8.0f/(_width-1);
	vertex_index++;
	for(int i = 1; i<_width-1;i++) {
		_normalarray[vertex_index][0] = _vertexarray[vertex_index-1][2]-_vertexarray[vertex_index+1][2];
		_normalarray[vertex_index][1] = _vertexarray[vertex_index-_width][2];
		_normalarray[vertex_index][2] = 8.0f/(_width-1);
		vertex_index++;
	};
	_normalarray[vertex_index][0] = _vertexarray[vertex_index-1][2];
	_normalarray[vertex_index][1] = -_vertexarray[vertex_index-_width][2];
	_normalarray[vertex_index][2] = 8.0f/(_width-1);

	// make normals unit
	_normalize();

}

void terrain::_normalize() {
	float temp;

	for(int j=0;j<_height*_width;j++) {
		temp = _normalarray[j][0]*_normalarray[j][0]+_normalarray[j][1]*_normalarray[j][1]+_normalarray[j][2]*_normalarray[j][2];
		temp = sqrt(temp);
		_normalarray[j][0] /= temp;_normalarray[j][1] /= temp;_normalarray[j][2] /= temp;
	};
}
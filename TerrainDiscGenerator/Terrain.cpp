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
	int postVertexCount;
};

struct Overlap
{
public:
	int start, end;
	int length;
};

struct Color
{
public:
	float r,g,b;

	Color() : r(0), g(0), b(0) { };
	Color(float _r, float _g, float _b)
		: r(_r), g(_g), b(_b) { };
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
		Vec2f p2(circle_centre.x, i);

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

				v.r = v.g = v.b = 255;

				v.vecid = _tex.size();

				_vertices.push_back(v);

				++pt;
			}

			r.postVertexCount = _vertices.size();
			_rows.push_back(r);
		}
	}

	Color colors[] =
	{
		Color(255,0,0),
		Color(0,255,0),
		Color(0,0,255),
		Color(255,255,0),
		Color(0,255,255),
		Color(255,0,255)
	};
	int currentColor = 0;
	int totalColors = sizeof(colors) / sizeof(Color);

	int itk=0;
	// Find the overlapping pixels and make the corresponding vertex red
	for( std::vector<RowInfo>::iterator it = _rows.begin(); it != _rows.end()-2; ++it )
	{
		// Consider 2 rows at a time, the CurrentRow and NextRow
		RowInfo &CurrentRow = *it;
		RowInfo &NextRow = *(it+1);

		Overlap overlap;

		int D = max(CurrentRow.pixstart.x, NextRow.pixstart.x) - min(CurrentRow.pixstart.x,NextRow.pixstart.x);
		int OverlapStart = min(CurrentRow.pixstart.x, NextRow.pixstart.x) + D;
		int OverlapEnd =  OverlapStart + min(CurrentRow.pixcount, NextRow.pixcount);

		printf("Row %d (%d,%d) and Row %d (%d,%d). Start: %d End: %d\n", itk, CurrentRow.pixstart.x, CurrentRow.pixend.x,
			itk+1, NextRow.pixstart.x, NextRow.pixend.x, OverlapStart, OverlapEnd);
		
		++itk;
		
		int ct = OverlapEnd - OverlapStart;
		Face f1, f2;
		for(int i=0; i < ct; ++i) // build overlapping faces
		{
			Vec3f v1 = _vertices[ CurrentRow.currentVertexCount + i + 1];
			Vec3f v2 = _vertices[ CurrentRow.currentVertexCount + i];
			Vec3f v3 = _vertices[ NextRow.currentVertexCount + i];
			Vec3f v4 = _vertices[ NextRow.currentVertexCount + i + 1 ];

			f1.pos1 = v1;
			f1.pos2 = v2;
			f1.pos3 = v3;
			f1.pos1.r = colors[currentColor].r;
			f1.pos1.g = colors[currentColor].g;
			f1.pos1.b = colors[currentColor].b;
			++currentColor;
			if(currentColor >= totalColors)
				currentColor = 0;

			f2.pos1 = v1;
			f2.pos2 = v3;
			f2.pos3 = v4;
			f2.pos1.r = colors[currentColor].r;
			f2.pos1.g = colors[currentColor].g;
			f2.pos1.b = colors[currentColor].b;
			if(currentColor >= totalColors)
				currentColor = 0;

			faces.push_back(f1);
			faces.push_back(f2);
		}

		// connect the ends (2 faces)
		
		/*Face leftFace;
		leftFace.pos1 = _vertices[ CurrentRow.currentVertexCount ];
		leftFace.pos2 = _vertices[ NextRow.currentVertexCount ];
		leftFace.pos3 = _vertices[ NextRow.currentVertexCount ];
		leftFace.pos1.r = leftFace.pos1.g = leftFace.pos1.b = 255;
		faces.push_back(leftFace);
		
		Face rightFace;
		rightFace.pos1 = _vertices[ CurrentRow.postVertexCount ];
		rightFace.pos2 = _vertices[ NextRow.currentVertexCount];
		rightFace.pos3 = _vertices[ NextRow.postVertexCount ];
		faces.push_back(rightFace);*/

		//Face rightFace;
		//rightFace.pos1 = f2.pos1;
		//rightFace.pos2 = f2.pos2;
		//rightFace.pos3 = f2.pos3;
		//rightFace.pos1.r = rightFace.pos1.g = rightFace.pos1.b = 255;
		//faces.push_back(rightFace);
		//faces.push_back(rightFace);

		++itk;
		printf("%d\n",itk);

		if(itk == 30)
		{
			printf("");
		}
	}

	return;

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

void terrain::DrawPoints() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
	glBegin(GL_POINTS);
	for(int i=0;i<_vertices.size();++i)
	{
		glColor3f(_vertices[i].r, _vertices[i].g, _vertices[i].b);
		glVertex3f(_vertices[i].x, _vertices[i].y, _vertices[i].z);
	}
	glEnd();
};

void terrain::DrawFaces() const
{
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	for(int i=0;i<faces.size();++i)
	{
		glBegin(GL_TRIANGLES);

		Face f = faces[i];

		//glColor3f(f.pos1.r, f.pos1.g, f.pos1.b);

		glTexCoord2f(_tex[f.pos1.vecid].x, _tex[f.pos1.vecid].y);
		glVertex3f(f.pos1.x, f.pos1.y, f.pos1.z);

		glTexCoord2f(_tex[f.pos2.vecid].x, _tex[f.pos2.vecid].y);
		glVertex3f(f.pos2.x, f.pos2.y, f.pos2.z);

		glTexCoord2f(_tex[f.pos3.vecid].x, _tex[f.pos3.vecid].y);
		glVertex3f(f.pos3.x, f.pos3.y, f.pos3.z);

		glEnd();
	}
};

void terrain::draw(bool texon) const {

	glDisable(GL_LIGHTING);
	//glEnable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);

	glDisable(GL_CULL_FACE);

	//DrawPoints();
	DrawFaces();
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
#include "Mat44.h"
#include "float4.h"
#include <pmmintrin.h>

const f32 IDENTITY_MAT44_DATA[] = 
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

const Mat44 Mat44::IDENTITY = Mat44();

Mat44::Mat44(void)
{
	// Alternative to memcpy (in SetMatrix) is to memset(...) all to 0, then set mat indices 0,5,10 and 15 to 1
	SetMatrix(IDENTITY_MAT44_DATA);
}

Mat44::Mat44(const f32* _mat) // use existing FP array to initialise the matrix
{
	SetMatrix(_mat);
};

Mat44::Mat44(
		const f32 m11, const f32 m12, const f32 m13, const f32 m14,
		const f32 m21, const f32 m22, const f32 m23, const f32 m24,
		const f32 m31, const f32 m32, const f32 m33, const f32 m34,
		const f32 m41, const f32 m42, const f32 m43, const f32 m44)
{
	// set each value individually
	mat[0]  = m11;  mat[1]  = m12;  mat[2]  = m13;  mat[3]  = m14;
	mat[4]  = m21;  mat[5]  = m22;  mat[6]  = m23;  mat[7]  = m24;
	mat[8]  = m31;  mat[9]  = m32;  mat[10] = m33;  mat[11] = m34;
	mat[12] = m41;  mat[13] = m42;  mat[14] = m43;  mat[15] = m44;
};

Mat44::~Mat44(void)
{
}

const f32* const Mat44::GetMatrix() const
{
	return mat;
};

void Mat44::SetMatrix(const f32* _mat)
{
	memcpy(mat, _mat, sizeof(f32) * Mat44::MATRIX_ELEMENT_COUNT);
};

void Mat44::Identity()
{
	SetMatrix(IDENTITY_MAT44_DATA);
};

Mat44 Mat44::Add(const Mat44 &m) const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	__m128 mat2rows[] = { _mm_load_ps(m.mat), _mm_load_ps(m.mat+4), _mm_load_ps(m.mat+8), _mm_load_ps(m.mat+12) };
	
	Mat44 res;
	_mm_store_ps(res.mat   , _mm_add_ps(mat1rows[0], mat2rows[0]));
	_mm_store_ps(res.mat+4 , _mm_add_ps(mat1rows[1], mat2rows[1]));
	_mm_store_ps(res.mat+8 , _mm_add_ps(mat1rows[2], mat2rows[2]));
	_mm_store_ps(res.mat+12, _mm_add_ps(mat1rows[3], mat2rows[3]));
	return res;
};

Mat44 Mat44::Add(const f32 v) const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	__m128 splatAdd = _mm_load1_ps(&v);

	Mat44 res;
	_mm_store_ps(res.mat   , _mm_add_ps(mat1rows[0], splatAdd));
	_mm_store_ps(res.mat+4 , _mm_add_ps(mat1rows[1], splatAdd));
	_mm_store_ps(res.mat+8 , _mm_add_ps(mat1rows[2], splatAdd));
	_mm_store_ps(res.mat+12, _mm_add_ps(mat1rows[3], splatAdd));
	return res;
};

Mat44 Mat44::Sub(const Mat44 &m) const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	__m128 mat2rows[] = { _mm_load_ps(m.mat), _mm_load_ps(m.mat+4), _mm_load_ps(m.mat+8), _mm_load_ps(m.mat+12) };
	
	Mat44 res;
	_mm_store_ps(res.mat   , _mm_sub_ps(mat1rows[0], mat2rows[0]));
	_mm_store_ps(res.mat+4 , _mm_sub_ps(mat1rows[1], mat2rows[1]));
	_mm_store_ps(res.mat+8 , _mm_sub_ps(mat1rows[2], mat2rows[2]));
	_mm_store_ps(res.mat+12, _mm_sub_ps(mat1rows[3], mat2rows[3]));
	return res;
};

Mat44 Mat44::Sub(const f32 v) const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	__m128 splatSub = _mm_load1_ps(&v);

	Mat44 res;
	_mm_store_ps(res.mat   , _mm_sub_ps(mat1rows[0], splatSub));
	_mm_store_ps(res.mat+4 , _mm_sub_ps(mat1rows[1], splatSub));
	_mm_store_ps(res.mat+8 , _mm_sub_ps(mat1rows[2], splatSub));
	_mm_store_ps(res.mat+12, _mm_sub_ps(mat1rows[3], splatSub));
	return res;
};

Mat44 Mat44::Mult(const f32 scale) const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	__m128 splatMult = _mm_load1_ps(&scale);

	Mat44 res;
	_mm_store_ps(res.mat   , _mm_mul_ps(mat1rows[0], splatMult));
	_mm_store_ps(res.mat+4 , _mm_mul_ps(mat1rows[1], splatMult));
	_mm_store_ps(res.mat+8 , _mm_mul_ps(mat1rows[2], splatMult));
	_mm_store_ps(res.mat+12, _mm_mul_ps(mat1rows[3], splatMult));
	return res;
};

Mat44 Mat44::Mult_ComponentWise(const Mat44 &m) const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	__m128 mat2rows[] = { _mm_load_ps(m.mat), _mm_load_ps(m.mat+4), _mm_load_ps(m.mat+8), _mm_load_ps(m.mat+12) };
	
	Mat44 res;
	_mm_store_ps(res.mat   , _mm_mul_ps(mat1rows[0], mat2rows[0]));
	_mm_store_ps(res.mat+4 , _mm_mul_ps(mat1rows[1], mat2rows[1]));
	_mm_store_ps(res.mat+8 , _mm_mul_ps(mat1rows[2], mat2rows[2]));
	_mm_store_ps(res.mat+12, _mm_mul_ps(mat1rows[3], mat2rows[3]));
	return res;
};

Mat44 Mat44::Mult(const Mat44 &m) const
{
	Mat44 a = Transpose();
	Mat44 b = m.Transpose();
	Mat44 out;

	__m128 a_line, b_line, r_line;
	for(c8 i=0;i<16;i+=4)
	{
		// First column
		a_line = _mm_load_ps(a.mat);
		b_line = _mm_load1_ps(&b.mat[i]);
		r_line = _mm_mul_ps(a_line, b_line);

		// Second column
		a_line = _mm_load_ps(&a.mat[4]);
		b_line = _mm_load1_ps(&b.mat[i+1]);
		r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

		// Third column
		a_line = _mm_load_ps(&a.mat[8]);
		b_line = _mm_load1_ps(&b.mat[i+2]);
		r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

		// Last column
		a_line = _mm_load_ps(&a.mat[12]);
		b_line = _mm_load1_ps(&b.mat[i+3]);
		r_line = _mm_add_ps(_mm_mul_ps(a_line, b_line), r_line);

		_mm_store_ps(&out.mat[i], r_line);
	}

	return out.Transpose();
};

float4 Mat44::Mult(const float4 &m) const
{
	Mat44 tr = Transpose();
	__m128 matcols[] =
	{
		_mm_load_ps(tr.mat),
		_mm_load_ps(tr.mat+4),
		_mm_load_ps(tr.mat+8),
		_mm_load_ps(tr.mat+12)
	};

	__m128 v = _mm_load_ps(m.vec);
	
	// Broadcast vector into SSE registers
	__m128 xb = _mm_shuffle_ps(v,v,0x00);
	__m128 yb = _mm_shuffle_ps(v,v,0x55);
	__m128 zb = _mm_shuffle_ps(v,v,0xAA);
	__m128 wb = _mm_shuffle_ps(v,v,0xFF);

	// Perform multiplication by matrix columns
	xb = _mm_mul_ps(xb, matcols[0]);
	yb = _mm_mul_ps(yb, matcols[1]);
	zb = _mm_mul_ps(zb, matcols[2]);
	wb = _mm_mul_ps(wb, matcols[3]);

	// Add results
	__m128 r = _mm_add_ps(_mm_add_ps(xb, yb),_mm_add_ps(zb, wb));

	float4 returnVec;
	_mm_store_ps(returnVec.vec, r);
	return returnVec;
};

void Mat44::BatchMult(const float4 * const in, float4 *out, u32 len) const
{
	Mat44 tr = Transpose();
	__m128 matcols[] =
	{
		_mm_load_ps(tr.mat),
		_mm_load_ps(tr.mat+4),
		_mm_load_ps(tr.mat+8),
		_mm_load_ps(tr.mat+12)
	};

	while(len--)
	{
		__m128 v = _mm_load_ps(in[len].vec);
		_mm_prefetch((const char*)&in[len+1], _MM_HINT_T0);

		// Broadcast vector into SSE registers
		__m128 xb = _mm_shuffle_ps(v,v,0x00);
		__m128 yb = _mm_shuffle_ps(v,v,0x55);
		__m128 zb = _mm_shuffle_ps(v,v,0xAA);
		__m128 wb = _mm_shuffle_ps(v,v,0xFF);

		// Perform multiplication by matrix columns
		xb = _mm_mul_ps(xb, matcols[0]);
		yb = _mm_mul_ps(yb, matcols[1]);
		zb = _mm_mul_ps(zb, matcols[2]);
		wb = _mm_mul_ps(wb, matcols[3]);

		// Add results
		__m128 r = _mm_add_ps(_mm_add_ps(xb, yb),_mm_add_ps(zb, wb));

		_mm_prefetch((const char*)&out[len+1], _MM_HINT_T0);
		_mm_store_ps(out[len].vec, r);
	}
};

Mat44 Mat44::Transpose() const
{
	__m128 mat1rows[] = { _mm_load_ps(mat), _mm_load_ps(mat+4), _mm_load_ps(mat+8), _mm_load_ps(mat+12) };
	_MM_TRANSPOSE4_PS(mat1rows[0], mat1rows[1], mat1rows[2], mat1rows[3]);

	Mat44 res;
	_mm_store_ps(res.mat   , mat1rows[0]);
	_mm_store_ps(res.mat+4 , mat1rows[1]);
	_mm_store_ps(res.mat+8 , mat1rows[2]);
	_mm_store_ps(res.mat+12, mat1rows[3]);
	return res;
};

void Mat44::write(std::ostream &out)
{
	out <<
		mat[0] << "," << mat[1] << "," << mat[2] << "," << mat[3] << "\n" <<
		mat[4] << "," << mat[5] << "," << mat[6] << "," << mat[7] << "\n" <<
		mat[8] << "," << mat[9] << "," << mat[10] << "," << mat[11] << "\n" <<
		mat[12] << "," << mat[13] << "," << mat[14] << "," << mat[15] << std::endl;
};

std::ostream& operator<<(std::ostream &out, Mat44 &m)
{
	m.write(out);
	return out;
};
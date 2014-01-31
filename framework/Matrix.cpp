#include "Common.h"

#include "Matrix.h"

void matrix::SetIdentity(Mat4x4& matrix)
{
	matrix.col[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
	matrix.col[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	matrix.col[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
	matrix.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);
}

void matrix::SetTranslation(Mat4x4& matrix, const Vec3& pos)
{
	// Translation [Real-time Rendering, page 56]

	matrix.col[3] = Vec4(pos.x, pos.y, pos.z, 1.0f);
}


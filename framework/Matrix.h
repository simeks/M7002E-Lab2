#ifndef __FRAMEWORK_MATRIX_H__
#define __FRAMEWORK_MATRIX_H__

#include "Vector.h"

/// @brief Column-major matrix.
struct Mat4x4
{
	Vec4 col[4]; // Columns

};

namespace matrix
{
	void SetIdentity(Mat4x4& matrix);
	void SetTranslation(Mat4x4& matrix, const Vec3& pos);

};

#endif // __FRAMEWORK_MATRIX_H__

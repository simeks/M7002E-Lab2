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
	/// @brief Creates an identity matrix.
	Mat4x4 CreateIdentity();

	/// @brief Creates a translation matrix with the specified translation.
	Mat4x4 CreateTranslationMatrix(const Vec3& translation);
	
	/// @brief Creates a rotation matrix with the specified angle around the x-axis.
	Mat4x4 CreateRotationMatrixX(float angle);

	/// @brief Creates a rotation matrix with the specified angle around the y-axis.
	Mat4x4 CreateRotationMatrixY(float angle);

	/// @brief Creates a rotation matrix with the specified angle around the z-axis.
	Mat4x4 CreateRotationMatrixZ(float angle);

	/// @brief Creates a rotation matrix with the specified rotation.
	Mat4x4 CreateRotationMatrixXYZ(float head, float pitch, float roll);

	/// @brief Creates a scaling matrix with the specified scale.
	Mat4x4 CreateScalingMatrix(const Vec3& scale);

	/// @brief Multiplies two matrices.
	Mat4x4 Multiply(const Mat4x4& lhs, const Mat4x4& rhs);

};

#endif // __FRAMEWORK_MATRIX_H__

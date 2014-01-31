#include "Common.h"

#include "Matrix.h"

Mat4x4 matrix::CreateIdentity()
{
	Mat4x4 result;

	result.col[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
	result.col[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	result.col[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateTranslationMatrix(const Vec3& translation)
{
	Mat4x4 result = CreateIdentity();

	// Translation [Real-time Rendering, 4.2, page 56]
	result.col[3] = Vec4(translation.x, translation.y, translation.z, 1.0f);

	return result;
}
Mat4x4 matrix::CreateRotationMatrixX(float angle)
{
	Mat4x4 result;

	// Rotation [Real-time Rendering, 4.3, page 57]
	result.col[0] = Vec4(1.0f, 0.0f, 0.0f, 0.0f);
	result.col[1] = Vec4(0.0f, cosf(angle), sinf(angle), 0.0f);
	result.col[2] = Vec4(0.0f, -sinf(angle), cosf(angle), 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateRotationMatrixY(float angle)
{
	Mat4x4 result;

	// Rotation [Real-time Rendering, 4.4, page 57]
	result.col[0] = Vec4(cosf(angle), 0.0f, -sinf(angle), 0.0f);
	result.col[1] = Vec4(0.0f, 1.0f, 0.0f, 0.0f);
	result.col[2] = Vec4(sinf(angle), 0.0f, cosf(angle), 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateRotationMatrixZ(float angle)
{
	Mat4x4 result;

	// Rotation [Real-time Rendering, 4.5, page 57]
	result.col[0] = Vec4(cosf(angle), sinf(angle), 0.0f, 0.0f);
	result.col[1] = Vec4(-sinf(angle), cosf(angle), 0.0f, 0.0f);
	result.col[2] = Vec4(0.0f, 0.0f, 1.0f, 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::CreateRotationMatrixXYZ(float head, float pitch, float roll)
{

	// To rotate in (head, pitch, roll) we first need to create one matrix for each axis,
	//	then we can just multiply them to get the resulting rotation matrix.
	//	E(yaw, pitch, roll) = Rz(roll) Rx(pitch) Ry(head) [RTR, 4.17, page 66]

	Mat4x4 rx = CreateRotationMatrixY(pitch); 
	Mat4x4 ry = CreateRotationMatrixX(head); 
	Mat4x4 rz = CreateRotationMatrixX(roll); 

	Mat4x4 result = matrix::Multiply(ry, rz);
	result = matrix::Multiply(rx, result);

	return result;
}

Mat4x4 matrix::CreateScalingMatrix(const Vec3& scale)
{
	Mat4x4 result;

	// Scaling [Real-time Rendering, 4.8, page 58]
	result.col[0] = Vec4(scale.x, 0.0f, 0.0f, 0.0f);
	result.col[1] = Vec4(0.0f, scale.y, 0.0f, 0.0f);
	result.col[2] = Vec4(0.0f, 0.0f, scale.z, 0.0f);
	result.col[3] = Vec4(0.0f, 0.0f, 0.0f, 1.0f);

	return result;
}

Mat4x4 matrix::Multiply(const Mat4x4& lhs, const Mat4x4& rhs)
{
	Mat4x4 result;

	// Matrix multiplication [Real-Time Rendering, A.26, page 899]
	result.col[0].x = lhs.col[0].x * rhs.col[0].x + lhs.col[1].x * rhs.col[0].y + lhs.col[2].x * rhs.col[0].z + lhs.col[3].x * rhs.col[0].w;
	result.col[0].y = lhs.col[0].y * rhs.col[0].x + lhs.col[1].y * rhs.col[0].y + lhs.col[2].y * rhs.col[0].z + lhs.col[3].y * rhs.col[0].w;
	result.col[0].z = lhs.col[0].z * rhs.col[0].x + lhs.col[1].z * rhs.col[0].y + lhs.col[2].z * rhs.col[0].z + lhs.col[3].z * rhs.col[0].w;
	result.col[0].w = lhs.col[0].w * rhs.col[0].x + lhs.col[1].w * rhs.col[0].y + lhs.col[2].w * rhs.col[0].z + lhs.col[3].w * rhs.col[0].w;

	result.col[1].x = lhs.col[0].x * rhs.col[1].x + lhs.col[1].x * rhs.col[1].y + lhs.col[2].x * rhs.col[1].z + lhs.col[3].x * rhs.col[1].w;
	result.col[1].y = lhs.col[0].y * rhs.col[1].x + lhs.col[1].y * rhs.col[1].y + lhs.col[2].y * rhs.col[1].z + lhs.col[3].y * rhs.col[1].w;
	result.col[1].z = lhs.col[0].z * rhs.col[1].x + lhs.col[1].z * rhs.col[1].y + lhs.col[2].z * rhs.col[1].z + lhs.col[3].z * rhs.col[1].w;
	result.col[1].w = lhs.col[0].w * rhs.col[1].x + lhs.col[1].w * rhs.col[1].y + lhs.col[2].w * rhs.col[1].z + lhs.col[3].w * rhs.col[1].w;
	
	result.col[2].x = lhs.col[0].x * rhs.col[2].x + lhs.col[1].x * rhs.col[2].y + lhs.col[2].x * rhs.col[2].z + lhs.col[3].x * rhs.col[2].w;
	result.col[2].y = lhs.col[0].y * rhs.col[2].x + lhs.col[1].y * rhs.col[2].y + lhs.col[2].y * rhs.col[2].z + lhs.col[3].y * rhs.col[2].w;
	result.col[2].z = lhs.col[0].z * rhs.col[2].x + lhs.col[1].z * rhs.col[2].y + lhs.col[2].z * rhs.col[2].z + lhs.col[3].z * rhs.col[2].w;
	result.col[2].w = lhs.col[0].w * rhs.col[2].x + lhs.col[1].w * rhs.col[2].y + lhs.col[2].w * rhs.col[2].z + lhs.col[3].w * rhs.col[2].w;
	
	result.col[3].x = lhs.col[0].x * rhs.col[3].x + lhs.col[1].x * rhs.col[3].y + lhs.col[2].x * rhs.col[3].z + lhs.col[3].x * rhs.col[3].w;
	result.col[3].y = lhs.col[0].y * rhs.col[3].x + lhs.col[1].y * rhs.col[3].y + lhs.col[2].y * rhs.col[3].z + lhs.col[3].y * rhs.col[3].w;
	result.col[3].z = lhs.col[0].z * rhs.col[3].x + lhs.col[1].z * rhs.col[3].y + lhs.col[2].z * rhs.col[3].z + lhs.col[3].z * rhs.col[3].w;
	result.col[3].w = lhs.col[0].w * rhs.col[3].x + lhs.col[1].w * rhs.col[3].y + lhs.col[2].w * rhs.col[3].z + lhs.col[3].w * rhs.col[3].w;
	
	return result;
}

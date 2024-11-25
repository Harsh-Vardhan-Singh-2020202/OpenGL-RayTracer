//transformMatrix.h
#ifndef _TRANSFORMMATRIX_H_
#define _TRANSFORMMATRIX_H_

#include "vector3D.h"

class TransformMatrix{
private:

public:
    float m[4][4];
    TransformMatrix();
    TransformMatrix(float mat[4][4]);

    // Methods for transformations
    TransformMatrix Translation(float tx, float ty, float tz);
    TransformMatrix Scaling(float sx, float sy, float sz);
    TransformMatrix RotationX(float angle);
    TransformMatrix RotationY(float angle);
    TransformMatrix RotationZ(float angle);
    TransformMatrix Identity();
    TransformMatrix Inverse();
    float determinant();

    // Multiply 2 matrices
    TransformMatrix operator*(const TransformMatrix& other) const;

    // Multiply  matrix with vector
    Vector3D operator*(const Vector3D& v) const;
};

#endif
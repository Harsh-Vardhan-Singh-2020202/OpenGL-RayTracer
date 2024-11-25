//transformMatrix.cpp
#include "transformMatrix.h"
#include "vector3D.h"
#include <cmath>

TransformMatrix::TransformMatrix() {
    // Initialize as an identity matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            if (i == j) m[i][j] = 1.0f;
            else m[i][j] = 0.0f;
        }
    }
}

TransformMatrix::TransformMatrix(float mat[4][4]) {
    // Initialize with the given matrix
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            m[i][j] = mat[i][j];
        }
    }
}

TransformMatrix TransformMatrix::Translation(float tx, float ty, float tz) {
    TransformMatrix translation;
    translation.m[0][0] = 1.0;
    translation.m[1][1] = 1.0;
    translation.m[2][2] = 1.0;
    translation.m[3][3] = 1.0;
    translation.m[0][3] = tx;
    translation.m[1][3] = ty;
    translation.m[2][3] = tz;
    return translation;
}

TransformMatrix TransformMatrix::Scaling(float sx, float sy, float sz) {
    TransformMatrix scaling;
    scaling.m[0][0] = sx;
    scaling.m[1][1] = sy;
    scaling.m[2][2] = sz;
    scaling.m[3][3] = 1.0;
    return scaling;
}

TransformMatrix TransformMatrix::RotationX(float angle) {
    TransformMatrix rotation;
    float cosA = cos(angle);
    float sinA = sin(angle);
    rotation.m[1][1] = cosA;
    rotation.m[1][2] = -sinA;
    rotation.m[2][1] = sinA;
    rotation.m[2][2] = cosA;
    return rotation;
}

TransformMatrix TransformMatrix::RotationY(float angle) {
    TransformMatrix rotation;
    float cosA = cos(angle);
    float sinA = sin(angle);
    rotation.m[0][0] = cosA;
    rotation.m[0][2] = sinA;
    rotation.m[2][0] = -sinA;
    rotation.m[2][2] = cosA;
    return rotation;
}

TransformMatrix TransformMatrix::RotationZ(float angle)
{
    TransformMatrix rotation;
    float cosA = cos(angle);
    float sinA = sin(angle);
    rotation.m[0][0] = cosA;
    rotation.m[0][1] = -sinA;
    rotation.m[1][0] = sinA;
    rotation.m[1][1] = cosA;
    return rotation;
}

TransformMatrix TransformMatrix::Identity()
{
    float id_mat[4][4] = {{1.0, 0.0, 0.0, 0.0},
                          {0.0, 1.0, 0.0, 0.0},
                          {0.0, 0.0, 1.0, 0.0},
                          {0.0, 0.0, 0.0, 1.0}};
    return TransformMatrix(id_mat);
}

float TransformMatrix::determinant()
{
    const float (*mat)[4] = m; // Use a pointer to float[4] for easier indexing

    // Calculate the determinant using Laplace expansion
    float det = 0.0;

    for (int i = 0; i < 4; i++) {
        // Calculate the sub matrix for this element
        float subMat[3][3];
        int subRow = 0, subCol = 0;

        for (int row = 0; row < 4; row++) {
            if (row == i) continue;
            subCol = 0;

            for (int col = 0; col < 4; col++) {
                if (col == 0) continue;
                subMat[subRow][subCol] = mat[row][col];
                subCol++;
            }

            subRow++;
        }

        // Calculate the determinant of the submatrix
        float subDet = subMat[0][0] * (subMat[1][1] * subMat[2][2] - subMat[1][2] * subMat[2][1]) -
                       subMat[0][1] * (subMat[1][0] * subMat[2][2] - subMat[1][2] * subMat[2][0]) +
                       subMat[0][2] * (subMat[1][0] * subMat[2][1] - subMat[1][1] * subMat[2][0]);

        // Accumulate the determinant with proper sign
        det += mat[i][0] * subDet * ((i % 2 == 0) ? 1.0f : -1.0f);
    }

    return det;
}

TransformMatrix TransformMatrix::Inverse()
{
    TransformMatrix result;

    // Calculate the determinant of the original matrix
    float det = determinant();

    // Define a small value for EPSILON
    float EPSILON = 1e-6;

    // Check if the matrix is invertible (determinant not close to zero)
    if (fabs(det) < EPSILON) {
        // Return the identity matrix if the matrix is not invertible
        return Identity();
    }

    // Calculate the elements of the inverse matrix
    for (int row = 0; row < 4; row++) {
        for (int col = 0; col < 4; col++) {
            // Calculate the sub matrix for this element
            float subMatrix[3][3];
            int subRow = 0, subCol = 0;
            for (int i = 0; i < 4; i++) {
                if (i == row) continue;
                subCol = 0;
                for (int j = 0; j < 4; j++) {
                    if (j == col) continue;
                    subMatrix[subRow][subCol] = m[i][j];
                    subCol++;
                }
                subRow++;
            }

            // Calculate the determinant of the sub matrix
            float subDet = subMatrix[0][0] * (subMatrix[1][1] * subMatrix[2][2] - subMatrix[1][2] * subMatrix[2][1]) -
                           subMatrix[0][1] * (subMatrix[1][0] * subMatrix[2][2] - subMatrix[1][2] * subMatrix[2][0]) +
                           subMatrix[0][2] * (subMatrix[1][0] * subMatrix[2][1] - subMatrix[1][1] * subMatrix[2][0]);

            // Calculate the cofactor
            float cofactor = subDet / det;

            // Place the cofactor in the adjudicate matrix with proper sign
            result.m[row][col] = (row + col) % 2 == 0 ? cofactor : -cofactor;
        }
    }

    return result;
}

Vector3D TransformMatrix::operator*(const Vector3D& v) const {
    // Perform matrix-vector multiplication
    float x = m[0][0] * v.X() + m[0][1] * v.Y() + m[0][2] * v.Z() + m[0][3];
    float y = m[1][0] * v.X() + m[1][1] * v.Y() + m[1][2] * v.Z() + m[1][3];
    float z = m[2][0] * v.X() + m[2][1] * v.Y() + m[2][2] * v.Z() + m[2][3];

    return Vector3D(x, y, z);
}

TransformMatrix TransformMatrix::operator*(const TransformMatrix& other) const {
    TransformMatrix result;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += m[i][k] * other.m[k][j];
            }
        }
    }

    return result;
}

//
// Created by haley on 10/6/25.
//





// Unil Friday, I did not realize we were allowed to use GLM in order to make matrix math easier.
// As a result, I implemented some GLM functionality by myself....
// home cooked linear algebra :sob:
#include <cmath>
 struct Mat4 {
    float m[16]; // 4x4 matrix

    Mat4() {
        // initialize to identity matrix
        for (int i = 0; i < 16; i++) m[i] = 0.0f;
        m[0] = m[5] = m[10] = m[15] = 1.0f;
    }
     Mat4(float m00, float m01, float m02, float m03,
          float m10, float m11, float m12, float m13,
          float m20, float m21, float m22, float m23,
          float m30, float m31, float m32, float m33) {
        m[0] = m00; m[1] = m01; m[2] = m02; m[3] = m03;
        m[4] = m10; m[5] = m11; m[6] = m12; m[7] = m13;
        m[8] = m20; m[9] = m21; m[10] = m22; m[11] = m23;
        m[12] = m30; m[13] = m31; m[14] = m32; m[15] = m33;
          }

};

// return identity matrix
Mat4 identity() {
    return Mat4();
}



// Perspective projection !!
// lifted wholesale from GLM documentation and a stackoverflow post
// https://stackoverflow.com/questions/8115352/glmperspective-explanation
Mat4 perspective(float fovy, float aspect, float near, float far) {
    Mat4 result;
    float f = 1.0f / tan(fovy / 2.0f);

    result.m[0] = f / aspect;
    result.m[5] = f;
    result.m[10] = (far + near) / (near - far);
    result.m[11] = -1.0f;
    result.m[14] = (2.0f * far * near) / (near - far);
    result.m[15] = 0.0f;

    return result;
}

// Create translation matrix --
Mat4 translate(float x, float y, float z) {
    Mat4 result;
    result.m[12] = x;
    result.m[13] = y;
    result.m[14] = z;
    return result;
}

// Create lookAt view matrix like in GLM
// referenced https://stackoverflow.com/questions/21830340/understanding-glmlookat
Mat4 lookAt(float eyeX, float eyeY, float eyeZ,
            float centerX, float centerY, float centerZ,
            float upX, float upY, float upZ) {
    // Calculate forward vector (eye to center)
    float fx = centerX - eyeX;
    float fy = centerY - eyeY;
    float fz = centerZ - eyeZ;

    // Normalize forward
    float flen = sqrt(fx*fx + fy*fy + fz*fz);
    fx /= flen; fy /= flen; fz /= flen;

    // Calculate right vector (forward x up)
    float rx = fy * upZ - fz * upY;
    float ry = fz * upX - fx * upZ;
    float rz = fx * upY - fy * upX;

    // Normalize right
    float rlen = sqrt(rx*rx + ry*ry + rz*rz);
    rx /= rlen; ry /= rlen; rz /= rlen;

    // Calculate true up vector (right x forward)
    float ux = ry * fz - rz * fy;
    float uy = rz * fx - rx * fz;
    float uz = rx * fy - ry * fx;

    Mat4 result;
    result.m[0] = rx;   result.m[4] = ux;   result.m[8]  = -fx;  result.m[12] = 0;
    result.m[1] = ry;   result.m[5] = uy;   result.m[9]  = -fy;  result.m[13] = 0;
    result.m[2] = rz;   result.m[6] = uz;   result.m[10] = -fz;  result.m[14] = 0;
    result.m[3] = 0;    result.m[7] = 0;    result.m[11] = 0;    result.m[15] = 1;

    // Translate
    result.m[12] = -(rx * eyeX + ry * eyeY + rz * eyeZ);
    result.m[13] = -(ux * eyeX + uy * eyeY + uz * eyeZ);
    result.m[14] = (fx * eyeX + fy * eyeY + fz * eyeZ);

    return result;
}
Mat4 multiplyMatrices(const float* a, const float* b) {
    Mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++) {
                result.m[i * 4 + j] += a[i * 4 + k] * b[k * 4 + j];
            }
        }
    }
    return result;
}
// Rotation around Y axis
Mat4 rotateY(float angle) {
    Mat4 result;
    float c = cos(angle);
    float s = sin(angle);

    result.m[0] = c;
    result.m[2] = s;
    result.m[8] = -s;
    result.m[10] = c;

    return result;
}
// Same but for x
Mat4 rotateX(float angle) {
    Mat4 result;
    float c = cos(angle);
    float s = sin(angle);

    result.m[5] = c;
    result.m[6] = s;
    result.m[9] = -s;
    result.m[10] = c;
}


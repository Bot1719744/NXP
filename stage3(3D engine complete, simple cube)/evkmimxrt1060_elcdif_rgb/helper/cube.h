#define APP_IMG_HEIGHT 272
#define APP_IMG_WIDTH  480
#define PI 3.14159265

/*******************************************************************************
 *Vector stuff here
 ******************************************************************************/
typedef struct {
    float x;
    float y;
    float z;
} Vector3;

/* check */
Vector3 initVector3(float x, float y, float z) {
    Vector3 v;
    v.x = x;
    v.y = y;
    v.z = z;    

    return v;
}

/* check */
void normalize(Vector3* v){
    float magnitude = sqrt(v->x * v->x + v->y * v->y + v->z * v->z);
    if (magnitude > 0)
    {
        v->x /= magnitude;
        v->y /= magnitude;
        v->z /= magnitude;
    }
}

/* check */
Vector3 cross(const Vector3 v1, const Vector3 v2){
    Vector3 result;
    result.x = v1.y * v2.z - v1.z * v2.y;
    result.y = v1.z * v2.x - v1.x * v2.z;
    result.z = v1.x * v2.y - v1.y * v2.x;
    return result;
}

/* check */
Vector3 subtractVectors(Vector3 v1, Vector3 v2){
    Vector3 result;
    result.x = v1.x - v2.x;
    result.y = v1.y - v2.y;
    result.z = v1.z - v2.z;
    return result;
}


/*******************************************************************************
 * Matrix stuff here
 ******************************************************************************/
typedef struct {
    float elements[16];
} Matrix4;

void identity(Matrix4* mat)
{
    for(int i = 0; i < 16; i++)
    {
        mat->elements[i] = 0;
    }
    mat->elements[0] = 1;
    mat->elements[5] = 1;
    mat->elements[10] = 1;
    mat->elements[15] = 1;
}


/* Matrix4 multiply vector4 and store result into vector4 (check) */
void multiplyVector4(Matrix4 matrix4, float vector[4]){
    float x = vector[0];
    float y = vector[1];
    float z = vector[2];
    float w = vector[3];

    vector[0] = (x * matrix4.elements[0]) + (y * matrix4.elements[1]) + (z * matrix4.elements[2]) + (w * matrix4.elements[3]);
    vector[1] = (x * matrix4.elements[4]) + (y * matrix4.elements[5]) + (z * matrix4.elements[6]) + (w * matrix4.elements[7]);
    vector[2] = (x * matrix4.elements[8]) + (y * matrix4.elements[9]) + (z * matrix4.elements[10]) + (w * matrix4.elements[11]);
    vector[3] = (x * matrix4.elements[12]) + (y * matrix4.elements[13]) + (z * matrix4.elements[14]) + (w * matrix4.elements[15]);

}


/* A multiply B and return a matrix (check) */
Matrix4 multiplyMatrix4(Matrix4 A, Matrix4 B){
    Matrix4 res;

    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            res.elements[i * 4 + j] = 0;
            for (int k = 0; k < 4; k++)
                res.elements[i * 4 + j] += A.elements[i * 4 + k] * B.elements[k * 4 + j];
        }
    }

    return res;
}

/* For modelMatrix (check) */
void translate(Matrix4* mat, float x, float y, float z){
    Matrix4 res;
    identity(&res);
    res.elements[3] = x;
    res.elements[7] = y;
    res.elements[11] = z;

    *mat = multiplyMatrix4(*mat, res);
}

/* For modelviewMatrix (check) */
void rotate(Matrix4* mat, float degree, float x, float y, float z){
    Matrix4 res;
    identity(&res);

    float c = cos(degree * PI / 180.0f);
    float s = sin(degree * PI / 180.0f);

    res.elements[0] = (1 - c) * x * x + c;
    res.elements[1] = (1 - c) * x * y - s * z;
    res.elements[2] = (1 - c) * x * z + s * z;

    res.elements[4] = (1 - c) * x * y + s * z;
    res.elements[5] = (1 - c) * y * y + c;
    res.elements[6] = (1 - c) * y * z - s * x;

    res.elements[8] = (1 - c) * x * z - s * y;
    res.elements[9] = (1 - c) * y * z + s * x;
    res.elements[10] = (1 - c) * z * z + c;

   *mat = multiplyMatrix4(*mat, res);
}

/* For modelviewMatrix */
void scaleMatrix_set(Matrix4* mat, float x, float y, float z){
    Matrix4 res;
    for(int i = 0; i < 16; i++)
    {
        res.elements[i] = 0;
    }

    res.elements[0] = x;
    res.elements[5] = y;
    res.elements[10] = z;
    res.elements[15] = 1;

    *mat = multiplyMatrix4(*mat, res);
}

/* For perspective projection (check) */
void perspective(Matrix4 *mat, float l, float r, float b, float t, float n, float f){
    Matrix4 res;
    for (int i = 0; i < 16; i++){
        res.elements[i] = 0;
    }

    res.elements[0]  =  2 * n / (r - l);
    res.elements[2]  =  (r + l) / (r - l);
    res.elements[5]  =  2 * n / (t - b);
    res.elements[6]  =  (t + b) / (t - b);
    res.elements[10] = -(f + n) / (f - n);
    res.elements[11] = -(2 * f * n) / (f - n);
    res.elements[14] = -1;

    *mat = multiplyMatrix4(*mat, res);
}

/* For transform into Window Coordinates */
void window_coordinates(float pointTable[8][4], float X, float Y, float w, float h, float n, float f){
    float x, y, z;
    for (int i = 0; i < 8; i++){
        x =  pointTable[i][0];
        y =  pointTable[i][1];
        z =  pointTable[i][2];

        pointTable[i][0] = w/2 * x + X + w/2;
        pointTable[i][1] = h/2 * y + Y + h/2;
        pointTable[i][2] = (f - n) / 2 * z + (f + n) / 2;
    }
}

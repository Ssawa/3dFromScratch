#ifndef MATRIX_H
#define MATRIX_H

#define MATRIX_BLANK(y, x) {y, x, (double*)(double[y][x]) {}}
#define MATRIX(y, x, ...) {y, x, (double*)(double[y][x]) __VA_ARGS__}
// TODO - See if there's an easier way to handle scalable 2d arrays
// preferably something not involving macro magic 
typedef struct matrix {
	unsigned int rows;
	unsigned int columns;
	double* array;
} Matrix;

typedef struct vector3d {
	double x;
	double y;
	double z;
} Vector3d;

typedef struct vector2d {
	double x;
	double y;
} Vector2d;

int addMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum);
int subtractMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum);

int multiplyMatrixByScalar(Matrix* matrix, int scalar);
int divideMatrixByScalar(Matrix* matrix, int scalar);

int multiplyMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixProduct);

void getIdentityMatrix(Matrix* matrix);
void getTranslationMatrix(Matrix* matrix, int x, int y, int z);
void getScalingMatrix(Matrix* matrix, int x, int y, int z);
void getRotationXMatrix(Matrix* matrix, double theta);
void getRotationYMatrix(Matrix* matrix, double theta);
void getRotationZMatrix(Matrix* matrix, double theta);

// Matrix should be small enough, for our purposes, we can pass it by value
void multiplyVectorByMatrix(Vector3d* vector, Matrix matrix);

// Project 3d vector onto 2d screen
Vector2d project3dToScreen(Vector3d vector, int screenWidth, int screenHeight);

// For debugging
void printMatrix(Matrix matrix);

#endif
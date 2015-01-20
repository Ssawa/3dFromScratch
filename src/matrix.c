#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "matrix.h"

int getIndex(int width, int x, int y) {
	return (width * x + y);
}

typedef enum {ADD, SUBTRACT} AddOrSubtract;
// Return 0 for success, anything else for an error
int addOrSubtractMatrixes(AddOrSubtract addOrSubtract, Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {

	// To add matrixes they must be the same dimensions
	if (matrix1.rows != matrix2.rows || matrix1.columns != matrix2.columns) {
		return 1;
	}

	// This could just as easily have been initialized with matrix2, as we should have
	// already verified that they are the same dimensions.
	int sumRows = matrix1.rows;
	int sumColumns = matrix1.columns;
	matrixSum->rows = sumRows;
	matrixSum->columns = sumColumns;

	for (int y = 0; y < sumRows; y++) {
		for (int x = 0; x < sumColumns; x++) {
			int index = getIndex(sumColumns, y, x);
			switch (addOrSubtract) {
				case ADD: {
					matrixSum->array[index] = matrix1.array[index] + matrix2.array[index];
					break;
				}
				case SUBTRACT: {
					matrixSum->array[index] = matrix1.array[index] - matrix2.array[index];
					break;
				}
			}
		}
	}
	return 0;
}
// Small convinence functions
int addMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	return addOrSubtractMatrixes(ADD, matrix1, matrix2, matrixSum);
}
int subtractMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	return addOrSubtractMatrixes(SUBTRACT, matrix1, matrix2, matrixSum);
}

typedef enum {MULTIPLY, DIVIDE} MultiplyOrDivide;
// Will write directly to the origin matrix
int multiplyOrDivideMatrixByScalar(MultiplyOrDivide multiplyOrDivide, Matrix* matrix, int scalar) {
	for (int y = 0; y < matrix->rows; y++) {
		for (int x = 0; x < matrix->columns; x++) {
			int index = getIndex(matrix->columns, y, x);
			switch (multiplyOrDivide) {
				case MULTIPLY: {
					matrix->array[index] = matrix->array[index] * scalar;
					break;
				}
				case DIVIDE: {
					matrix->array[index] = matrix->array[index] / scalar;
					break;
				}
			}
		}
	}
	// We may possibly add failure returns in the future
	return 0;
}
int multiplyMatrixByScalar(Matrix* matrix, int scalar) {
	return multiplyOrDivideMatrixByScalar(MULTIPLY, matrix, scalar);
}
int divideMatrixByScalar(Matrix* matrix, int scalar) {
	return multiplyOrDivideMatrixByScalar(DIVIDE, matrix, scalar);
}

int multiplyMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixProduct) {
	// The columns of the first matrix must be equal to the columns of the second matrix
	// when multiplying two matrixes
	if (matrix1.columns != matrix2.rows) {
		return 1;
	}

	int productRows = matrix1.rows;
	int productColumns = matrix2.columns;
	matrixProduct->rows = productRows;
	matrixProduct->columns = productColumns;

	// TODO - Thoroughly test this function to make sure the math works
	for (int y = 0; y < productRows; y++) {
		for (int x = 0; x < productColumns; x++) {

			// Clear out the array before we write to its elements just incase
			// it has already been set to something
			int index = getIndex(productColumns, y, x);
			matrixProduct->array[index] = 0;

			// This could also be matrix2.rows as we should have already verified they are equal
			for (int i = 0; i < matrix1.columns; i++) {
				matrixProduct->array[index] += matrix1.array[getIndex(matrix1.columns, y, i)] * matrix2.array[getIndex(matrix2.columns, i, x)];
			}
		}
	}
	return 0;
}
void printMatrix(Matrix matrix) {
	int rw = matrix.rows;
	int clm = matrix.columns;

	for (int y = 0; y < rw; y++) {
		for (int x = 0; x < clm; x++) {
			printf("%f", matrix.array[getIndex(clm, y, x)]);

			if ( x == clm - 1) {
				printf("\n");
			} else {
				printf(" | ");
			}
		}
	}
}

typedef struct camera {
	Vector3d position;
	Vector3d target;
} Camera;

void copyArrayToMemory(int rows, int columns, double* array, double* pointer) {
	for(int y = 0; y < rows; y++) {
		for (int x = 0; x < columns; x++) {
			int index = getIndex(columns, y, x);
			pointer[index] = array[index];
		}
	}
}

void getIdentityMatrix(Matrix* matrix) {
	matrix->rows = 4;
	matrix->columns = 4;
	double* array = (double*)(double[4][4])
	{1, 0, 0, 0,
	 0, 1, 0, 0,
	 0, 0, 1, 0,
	 0, 0, 0, 1};
	copyArrayToMemory(4, 4, array, matrix->array);
}

void getTranslationMatrix(Matrix* matrix, int x, int y, int z) {
	matrix->rows = 4;
	matrix->columns = 4;
	double* array = (double*)(double[4][4])
	{1, 0, 0, x,
	 0, 1, 0, y,
	 0, 0, 1, z,
	 0, 0, 0, 1};
	copyArrayToMemory(4, 4, array, matrix->array);
}

void getScalingMatrix(Matrix* matrix, int x, int y, int z) {
	matrix->rows = 4;
	matrix->columns = 4;
	double* array = (double*)(double[4][4])
	{x, 0, 0, 0,
	 0, y, 0, 0,
	 0, 0, z, 0,
	 0, 0, 0, 1};
	copyArrayToMemory(4, 4, array, matrix->array);
}

void getRotationXMatrix(Matrix* matrix, double theta) {
	matrix->rows = 4;
	matrix->columns = 4;
	double* array = (double*)(double[4][4])
	{1, 0, 0, 0,
	 0, cos(theta), -sin(theta), 0,
	 0, sin(theta), cos(theta), 0,
	 0, 0, 0, 1};
	copyArrayToMemory(4, 4, array, matrix->array);
}

void getRotationYMatrix(Matrix* matrix, double theta) {
	matrix->rows = 4;
	matrix->columns = 4;
	double* array = (double*)(double[4][4])
	{cos(theta), 0, sin(theta), 0,
	 0, 1, 0, 0,
	 -sin(theta), 0, cos(theta), 0,
	 0, 0, 0, 1};
	copyArrayToMemory(4, 4, array, matrix->array);
}

void getRotationZMatrix(Matrix* matrix, double theta) {
	matrix->rows = 4;
	matrix->columns = 4;
	double* array = (double*)(double[4][4])
	{cos(theta), -sin(theta), 0, 0,
	 sin(theta), cos(theta), 0, 0,
	 0, 0, 1, 0,
	 0, 0, 0, 1};
	copyArrayToMemory(4, 4, array, matrix->array);
}

void vector3dToMatrix(Vector3d vector, Matrix* matrix) {
	matrix->rows = 4;
	matrix->columns = 1;
	matrix->array[getIndex(1,0,0)] = vector.x;
	matrix->array[getIndex(1,1,0)] = vector.y;
	matrix->array[getIndex(1,2,0)] = vector.z;

	// Right now we are assuming all vectors represent points,
	// rather than direcions
	matrix->array[getIndex(1,3,0)] = 1;
}

void multiplyVectorByMatrix(Vector3d* vector, Matrix matrix) {
	Matrix matSum = MATRIX_BLANK(4, 1);
	Matrix vectorMat = MATRIX_BLANK(4, 1);
	vector3dToMatrix(*vector, &vectorMat);

	multiplyMatrixes(matrix, vectorMat, &matSum);

	vector->x = matSum.array[getIndex(1,0,0)];
	vector->y = matSum.array[getIndex(1,1,0)];
	vector->z = matSum.array[getIndex(1,2,0)];
}

void multiplyMeshByMatrix(Mesh* mesh, Matrix matrix) {

}

// A *very* siimple projection function (completely orthoganal,
// no projection matrix applied) Simply positions the point relative
// to middle of screen
Vector2d project3dToScreen(Vector3d vector3d, int screenWidth, int screenHeight) {
	Vector2d vector2d = {};
	vector2d.x = (vector3d.x * screenWidth) + (screenWidth / 2);
	vector2d.y = (vector3d.y * screenHeight) + (screenHeight / 2);
	return vector2d;
}

double distBetweenPoints(Vector2d point1, Vector2d point2) {
	int xDiff = point2.x - point1.x;
	int yDiff = point2.y - point1.y;
	return sqrt((xDiff * xDiff) + (yDiff * yDiff));
}

Vector2d getMidPoint(Vector2d point1, Vector2d point2) {
	Vector2d midPoint = {};
	midPoint.x = (point1.x + point2.x) / 2;
	midPoint.y = (point1.y + point2.y) / 2;
	return midPoint;
}
#include <stdlib.h>
#include <stdio.h>

typedef struct vector3d {
	int x;
	int y;
	int z;
} Vector3d;

// TODO - See if there's an easier way to handle scalable 2d arrays
// preferably something not involving macro magic 
typedef struct matrix {
	unsigned int rows;
	unsigned int columns;
	int* array;
} Matrix;

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

	for (int x = 0; x < sumRows; x++) {
		for (int y = 0; y < sumColumns; y++) {
			int index = getIndex(sumColumns, x, y);
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
	for (int x = 0; x < matrix->rows; x++) {
		for (int y = 0; y < matrix->columns; y++) {
			int index = getIndex(matrix->columns, x, y);
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
	for (int x = 0; x < productRows; x++) {
		for (int y = 0; y < productColumns; y++) {

			// Clear out the array before we write to its elements just incase
			// it has already been set to something
			int index = getIndex(productColumns, x, y);
			matrixProduct->array[index] = 0;

			// This could also be matrix2.rows as we should have already verified they are equal
			for (int i = 0; i < matrix1.columns; i++) {
				matrixProduct->array[index] += matrix1.array[getIndex(matrix1.columns, x, i)] * matrix2.array[getIndex(matrix2.columns, i, y)];
			}
		}
	}

}
void printMatrix(Matrix matrix) {
	int rw = matrix.rows;
	int clm = matrix.columns;

	for (int x = 0; x < rw; x++) {
		for (int y = 0; y < clm; y++) {
			printf("%d", matrix.array[getIndex(clm, x, y)]);

			if ( y == clm - 1) {
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

typedef struct cube {
	char* name;
	Vector3d* vertices;
	Vector3d position;
	Vector3d target;
} Cube;

int main() {
	Vector3d testVector = {1, 1, 1};
	Camera testCamera = {testVector, testVector};
	Vector3d verticeArray[8] = {
		{-1, 1, 1},
		{1, 1, 1},
		{-1. -1, 1},
		{-1, -1, -1},
		{-1, 1, -1},
		{1, 1, -1},
		{1, -1, 1},
		{1, -1, -1}
	};

	int arrayTest[5][5] = {{1,2,3,4,5},{6,7,8,9,10},{11,12,13,14,15},{16,17,18,19,20},{21,22,23,24,25}};
	Matrix matrixTest = {5, 5, (int*) arrayTest};
	printMatrix(matrixTest);

	// We need to allocate some space on the stack or else the Matrix will just point
	// towards a zero address.
	int arrayBlank[5][5] = {};
	Matrix sumMat = {0, 0, (int*) arrayBlank};
	addMatrixes(matrixTest, matrixTest, &sumMat);

	printMatrix(sumMat);

	multiplyMatrixByScalar(&sumMat, 3);
	printMatrix(sumMat);

	// Test matrix multiplication
	printf("-------------\n");
	int array1[3][2] = {{2,-1},{0,3},{1,0}};
	int array2[2][4] = {{0,1,4,-1},{-2,0,0,2}};
	int fillerArray[3][4] = {};
	Matrix matrix1 = {3, 2, (int*) array1};
	Matrix matrix2 = {2, 4, (int*) array2};
	Matrix fillMatrix = {0, 0, (int*) fillerArray};
	multiplyMatrixes(matrix1, matrix2, &fillMatrix);
	printMatrix(fillMatrix);

	Cube testCube = {"TestCube", verticeArray, testVector, testVector};
	printf("%s\n", testCube.name);
	printf("%d\n", testCube.vertices[1].x);

	return 0;
}
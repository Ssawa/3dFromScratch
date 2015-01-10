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
void addOrSubtracMatrixes(AddOrSubtract addOrSubtract, Matrix matrix1, Matrix matrix2, Matrix *matrixSum) {

	// To add matrixes they must be the same dimensions
	if (matrix1.rows != matrix2.rows || matrix1.columns != matrix2.columns) {
		return;
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

	int arrayBlank[5][5] = {};
	Matrix sumMat = {0, 0, (int*) arrayBlank};
	addOrSubtracMatrixes(ADD, matrixTest, matrixTest, &sumMat);

	printMatrix(sumMat);

	Cube testCube = {"TestCube", verticeArray, testVector, testVector};
	printf("%s\n", testCube.name);
	printf("%d\n", testCube.vertices[1].x);

	return 0;
}
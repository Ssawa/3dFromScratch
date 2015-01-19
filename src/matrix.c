#include <stdlib.h>
#include <stdio.h>

typedef struct matrix {
	// This should always be four
	int width;
	int height;
	double (*array)[4];
} Matrix;

typedef enum {ADD, SUBTRACT} AddOrSubtract;
// Return 0 for success, anything else for an error
int addOrSubtractMatrixes(AddOrSubtract addOrSubtract, Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	if (matrix1.width != matrix2.width || matrix1.height != matrix2.height) {
		return 1;
	}

	// This could just as easily have been initialized with matrix2, as we should have
	// already verified that they are the same dimensions.
	int sumWidth = matrix1.width;
	int sumHeight = matrix1.height;
	matrixSum->width = sumWidth;
	matrixSum->height = sumHeight;

	for (int y = 0; y < sumHeight; y++) {
		for (int x = 0; x < sumWidth; x++) {
			switch (addOrSubtract) {
				case ADD: {
					matrixSum->array[y][x] = matrix1.array[y][x] + matrix2.array[y][x];
					break;
				}
				case SUBTRACT: {
					matrixSum->array[y][x] = matrix1.array[y][x] - matrix2.array[y][x];
					break;
				}
			}
		}
	}
	return 0;
}
int addMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	return addOrSubtractMatrixes(ADD, matrix1, matrix2, matrixSum);
}
int subtractMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	return addOrSubtractMatrixes(SUBTRACT, matrix1, matrix2, matrixSum);
}

typedef enum {MULTIPLY, DIVIDE} MultiplyOrDivide;
// Will write directly to the origin matrix
int multiplyOrDivideMatrixByScalar(MultiplyOrDivide multiplyOrDivide, Matrix* matrix, double scalar) {
	for (int y = 0; y < matrix->height; y++) {
		for (int x = 0; x < matrix->width; x++) {
			switch (multiplyOrDivide) {
				case MULTIPLY: {
					matrix->array[y][x] = matrix->array[y][x] * scalar;
					break;
				}
				case DIVIDE: {
					matrix->array[y][x] = matrix->array[y][x] / scalar;
					break;
				}
			}
		}
	}
	// We may possibly add failure returns in the future
	return 0;
}
int multiplyMatrixByScalar(Matrix* matrix, float scalar) {
	return multiplyOrDivideMatrixByScalar(MULTIPLY, matrix, scalar);
}
int divideMatrixByScalar(Matrix* matrix, float scalar) {
	return multiplyOrDivideMatrixByScalar(DIVIDE, matrix, scalar);
}

void printMatrix(Matrix matrix) {
	for (int y = 0; y < matrix.height; y++) {
		for (int x = 0; x < matrix.width; x++) {
			printf("%f", matrix.array[y][x]);

			if ( x == matrix.width - 1) {
				printf("\n");
			} else {
				printf(" | ");
			}
		}
	}
}

typedef int x2[2][2];

int main() {
	int test[2][2] = {{1,2},{3,4}};
	int* point = (int*)(int[2][2]){{1,2},{3,4}};
	Matrix matrix1 = {4, 4, (double[4][4]){{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}}};
	Matrix matrix2 = {0, 0, (double[4][4]){}};
	addMatrixes(matrix1, matrix1, &matrix2);
	printMatrix(matrix2);
	printf("-------------\n");
	multiplyMatrixByScalar(&matrix1, 4);
	printMatrix(matrix1);
	printf("-------------\n");
}

/*typedef enum {ADD, SUBTRACT} AddOrSubtract;
// Return 0 for success, anything else for an error
int addOrSubtractMatrixes(AddOrSubtract addOrSubtract, Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {

	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			switch (addOrSubtract) {
				case ADD: {
					(*matrixSum)[y][x] = matrix1[y][x] + matrix2[y][x];
					break;
				}
				case SUBTRACT: {
					(*matrixSum)[y][x] = matrix1[y][x] - matrix2[y][x];
					break;
				}
			}
		}
	}
	return 0;
}
int addMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	return addOrSubtractMatrixes(ADD, matrix1, matrix2, matrixSum);
}
int subtractMatrixes(Matrix matrix1, Matrix matrix2, Matrix* matrixSum) {
	return addOrSubtractMatrixes(SUBTRACT, matrix1, matrix2, matrixSum);
}

typedef enum {MULTIPLY, DIVIDE} MultiplyOrDivide;
// Will write directly to the origin matrix
int multiplyOrDivideMatrixByScalar(MultiplyOrDivide multiplyOrDivide, Matrix* matrix, double scalar) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			switch (multiplyOrDivide) {
				case MULTIPLY: {
					(*matrix)[y][x] = (*matrix)[y][x] * scalar;
					break;
				}
				case DIVIDE: {
					(*matrix)[y][x] = (*matrix)[y][x] / scalar;
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
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {

			// Clear out the array before we write to its elements just incase
			// it has already been set to something
			(*matrixProduct)[y][x] = 0;

			for (int i = 0; i < 4; i++) {
				(*matrixProduct)[y][x] += matrix1[y][i] * matrix2[i][x];
			}
		}
	}
	return 0;
}

void printMatrix(Matrix matrix) {
	for (int y = 0; y < 4; y++) {
		for (int x = 0; x < 4; x++) {
			printf("%f", matrix[y][x]);

			if ( x == 4 - 1) {
				printf("\n");
			} else {
				printf(" | ");
			}
		}
	}
}

int main() {
	int test[2][4] = {{1, 2, 3, 4}, {5, 6, 7, 8}};
	int (*array)[4] = test;
	Matrix matrix1 = (double[4][4]){1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
	Matrix matrix2 = (double[4][4]){};
	multiplyMatrixes(matrix1, matrix1, &matrix2);
	printMatrix(matrix2);
}*/
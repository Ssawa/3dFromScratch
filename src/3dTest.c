#include <stdlib.h>
#include <stdio.h>

typedef struct vector3d {
	int x;
	int y;
	int z;
} Vector3d;


// TODO - MOVE INTO A SEPERATE, ENCAPSULATED FILE
typedef struct matrix {
	int rows;
	int columns;
	int* array;

	void (*delete) (struct matrix*);
	int (*get) (struct matrix*, unsigned x, unsigned y);
} Matrix;

void deleteMatrix(Matrix* self);
int getMatrixElement(Matrix* self, unsigned x, unsigned y);

// Not as pretty as a multidimensional array but it is functionally identical and gives
// us the benefit of having arbitrarily sized matrixes, and is much cleaner than a jagged
// array, in my mind at least. As this is a simple, purposfully limited, test library
// we could probably get away with defining a few 
Matrix* newMatrix(int rows, int columns, int array[rows][columns]) {
	int* flatArray = malloc(sizeof(int[rows * columns]));
	for (int x = 0; x < rows; x++) {
		for (int y = 0; y < columns; y++) {
			flatArray[rows * x + y] = array[x][y];
		}
	}
	Matrix* matrix = malloc(sizeof(Matrix));
	matrix->rows = rows;
	matrix->columns = columns;
	matrix->array = flatArray;

	matrix->delete = &deleteMatrix;
	matrix->get = &getMatrixElement;
	return matrix;
}
void deleteMatrix(Matrix* self) {
	free(self->array);
	free(self);
}
int getMatrixElement(Matrix* self, unsigned int x, unsigned int y) {
	if (x > self->rows-1 || y > self->columns-1) {
		// Okay, so this might not be the safest thing to do to indicate an error
		// but for debugging purposes it should be sufficiant at indicating to me that
		// I screwed something up
		return -1000000;
	}
	return self->array[self->rows * x + y];
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

	int twod[3][3] = {{1,2,3},{4,5,6},{7,8,9}};
	Matrix* testMatrix = newMatrix(3, 3, twod);

	Cube testCube = {"TestCube", verticeArray, testVector, testVector};
	printf("%s\n", testCube.name);
	printf("%d\n", testCube.vertices[1].x);
	printf("%d\n", testMatrix->get(testMatrix, 1, 3));
	testMatrix->delete(testMatrix);
}
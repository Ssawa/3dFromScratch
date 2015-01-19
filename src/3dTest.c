#include <stdio.h>

#include "matrix.h"

int main() {
	Matrix test = MATRIX(2, 2, {{1,2},{3,4}});

	Matrix identityMatrix = MATRIX_BLANK(4,4);
	getIdentityMatrix(&identityMatrix);
	printMatrix(identityMatrix);
	printf("----------------\n");
	Matrix translationMatrix = MATRIX_BLANK(4, 4);
	getTranslationMatrix(&translationMatrix, 4, 5, 6);
	printMatrix(translationMatrix);
	printf("----------------\n");
	Matrix scalingMatrix = MATRIX_BLANK(4, 4);
	getScalingMatrix(&scalingMatrix, 4, 5, 6);
	printMatrix(scalingMatrix);
	printf("----------------\n");
	Matrix rotatinXMatrix = MATRIX_BLANK(4, 4);
	getRotationXMatrix(&rotatinXMatrix, 1);
	printMatrix(rotatinXMatrix);
	printf("----------------\n");
	Vector3d vect1 = {1, 1, 1};
	multiplyVectorByMatrix(&vect1, rotatinXMatrix);
	Vector2d vect2 = project3dToScreen(vect1, 500, 500);
	return 0;
}
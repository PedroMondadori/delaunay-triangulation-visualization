#pragma once

#include "raylib.h"

bool planarPointWithinTriangle(float P[2], float V1[2], float V2[2], float V3[2]);
void delaunayTriangulate(float points[][2], int numPoints);

#ifdef DELAUNAY_IMPLEMENTATION

bool planarPointWithinTriangle(float P[2], float V1[2], float V2[2], float V3[2]) {

	float AB[2] = { V2[0] - V1[0],V2[1] - V1[1] };
	float BC[2] = { V3[0] - V2[0],V3[1] - V2[1] };
	float CA[2] = { V1[0] - V3[0],V1[1] - V3[1] };
	float AP[2] = { P[0] - V1[0],P[1] - V1[1] };
	float BP[2] = { P[0] - V2[0],P[1] - V2[1] };
	float CP[2] = { P[0] - V3[0],P[1] - V3[1] };

	float N1[2] = { AB[1],-AB[0] };
	float N2[2] = { BC[1],-BC[0] };
	float N3[2] = { CA[1],-CA[0] };

	float S1 = AP[0] * N1[0] + AP[1] * N1[1];
	float S2 = BP[0] * N2[0] + BP[1] * N2[1];
	float S3 = CP[0] * N3[0] + CP[1] * N3[1];

	float tolerance = 0.0001;

	if ((S1 < 0 && S2 < 0 && S3 < 0) ||
		(S1 < tolerance && S2 < 0 && S3 < 0) ||
		(S2 < tolerance && S1 < 0 && S3 < 0) ||
		(S3 < tolerance && S1 < 0 && S2 < 0)) { // inside triangle
		return 1;
	}
	else {
		return 0;
	}

}

void delaunayTriangulate(float in_points[][2], int numPoints) {

	float(*points)[2] = malloc(numPoints * 2 * sizeof(float));

	for (int i = 0; i < numPoints; i++)
	{
		points[i][0] = in_points[i][0];
		points[i][1] = in_points[i][1];
	}

	// find min and max boundaries of point cloud
	float xmin = 0, xmax = 0, ymin = 0, ymax = 0;
	for (int i = 0; i < numPoints; i++) {
		if (points[i][0] > xmax)
			xmax = points[i][0];
		else if (points[i][0] < xmin)
			xmin = points[i][0];
		if (points[i][1] > ymax)
			ymax = points[i][1];
		else if (points[i][1] < ymin)
			ymin = points[i][1];
	}

	// remap everything (preserving the aspect ratio) to between (0,0)-(1,1)
	float height = ymax - ymin;
	float width = xmax - xmin;
	float d = height; // d=largest dimension
	if (width > d)
		d = width;
	for (int i = 0; i < numPoints; i++) {
		points[i][0] = (points[i][0] - xmin) / d;
		points[i][1] = (points[i][1] - ymin) / d;
	}

	// sort points by proximity
	int NbinRows = (int)ceil(pow(numPoints, 0.25));
	int* bins = malloc(numPoints * sizeof(int));
	for (int i = 0; i < numPoints; i++) {
		int p = (int)(points[i][1] * NbinRows * 0.999); // bin row
		int q = (int)(points[i][0] * NbinRows * 0.999); // bin column
		if (p % 2)
			bins[i] = (p + 1) * NbinRows - q;
		else
			bins[i] = p * NbinRows + q + 1;
	}
	int key;
	for (int i = 1; i < numPoints; i++) { // insertion sort
		key = bins[i];
		float tempF[2] = { points[i][0],points[i][1] };
		int j = i - 1;
		while (j >= 0 && (bins[j] > key)) {
			bins[j + 1] = bins[j];
			points[j + 1][0] = points[j][0]; points[j + 1][1] = points[j][1];
			j--;
		}
		bins[j + 1] = key;
		points[j + 1][0] = tempF[0]; points[j + 1][1] = tempF[1];
	}

	free(bins);

	// add big triangle around our point cloud
	points = realloc(points, (3 + numPoints) * 2 * sizeof(float));
	points[numPoints][0] = -100; points[numPoints][1] = -100;
	points[numPoints + 1][0] = 100; points[numPoints + 1][1] = -100;
	points[numPoints + 2][0] = 0; points[numPoints + 2][1] = 100;
	numPoints += 3;

	// data structures required
	int(*verts)[3] = malloc(3 * sizeof(int));
	verts[0][0] = numPoints - 3;
	verts[0][1] = numPoints - 2;
	verts[0][2] = numPoints - 1;
	int(*tris)[3] = malloc(3 * sizeof(int));
	tris[0][0] = -1;
	tris[0][1] = -1;
	tris[0][2] = -1;
	int nT = 1;
	int* triangleStack = malloc((numPoints - 3) * sizeof(int)); // is this a big enough stack?
	int tos = -1;
	// insert all points and triangulate one by one
	for (int ii = 0; ii < (numPoints - 3); ii++) {
		// find triangle T which contains points[i]
		int j = nT - 1; // last triangle created
		int iterCount = 0;
		while (1) {
			iterCount++;
			if (iterCount > 1000) {
				// printf("==================== DEU RUIM MAS DEU BOM ====================\n");
				free(triangleStack);
				free(points);
				free(tris);
				free(verts);
				return;
			}
			if (planarPointWithinTriangle(points[ii], points[verts[j][0]], points[verts[j][1]], points[verts[j][2]])) {
				
				nT += 2;
				// delete triangle T and replace it with three sub-triangles touching P 
				tris = realloc(tris, (nT) * 3 * sizeof(int));
				verts = realloc(verts, (nT) * 3 * sizeof(int));
				// vertices of new triangles
				verts[nT - 2][0] = ii;
				verts[nT - 2][1] = verts[j][1];
				verts[nT - 2][2] = verts[j][2];
				verts[nT - 1][0] = ii;
				verts[nT - 1][1] = verts[j][2];
				verts[nT - 1][2] = verts[j][0];
				// update adjacencies of triangles surrounding the old triangle
				// fix adjacency of A
				int adj1 = tris[j][0];
				int adj2 = tris[j][1];
				int adj3 = tris[j][2];
				if (adj1 >= 0)
					for (int m = 0; m < 3; m++)
						if (tris[adj1][m] == j) {
							tris[adj1][m] = j;
							break;
						}
				if (adj2 >= 0)
					for (int m = 0; m < 3; m++)
						if (tris[adj2][m] == j) {
							tris[adj2][m] = nT - 2;
							break;
						}
				if (adj3 >= 0)
					for (int m = 0; m < 3; m++)
						if (tris[adj3][m] == j) {
							tris[adj3][m] = nT - 1;
							break;
						}
				// adjacencies of new triangles	
				tris[nT - 2][0] = j;
				tris[nT - 2][1] = tris[j][1];
				tris[nT - 2][2] = nT - 1;
				tris[nT - 1][0] = nT - 2;
				tris[nT - 1][1] = tris[j][2];
				tris[nT - 1][2] = j;
				// replace v3 of containing triangle with P and rotate to v1
				verts[j][2] = verts[j][1];
				verts[j][1] = verts[j][0];
				verts[j][0] = ii;
				// replace 1st and 3rd adjacencies of containing triangle with new triangles
				tris[j][1] = tris[j][0];
				tris[j][2] = nT - 2;
				tris[j][0] = nT - 1;
				// place each triangle containing P onto a stack, if the edge opposite P has an adjacent triangle
				if (tris[j][1] >= 0)
					triangleStack[++tos] = j;
				if (tris[nT - 2][1] >= 0)
					triangleStack[++tos] = nT - 2;
				if (tris[nT - 1][1] >= 0)
					triangleStack[++tos] = nT - 1;
				while (tos >= 0) { // looping thru the stack
					int L = triangleStack[tos--];
					float v1[2] = { points[verts[L][2]][0],points[verts[L][2]][1] };
					float v2[2] = { points[verts[L][1]][0],points[verts[L][1]][1] };
					int oppVert = -1;
					int oppVertID = -1;
					for (int k = 0; k < 3; k++) {
						if ((verts[tris[L][1]][k] != verts[L][1])
							&& (verts[tris[L][1]][k] != verts[L][2])) {
							oppVert = verts[tris[L][1]][k];
							oppVertID = k;
							break;
						}
					}
					float v3[2] = { points[oppVert][0],points[oppVert][1] };
					float P[2] = { points[ii][0],points[ii][1] };

					// check if P in circumcircle of triangle on top of stack
					float cosa = ((v1[0] - v3[0]) * (v2[0] - v3[0]) + (v1[1] - v3[1]) * (v2[1] - v3[1]));
					float cosb = ((v2[0] - P[0]) * (v1[0] - P[0]) + (v2[1] - P[1]) * (v1[1] - P[1]));
					float sina = ((v1[0] - v3[0]) * (v2[1] - v3[1]) - (v1[1] - v3[1]) * (v2[0] - v3[0]));
					float sinb = ((v2[0] - P[0]) * (v1[1] - P[1]) - (v2[1] - P[1]) * (v1[0] - P[0]));

					if (((cosa < 0) && (cosb < 0)) ||
						((-cosa * ((v2[0] - P[0]) * (v1[1] - P[1]) - (v2[1] - P[1]) * (v1[0] - P[0]))) >
							(cosb * ((v1[0] - v3[0]) * (v2[1] - v3[1]) - (v1[1] - v3[1]) * (v2[0] - v3[0]))))) {

						// swap diagonal, and redo triangles L R A & C
						// initial state:
						int R = tris[L][1];
						int C = tris[L][2];
						int A = tris[R][(oppVertID + 2) % 3];
						// fix adjacency of A
						if (A >= 0)
							for (int m = 0; m < 3; m++)
								if (tris[A][m] == R) {
									tris[A][m] = L;
									break;
								}
						// fix adjacency of C
						if (C >= 0)
							for (int m = 0; m < 3; m++)
								if (tris[C][m] == L) {
									tris[C][m] = R;
									break;
								}
						// fix vertices and adjacency of R
						for (int m = 0; m < 3; m++)
							if (verts[R][m] == oppVert) {
								verts[R][(m + 2) % 3] = ii;
								break;
							}
						for (int m = 0; m < 3; m++)
							if (tris[R][m] == L) {
								tris[R][m] = C;
								break;
							}
						for (int m = 0; m < 3; m++)
							if (tris[R][m] == A) {
								tris[R][m] = L;
								break;
							}
						for (int m = 0; m < 3; m++)
							if (verts[R][0] != ii) {
								int temp1 = verts[R][0];
								int temp2 = tris[R][0];
								verts[R][0] = verts[R][1];
								verts[R][1] = verts[R][2];
								verts[R][2] = temp1;
								tris[R][0] = tris[R][1];
								tris[R][1] = tris[R][2];
								tris[R][2] = temp2;
							}

						// fix vertices and adjacency of L
						verts[L][2] = oppVert;
						for (int m = 0; m < 3; m++)
							if (tris[L][m] == C) {
								tris[L][m] = R;
								break;
							}
						for (int m = 0; m < 3; m++)
							if (tris[L][m] == R) {
								tris[L][m] = A;
								break;
							}
						// add L and R to stack if they have triangles opposite P;
						if (tris[L][1] >= 0)
							triangleStack[++tos] = L;
						if (tris[R][1] >= 0)
							triangleStack[++tos] = R;
					}
				}
				break;
			}

			// adjust j in the direction of target point ii
			float AB[2] = { points[verts[j][1]][0] - points[verts[j][0]][0],points[verts[j][1]][1] - points[verts[j][0]][1] };
			float BC[2] = { points[verts[j][2]][0] - points[verts[j][1]][0],points[verts[j][2]][1] - points[verts[j][1]][1] };
			float CA[2] = { points[verts[j][0]][0] - points[verts[j][2]][0],points[verts[j][0]][1] - points[verts[j][2]][1] };
			float AP[2] = { points[ii][0] - points[verts[j][0]][0],points[ii][1] - points[verts[j][0]][1] };
			float BP[2] = { points[ii][0] - points[verts[j][1]][0],points[ii][1] - points[verts[j][1]][1] };
			float CP[2] = { points[ii][0] - points[verts[j][2]][0],points[ii][1] - points[verts[j][2]][1] };
			float N1[2] = { AB[1],-AB[0] };
			float N2[2] = { BC[1],-BC[0] };
			float N3[2] = { CA[1],-CA[0] };
			float S1 = AP[0] * N1[0] + AP[1] * N1[1];
			float S2 = BP[0] * N2[0] + BP[1] * N2[1];
			float S3 = CP[0] * N3[0] + CP[1] * N3[1];
			if ((S1 > 0) && (S1 >= S2) && (S1 >= S3))
				j = tris[j][0];
			else if ((S2 > 0) && (S2 >= S1) && (S2 >= S3))
				j = tris[j][1];
			else if ((S3 > 0) && (S3 >= S1) && (S3 >= S2))
				j = tris[j][2];
		}
	}

	// count how many triangles we have that dont involve supertriangle vertices
	int nT_final = nT;
	int* renumberAdj = calloc(nT, sizeof(int));
	bool* deadTris = calloc(nT, sizeof(bool));
	for (int i = 0; i < nT; i++)
		if ((verts[i][0] >= (numPoints - 3))
			|| (verts[i][1] >= (numPoints - 3))
			|| (verts[i][2] >= (numPoints - 3))) {
			deadTris[i] = 1;
			renumberAdj[i] = nT - (nT_final--);
		}
		else
			renumberAdj[i] = nT - (nT_final);

	// delete any triangles that contain the supertriangle vertices
	int(*verts_final)[3] = malloc(3 * nT_final * sizeof(int));
	int(*tris_final)[3] = malloc(3 * nT_final * sizeof(int));
	int index = 0;
	for (int i = 0; i < nT; i++)
		if ((verts[i][0] < (numPoints - 3))
			&& (verts[i][1] < (numPoints - 3))
			&& (verts[i][2] < (numPoints - 3))) {
			verts_final[index][0] = verts[i][0];
			verts_final[index][1] = verts[i][1];
			verts_final[index][2] = verts[i][2];
			tris_final[index][0] = (1 - deadTris[tris[i][0]]) * tris[i][0] - deadTris[tris[i][0]];
			tris_final[index][1] = (1 - deadTris[tris[i][1]]) * tris[i][1] - deadTris[tris[i][1]];
			tris_final[index++][2] = (1 - deadTris[tris[i][2]]) * tris[i][2] - deadTris[tris[i][2]];
		}
	for (int i = 0; i < nT_final; i++) {
		if (tris_final[i][0] >= 0)
			tris_final[i][0] -= renumberAdj[tris_final[i][0]];
		if (tris_final[i][1] >= 0)
			tris_final[i][1] -= renumberAdj[tris_final[i][1]];
		if (tris_final[i][2] >= 0)
			tris_final[i][2] -= renumberAdj[tris_final[i][2]];
	}

	// undo the mapping
	numPoints -= 3;
	if (width > d)
		d = width;
	for (int i = 0; i < numPoints; i++) {
		points[i][0] = points[i][0] * d + xmin;
		points[i][1] = points[i][1] * d + ymin;
	}

	//printf("Triangle Vertices:\n");
	for (int i = 0; i < nT_final; i++) {
		DrawTriangle(
			(Vector2) { points[verts_final[i][1]][0], points[verts_final[i][1]][1] },
			(Vector2) { points[verts_final[i][0]][0], points[verts_final[i][0]][1] },
			(Vector2) {	points[verts_final[i][2]][0], points[verts_final[i][2]][1] },
			(Color) { 
				(int)points[verts_final[i][1]][0] % 128,
				(int)points[verts_final[i][1]][0] % 128, 
				(int)points[verts_final[i][1]][0] % 128,
				0xFF
			}
		);
		DrawTriangleLines(
			(Vector2) { points[verts_final[i][0]][0], points[verts_final[i][0]][1] },
			(Vector2) {	points[verts_final[i][1]][0], points[verts_final[i][1]][1] },
			(Vector2) {	points[verts_final[i][2]][0], points[verts_final[i][2]][1] },
			BLACK
		);
	}
	/*printf("Triangle Adjacencies:\n");
	for (int i = 0; i < nT_final; i++)
		printf("[%d,%d,%d]\n", tris_final[i][0], tris_final[i][1], tris_final[i][2]);
	printf("Point Cloud:\n");
	for (int i = 0; i < numPoints; i++)
		printf("%d:(%f,%f)\n", i, points[i][0], points[i][1]);*/

	free(points);
	free(verts);
	free(tris);
	free(triangleStack);
	free(renumberAdj);
	free(deadTris);
	free(verts_final);
	free(tris_final);

	return;
}

#endif
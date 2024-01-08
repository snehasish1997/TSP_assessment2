#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

extern double calculateEuclideanDistance(double x1, double y1, double x2, double y2);
extern double **generateDistanceMatrix(double **coords, int numOfCoords);
extern int readNumOfCoords(char *filename);
extern double **readCoords(char *filename, int numOfCoords);
extern void writeTourToFile(int *tour, int tourLength, char *filename);
extern double calculateTourCost(double **distanceMatrix, int *tour, int numOfCoords);

void cheapestInsertion(double **distanceMatrix, int numOfCoords, int *tour, int startVertex) {
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));
    int tourSize = 1;
    tour[0] = startVertex;
    visited[startVertex] = true;

    while (tourSize < numOfCoords) {
        double minCost = -1;
        int minVertex = -1;
        int insertPos = -1;

        for (int i = 0; i < numOfCoords; ++i) {
            if (!visited[i]) {
                for (int j = 0; j < tourSize; ++j) {
                    int current = tour[j];
                    int next = tour[(j + 1) % tourSize];
                    double cost = distanceMatrix[current][i] + distanceMatrix[i][next] - distanceMatrix[current][next];
                    
                    if (minCost < 0 || cost < minCost) {
                        minCost = cost;
                        minVertex = i;
                        insertPos = j;
                    }
                }
            }
        }

        for (int k = tourSize; k > insertPos; --k) {
            tour[k] = tour[k - 1];
        }
        tour[insertPos + 1] = minVertex;
        visited[minVertex] = true;
        tourSize++;
    }

    free(visited);
}
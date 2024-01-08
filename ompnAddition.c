#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

double calculateEuclideanDistance(double x1, double y1, double x2, double y2);
double **generateDistanceMatrix(double **coords, int numOfCoords);
double calculateTourCost(double **distanceMatrix, int *tour, int numOfCoords);
int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void writeTourToFile(int *tour, int tourLength, char *filename);


void nearestAddition(double **distanceMatrix, int numOfCoords, int *tour) {
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));
    int tourSize = 1;

    // Start with vertex 0 (or any other vertex of your choice)
    tour[0] = 0;
    visited[0] = true;

    while (tourSize < numOfCoords) {
        double minDistance = -1;
        int nearestVertex = -1;
        int insertPos = -1;

        for (int i = 0; i < numOfCoords; ++i) {
            if (!visited[i]) {
                for (int j = 0; j < tourSize; ++j) {
                    double distance = distanceMatrix[tour[j]][i];
                    if (minDistance < 0 || distance < minDistance) {
                        minDistance = distance;
                        nearestVertex = i;
                        insertPos = j;
                    }
                }
            }
        }

        // Insert nearestVertex after insertPos
        for (int k = tourSize; k > insertPos; --k) {
            tour[k] = tour[k - 1];
        }
        tour[insertPos + 1] = nearestVertex;
        visited[nearestVertex] = true;
        tourSize++;
    }

    free(visited);
}
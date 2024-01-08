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


void farthestInsertion(double **distanceMatrix, int numOfCoords, int *tour, int startVertex) {
    bool *visited = (bool *)calloc(numOfCoords, sizeof(bool));
    int tourSize = 1;
    tour[0] = startVertex;
    visited[startVertex] = true;

    while (tourSize < numOfCoords) {
        double maxDistance = -1;
        int farthestVertex = -1;
        int insertPos = -1;

        for (int i = 0; i < numOfCoords; ++i) {
            if (!visited[i]) {
                for (int j = 0; j < tourSize; ++j) {
                    int current = tour[j];
                    double distance = distanceMatrix[current][i];
                    if (distance > maxDistance) {
                        maxDistance = distance;
                        farthestVertex = i;
                        insertPos = j;
                    }
                }
            }
        }
    
        double minIncrease = -1;
        for (int j = 0; j < tourSize; ++j) {
            int current = tour[j];
            int next = tour[(j + 1) % tourSize];
            double costIncrease = distanceMatrix[current][farthestVertex] + distanceMatrix[farthestVertex][next] - distanceMatrix[current][next];
            
            if (minIncrease < 0 || costIncrease < minIncrease) {
                minIncrease = costIncrease;
                insertPos = j;
            }
        }

        for (int k = tourSize; k > insertPos; --k) {
            tour[k] = tour[k - 1];
        }
        tour[insertPos + 1] = farthestVertex;
        visited[farthestVertex] = true;
        tourSize++;
    }

    free(visited);
}

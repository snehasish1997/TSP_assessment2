#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

double calculateEuclideanDistance(double x1, double y1, double x2, double y2);
double **generateDistanceMatrix(double **coords, int numOfCoords);
double calculateTourCost(double **distanceMatrix, int *tour, int numOfCoords);
int readNumOfCoords(char *filename);
double **readCoords(char *filename, int numOfCoords);
void writeTourToFile(int *tour, int tourLength, char *filename);

void cheapestInsertion(double **distanceMatrix, int numOfCoords, int *tour, int startVertex);
void farthestInsertion(double **distanceMatrix, int numOfCoords, int *tour, int startVertex);
void nearestAddition(double **distanceMatrix, int numOfCoords, int *tour);


int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <coord_file> <out_cheapest> <out_farthest> <out_nearest>\n", argv[0]);
        return EXIT_FAILURE;
    }

    char *coordFileName = argv[1];
    int numOfCoords = readNumOfCoords(coordFileName);
    double **coords = readCoords(coordFileName, numOfCoords);
    double **distanceMatrix = generateDistanceMatrix(coords, numOfCoords);

    int *tour = (int *)malloc(numOfCoords * sizeof(int));
    int *bestTourCheapest = (int *)malloc(numOfCoords * sizeof(int));
    int *bestTourFarthest = (int *)malloc(numOfCoords * sizeof(int));
    int *bestTourNearest = (int *)malloc(numOfCoords * sizeof(int));
    if (!tour || !bestTourCheapest || !bestTourFarthest || !bestTourNearest) {
        perror("Memory allocation for tours failed");
        exit(EXIT_FAILURE);
    }

    double bestCostCheapest = -1, bestCostFarthest = -1, bestCostNearest = -1;

    for (int startVertex = 0; startVertex < numOfCoords; ++startVertex) {
        // Cheapest Insertion
        cheapestInsertion(distanceMatrix, numOfCoords, tour, startVertex);
        double currentCost = calculateTourCost(distanceMatrix, tour, numOfCoords);
        if (bestCostCheapest < 0 || currentCost < bestCostCheapest) {
            bestCostCheapest = currentCost;
            memcpy(bestTourCheapest, tour, numOfCoords * sizeof(int));
        }

        // Farthest Insertion
        farthestInsertion(distanceMatrix, numOfCoords, tour, startVertex);
        currentCost = calculateTourCost(distanceMatrix, tour, numOfCoords);
        if (bestCostFarthest < 0 || currentCost < bestCostFarthest) {
            bestCostFarthest = currentCost;
            memcpy(bestTourFarthest, tour, numOfCoords * sizeof(int));
        }

        // Nearest Addition
        nearestAddition(distanceMatrix, numOfCoords, tour);
        currentCost = calculateTourCost(distanceMatrix, tour, numOfCoords);
        if (bestCostNearest < 0 || currentCost < bestCostNearest) {
            bestCostNearest = currentCost;
            memcpy(bestTourNearest, tour, numOfCoords * sizeof(int));
        }
    }

    // Write the best tours to the output files
    writeTourToFile(bestTourCheapest, numOfCoords, argv[2]);
    writeTourToFile(bestTourFarthest, numOfCoords, argv[3]);
    writeTourToFile(bestTourNearest, numOfCoords, argv[4]);

    // Cleanup
    free(tour);
    free(bestTourCheapest);
    free(bestTourFarthest);
    free(bestTourNearest);
    for (int i = 0; i < numOfCoords; ++i) {
        free(distanceMatrix[i]);
        free(coords[i]);
    }
    free(distanceMatrix);
    free(coords);

    return 0;
}

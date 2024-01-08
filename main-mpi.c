#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <omp.h>
#include <float.h>
#include <string.h>

void cheapestInsertion(double **distanceMatrix, int numOfCoords, int *tour, int startVertex);
void farthestInsertion(double **distanceMatrix, int numOfCoords, int *tour, int startVertex);
void nearestAddition(double **distanceMatrix, int numOfCoords, int *tour);
double calculateTourCost(double **distanceMatrix, int *tour, int numOfCoords);
double **generateDistanceMatrix(double **coords, int numOfCoords);
double **readCoords(char *filename, int numOfCoords);
int readNumOfCoords(char *filename);
void writeTourToFile(int *tour, int tourLength, char *filename);

typedef struct {
    int *tour;
    double cost;
} TourResult;

int main(int argc, char *argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <coord_file> <out_cheapest> <out_farthest> <out_nearest>\n", argv[0]);
        return EXIT_FAILURE;
    }

    MPI_Init(&argc, &argv);

    int world_size, world_rank;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    char *coordFileName = argv[1];
    int numOfCoords = readNumOfCoords(coordFileName);
    double **coords = readCoords(coordFileName, numOfCoords);
    double **distanceMatrix = generateDistanceMatrix(coords, numOfCoords);

    int *tour = (int *)malloc(numOfCoords * sizeof(int));
    TourResult bestCheapest, bestFarthest, bestNearest;
    bestCheapest.tour = (int *)malloc(numOfCoords * sizeof(int));
    bestFarthest.tour = (int *)malloc(numOfCoords * sizeof(int));
    bestNearest.tour = (int *)malloc(numOfCoords * sizeof(int));
    bestCheapest.cost = bestFarthest.cost = bestNearest.cost = DBL_MAX;

    int startVertex = world_rank;
    while (startVertex < numOfCoords) {
        double currentCost;

        // Execute Cheapest Insertion
        cheapestInsertion(distanceMatrix, numOfCoords, tour, startVertex);
        currentCost = calculateTourCost(distanceMatrix, tour, numOfCoords);
        if (currentCost < bestCheapest.cost) {
            bestCheapest.cost = currentCost;
            memcpy(bestCheapest.tour, tour, numOfCoords * sizeof(int));
        }

        // Execute Farthest Insertion
        farthestInsertion(distanceMatrix, numOfCoords, tour, startVertex);
        currentCost = calculateTourCost(distanceMatrix, tour, numOfCoords);
        if (currentCost < bestFarthest.cost) {
            bestFarthest.cost = currentCost;
            memcpy(bestFarthest.tour, tour, numOfCoords * sizeof(int));
        }

        // Execute Nearest Addition
        nearestAddition(distanceMatrix, numOfCoords, tour);
        currentCost = calculateTourCost(distanceMatrix, tour, numOfCoords);
        if (currentCost < bestNearest.cost) {
            bestNearest.cost = currentCost;
            memcpy(bestNearest.tour, tour, numOfCoords * sizeof(int));
        }

        startVertex += world_size;
    }

    // Gather results at the master process (rank 0)
    TourResult globalBestCheapest, globalBestFarthest, globalBestNearest;
    if (world_rank == 0) {
        globalBestCheapest.tour = (int *)malloc(numOfCoords * sizeof(int));
        globalBestFarthest.tour = (int *)malloc(numOfCoords * sizeof(int));
        globalBestNearest.tour = (int *)malloc(numOfCoords * sizeof(int));
    }

    // Use MPI_Reduce to find the minimum cost and corresponding tour
    MPI_Reduce(&bestCheapest.cost, &globalBestCheapest.cost, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&bestFarthest.cost, &globalBestFarthest.cost, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);
    MPI_Reduce(&bestNearest.cost, &globalBestNearest.cost, 1, MPI_DOUBLE, MPI_MIN, 0, MPI_COMM_WORLD);


    if (world_rank == 0) {
        // Write the best tours to files
        writeTourToFile(globalBestCheapest.tour, numOfCoords, argv[2]); // for Cheapest Insertion
        writeTourToFile(globalBestFarthest.tour, numOfCoords, argv[3]); // for Farthest Insertion
        writeTourToFile(globalBestNearest.tour, numOfCoords, argv[4]); // for Nearest Addition

        // Free memory allocated for global best tours
        free(globalBestCheapest.tour);
        free(globalBestFarthest.tour);
        free(globalBestNearest.tour);
    }

    // Free memory allocated for local best tours and distance matrix
    free(bestCheapest.tour);
    free(bestFarthest.tour);
    free(bestNearest.tour);
    free(tour);
    for (int i = 0; i < numOfCoords; i++) {
        free(distanceMatrix[i]);
        free(coords[i]);
    }
    free(distanceMatrix);
    free(coords);

    MPI_Finalize();
    return 0;
}

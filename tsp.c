#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NMAX 50

// x,y coordinates
typedef struct Position {
	int x;
	int y;
} Position;

// Populate cities coordinates using user input
void populate_cities(Position *cities, int n) {
	int x, y;
	for (int i = 0; i < n; i++) {
		scanf("%d,%d", &x, &y);
		cities[i] = (Position){.x = x, .y = y};
	}
}

// Compute distances between cities
void compute_distances(double *distance, Position *cities, int n) {
	for (int i = 0; i < n; i++) {
		for (int j = 0; j < n; j++) {
			// distance between cities[i] and cities[j]
			int x = cities[i].x - cities[j].x;
			int y = cities[i].y - cities[j].y;
			double dist = sqrt((double)(x*x) + (double)(y*y));
			// stored in distance[n*j+i]
			distance[n*j+i] = dist;
		}
	}
}

// Initialise the path
void init_path(int *path, int n) {
	for (int i = 0; i < n; i++)
		path[i] = i;
}

// Swap two random cities in the path
void path_swap_cities(int *path, int n) {
	// Two random integers in [0, n)
	int rand_a = rand() % n;
	int rand_b = rand() % n;

	// Swap cities in path
	int temp = path[rand_a];
	path[rand_a] = path[rand_b];
	path[rand_b] = temp;
}

// Inverts the order of a random section of the path
void path_invert_section(int *path, int n) {
	// Random start value
	// End value bounded [start, n)
	int start = rand() % n;
	int end = rand() % (n - start) + start;
	
	// Swap values from opposing ends of the section bound
	// incrementally moving closer to the center of the section
	for (int i = 0; i < (end-start+1)/2; i++) {
		int temp = path[start+i];
		path[start+i] = path[end-i];
		path[end-i] = temp;
	}
}

// Shifts all entries in array up or down one position (random)
void path_shift(int *path, int n) {
	// Boolean to decide if we shift up or not(= down)
	// (found this to work better than the given random bool method)
	int up = rand() & 1;

	// Shift every value up one position
	if (up) {
		int temp = path[n-1];
		for (int i = n; i > 0; i--)
			path[i] = path[i-1];
		path[0] = temp;
	} 

	// Shift every value down one position
	if (!up) {
		int temp = path[0];
		for (int i = 0; i < n-1; i++)
			path[i] = path[i+1];
		path[n-1] = temp;
	}
}

// Returns the total path length (energy E)
double path_length(int *path, double *distance, int n) {
	double total = 0;
	// Sum up all distances
	// Indexed using path array
	for (int i = 0; i < n-1; i++) {
		total += distance[n*path[i]+path[i+1]];
	}
	total += distance[n*path[n-1]+path[0]];	// path is a cycle

	return total;
}

// Generates a new random modification of a path
void new_path(int *path, int *path_temp, int n) {
	// Duplicate current path into temp
	memcpy(path_temp, path, n * sizeof(int));
	// Modify temp
	path_swap_cities(path_temp, n);
	path_shift(path_temp, n);
	path_invert_section(path_temp, n);
}

// Returns the energy (path length) difference of two paths
double energy_diff(int *path, int *path_temp, double *distance, int n) {
	double E_i = path_length(path, distance, n);
	double E_temp = path_length(path_temp, distance, n);

	return E_i - E_temp;
}

int main(int argc, char const *argv[]) {
	srand(time(NULL)); // randomize seed

	// Number of cities (user input)
	int n;
	// Make sure we get a value <= NMAX
	do {
		scanf("%d", &n);
		if (n > NMAX) printf("Please choose a value less than or equal to %d\n", NMAX);
	} while (n > NMAX);

	// Integer array of size n for cities
	Position *cities = malloc(n * sizeof(Position));
	// 1D distance array of doubles indexed as [n*j+i] => [i][j]
	double *distance = malloc(n * n * sizeof(double));
	// Integer array of size n for path and path_temp
	int *path = malloc(n * sizeof(n));
	int *path_temp = malloc(n * sizeof(n));

	// Populate cities
	populate_cities(cities, n);

	// Compute distances
	compute_distances(distance, cities, n);

	// Initialise the path
	init_path(path, n);

	// Start simulated annealing
	int iteration = 0;
	int temperature = 100;
	while (iteration < 10000 && temperature > 27) {
		// Generate path_temp
		new_path(path, path_temp, n);

		// Energy difference
		double E_diff = energy_diff(path, path_temp, distance, n);

		// SEE WHAT IS HAPPENING
		// We should probably remove this afterwards
		printf("Iteration: %d, T: %d, diff: %f\n", iteration, temperature, E_diff);

		// Negative delta E
		// Meaning path_temp is actually worse than what we had
		if (E_diff < 0) {
			// Probability P to change path to path_temp
			if ((rand() / RAND_MAX) <= exp(E_diff / temperature)) {
				memcpy(path, path_temp, n * sizeof(int));
			}
		// Path_temp is better than or equal to what we had
		} else {
			memcpy(path, path_temp, n * sizeof(int));
		}

		// Cool down every 100 iterations
		if (iteration % 100 == 0) temperature--;

		iteration++;
	}

	printf("PATH: \n");
	printf("%d", path[0]);
	for (int i = 1; i < n; i++) {
		printf(", %d", path[i]);
	} printf("\n");

	free(cities);
	free(distance);
	free(path);
	free(path_temp);
	
	return 0;
}

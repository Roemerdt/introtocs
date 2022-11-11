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

int min(int num1, int num2) {
	return (num1 > num2 ) ? num2 : num1;
}

int max(int num1, int num2) {
	return (num1 > num2 ) ? num1 : num2;
}


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
}

// Returns the energy (path length) difference of two paths
double energy_diff(int *path, int *path_temp, double *distance, int n) {
	double E_i = path_length(path, distance, n);
	double E_temp = path_length(path_temp, distance, n);

	return E_i - E_temp;
}

// Computing the error value for a path
int path_error(int *path, double *distance, int n) {
	int dist = path_length(path, distance, n);
	int min_distance = n-1;
	// The length of the path and the minimum distance
	return dist - min_distance;
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
	int *path = malloc(n * sizeof(int));
	int *path_temp = malloc(n * sizeof(int));
	int *path_swap = malloc(n*sizeof(int));

	// Populate cities
	populate_cities(cities, n);

	// Compute distances
	compute_distances(distance, cities, n);

	// Initialize the path
	init_path(path, n);

	// Start simulated annealing
	int iteration = 0;
	// Set temperature
	float temperature = 10000.0;
	// Set initial error value
	int err = path_error(path, distance, n);
	
	while (iteration < 2500 && err > 0.0) {
		// Generate path_temp
		new_path(path, path_temp, n);
		
		// Energy difference
		double E_diff = energy_diff(path, path_temp, distance, n);
		
		// Storing the error value of an "adjacent" path
		
		// 2 swapped elements
		memcpy(path_swap, path_temp, n * sizeof(int));
		path_swap_cities(path_swap, n);
    int swap_err = path_error(path_swap, distance, n);
		
		// Shifted path with 1 up or 1 down
		memcpy(path_swap, path_temp, n * sizeof(int));
		path_shift(path_swap, n);
		int shift_err = path_error(path_swap, distance, n);
		
		// Random inverted segment of the path
		memcpy(path_swap, path_temp, n * sizeof(int));
		path_invert_section(path_swap, n);
		int invert_err = path_error(path_swap, distance, n);

		// Store the lowest error between all of the above
		int adj_err = min(min(swap_err, shift_err), invert_err);

		// SEE WHAT IS HAPPENING
		// printf("|Iteration: %d, T: %f, diff: %f, err:%d\n", iteration, temperature, E_diff, err);

		// The current solution is better than the adjacent
		if (adj_err < err) {
			memcpy(path, path_temp, n * sizeof(int));
		} else {
			// It's a worse solution
			// Probability P to change path to path_temp anyway
			if (rand() < exp(E_diff / temperature)) {
				memcpy(path, path_temp, n * sizeof(int));
				err = adj_err;
			}
		}
		// Keep the temperature at 0.000001
		if (temperature < 0.00001) {
      temperature = 0.00001;
		}
    else {
			// Lower temperature by 1%
      temperature *= 0.99;
		}
		iteration++;
	}

	// Print result
	printf("%d", path[0]);
	for (int i = 1; i < n; i++) {;
		printf(",%d", path[i]);
	} printf("\n");

	free(cities);
	free(distance);
	free(path);
	free(path_temp);
	free(path_swap);
	return 0;
}

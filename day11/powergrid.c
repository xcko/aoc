#include <err.h>
#include <stdio.h>

#define ROWS 300
#define COLS 300

static void
putgrid(long int grid[ROWS][COLS], int serial)
{
	long int p, x, y;
	int i, j;

	for (i = 0; i < ROWS; ++i) {
		x = i + 1;	/* problem wants 1 indexed arrays, we use 0 */
		for (j =0; j < COLS; ++j) {
			y = j + 1;
			p = (((x + 10) * y) + serial) * (x + 10);
			p = (p / 100) % 10;	/* get third digit */
			p -= 5;
			grid[i][j] = p;
		}
	}
}

static void
putmgrid(int metagrid[ROWS][COLS], const long int fuelgrid[ROWS][COLS], const int msize)
{
	int i, j, u, v, s;

	for (i = 0; i < ROWS - msize + 1; ++i) {	/* + 1 b/c msize includes current pos */
		for (j = 0; j < COLS - msize + 1; ++j) {
			s = 0;
			for (u = i; u < i + msize; ++u) {
				for (v = j; v < j + msize; ++v) {
					s += fuelgrid[u][v];
				}
			}
			metagrid[i][j] = s;
		}
	}
}

static void
getlargest(int res[3], const int mgrid[ROWS][COLS], const int msize)
{
	int i, j;

	res[2] = 0;	/* res[2] == size of largest found */
	for (i = 0; i < ROWS - msize + 1; ++i)	/* + 1 b/c msize includes current res */
		for (j = 0; j < COLS - msize + 1; ++j)
			if (mgrid[i][j] > res[2]) {
				res[0] = i;
				res[1] = j;
				res[2] = mgrid[i][j];
			}
}

int
main()
{
	long int fuelgrid[ROWS][COLS];	/* main powergrid */
	int metagrid[ROWS][COLS];	/* meta grid of sum values */
	int res[3], tmp[3];	/* X,Y,Size */
	int mres;	/* size of meta array result (step2) */
	int i;

	/* example checks */
	putgrid(fuelgrid, 57);
	printf("fuel cell 122,79: %ld\n", fuelgrid[121][78]);
	putgrid(fuelgrid, 39);
	printf("fuel cell 217,196: %ld\n", fuelgrid[216][195]);
	putgrid(fuelgrid, 71);
	printf("fuel cell 101,153: %ld\n", fuelgrid[100][152]);

	/* example checks */
	putgrid(fuelgrid, 18);
	putmgrid(metagrid, fuelgrid, 3);
	getlargest(res, metagrid, 3);
	printf("fuel grid: %d, %d was the largest %d\n", res[0] + 1, res[1] + 1, res[2]);
	putgrid(fuelgrid, 42);
	putmgrid(metagrid, fuelgrid, 3);
	getlargest(res, metagrid, 3);
	printf("fuel grid: %d, %d was the largest %d\n", res[0] + 1, res[1] + 1, res[2]);

	/* puzzle input - step 1 */
	putgrid(fuelgrid, 9005);
	putmgrid(metagrid, fuelgrid, 3);
	getlargest(res, metagrid, 3);
	printf("fuel grid: %d, %d was the largest %d\n", res[0] + 1, res[1] + 1, res[2]);

	/* example checks */
	putgrid(fuelgrid, 18);
	res[2] = tmp[2] = 0;
	for (i = 1; i <= ROWS; ++i) {
		putmgrid(metagrid, fuelgrid, i);
		getlargest(tmp, metagrid, i);
		if (tmp[2] > res[2]) {
			res[0] = tmp[0];
			res[1] = tmp[1];
			res[2] = tmp[2];
			mres = i;
		}
	}
	printf("fuel grid: %d,%d,%d was the largest %d\n", res[0] + 1, res[1] + 1, mres, res[2]);

	/* example checks */
	putgrid(fuelgrid, 42);
	res[2] = tmp[2] = 0;
	for (i = 1; i <= ROWS; ++i) {
		putmgrid(metagrid, fuelgrid, i);
		getlargest(tmp, metagrid, i);
		if (tmp[2] > res[2]) {
			res[0] = tmp[0];
			res[1] = tmp[1];
			res[2] = tmp[2];
			mres = i;
		}
	}
	printf("fuel grid: %d,%d,%d was the largest %d\n", res[0] + 1, res[1] + 1, mres, res[2]);

	/* puzzle input - step 2 */
	putgrid(fuelgrid, 9005);
	res[2] = tmp[2] = 0;
	for (i = 1; i <= ROWS; ++i) {
		putmgrid(metagrid, fuelgrid, i);
		getlargest(tmp, metagrid, i);
		if (tmp[2] > res[2]) {
			res[0] = tmp[0];
			res[1] = tmp[1];
			res[2] = tmp[2];
			mres = i;
		}
	}
	printf("fuel grid: %d,%d,%d was the largest %d\n", res[0] + 1, res[1] + 1, mres, res[2]);
	return 0;
}

#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define AXIS 1000

/* read list of coordinates from file, return length */
int
readcoor(int arr[][2], FILE *f)
{
	char buf[BUFSIZ], *p;
	int i;

	arr[0][0] = arr[0][1] = 0;	/* 0 means any or none later on */
	for (i = 1; fgets(buf, sizeof(buf), f) != NULL; ++i) {
			arr[i][0] = atoi(buf);
			p = strchr(buf, ' ');	/* mv to second num */
			arr[i][1] = atoi(p);
	}
	return i;
}

/* read from coordinates, create map of point to point distances */
void
makedmap(int map[][AXIS], int loc[][2], int num)
{
	int dist, tmp;
	int i, j, k;

	for (i = 0; i < AXIS; ++i) {
		for (j = 0; j < AXIS; ++j) {
			dist = AXIS;	/* max distance */
			map[i][j] = 0;	/* paranoid check to avoid uninitialized at runtime errors */
			for (k = 1; k < num; ++k) {	/* 0th element is our dummy var in loc */
				/* distance in manhatten geo */
				tmp = abs(i - loc[k][0]) + abs(j - loc[k][1]);
				if (tmp < dist) {
					dist = tmp;
					map[i][j] = k;
				} else if (tmp == dist) {
					map[i][j] = 0;	/* multiple points are closest */
				}
			}
		}
	}
}

/* read from coordinates, create map of sum of distances to coordinates. */
void
maketmap(int map[][AXIS], int loc[][2], int num)
{
	int dist;
	int i, j, k;

	for (i = 0; i < AXIS; ++i) {
		for (j = 0; j < AXIS; ++j) {
			dist = 0;
			for (k = 1; k < num; ++k) {	/* 0th element is our dummy var in loc*/
				dist += abs(i - loc[k][0]) + abs(j - loc[k][1]);
			}
			map[i][j] = dist;
		}
	}
}

/* find largest finite area and return size of area */
int
getlargest(int map[][AXIS], int num)
{
	int area[num], max, tmp;
	int i, j;

	for (i = 0; i < num; ++i)
		area[i] = 0;
	for (i = 0; i < AXIS; ++i)
		for (j = 0; j < AXIS; ++j)
			++area[map[i][j]];	/* map[][] should range from 0 to num */
	/* discount inifinite areas or areas bordering an edge */
	area[0] = 0;	/* 0 in map[][] means no point is closest (ie multiple points are close) */
	for (i = 0; i < AXIS; ++i) {
		area[map[i][0]] = 0;
		area[map[0][i]] = 0;
		area[map[i][AXIS-1]] = 0;
		area[map[AXIS-1][i]] = 0;
	}
	max = 0;
	for (i = 0; i < num; ++i) {
		tmp = area[i];
		if (tmp > max)
			max = area[i];
	}
	return max;
}

/* return the number of points less than 10000 the sum distance to all coordinates. */
int
getsumarea(int map[][AXIS], int num)
{
	int area;
	int i, j;

	area = 0;
	for (i = 0; i < AXIS; ++i)
		for (j = 0; j < AXIS; ++j)
			if (map[i][j] < 10000)
				++area;
	return area;
}

int
main()
{
	FILE *fp;
	int loc[BUFSIZ][2];	/* list of x, y coordinates. 1-indexed */
	int map[AXIS][AXIS];	/* x,y plane mapping closest points, 0 is no point is closest*/
	int len, res;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	len = readcoor(loc, fp);
	makedmap(map, loc, len);
	res = getlargest(map, len);
	printf("The largest finite area is: %d\n", res);
	maketmap(map, loc, len);
	res = getsumarea(map, len);
	printf("The area of points 10000 or less from the sum manhatten distance of all points is: %d\n", res);
	fclose(fp);
	return 0;
}

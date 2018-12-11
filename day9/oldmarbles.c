#include <err.h>
#include <stdio.h>
#include <stdlib.h>

/*
 * place marble pnt after pos in marbles, move elements in *marbles if needed,
 * return num of marbles.
 */
int
putmarble(int *marbles, const int pos, int pnt, int len)
{
	int i;

	printf("before: ");
	for (i = 0; i < len; ++i) {
		if (i == pos)
			printf(" (%d)", marbles[i]);
		else
			printf(" %d", marbles[i]);
	}
	putchar('\n');
	for (i = len; i > pos + 1; --i) {
		marbles[i + 1] = marbles[i];
	}
	marbles[i] = pnt;
	printf("after:  ");
	for (i = 0; i < len + 1; ++i) {
		if (i == pos)
			printf(" (%d)", marbles[i]);
		else
			printf(" %d", marbles[i]);
	}
	putchar('\n');
	return len + 1;
}

/* remove marble and return num of marbles */
int
rmmarble(int *marbles, const int pos)
{
	int i;

	for (i = pos; marbles[i + 1] != 0; ++i)
		marbles[i] = marbles[i + 1];
	marbles[i] = 0;
	return --i;
}

/* return winning score */
int
winner(int *players, int nplyrs)
{
	int i, max;

	max = 0;
	for (i = 0; i <= nplyrs; ++i)
		if (players[i] > max)
			max = players[i];
	return max;
}

/* dump marbles to stdout with cur position and cur player */
void
debugmarbles(int *marbles, const int pos, int plyr)
{
	int i;

	printf("[%d]", plyr);
	i = 0;
	do {
		if (i == pos - 1)
			printf(" (%d)", i);
		else
			printf(" %d", i);
	} while (marbles[i++] != 0);
	putchar('\n');
}

int
whowins(int *marbles, int *players, int npnts, int nplyrs)
{
	int plyr, pnt, pos, nm;

	marbles[0] = 0;	/* set zero marble to start game */
	nm = 1;
	/* each player takes a turn placing a point */
	for (plyr = 1, pnt = 1, pos = 0; pnt <= npnts; ++pnt, plyr = (plyr % nplyrs) + 1) {
		if (pnt % 23 != 0) {
			nm = putmarble(marbles, pos, pnt, nm);
			pos += 2;
			if (pos >= nm)
				pos %= nm;
		} else {	/* calcuate points */
			players[plyr] += pnt;
			pos -= 7;
			while (pos < 0)
				pos += nm;
			players[plyr] += marbles[pos];
			nm = rmmarble(marbles, pos);
			pos += 6;
			if (pos > nm)
				pos %= nm;
		}
	}
	pnt = winner(players, nplyrs);
	return pnt;
}

int
main()
{
	FILE *fp;
	int nplyrs, npnts, winner;
	int *marbles, *players;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	fscanf(fp, "%d players; last marble is worth %d points\n", &nplyrs, &npnts);
	printf("nplyrs: %d, npnts: %d\n", nplyrs, npnts);

	/* calloc initializes memory to zero */
	/* length of marbles is max number of points plus trailing zero */
	if ((marbles = (int *) calloc(sizeof(int), npnts + 1)) == NULL)
		err(1, "malloc");
	if ((players = (int *) calloc(sizeof(int), nplyrs + 1)) == NULL)
		err(1, "malloc");

	winner = whowins(marbles, players, npnts, nplyrs);
	printf("winner: %d\n", winner);

	free(players);
	free(marbles);
	fclose(fp);
	return 0;
}

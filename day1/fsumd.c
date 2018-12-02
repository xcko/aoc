#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#define HISTMAX 1000000

static const char *fileinput = "./input";
static int history[HISTMAX];	/* dynamic? */
static unsigned long int histlen = 0;

/*
 * since the input file has a strict format, we rely on it
 * +123
 * -432
 * ...
 */

static int
getnum(FILE *fp)
{
	int c, n;

	n = 0;
	while ((c = fgetc(fp)) != '\n') {
		int t;

		t = c - '0';	/* dependence on ascii compatible */
		if (t >= 0 || t <= 9)	/* check valid num */
			n = (n * 10) + t;
	}
	return n;
}

/* debugging function */
static void
dumphistory()
{
	int i;

	for (i = 0; i < histlen; ++i)
		printf("%d\n", history[i]);
}

/* add val to history */
static void
addhistory(int val)
{
	history[histlen] = val;
	histlen++;
	if (histlen >= HISTMAX)
		errx(1, "histlen too short");
}

/* check if val is in history, return 1 on true and 0 on false */
static int
isinhistory(int val)
{
	int i;

	for (i = 0; i < histlen; i++) {
		if (val == history[i]) {
			printf("found dup: %d after %d\n", val, histlen);
			return 1;
		}
	}
	return 0;
}

static int
parseinput(long int seed)
{
	FILE *fp;
	int c;
	long int n;

	if ((fp = fopen(fileinput, "r")) == NULL)
		err(1, "fopen");
	n = seed;
	while ((c = fgetc(fp)) != EOF) {
		switch(c) {
		case '+':
			n += getnum(fp);
			if (isinhistory(n))
				exit(0);
			addhistory(n);
			break;
		case '-':
			n -= getnum(fp);
			if (isinhistory(n))
				exit(0);
			addhistory(n);
			break;
		default:
			errx(1, "bad input.");
			break;
		}
	}
	fclose(fp);
	return n;
}

int
main()
{
	long int n;

	n = 0;
	history[histlen] = 0;
	histlen++;
	/* parseinput() will exit */
	do {
		n = parseinput(n);
		printf("status: histlen: %d\n", histlen);
	} while (1);
	return 0;
}

#include <err.h>
#include <stdio.h>

static const char *fileinput = "./input";

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

int
main()
{
	FILE *fp;
	int c, n;

	if ((fp = fopen(fileinput, "r")) == NULL)
		err(1, "fopen");
	n = 0;
	while ((c = fgetc(fp)) != EOF) {
		switch(c) {
		case '+':
			n += getnum(fp);
			break;
		case '-':
			n -= getnum(fp);
			break;
		default:
			errx(1, "bad input.");
			break;
		}
	}
	fclose(fp);
	printf("Total from %s is %d\n", fileinput, n);
	return 0;
}

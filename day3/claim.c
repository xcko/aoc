#include <err.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "input.h"

extern const char *input[];

#define MAXID 10000
#define MAXW 2000
#define MAXH 2000

static int cloth[MAXW][MAXH];

struct claim {
	long long id;
	long long x;
	long long y;
	long long w;
	long long h;
};

void
zerocloth()
{
	int i, j;

	for (i = 0; i < MAXW; ++i)
		for (j = 0; j < MAXH; ++j)
			cloth[i][j] = 0;
}

void
readclaim(struct claim *c, int num)
{
	char *line, *tok, *end;
	const char *errstr = NULL;
	int i;

	line = end = strdup(input[num]);
	if (!line)
		err(1, "strdup");
	i = 0;
	while ((tok = strsep(&end, " #@,:x")) != NULL) {
		if (*tok != '\0') {	/* for when there are delims sequentially */
			switch (i) {
			case 0:
				c->id = strtonum(tok, 0, MAXID, &errstr); break;
			case 1:
				c->x = strtonum(tok, 0, MAXW, &errstr); break;
			case 2:
				c->y = strtonum(tok, 0, MAXH, &errstr); break;
			case 3:
				c->w = strtonum(tok, 0, MAXW - c->x, &errstr); break;
			case 4:
				c->h = strtonum(tok, 0, MAXH - c->y, &errstr); break;
			default: break;
			}
			if (errstr != NULL)
				errx(1, "number is %s: %s", errstr, tok);
			++i;
		}
	}
	free(line);
}

void
claimcloth(struct claim c)
{
	int i, j;

	for (i = c.x; i < c.x + c.w; ++i)
		for (j = c.y; j < c.y + c.h; ++j)
			++cloth[i][j];
}

int
twoclaimedcloth()
{
	int i, j, n;

	n = 0;
	for (i = 0; i < MAXW; ++i)
		for (j = 0; j < MAXH; ++j)
			if (cloth[i][j] >= 2)
				++n;
	return n;
}

int
verifyclaim(struct claim c)
{
	int i, j;

	for (i = c.x; i < c.x + c.w; ++i)
		for (j = c.y; j < c.y + c.h; ++j)
			if (cloth[i][j] > 1)
				return 0;
	return 1;
}

int
main()
{
	struct claim c;
	int i, res;

	zerocloth();
	for (i = 0; i < sizeof(input)/sizeof(input[0]); ++i) {
		readclaim(&c, i);
		claimcloth(c);
	}
	res = twoclaimedcloth();
	printf("claim conflicts of two or more: %d\n", res);
	for (i = 0; i < sizeof(input)/sizeof(input[0]); ++i) {
		readclaim(&c, i);
		if (verifyclaim(c))
			printf("claim %lld has no conflicts\n", c.id);
	}
	return 0;
}


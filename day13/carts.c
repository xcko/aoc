#include <err.h>
#include <stdio.h>
#include <stdlib.h>

struct cart {
	int	row;
	int	col;
	int	nturns;
	char	direct;
	int	crash;
};

int numcarts;

void
printmap(char **map)
{
	int i;

	for (i = 0; map[i] != NULL; ++i)
		printf("%s", map[i]);
}

void
printcarts(struct cart *carts)
{
	int i;

	for (i = 0; i < numcarts; ++i)
		printf("row: %d, col: %d, nturns: %d, direct: %c, state: %d\n",
			carts[i].row, carts[i].col,
			carts[i].nturns, carts[i].direct, carts[i].crash);
}

void
printcwm(struct cart *carts, char **map)
{
	char c;
	int i, j, k, t;

	for (i = 0; map[i] != NULL; ++i) {
		for (j = 0; map[i][j] != '\0'; ++j) {
			t = 0;
			c = map[i][j];
			for (k = 0; k < numcarts; ++k) {
				if (carts[k].row == i && carts[k].col == j)
					c = carts[k].direct;
					if (carts[k].crash == 1)
						c = 'G';	/* "ghost" */
					++t;
				if (t > 1)
					c = 'X';
					break;
			}
			putchar(c);
		}
	}
}

char **
makemap(FILE *fp)
{
	char **map;
	int c, i, n;

	/* count num lines for memory alloocation */
	n = 0;
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			++n;
	rewind(fp);
	if ((map = calloc(sizeof(char *), n + 1)) == NULL)	/* n + 1 so last char * = NULL */
		err(1, "calloc");
	/* instead of calloc'ing and using fgets, loops could be combined into getline's which allocate for us */
	for (i = 0; i < n; ++i)
		if ((map[i] = calloc(sizeof(char), BUFSIZ)) == NULL)
			err(1, "calloc");
	for (i = 0; i < n; ++i)
		if (fgets(map[i], BUFSIZ, fp) == NULL)
			err(1, "fgets");
	return map;
}

struct cart *
makecarts(char **map)
{
	struct cart *carts;
	int i, j, k;

	/* count carts for allocation */
	numcarts = 0;
	for (i = 0; map[i] != NULL; ++i)
		for (j = 0; map[i][j] != '\0'; ++j)
			switch(map[i][j]) {
			case '^': case'<': case '>': case 'v':
				++numcarts;
				break;
			}
	if ((carts = calloc(sizeof(*carts), numcarts)) == NULL)
		err(1, "calloc");

	k = 0;
	for (i = 0; map[i] != NULL; ++i) {
		for (j = 0; map[i][j] != '\0'; ++j) {
			switch (map[i][j]) {
			case '^': case '<': case '>': case 'v':
				carts[k].row = i;
				carts[k].col = j;
				carts[k].nturns = 0;
				carts[k].direct = map[i][j];
				carts[k].crash = 0;
				++k;
				break;
			}
		}
	}
	return carts;
}

/* remove carts initial positions from map */
void
cleanmap(char **map)
{
	int i, j;

	for (i = 0; map[i] != NULL; ++i)
		for (j = 0; map[i][j] != '\0'; ++j)
			switch (map[i][j]) {
			case '^':
			case 'v':
				map[i][j] = '|';
				break;
			case '<':
			case '>':
				map[i][j] = '-';
				break;
			}
}

/* there has to be a better way of expressing this */
void
stepcart(struct cart *p, char **map)
{
	/* checks cart direction, moves cart, updates direction */
	switch (p->direct) {
	case '^':
		p->row--;
		switch (map[p->row][p->col]) {
		case '|':
			break;
		case '\\':
			p->direct = '<';
			break;
		case '/':
			p->direct = '>';
			break;
		case '+':
			switch (p->nturns % 3) {
			case 0:
				p->direct = '<';
				break;
			case 1:
				p->direct = '^';
				break;
			case 2:
				p->direct = '>';
				break;
			}
			p->nturns++;
			break;
		default:
			warnx("cart off track: X:%d Y:%d nturns:%d direct:%c", p->col, p->row, p->nturns, p->direct);
			break;
		}
		break;
	case '<':
		p->col--;
		switch (map[p->row][p->col]) {
		case '-':
			break;
		case '\\':
			p->direct = '^';
			break;
		case '/':
			p->direct = 'v';
			break;
		case '+':
			switch (p->nturns % 3) {
			case 0:
				p->direct = 'v';
				break;
			case 1:
				p->direct = '<';
				break;
			case 2:
				p->direct = '^';
				break;
			}
			p->nturns++;
			break;
		default:
			warnx("cart off track: X:%d Y:%d nturns:%d direct:%c", p->col, p->row, p->nturns, p->direct);
			break;
		}
		break;
	case '>':
		p->col++;
		switch (map[p->row][p->col]) {
		case '-':
			break;
		case '\\':
			p->direct = 'v';
			break;
		case '/':
			p->direct = '^';
			break;
		case '+':
			switch (p->nturns % 3) {
			case 0:
				p->direct = '^';
				break;
			case 1:
				p->direct = '>';
				break;
			case 2:
				p->direct = 'v';
				break;
			}
			p->nturns++;
			break;
		default:
			warnx("cart off track: X:%d Y:%d nturns:%d direct:%c", p->col, p->row, p->nturns, p->direct);
			break;
		}
		break;
	case 'v':
		p->row++;
		switch (map[p->row][p->col]) {
		case '|':
			break;
		case '/':
			p->direct = '<';
			break;
		case '\\':
			p->direct = '>';
			break;
		case '+':
			switch (p->nturns % 3) {
			case 0:
				p->direct = '>';
				break;
			case 1:
				p->direct = 'v';
				break;
			case 2:
				p->direct = '<';
				break;
			}
			p->nturns++;
			break;
		default:
			warnx("cart off track: X:%d Y:%d nturns:%d direct:%c", p->col, p->row, p->nturns, p->direct);
			break;
		}
		break;
	default:
		err(1, "cart in impossible state");
		break;
	}
}

int
sortcarts(const void *p0, const void *p1)
{
	struct cart *a, *b;

	a = (struct cart *) p0;
	b = (struct cart *) p1;
	if (a->row > b->row) {
		return 1;
	} else if (a->row < b->row) {
		return -1;
	} else {
		if (a->col > b->col) {
			return 1;
		} else if (a->col < b->col) {
			return -1;
		} else {
			return 0;
		}
	}
}

/* zero on false, one on true */
int
iscolld(int coor[2], struct cart *carts)
{
	int i, j;

	/* naively compare each cart to the next */
	for (i = 0; i + 1 < numcarts; ++i) {
		for (j = i + 1; j < numcarts; ++j) {
			if (carts[i].row == carts[j].row &&
			    carts[i].col == carts[j].col &&
			    carts[i].crash == 0 && carts[j].crash == 0) {
				coor[0] = carts[i].row;
				coor[1] = carts[i].col;
				carts[i].crash = carts[j].crash = 1;
				return 1;
			}
		}
	}
	return 0;
}

void
findcollision(struct cart *carts, char **map)
{ 
	int coor[2];
	int i, n;

	for (n = numcarts; n > 1;) {
		qsort(carts, numcarts, sizeof(*carts), sortcarts);
		for (i = 0; i < numcarts; ++i) {
			stepcart(&carts[i], map);
			if (iscolld(coor, carts)) {
				printf("collision at X: %d, Y: %d\n", coor[1], coor[0]);
				n -= 2;
			}
		}
	}
	for (i = 0; i < numcarts; ++i)
		if (carts[i].crash == 0)
			printf("last cart at X: %d, Y: %d\n", carts[i].col, carts[i].row);
}

void
cleanup(struct cart *carts, char **map)
{
	int i;

	free(carts);
	for (i = 0; map[i] != NULL; ++i)
		free(map[i]);
	free(map);
}

int
main()
{
	FILE *fp;
	char **map;
	struct cart *carts;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	map = makemap(fp);
	fclose(fp);
	carts = makecarts(map);
	cleanmap(map);
	findcollision(carts, map);
	cleanup(carts, map);
	return 0;
}

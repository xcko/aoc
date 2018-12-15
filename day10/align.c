#include <err.h>
#include <stdio.h>
#include <stdlib.h>

/* single linked list */
struct star {
	int	x;
	int	y;
	int	dx;
	int	dy;
	struct star *next;
};

struct star *
getstars(FILE *fp)
{
	struct star *head, *p, *tmp;
	char buf[256];

	head = NULL;
	while (fgets(buf, sizeof(buf), fp) != NULL) {
		tmp = malloc(sizeof(struct star));
		if (tmp == NULL)
			err(1, "malloc");
		sscanf(buf, "position=<%i,  %i> velocity=<%i, %i>\n",
			&tmp->x, &tmp->y, &tmp->dx, &tmp->dy);
		tmp->next = NULL;
		if (head == NULL) {	/* creating first node */
			head = tmp;
			p = head;
		} else {	/* all the rest */
			p->next = tmp;	/* connect */
			p = tmp;	/* move pointer */
		}
	}
	return head;
}

void
freestars(struct star *head)
{
	struct star *p, *tmp;

	p = head;
	while (p != NULL) {
		tmp = p;
		p = p->next;
		free(tmp);
	}
}

long int
areastars(struct star *stars)
{
	struct star *p;
	long int tmpx, tmpy;
	long int maxx, maxy;
	long int minx, miny;

	maxx = maxy = -99999;
	minx = miny = 99999;	/* assume stars are centered around zero... */
	for (p = stars; p != NULL; p = p->next) {
		tmpx = p->x;
		tmpy = p->y;
		if (tmpx < minx)
			minx = tmpx;
		if (tmpy < miny)
			miny = tmpy;
		if (tmpx > maxx)
			maxx = tmpx;
		if (tmpy > maxy)
			maxy = tmpy;
	}
	return labs(maxx - minx) * labs(maxy - miny);
}

void
stepstars(struct star *head, int fob)	/* fob: forward or backward */
{
	struct star *p;

	if (fob > 0) {
		for (p = head; p != NULL; p = p->next) {
			p->x += p->dx;
			p->y += p->dy;
		}
	} else if (fob < 0) {
		for (p = head; p != NULL; p = p->next) {
			p->x -= p->dx;
			p->y -= p->dy;
		}
	}
}

void
printimage(int **image, int maxx, int maxy)
{
	int i, j;

	for (i = 0; i < maxy; ++i) {
		for (j = 0; j < maxx; ++j) {
			if (image[j][i] == 0)
				putchar('.');
			else
				putchar('#');
		}
		putchar('\n');
	}
}

/* assume minx and miny are at 0 */
void
printstars(struct star *stars)
{
	int **image;
	struct star *p;
	long int tmpx, tmpy;
	long int maxx, maxy;
	long int minx, miny;
	int i;

	/* calculate min and max dimensions */
	maxx = maxy = -99999;
	minx = miny = 99999;
	for (p = stars; p != NULL; p = p->next) {
		tmpx = p->x;
		tmpy = p->y;
		if (tmpx < minx)
			minx = tmpx;
		if (tmpy < miny)
			miny = tmpy;
		if (tmpx > maxx)
			maxx = tmpx;
		if (tmpy > maxy)
			maxy = tmpy;
	}
	/* printf("maxx %li, maxy %li, minx %li, miny %li\n", maxx, maxy, minx, miny); */
	/* allocate for image with image translated so minx = 0 and miny = 0 */
	if ((image = calloc(sizeof(int *), maxx - minx + 1)) == NULL)
		err(1, "calloc");
	for (i = 0; i < maxx - minx + 1; ++i)
		if ((image[i] = calloc(sizeof(int), maxy - miny + 1)) == NULL)
			err(1, "calloc");
	for (p = stars; p != NULL; p = p->next)
		++image[p->x - minx][p->y - miny];
	/* print image */
	printimage(image, maxx - minx + 1, maxy - miny + 1);
	for (i = 0; i < maxx - minx + 1; ++i)
		free(image[i]);
	free(image);
}

void
runstars(struct star *stars)
{
	long int next, prev;
	int time;

	time = 0;
	next = areastars(stars);
	do {
		prev = next;
		stepstars(stars, 1);
		++time;
		next = areastars(stars);
	} while (next < prev);
	stepstars(stars, -1);	/* we iterate past during comparison */
	--time;
	/* printf("area %li\n", areastars(stars)); */
	printf("after %i seconds, this message will appear:\n", time);
	printstars(stars);
}

int
main()
{
	FILE *fp;
	struct star *stars;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	if ((stars = getstars(fp)) == NULL)
		errx(1, "getstars()");
	fclose(fp);
	runstars(stars);
	freestars(stars);
	return 0;
}


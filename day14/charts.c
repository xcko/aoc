#include <err.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX 200000000

static int input = 170641;

/* return 1 if we made two recipes */
int
mkrecipe(int *sb, int *nsb, int e1, int e2)
{
	int r;

	/* max new recipe is 9 + 9 = 18 */
	r = sb[e1] + sb[e2];
	if (r < 10) {
		sb[(*nsb)++] = r;
		return 0;
	} else if (r < 20) {
		sb[(*nsb)++] = 1;
		sb[(*nsb)++] = r - 10;
		return 1;
	} else {
		errx(1, "new recipe out of range");
	}
}

void
mvelves(int *sb, int nsb, int *e1, int *e2)
{
	*e1 = (*e1 + sb[*e1] + 1) % nsb;
	*e2 = (*e2 + sb[*e2] + 1) % nsb;
}

void
dump(int *sb, int nsb, int e1, int e2)
{
	int i;

	for (i = 0; i < nsb; ++i) {
		if (i == e1 && i == e2)
			printf("[(%d)]", sb[i]);
		else if (i == e1)
			printf("(%d)", sb[i]);
		else if (i == e2)
			printf("[%d]", sb[i]);
		else
			printf(" %d ", sb[i]);
	}
	putchar('\n');
}

void
part1(int *sb, int nsb, int e1, int e2)
{
	int i, n;

	while (nsb < input + 10) {
		if (nsb > MAX - 2)
			errx(1, "out of room");
		n = mkrecipe(sb, &nsb, e1, e2);
		mvelves(sb, nsb, &e1, &e2);
	}

	printf("scores of last 10 recipes after making %d recipes: ", input);
	for (i = nsb - (10 + n); i > 0 && i < nsb - n; ++i)	/* n (num new recipies) is 0 or 1 */
		printf("%d", sb[i]);
	putchar('\n');
}

int
chksb(int *sb, int nsb)
{
	int tmp, i;

	/* check against last recipe */
	tmp = input;
	for (i = nsb - 1; i > 0; --i) {
		if (sb[i] == tmp)
			return i;	/* we're done */
		else if (sb[i] == tmp % 10)
			tmp /= 10;
		else
			break;
	}

	/* check against recipe before it in case we made two */
	tmp = input;
	for (i = nsb - 2; i > 0; --i) {
		if (sb[i] == tmp)
			return i;	/* we're done */
		else if (sb[i] == tmp % 10)
			tmp /= 10;
		else
			break;
	}
	return 0;
}

void
part2(int *sb, int nsb, int e1, int e2)
{
	int res;

	while ((res = chksb(sb, nsb)) == 0) {
		if (nsb > MAX - 2) {
			errx(1, "out of room");
		}
		mkrecipe(sb, &nsb, e1, e2);
		mvelves(sb, nsb, &e1, &e2);
	}
	printf("%d first appears after %d recipes.\n", input, res);
}

int
main()
{
	int *sb, nsb;
	int e1, e2;

	if ((sb = calloc(sizeof(*sb), MAX)) == NULL)
		err(1, "malloc");
	/* seed */
	sb[0] = 3;
	sb[1] = 7;
	nsb = 2;
	e1 = 0;
	e2 = 1;

	/* part1(sb, nsb, e1, e2); */
	part2(sb, nsb, e1, e2);

	free(sb);
	return 0;
}

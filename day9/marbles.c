#include <sys/limits.h>

#include <err.h>
#include <stdio.h>
#include <stdlib.h>

struct marbles {
	struct	marbles *next;
	struct	marbles *prev;
	unsigned long long int	val;
};

/* get highscore */
unsigned long long int
highscore(unsigned long long int *plyrs, int nplyrs)
{
	unsigned long long int max;
	int i;

	max = 0;
	for (i = 0; i < nplyrs; ++i)
		if (plyrs[i] > max)
			max = plyrs[i];
	return max;
}

/* play game, return winning score */
unsigned long long int
playmarbles(int nplyrs, unsigned long long int npnts)
{
	struct marbles *head, *pos, *tmp;
	unsigned long long int pnts, plyrs[nplyrs];
	int plyr;
	int n, i;

	for (i = 0; i < nplyrs; ++i)
		plyrs[i] = 0;
	if ((head = malloc(sizeof(struct marbles))) == NULL)
		err(1, "malloc");
	head->val = 0;
	head->prev = head;
	head->next = head;
	n = 1;
	pos = head;
	for (pnts = 1, plyr = 0; pnts < npnts; ++pnts, plyr = (plyr + 1) % nplyrs) {
		if (pnts % 23 == 0) {
			plyrs[plyr] += pnts;		
			for (i = 0; i < 7; ++i)
				pos = pos->prev;
			/* printf("%d\n", pos->val); */	/* debug */
			plyrs[plyr] += pos->val;
			tmp = pos;
			pos = pos->next;
			tmp->prev->next = pos;
			pos->prev = tmp->prev;
			--n;
		} else {
			pos = pos->next;	/* mv right, then add node after */
			tmp = pos;
			if ((pos = malloc(sizeof(struct marbles))) == NULL)
				err(1, "malloc");
			pos->val = pnts;
			pos->prev = tmp;
			pos->next = tmp->next;
			tmp->next = pos;
			pos->next->prev = pos;
			++n;
		}
		/* debug */
		/* printf("[%d]", plyr + 1); */	/* just to match example */
		/* tmp = head;
		 * for (i = 0; i < n; ++i) {
			 * if (tmp == pos)
				 * printf(" (%d)", tmp->val);
			 * else
				 * printf(" %d", tmp->val);
			 * tmp = tmp->next;
		 * }
		 * putchar('\n');
		*/
	}
	pos = head;
	for (i = 0; i < n; ++i) {
		tmp = pos;
		pos = pos->next;
		free(tmp);
	}
	return highscore(plyrs, nplyrs);
}

int
main()
{
	FILE *fp;
	int nplyrs;
	unsigned long long int npnts;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	fscanf(fp, "%d players; last marble is worth %llu points\n", &nplyrs, &npnts);
	fclose(fp);
	printf("winning score: %llu\n", playmarbles(nplyrs, npnts));
	printf("max: %llu, last marble: %llu\n", ULLONG_MAX, npnts*100);
	printf("winning score if last marble is worth 100*%llu: %llu\n", npnts, playmarbles(nplyrs, 100*npnts));
	return 0;
}

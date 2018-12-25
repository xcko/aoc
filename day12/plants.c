#include <err.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAXBUF 100000

struct rule {
	char	match[6];
	char	res;
	int	end;
};

/* pad string of plants with empty pots on both ends */
int
padplants(char *plants)
{
	int pos;
	char buf[MAXBUF];

	pos = 0;
	if (strncmp(plants, ".....", 5) != 0) {
		if (strlcpy(buf, ".....", MAXBUF) >= MAXBUF)
			err(1, "strlcpy");
		if (strlcat(buf, plants, MAXBUF) >= MAXBUF)
			err(1, "strlcat");
		if (strlcpy(plants, buf, MAXBUF) >= MAXBUF)
			err(1, "strlcpy");
		pos = -5;
	}
	if (strncmp(&plants[strlen(plants) - 5], ".....", 5) != 0) {
		if (strlcat(plants, ".....", MAXBUF) >= MAXBUF)
			err(1, "strlcat");
	}
	return pos;
}

/* iterate once over plants and apply rules */
void
stepplants(char *plants, struct rule *rules)
{
	char step[MAXBUF];
	char tmp[6];
	int i, j;

	strlcpy(step, plants, MAXBUF);
	for (i = 2; i < strlen(plants) - 2; ++i) {
		int m;

		strncpy(tmp, &plants[i - 2], 5);
		tmp[5] = '\0';
		m = 0;
		for (j = 0; rules[j].end != 1; ++j) {
			if (strcmp(tmp, rules[j].match) == 0) {
				step[i] = rules[j].res;
				m = 1;
				break;
			}
		}
		if (!m)
			step[i] = '.';
	}
	strlcpy(plants, step, MAXBUF);
}

/* count plants using pos indicator */
int
countplants(char *plants, int p)
{
	int i, num;

	num = 0;
	for (i = 0; i < strlen(plants); ++i)
		if (plants[i] == '#') {
			num += p + i;
	}
	return num;
}

/* run the plants through n interations */
void
runplants(char *plants, struct rule *rules, int num)
{
	int i, p;
	int hist[500];

	p = 0;	/* this is the zero position */
	for (i = 0; i < num; ++i) {
		p += padplants(plants);	/* make sure we have room */
		stepplants(plants, rules);
		hist[i] = countplants(plants, p);
	}
	/* discovered this solution for part 2 by looking at others */
	printf("rate of change at gen: %d is %d plants per %d gen\n", num, hist[499] - hist[494], 5);
	printf("or %d per gen, starting at gen 500 with %d plants implies:\n", (hist[499] - hist[494])/5, hist[499]);
	printf("%ld plants by gen 50 billion\n", (((long) hist[499] - (long) hist[494])/5) * (50000000000 - 500) + (long) hist[499]);
}

/* read initial state and rules into structures */
int
main()
{
	FILE *fp;
	char plants[MAXBUF];
	struct rule rules[100];
	char c;
	int i;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen()");
	fscanf(fp, "initial state: %s\n", plants);
	for (i = 0; (c = fgetc(fp)) != EOF;) {
		c = ungetc(c, fp);
		if (c == '\n')	/* emtpy line */
			continue;
		fscanf(fp, "%s => %c\n", rules[i].match, &rules[i].res);
		rules[i].end = 0;	/* false */
		++i;
	}
	rules[i].end = 1;	/* flag value to know end of rules */
	fclose(fp);
	runplants(plants, rules, 500);
	return 0;
}

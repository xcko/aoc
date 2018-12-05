#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAXLINES 2000
#define MAXID	5000

enum {
	AWAKE,
	SLEEP
};

struct entry {
	int id;
	int state[60];	/* state for each minute per hour */
};

int
cmplines(const void *line1, const void *line0)
{
	char *a, *b;

	a = (char *)line0;
	b = (char *)line1;
	return strncmp(b, a, BUFSIZ);
}

void
addentry(struct entry *into, int id, int state, int min)
{
	int i;

	into->id = id;
	for (i = min; i < 60; ++i)
		into->state[i] = state;
}

void
zeroentries(struct entry entries[], int num)
{
	int i, j;

	for (i = 0; i < num; ++i) {
		entries[i].id = 0;
		for (j = 0; j < 60; ++j)
			entries[i].state[j] = 0;
	}
}

void
parselines(struct entry log[], char lines[MAXLINES][BUFSIZ], int num)
{
	static int id;
	struct tm date;
	char *p, *d;
	int aos, i;

	for (i = 0; i < num; ++i) {
		d = strndup(lines[i], BUFSIZ);	/* we may modify line */
		if (!d)
			err(1, "strndup");
		p = strptime(d, "[%F %R]", &date);
		if (!p)
			err(1, "strptime");
		++p;	/* skip space */
		switch (*p) {
		case 'G':	/* "Guard #<num> begins shift" */
			p = strtok(d, "#");	/* modifies string d */
			p = strtok(NULL, " ");	/* we want just the num */
			id = atoi(p);	/* input is safe */
			aos = AWAKE;	/* new guards are awake */
			date.tm_min = 0;	/* guards essentially start at 00:00 */
			break;
		case 'w':	/* "wakes up" */
			aos = AWAKE;
			break;
		case 'f':	/* "falls asleep" */
			aos = SLEEP;
			break;
		default:
			err(1, "unexpected line from input: %s", p);
			break;
		}
		if (date.tm_hour > 0)	/* ie 23:XX */
			addentry(&log[date.tm_yday + 1], id, aos, date.tm_min);
		else
			addentry(&log[date.tm_yday], id, aos, date.tm_min);
		free(d);
	}
}

int
findsleepiest(struct entry log[], int num)
{
	int guard[MAXID], i, j, t, res;

	for (i = 0; i < MAXID; ++i)
		guard[i] = 0;
	for (i = 0; i < num; ++i) {
		t = 0;
		for (j = 0; j < 60; ++j) {
			if (log[i].state[j] == SLEEP)
				++t;
		}
		guard[log[i].id] += t;
	}

	t = 0;
	res = 0;
	for (i = 0; i < MAXID; ++i) {
		if (guard[i] > t) {
			t = guard[i];
			res = i;
		}
	}
	return res;
}

void
dumpinfo(struct entry info[], int guard, int lim)
{
	int i, j;

	for (i = 0; i < lim; ++i) {
		if (info[i].id != guard)
			continue;
		printf("guard %d on %d\n", info[i].id, i);
		for (j = 0; j < 60; ++j)
			printf("%d", info[i].state[j]);
	}
}

int
sleepyminute(struct entry info[], int guard, int lim)
{
	int m[60], i, j, t, res;

	for (i = 0; i < 60; ++i)
		m[i] = 0;
	for (i = 0; i < lim; ++i) {
		if (info[i].id != guard)
			continue;
		for (j = 0; j < 60; ++j)
			if (info[i].state[j] == SLEEP)
				++m[j];
	}
	res = 0;
	t = 0;
	for (i = 0; i < 60; ++i) {
		if (t < m[i]) {
			t = m[i];
			res = i;
		}
	}
if (res < 0 || res > 60) {
	dumpinfo(info, guard, lim);
	errx(1, "res is outofbounds %d, i %d "
		"guard %d, lim %d ", res, i, guard, lim);
}
	return res;
}

int
fofminute(struct entry data[], int guard, int whichm, int lim)
{
	int i, res;

	res = 0;
	for (i = 0; i < lim; ++i) {
		if (data[i].id != guard)
			continue;
		if (data[i].state[whichm] == SLEEP)
			++res;
	}
	return res;
}

int
main()
{
	FILE *fp;
	char lines[MAXLINES][BUFSIZ];
	struct entry log[365];	/* num days / year */
	int n, g, s, f;
	int i, t, u;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	n = 0;
	while (fgets(lines[n], sizeof(lines[n]), fp) != NULL)
		++n;
	qsort(lines, n, sizeof(lines[0]), cmplines);
	zeroentries(log, 365);
	parselines(log, lines, n);
	g = findsleepiest(log, 365);
	s = sleepyminute(log, g, 365);

	printf("sleepiest guard is: %d\n"
		"at minute %d\n"
		"cksum %d\n", g, s, g*s);

	t = u = 0;
	for (i = 0; i < 365; ++i) {
		if (log[i].id == 0)
			continue;
		s = sleepyminute(log, log[i].id, 365);
		f = fofminute(log, log[i].id, s, 365);
		if (f >= t) {
			t = f;
			g = log[i].id;
			u = s;
		}
	}
	printf("guard %d's sleepiest minute %d at %d times\n", g, u, t);
	printf("chksum %d\n", g * u);
	fclose(fp);
	return 0;
}

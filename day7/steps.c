#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEPS 26	/* alphabet is the range of possible values */
#define MAXIN 200
#define MAXSEC 3000
#define WRKRS 5

struct workers {
	char step;
	int end;
};

/* into[] will be of the form: into[1] depends on into[0] */
int
readin(char into[][2], FILE *fp)
{
	char buf[BUFSIZ], *p;
	int i;

	for (i = 0; fgets(buf, sizeof(buf), fp) != NULL && i < MAXIN; ++i) {
		p = strchr(buf, ' ');	/* skip first word "Step" */
		++p;	/* skip space */
		into[i][0] = *p;
		p = strchr(buf, 'c');	/* move to first char of "can" */
		p -= 2;	/* mv before " c" */
		into[i][1] = *p;
	}
	return i;
}

void
zeroarr(char into[], int lim)
{
	int i;

	for (i = 0; i < lim; ++i)
		into[i] = '\0';
}

void
addtoarr(char into[], char from)
{
	int i;

	for (i = 0; into[i] != '\0'; ++i)
		;
	into[i] = from;
}

int
isinarr(char into[], char from)
{
	int i;

	for (i = 0; into[i] != '\0'; ++i)
		if (into[i] == from)
			return 1;
	return 0;
}

void
finddeps(char into[STEPS], char match, char from[][2], int lim)
{
	int i;

	for (i = 0; i < lim; ++i) {
		if (match != from[i][1])
			continue;
		if (!isinarr(into, from[i][0]))
			addtoarr(into, from[i][0]);
		finddeps(into, from[i][0], from, lim);
	}
}

void
mkdeps(char into[STEPS][STEPS], char from[][2], int lim)
{
	int i;

	for (i = 0; i < STEPS; ++i) {
		zeroarr(into[i], STEPS);
		finddeps(into[i], (char)i + 'A', from, lim);
	}
}

int
hasdeps(char fin[], char from[])
{
	int i;

	for (i = 0; from[i] != '\0'; ++i)
		if (!isinarr(fin, from[i]))
			return 0;
	return 1;
}

void
sortdeps(char out[STEPS], char deps[STEPS][STEPS])
{
	int i, j;

	for (i = 0; i < STEPS; ++i) {
		for (j = 0; j < STEPS; ++j) {
			if (isinarr(out, j + 'A'))
				continue;
			if (hasdeps(out, deps[j])) {
				out[i] = j + 'A';
				break;
			}
		}
	}
	out[i] = '\0';
}

void
whatisdone(char into[], struct workers wrkrs[], int t)
{
	int i;

	for (i = 0; i < WRKRS; ++i) {
		if (t < 1) {
			wrkrs[i].step = '\0';
			wrkrs[i].end = 0;
			continue;
		}
		if (wrkrs[i].end != t)
			continue;
		/* they are done */
		addtoarr(into, wrkrs[i].step);
		wrkrs[i].step = '\0';
		wrkrs[i].end = 0;
	}
}

void
whatisavail(char into[], struct workers w[], char from[], char deps[][STEPS])
{
	int i, j, chk;

	for (i = 0; i < STEPS; ++i) {
		if (isinarr(from, i + 'A'))
			continue;
		chk = 0;
		for (j = 0; j < WRKRS; ++j)
			if (w[j].step == i + 'A')
				chk = 1;
		if (chk)
			continue;
		if (hasdeps(from, deps[i]))
			addtoarr(into, i + 'A');
	}
}

int
worksteps(char deps[][STEPS])
{
	struct workers elves[WRKRS];
	char avail[STEPS], done[STEPS];
	int i, j, time, n, m;

	m = 0;	/* count for num done */
	zeroarr(done, STEPS);
	for (time = 0; time < MAXSEC; ++time) {
		/* check if steps are finished */
		whatisdone(done, elves, time);
		for (i = 0; done[i] != '\0' && i < STEPS; ++i)
			;
		if (i == STEPS)
			return time;

		/* find what is available from what is in progress and done */
		zeroarr(avail, STEPS);
		whatisavail(avail, elves, done, deps);
		for (i = 0; avail[i] != '\0'; ++i)
			;
		n = i;
		/* assign work */
		j = 0;
		for (i = 0; i < WRKRS; ++i) {
			if (elves[i].step != '\0') /* elf is busy */
				continue;
			if (j < n) {
				elves[i].step = avail[j];
				elves[i].end = avail[j] - 'A' + 61 + time;
				++j;
			}
		}
/* debuginfo */
printf("%d\t", time);
for (i = 0; i < WRKRS; ++i)
	if (elves[i].step != '\0')
		printf("%c\t", elves[i].step);
	else
		printf(".\t");
printf("%s\n", done);
	}
	return -1;	/* we ran out of time */
}

int
main()
{
	FILE *fp;
	char in[MAXIN][2];
	char lodeps[STEPS][STEPS];
	char res[STEPS];
	int len;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	len = readin(in, fp);
	fclose(fp);
	mkdeps(lodeps, in, len);
 	zeroarr(res, STEPS);
	sortdeps(res, lodeps);
	printf("alphabetical bias for list of steps to complete is: %s\n", res);
	printf("if we have %d workers and each step takes 60 plus the alphanumeric value of each step, "
		"then it will take %d seconds to finish\n", WRKRS, worksteps(lodeps));
	return 0;
}

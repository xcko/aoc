#include <err.h>
#include <stdio.h>

static const char *fileinput = "./input";
static int numlines = 0;

/*
 * input file has strict format that we rely on:
 * icxjubroqtunzeyzpomfksahgw
 * ibxjvbroqtunleyzjdmfksahow
 * icxjvbroqtinleyzpdmflsahnw
 * icxjvbnoqtunleyvpgmfksahgw
 * wcxjvbroqrunleyzpdmfksahge
 * icxjtbroqtjzleyzpdmfksahgw
 * icxjvbrohtunleyzpdmfkbahsw
 * xcxjvbroqcunleyzpdmfksaxgw
 * ycxjvbroqtunleyzpowfksahgw
 * icxfvbroqtunleyzpdmfksncgw
 *
 * all lowercase, no numbers, english, same length, no spaces, newline seperated
 */

/* get line from open file pointer */
static int
getlinef(char s[], FILE *fp, int lim)
{
	int i, c;

	i = 0;
	while ((c = fgetc(fp)) != EOF && c != '\n') {
		s[i] = c;
		++i;
		if (i > lim - 1)
			errx(1, "buf limit is too small: %d", lim);
	}
	s[i] = '\0';
	return i;
}

static void
checkbufs(char a[], char b[], int lim)
{
	int diff, i;
	char c[BUFSIZ];

	diff = 0;
	for (i = 0; i < lim; ++i) {
		if (a[i] != b[i])
			++diff;
		else
			c[i - diff] = a[i];
	}
	c[i - diff] = '\0';
	if (diff <= 1) {
		printf("found line:\n %s\n"
			"and line:\n %s\n"
			"with only one difference:\n %s\n", a, b, c);
	}
}

int
main()
{
	FILE *fp;
	char buf1[BUFSIZ], buf2[BUFSIZ];
	int c, i, j, len;
	long pos;

	if ((fp = fopen(fileinput, "r")) == NULL)
		err(1, "fopen");
	while ((c = fgetc(fp)) != EOF)
		if (c == '\n')
			++numlines;
	rewind(fp);
	for (i = 0; i < numlines - 1; ++i) {
		len = getlinef(buf1, fp, BUFSIZ);
		if (!len)
			errx(1, "premature end of file 1");
		pos = ftell(fp);
		for (j = i; j < numlines - 1; ++j) {
			len = getlinef(buf2, fp, BUFSIZ);
			if (!len)
				errx(1, "premature end of file 2");
			checkbufs(buf1, buf2, len);
		}
		fseek(fp, pos, SEEK_SET);
	}
	fclose(fp);
	return 0;
}

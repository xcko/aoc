#include <ctype.h>
#include <err.h>
#include <stdio.h>

/* read node and return metasum */
int
readnodemeta(FILE *fp)
{
	int sum, nchild, nmeta, meta;
	int c, i;

	c = fgetc(fp);
	if (c == EOF)
		errx(1, "we reached EOF before finishing reading nodes");
	if (isdigit(c))	/* make sure we are at a num */
		ungetc(c, fp);
	/* read header */
	fscanf(fp, "%d %d", &nchild, &nmeta);
	/* act on header contents */
	sum = 0;
	for (i = 0; i < nchild; ++i) {
		sum += readnodemeta(fp);
	}
	for (i = 0; i < nmeta; ++i) {
		fscanf(fp, " %d", &meta);
		sum += meta;
	}
	return sum;
}

/* return sum total of metadata from input file */
int
summeta(FILE *fp)
{
	return readnodemeta(fp);
}

/* read a node and return it's value */
int
readnodeval(FILE *fp)
{
	int val, nchild, nmeta, meta;
	int c, i, j;
	long pos0, pos1;

	c = fgetc(fp);
	if (c == EOF)
		errx(1, "we reached EOF in readnodeval()");
	if (isdigit(c))	/* make sure we are at a num */
		ungetc(c, fp);
	/* read header */
	fscanf(fp, "%d %d", &nchild, &nmeta);
	/* act on header contents */
	val = 0;
	/* if a node has no children, its val is the sum of its meta */
	if (nchild == 0) {
		for (i = 0; i < nmeta; ++i) {
			fscanf(fp, " %d", &meta);
			val += meta;
		}
		return val;
	}
	/* if it does have children, then it's val is = to the sum of the children indexed by the meta */
	pos0 = ftell(fp);
	/* skip child nodes to get to metadata */
	for (i = 0; i < nchild; ++i) {
		/* use readnodemeta to move through children but discard return value */
		readnodemeta(fp);
	}
	/* now we can read meta values */
	for (i = 0; i < nmeta; ++i) {
		fscanf(fp, " %d", &meta);
		if (meta > nchild)
			continue;
		pos1 = ftell(fp);
		/* go to where child nodes begin */
		fseek(fp, pos0, SEEK_SET);
		for (j = 1; j < meta; ++j) {	/* children are one-indexed ie child 1 means skip none */
			readnodemeta(fp);	/* ignore return value, we're just skipping children */
		}
		/* get the value of the node we want */
		val += readnodeval(fp);
		/* return to just after last meta read */
		fseek(fp, pos1, SEEK_SET);
	}
	return val;
}

/* return val of root node */
int
rootval(FILE *fp)
{
	return readnodeval(fp);
}

int
main()
{
	FILE *fp;
	int sum, val;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	sum = summeta(fp);
	printf("the sum of metadata in the input is: %d\n", sum);
	rewind(fp);
	val = rootval(fp);
	printf("the val of the root node is: %d\n", val);
	fclose(fp);
	return 0;
}


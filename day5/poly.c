#include <err.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFMAX 100000

int
lofreact(char poly[], int len)
{
	char buf[BUFMAX];
	int i, j;

	for (i = 0, j = 0; i < BUFMAX && j < len; ++i, ++j) {
		buf[i] = poly[j];
		if (i < 1)	/* no comparison to be made */
			continue;
		if (isupper(buf[i]) && isupper(buf[i-1]))	/* same cases never react */
			continue;
		if (islower(buf[i]) && islower(buf[i-1]))
			continue;
		if ((isupper(buf[i]) && tolower(buf[i]) == buf[i-1]) ||
		    (islower(buf[i]) && toupper(buf[i]) == buf[i-1])) {
  			i -= 2;	/* polymers react */
		}
	}
	return i;
}

int
main()
{
	FILE *fp;
	char buf[BUFMAX];
	int tmp, res;
	int i, j;

	if ((fp = fopen("./input", "r")) == NULL)
		err(1, "fopen");
	res = BUFMAX;
	for (i = 0; i < 26; ++i) {
		for (j = 0; (buf[j] = fgetc(fp)) != EOF && j < BUFMAX; ++j) {
			if (buf[j] == i + 'a')
				--j;
			else if (buf[j] == i + 'A')
				--j;
		}
		--j;	/* ++j from EOF char */
		tmp = lofreact(buf, j);
		printf("length: %d after removing %c and %c\n", tmp, i + 'a', i + 'A');
		if (tmp < res)
			res = tmp;
		rewind(fp);
	}
	printf("most reacted poly after removing any one unit is: %d\n", res);
	fclose(fp);
	return 0;
}


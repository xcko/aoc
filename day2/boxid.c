#include <err.h>
#include <stdio.h>

static const char *fileinput = "./input";
static int foundthree = 0;	/* count how many lines with exactly three identical letters */
static int foundtwo = 0;	/* ditto but with exactly two */

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

/* zero array of length len */
static void
zeroarr(int arr[], int len)
{
	int i;

	for (i = 0; i < len; ++i)
		arr[i] = 0;
}

/* check arr[] for tripples and doubles */
static void
checkarr(int arr[], int len)
{
	int i;

	for (i = 0; i < len; ++i) {
		if (arr[i] == 3) {
			++foundthree;
			break;
		}
	}
	for (i = 0; i < len; ++i) {
		if (arr[i] == 2) {
			++foundtwo;
			break;
		}
	}
}

int
main()
{
	FILE *fp;
	int hist[26];	/* 26 letters in alphabet to count frequency */
	int c;

	if ((fp = fopen(fileinput, "r")) == NULL)
		err(1, "fopen");
	zeroarr(hist, 26);
	while ((c = fgetc(fp)) != EOF) {
		switch(c) {
		case '\n':
			checkarr(hist, 26);
			zeroarr(hist, 26);
			break;
		default:
			++hist[c - 'a'];
			break;
		}
	}
	printf("found %d lines with three matching letters\n"
		"found %d lines with two matching letters\n"
		"found checksum: %d\n", foundthree, foundtwo, foundthree * foundtwo);
	fclose(fp);
	return 0;
}

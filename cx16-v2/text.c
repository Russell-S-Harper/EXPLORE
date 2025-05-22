#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

#define STD_FMT_LEN	5		/* \aT[0-9A-F] \0 */
#define MAX_MSG_LEN	(40 - 2 + 1)	/* 40 (width) - 2 (for border) + 1 (for \0) */

static char f_customized_message[STD_FMT_LEN + MAX_MSG_LEN];

static const char *f_default_message = "Visit russell-harper.com for more...";

/*
	\bT<text-color 0-F>
	\xA9 maps to diagonal BL to TR stripes
	^ maps to ↑
*/

static char *f_messages[] =
{
	"\aTA Welcome to PvP-AI!",
	"\aT7 AI Opponents Battle for Survival",
	"\aT3 You're watching them battle live!",
	"\aTF Colors mean: \aTE\xA9 \aTFabove \aT8\xA9 \aTFbelow \aT1\xA9 \aTFclose",
	"\aTD All missiles are smart!",
	"\aTA Both attacker and target advance",
	"\aT7 Good or bad, all reach the last level",
	"\aT3 At the last level a kill is permanent",
	"\aTF Press C: cycle the focus - go ahead!",
	"\aTD Press J: join as a human",
	"\aTA (Yours will have a \"cockpit\")",
	"\aT7 Use gamepad or keyboard controls",
	"\aT3 (^s to move & space to fire)",
	"\aTF Press K: be the primary target (hard)",
	"\aTD Press L: no endgame refresh (harder!)",
	"\aTA Go ahead - press J, K, or L",
	"\aT7 Last survivor takes 1st place",
	"\aT3 High score among the fallen takes 2nd",
	"\aTF These two advance!",
	"\aTD Each generation, the players refine",
	"\aTA Then they start all over again",
	f_customized_message
};

int main(void)
{
	FILE *ofile;
	int16_t max, t;
	uint8_t i, l;
	char c = '\0', working[MAX_MSG_LEN];

	/* Get the message */
	fputs("\nEnter custom message: ", stdout);
	fgets(working, MAX_MSG_LEN, stdin);

	/* Format */
	working[strcspn(working, "\r\n")] = '\0';
	strcpy(f_customized_message, "\aT7 ");
	strcat(f_customized_message, *working? working: f_default_message);

	if (!(ofile = fopen("pvp-ai.txt", "wb"))) {
		fputs("\nCould not create PVP-AI.TXT!\n", stderr);
		return EXIT_FAILURE;
	}

	/* Will need this */	
	l = sizeof(f_messages) / sizeof(char *);

	/* Get the length of the longest string */
	for (i = 0, max = 0; i < l; ++i) {
		t = strlen(f_messages[i]);
		if (max < t)
			max = t;
	}
	/* Add 1 for the /0 terminator */
	++max;

	/* Output */
	fputs("\nAttract Messages ", stdout);
	fputc(CODE_AM, ofile);
	
	/* How many messages */
	t = l;
	fwrite(&t, sizeof(int16_t), 1, ofile);
	/* How much space per message */
	fwrite(&max, sizeof(int16_t), 1, ofile);
	/* Actual messages */
	for (i = 0; i < l; ++i) {
		t = strlen(f_messages[i]);
		fwrite(f_messages[i], sizeof(char), t, ofile);
		while (t < max) {
			fwrite(&c, sizeof(char), 1, ofile);
			++t;
		}
	}
	fputs("done\n", stdout);

	/* Indicate end-of-data */
	fputc(CODE_EF, ofile);
	fclose(ofile);
	
	return EXIT_SUCCESS;
}

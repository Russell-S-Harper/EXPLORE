#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

#define STD_FMT_LEN	5		/* \aT[0-9A-F] \0 */
#define MAX_MSG_LEN	(40 - 2 + 1)	/* 40 (width) - 2 (for border) + 1 (for \0) */

static char f_customized_message[STD_FMT_LEN + MAX_MSG_LEN];

static const char *f_default_message = "Visit russell-harper.com for more...";

static char *f_messages[] =
{
	"\aT7 Welcome to PvP-AI!",
	"\aTA AI Opponents Battle for Survival",
	"\aTD You're watching them battle live!",
	"\aTF Colors mean: \aTE\xA9 \aTFabove \aT8\xA9 \aTFbelow \aT1\xA9 \aTFclose",
	"\aT3 All missiles are smart!",
	"\aT7 Both attacker and target advance",
	"\aTA Good or bad, all reach the last level",
	"\aTD At the last level a kill is permanent",
	"\aTF Press C: cycle the focus - go ahead!",
	"\aT3 Press J: join as a human",
	"\aT7 (Yours will have a \"cockpit\")",
	"\aTA Use gamepad or keyboard controls",
	"\aTD (^s to move & space to fire)",
	"\aTF Press K: be the primary target (hard)",
	"\aT3 Press L: no endgame refresh (harder!)",
	"\aT7 Go ahead - press J, K, or L",
	"\aTA Last survivor takes 1st place",
	"\aTD High score among the fallen takes 2nd",
	"\aTF These two advance!",
	"\aT3 Each generation, the players refine",
	"\aT7 Then they start all over again",
	f_customized_message
};

int main(void)
{
	FILE *ofile;
	int16_t max, t;
	uint8_t i, l;
	char c = '\0', working[MAX_MSG_LEN];

	fputs("\nEnter custom prompt: ", stdout);
	fgets(working, MAX_MSG_LEN, stdin);
	working[strcspn(working, "\r\n")] = '\0';
	strcpy(f_customized_message, "\aTA ");
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

#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

#define STD_FMT_LEN	5		/* \aT[0-9A-F] \0 */
#define MAX_MSG_LEN	(40 - 2 + 1)	/* 40 (width) - 2 (for border) + 1 (for \0) */

/* Error messages */
static const char *f_error_messages[] = {
	/*ERR_FO*/ "\nThe data file has been erased or\nrenamed. Recreate or restore from\nbackup.\n",
	/*ERR_FC*/ "\nThe data file has been corrupted.\nRecreate or restore from backup.\n",
	/*ERR_XM*/ "\nA banked memory request larger than 8K\nwas requested.\n",
	/*ERR_GI*/ "\nThere is a problem involving graphics\ninitialization.\n",
	/*ERR_OB*/ "\nThere was an attempt to access an array\nout of bounds.\n",
	/*ERR_DC*/ "\nThe display configuration has been\ncorrupted.\n",
	/*ERR_AI*/ "\nThere is a problem with the AI file.\n"
};

/*
	\bT<text-color 0-F>
	\xA9 maps to diagonal BL to TR stripes
	^ maps to ↑
*/

static char f_customized_message[STD_FMT_LEN + MAX_MSG_LEN];

static const char *f_default_message = "Visit russell-harper.com for more...";

static const char *f_attract_messages[] =
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

static const char *f_summary_messages[] =
{
	"\aT3 Summary of Previous Battle",
	"\aTA Rank  Player  Score  Parent",
	"\aTF  4th  ",	/* Extra information is appended for these when displayeding the summary */
	"\aTF  3rd  ",
	"\aT7  2nd  ",
	"\aTD  1st  "
};

static void OutputMessages(const char **strings, int16_t count, const char *prompt, char code, FILE *ofile);

int main(void)
{
	FILE *ofile;
	char working[MAX_MSG_LEN];

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

	/* Set up the error messages first so that we can display them if there's a subsequent problem */
	fputc('\n', stdout);
	OutputMessages(f_error_messages, sizeof(f_error_messages) / sizeof(const char *), "Error Messages", CODE_EM, ofile);
	OutputMessages(f_attract_messages, sizeof(f_attract_messages) / sizeof(const char *), "Attract Messages", CODE_AM, ofile);
	OutputMessages(f_summary_messages, sizeof(f_summary_messages) / sizeof(const char *), "Summary Messages", CODE_SM, ofile);

	/* Indicate end-of-data */
	fputc(CODE_EF, ofile);
	fclose(ofile);

	return EXIT_SUCCESS;
}

static void OutputMessages(const char **strings, int16_t count, const char *prompt, char code, FILE *ofile)
{
	int16_t t, u, length, max;

	/* Get the longest string */
	for (t = 0, max = 0; t < count; ++t) {
		length = strlen(strings[t]);
		if (max < length) max = length;
	}
	/* Add one for \0 terminator */
	++max;

	/* Output */
	fputs(prompt, stdout);
	fputc(' ', stdout);
	fputc(code, ofile);
	/* How many messages */
	fwrite(&count, sizeof(int16_t), 1, ofile);
	/* Write out the size */
	fwrite(&max, sizeof(int16_t), 1, ofile);
	/* Write out the strings */
	for (t = 0; t < count; ++t) {
		fputs(strings[t], ofile);
		for (u = strlen(strings[t]); u < max; ++u)
			fputc('\0', ofile);
	}
	fputs("done\n", stdout);
}

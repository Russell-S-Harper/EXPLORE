#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common.h"

static char *f_prompts[] =
{
	"\bT7 Welcome to PvP-AI!",
	"\bTA AI Opponents Battle for Survival",
	"\bTD You're watching them battle live!",
	"\bTF Colors mean: \bTE\xA9 \bTFabove \bT8\xA9 \bTFbelow \bT1\xA9 \bTFclose",
	"\bT3 All missiles are smart!",
	"\bT7 Both attacker and target advance",
	"\bTA Good or bad, all reach the last level",
	"\bTD At the last level a kill is permanent",
	"\bTF Press C: cycle the focus - go ahead!",
	"\bT3 Press J: join as a human",
	"\bT7 (Yours will have a \"cockpit\")",
	"\bTA Use gamepad or keyboard controls",
	"\bTD (^s to move & space to fire)",
	"\bTF Press K: be the primary target (hard)",
	"\bT3 Press L: no endgame refresh (harder!)",
	"\bT7 Go ahead - press J, K, or L",
	"\bTA Last survivor takes 1st place",
	"\bTD High score among the fallen takes 2nd",
	"\bTF These two advance!",
	"\bT3 Each generation the players refine",
	"\bT7 Then they start all over again",
	"\bTA Visit russell-harper.com for more..."
};

int main(void)
{
	FILE *ofile;
	int16_t max, t;
	uint8_t i, l;
	char c = '\0';

	if (!(ofile = fopen("pvp-ai.txt", "wb"))) {
		fputs("\nCould not create PVP-AI.TXT!\n", stderr);
		return EXIT_FAILURE;
	}

	/* Will need this */	
	l = sizeof(f_prompts) / sizeof(char *);

	/* Get the length of the longest string */
	for (i = 0, max = 0; i < l; ++i) {
		t = strlen(f_prompts[i]);
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

	for (i = 0; i < l; ++i) {
		t = strlen(f_prompts[i]);
		fwrite(f_prompts[i], sizeof(char), t, ofile);
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

// Mozkovna-revival.cpp: Definuje vstupní bod pro aplikaci.
//

#include "Mozkovna-revival.h"

#include "ctype.h"
#include "stdio.h"
#include "time.h"
#include "stdlib.h"

#include "string.h"


#include "Mozkovna-revival-funkce.h"

void ShowRules() // bude vypisovat pravidla
{
	system("cls");
	printf("Pravidla ke hre MOZKOVNA REVIVAL:\n\n");
	printf("Po vybertu \"Novy Test\" budete vyzvani k zadani jmena. Po zadani jmena si vyberete tema testu.\nPotom uz zacne samotny test.\n");
	printf("Testy se skladaji vzdy ze 6 otazek, pricemz nektere jsou s vyberem odpovedi (abcd) a nektere s otevrenou odpovedi.\n");
	printf("Pri vyberu (abcd) piste pouze prislusne pismeno (je jedno, jestli male nebo velke).\nU otevrenych odpovedi rovnez nezalezi na pouzivani velkych a malych pismen.\n\n");
	printf("Na konci testu vam bude zobrazeno vase skore. Test potom ukoncite stiskem klavesy Enter.\n\n\n");
	printf("Stiskem klavesy Enter se vratite do hlavniho menu.");


	getchar();
}

void NewTest()
{
	// zadani jmena
	char nickname[NAME_SIZE];

	system("cls");
	printf("Zadejte jmeno: ");
	scanf_s("%s", nickname, NAME_SIZE);
	while (getchar() != '\n');

	// vyber tematu
	int topicCmd;
	char chosenTopic[20];

	do
	{
		system("cls");
		printf("Vyberte tema testu:\n\n");
		printf("H: Hubda\n");
		printf("P: Priroda\n");
		printf("D: Dejiny\n\n");

		topicCmd = tolower(getchar());

		// do chosenTopic se ulozi string s nazvem tematu
		if (topicCmd != EOF) {

			switch ((char)topicCmd) {
			case 'h':
				snprintf(chosenTopic, sizeof(chosenTopic), "hudba");
				break;
			case 'p':
				snprintf(chosenTopic, sizeof(chosenTopic), "priroda");
				break;
			case 'd':
				snprintf(chosenTopic, sizeof(chosenTopic), "dejiny");
				break;
			}
		}
	} while (topicCmd != 'h' && topicCmd != 'p' && topicCmd != 'd');
	getchar();



	int questionNumbers[QUESTIONS_PER_TEST];	// tady se budou generovat nahodna cisla otazek
	GenerateQuestionNumbers(questionNumbers);	// funkce generuje vektor nahodnych cisel bez opakovani, pomoci ktereho se budou vybirat otazky

	// PlayQuestion je samotny prubeh testu, numberOfCorrectAnswers je vystupem
	int numberOfCorrectAnswers = PlayQuestions(questionNumbers, chosenTopic);

	system("cls");
	printf("KONEC TESTU\n\nSkore: %d/6", numberOfCorrectAnswers);

	// ukladani vysledku
	storeResults(chosenTopic, nickname, numberOfCorrectAnswers);

	getchar();
}



int main()
{
	char cmd;

	do
	{
		system("cls");

		// trapnej pokus o ASCII art, ale funguje to
		printf(" __  __  ____ _______  ________      ___   _              _____  ________      _________      __     _\n");
		printf("|  \\/  |/ __ \\___  / |/ / __ \\ \\    / / \\ | |   /\\       |  __ \\|  ____\\ \\    / /_   _\\ \\    / /\\   | |\n");
		printf("| \\  / | |  | | / /| ' / |  | \\ \\  / /|  \\| |  /  \\      | |__) | |__   \\ \\  / /  | |  \\ \\  / /  \\  | |\n");
		printf("| |\\/| | |  | |/ / |  <| |  | |\\ \\/ / | . ` | / /\\ \\     |  _  /|  __|   \\ \\/ /   | |   \\ \\/ / /\\ \\ | |\n");
		printf("| |  | | |__| / /__| . \\ |__| | \\  /  | |\\  |/ ____ \\    | | \\ \\| |____   \\  /   _| |_   \\  / ____ \\| |____\n");
		printf("|_|  |_|\\____/_____|_|\\_\\____/   \\/   |_| \\_/_/    \\_\\   |_|  \\_\\______|   \\/   |_____|   \\/_/    \\_\\______|\n\n");

		// hlavni menu
		printf("\nN: Novy test");
		printf("\nP: Pravidla");
		printf("\nS: Statistiky");
		printf("\nV: Vymazat ulozena skore");
		printf("\nK: Konec\n\n");

		cmd = tolower(getchar());

		while (getchar() != '\n');

		switch (cmd)
		{
		case 'n':
			NewTest();
			break;
		case 'p':
			ShowRules();
			break;
		case 's':
			system("cls");
			printf("STATISTIKY:\n\n");
			printf("Prumerny vysledek v tematu hudba: %.0f%%\n", calculateAverage("Stats\\hudbaResults.txt"));
			printf("Prumerny vysledek v tematu priroda: %.0f%%\n", calculateAverage("Stats\\prirodaResults.txt"));
			printf("Prumerny vysledek v tematu dejiny: %.0f%%\n", calculateAverage("Stats\\dejinyResults.txt"));

			printf("\n");

			printLeaderboard("Stats\\hudbaResults.txt");
			printLeaderboard("Stats\\prirodaResults.txt");
			printLeaderboard("Stats\\dejinyResults.txt");

			printf("\n");

			printf("Stiskem klavesy Enter se vratite do hlavniho menu.\n");

			getchar();
			break;
		case 'v':
			// mazani statistik
			char clearCmd;
			system("cls");
			printf("Opravdu chcete smazat vsechna ulozena skore a resetovat vysledky?\n\n");
			printf("A: Ano\n");
			printf("N: Ne\n\n");

			clearCmd = tolower(getchar());

			while (getchar() != '\n');

			switch (clearCmd) {
			case 'a':
				clearAllResultsFiles();
				system("cls");
				printf("Ulozena skore byla vymazana!\n\n");
				printf("Stiskem klavesy Enter se vratite do hlavniho menu.\n");

				getchar();
				break;
			case 'n':
				break;
			}
			break;
		}
	} while (cmd != 'k');
	return 0;
}

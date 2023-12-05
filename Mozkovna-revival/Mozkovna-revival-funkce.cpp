#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "Mozkovna-revival-funkce.h"

#define INITIAL_BUFFER_SIZE 100

// struktura, ktera reprezentuje vysledky testu. V podsatate to, co se pak uklada
typedef struct {
    char nickname[NAME_SIZE];
    int numberOfCorrectAnswers;
} QuizResult;

// tahle funkce generuje vektor nahodnych cisel bez opakovani. Cisla z vektoru se pak v GetQuestion() pouziji ke generovani cesty k .txt souboru
void GenerateQuestionNumbers(int questionNumbers[6])
{
    srand(time(NULL)); // random seed

    int count = 0;

    while (count < QUESTIONS_PER_TEST) {
        int randNum = rand() % 10;  // generovani nahodneho cisla od 0 do 9
        randNum++;					// misto od 0 do 9 je ted od 1 do 10
        bool found = false;			// na zacatku predpokladame, ze cislo jeste ve vektoru neni

        for (int i = 0; i < QUESTIONS_PER_TEST; i++) {				// tady se hleda ve vektoru, jestli uz se tam cislo nachazi
            if (questionNumbers[i] == randNum) {
                found = true;
                break;
            }

        }
        if (!found) {							// pokud neni nalezeno, je pridano do vektoru
            questionNumbers[count] = randNum;
            count++;
        }
    }
}

// funkce, ktera vraci pocet znaku na radku. Pozmenena funkce getline() z knihoven, ktera nefungovala. Dal se pouziva v GetQuestion()
int custom_getline(char** line, size_t* len, FILE* file) {
    int ch; // sem se budou ukladat znaky v souboru
    size_t i = 0; // tady to bude hlidat aktualni pozici bufferu

    // alokace pameti, pokud uz nebyla alokovana
    if (*line == NULL) {
        *line = (char*)malloc(INITIAL_BUFFER_SIZE);
        if (*line == NULL) {
            perror("Chyba v prvotni alokaci pameti v custon_getline().");
            exit(EXIT_FAILURE);
        }
        *len = INITIAL_BUFFER_SIZE;
    }

    // cteni znaku ze souboru dokud nenarazime na EOF
    while ((ch = fgetc(file)) != EOF && ch != '\n') {
        if (i == *len - 1) {
            // zmena velikosti bufferu, pokud je to potreba
            *len *= 2;
            *line = (char*)realloc(*line, *len);
            if (*line == NULL) {
                perror("Chyba v realokaci pameti v custom_getline().");
                exit(EXIT_FAILURE);
            }
        }
        (*line)[i++] = (char)ch;
    }

    // null-termination
    (*line)[i] = '\0';

    // vraci pocet prectenych znaku nebo EOF, pokud se dostane na konec souboru bez precteni jedineho znaku
    return (i > 0) ? i : EOF;
}


// tahle funkce precte soubor dany parametrem filePath a obsah rozdeli ve ";" na spravnou odpoved a otazku + alokuje pamet pro obe
void GetQuestion(const char* filePath, char** correctAnswerToken, char** questionToken) {
    // otevreni souboru
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Chyba v otevirani souboru s otazkou.");
        exit(EXIT_FAILURE);
    }

    // precteni spravne odpovedi (predpokladame, ze je na prvnim radku)
    size_t len = 0;  // delka bufferu pro radek
    char* line = NULL;

    if (custom_getline(&line, &len, file) != EOF) {
        // hledani pozice stredniku
        char* semicolonPos = strchr(line, ';');
        if (semicolonPos == NULL) {
            fprintf(stderr, "Chyba: Strednik nebyl nalezen v prvnim radku souboru s otazkou.\n");
            fclose(file);
            free(line);
            exit(EXIT_FAILURE);
        }

        // vypocet delky spravne odpovedi
        size_t correctAnswerTokenLength = semicolonPos - line;

        // alokace pameti pro spravnou odpoved
        *correctAnswerToken = (char*)malloc(correctAnswerTokenLength + 1);
        if (*correctAnswerToken == NULL) {
            perror("Chyba v alokaci pameti pro spravnou odpoved v GetQuestion().");
            fclose(file);
            free(line);
            exit(EXIT_FAILURE);
        }

        // prekopirovani spravne odpovedi
        strncpy(*correctAnswerToken, line, correctAnswerTokenLength);
        (*correctAnswerToken)[correctAnswerTokenLength] = '\0';

        // precteni zbytku souboru jako otazku
        size_t questionTokenLength = strlen(line) + 1; // +1 pro znak noveho radku
        *questionToken = (char*)malloc(questionTokenLength + 1);
        if (*questionToken == NULL) {
            perror("Chyba v alokaci pameti pro otazku v GetQuestion()");
            fclose(file);
            free(line);
            free(*correctAnswerToken);
            exit(EXIT_FAILURE);
        }

        // prekopirovani radku s otazkou, bez spravne odpovedi
        strcpy(*questionToken, semicolonPos + 1);  // preskoceni stredniku
        strcat(*questionToken, "\n");  // pridani noveho radku za otazku

        // precteni zbyvajicich radku souboru a pripojeni za otazku (pouzite pro abcd moznosti)
        while (custom_getline(&line, &len, file) != EOF) {
            // vypocet delky otazky
            questionTokenLength += strlen(line) + 1; // +1 pro znak noveho radku

            // zmena velikosti bufferu pro otazku
            *questionToken = (char*)realloc(*questionToken, questionTokenLength + 1);
            if (*questionToken == NULL) {
                perror("Chyba v realokaci pameti pro otazku v GetQuestion().");
                fclose(file);
                free(line);
                free(*correctAnswerToken);
                exit(EXIT_FAILURE);
            }

            // pridani radku do otazky
            strcat(*questionToken, line);
            strcat(*questionToken, "\n");  // \n za kazdym radkem
        }
    }

    // pokud custom_getline() vrati EOF na prvnim redku, tak hazem error
    else {
        fprintf(stderr, "Chyba cteni prvniho radku v GetQuestion().\n");
        fclose(file);
        free(line);
        exit(EXIT_FAILURE);
    }

    // uvolneni pameti radku
    free(line);

    // zavreni souboru
    fclose(file);
}

// pouzito na prevod odpovedi hrace na lowercase, funkce hleda abecedni znaky a ty prevede, ostatni necha
void makeAlphabeticalsLowercase(char* input)
{
    for (int i = 0; input[i]; i++) {
        if (isalpha(input[i])) {
            input[i] = tolower(input[i]);
        }
    }
}

// samotny prubeh otazek v testu
int PlayQuestions(int questionNumbers[QUESTIONS_PER_TEST], const char* chosenTopic)
{
    int correctQuestionCount = 0; // ze zacatku je 0 spravnych odpovedi

    char* correctAnswerToken;
    char* questionToken;

    for (int questionPos = 0; questionPos < 6; questionPos++) { // loop pres vsechna cisla ve vektoru questionNumbers

        int currentQuestionNumber = questionNumbers[questionPos];

        // vytvoreni cesty se spravnym tematem a cislem
        char* questionFilePath;
        int pathLength = snprintf(NULL, 0, "Otazky\\%s\\%s%d.txt", chosenTopic, chosenTopic, currentQuestionNumber);
        questionFilePath = (char*)malloc(pathLength + 1);  //alokace pameti pro cestu
        snprintf(questionFilePath, pathLength + 1, "Otazky\\%s\\%s%d.txt", chosenTopic, chosenTopic, currentQuestionNumber);

        GetQuestion(questionFilePath, &correctAnswerToken, &questionToken); // tady se vzdy vola nacteni obsahu ze souboru a rozdeleni na odpoved a otazku
        free(questionFilePath);

        // vypsani otazky
        system("cls");
        printf("%s\n", questionToken);
        free(questionToken);  // uvolneni pameti

        // ziskani odpovedi od hrace
        char playerAnswer[PLAYER_ANSWER_SIZE];
        scanf_s("%[^\n]", playerAnswer, PLAYER_ANSWER_SIZE);
        while (getchar() != '\n');

        // uchovavame kopii spravne odpovedi pro odhaleni pokud hrac odpovedel spatne
        char* correctAnswerReveal = (char*)malloc(strlen(correctAnswerToken) + 1); // alokace pameti
        if (correctAnswerReveal == NULL) {
            printf("Memory allocation failure.");
            exit;
        }
        strcpy(correctAnswerReveal, correctAnswerToken); // kopie

        // prevod abecednich znaku na male
        makeAlphabeticalsLowercase(playerAnswer);
        makeAlphabeticalsLowercase(correctAnswerToken); // pro porovnani s playerAnswer

        // srovnani spravne a hracem vlozene odpovedi
        if (strcmp(correctAnswerToken, playerAnswer) == 0) {
            printf("\nSpravna odpoved!");
            correctQuestionCount++; // pocet spravnych odpovedi +1
        }
        else {
            printf("\nSpatna odpoved!\n");
            printf("\n    Spravna odpoved: %s\n", correctAnswerReveal);
        }

        // uvolneni pameti
        free(correctAnswerToken);
        free(correctAnswerReveal);

        getchar();
    }
    // funkce vraci pocet spravnych odpovedi
    return correctQuestionCount;
}


// Function to store quiz results in a text file
void storeResults(char chosenTopic[], char nickname[], int numberOfCorrectAnswers) {
    // vytvoreni cesty k souboru s vysledky na zaklade zvoleneho tematu chosenTopic
    char filePath[50];
    snprintf(filePath, sizeof(filePath), "Stats/%sResults.txt", chosenTopic);

    // otevreni suboru v "append" modu
    FILE* file = fopen(filePath, "a");
    if (file == NULL) {
        fprintf(stderr, "Chyba v otevreni souboru pro zapis v storeResults(): %s\n", filePath);
        exit(EXIT_FAILURE);
    }

    // zapsani vysledku do souboru
    fprintf(file, "%s\t%d\n", nickname, numberOfCorrectAnswers);

    // zavreni souboru
    fclose(file);
}

// funkce na vypocet prumerneho skore pro dane tema
float calculateAverage(const char filePath[]) {
    // otevreni souboru v rezimu cteni
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Chyba v otevreni souboru pro cteni v calculateAverage(): %s\n", filePath);
        exit(EXIT_FAILURE);
    }

    // promenne pro ulozeni celkoveho skore a poctu vysledku
    float totalScore = 0;
    int count = 0;

    // precteni vysledku ze souboru a aktualizace totalScore a count
    while (!feof(file)) {
        char nickname[NAME_SIZE];
        int score;
        if (fscanf(file, "%s\t%d\n", nickname, &score) == 2) {
            totalScore += score;
            count++;
        }
    }

    // zavreni souboru
    fclose(file);

    // vypocet a navrat prumerneho skore v procentech
    return count > 0 ? (totalScore / (count * 6)) * 100 : 0;
}

// funkce pro vypsani leaderboardu pro dane tema. Precte vysledky ze souboru, seradi je a vypise top 3 vysledky v kazdem tematu
void printLeaderboard(const char filePath[]) {
    // kopie souborove cesty pro manipulaci se stringama
    char* filePathCopy = strdup(filePath);
    if (filePathCopy == NULL) {
        fprintf(stderr, "Chyba v kopirovani cesty v printLeaderboard().\n");
        exit(EXIT_FAILURE);
    }

    // otevreni souboru v rezimu cteni
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Chyba v otevirani souboru pro cteni v printLeaderboard(): %s\n", filePath);
        free(filePathCopy);
        exit(EXIT_FAILURE);
    }

    // definice struktury pro ulozeni vysledku testu
    typedef struct {
        char nickname[NAME_SIZE];
        int numberOfCorrectAnswers;
    } QuizResult;

    QuizResult results[100]; // predpokladame, ze neni vic nez 100 vysledku
    int count = 0;

    // precteni vysledku ze souboru a aktualizace struktury
    while (!feof(file) && count < 100) {
        if (fscanf(file, "%s\t%d\n", results[count].nickname, &results[count].numberOfCorrectAnswers) == 2) {
            count++;
        }
    }

    // zavreni souboru
    fclose(file);

    // tady se serazuji vysledky podle poctu spravnych odpovedi
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (results[i].numberOfCorrectAnswers < results[j].numberOfCorrectAnswers) {
                // prohozeni results[i] a results[j]
                QuizResult temp = results[i];
                results[i] = results[j];
                results[j] = temp;
            }
        }
    }

    // tokenizace kopie cesty k souboru. Ziska nazev tematu ze jmena souboru pro vypis leaderboardu
    char* topicStart = strstr(filePathCopy, "Stats\\");
    if (topicStart != NULL) {
        topicStart += strlen("Stats\\");

        char* topicEnd = strstr(topicStart, "Results");
        if (topicEnd != NULL) {
            *topicEnd = '\0';  // null-terminate
        }

        // vypsani leaderboardu
        printf("Leaderboard tematu %s:\n", topicStart);
        for (int i = 0; i < (count < 3 ? count : 3); i++) { // vypis top 3 vysledku
            printf("%s\t%d/6\n", results[i].nickname, results[i].numberOfCorrectAnswers);
        }
        printf("\n");
    }
    else {
        fprintf(stderr, "Chyba v extrakci nazvu tematu ze souborova cesty v printLeaderboard().\n");
    }

    // uvolneni pameti
    free(filePathCopy);
}

// funkce na vymazani vsech souboru s vysledky
void clearAllResultsFiles() {
    // array s nazvy temat. Asi to neni idealni reseni, ale funguje to (zatim)
    const char* topics[] = { "hudba", "priroda", "dejiny" };

    // loop pres kazde tema
    for (int i = 0; i < sizeof(topics) / sizeof(topics[0]); i++) {
        // vytvoreni souborove cesty pro kazde tema
        char filePath[50];
        snprintf(filePath, sizeof(filePath), "Stats\\%sResults.txt", topics[i]);

        // otevreni souboru ve "write" modu. Kdyz se tak otevre soubor, ve kterem uz neco je, tak se to neco vymaze
        FILE* file = fopen(filePath, "w");
        if (file == NULL) {
            fprintf(stderr, "Chyba v otevirani souboru pro zapis v clearAllResultsFiles(): %s\n", filePath);
            exit(EXIT_FAILURE);
        }

        // zavreni souboru
        fclose(file);
    }
}
#include "time.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

#include "Mozkovna-revival-funkce.h"

#define INITIAL_BUFFER_SIZE 100

// Structure to represent a quiz result
typedef struct {
    char nickname[NAME_SIZE];
    int numberOfCorrectAnswers;
} QuizResult;

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

// ty custon_getline a GetQuestion funkce jsou s pomoci ChatGPT
int custom_getline(char** line, size_t* len, FILE* file) {
    int ch;
    size_t i = 0;

    // Allocate memory for the line buffer if it's not already allocated
    if (*line == NULL) {
        *line = (char*)malloc(INITIAL_BUFFER_SIZE);
        if (*line == NULL) {
            perror("Error allocating memory");
            exit(EXIT_FAILURE);
        }
        *len = INITIAL_BUFFER_SIZE;
    }

    while ((ch = fgetc(file)) != EOF && ch != '\n') {
        if (i == *len - 1) {
            // Resize the buffer if needed
            *len *= 2;
            *line = (char*)realloc(*line, *len);
            if (*line == NULL) {
                perror("Error reallocating memory");
                exit(EXIT_FAILURE);
            }
        }
        (*line)[i++] = (char)ch;
    }

    // Null-terminate the line
    (*line)[i] = '\0';

    return (i > 0) ? i : EOF;
}

void GetQuestion(const char* filePath, char** correctAnswerToken, char** questionToken) {
    // Open the file
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    // Read the correct answer token (assuming it is in the first line)
    size_t len = 0;  // Length of the line buffer
    char* line = NULL;

    if (custom_getline(&line, &len, file) != EOF) {
        // Find the position of the ';' character
        char* semicolonPos = strchr(line, ';');
        if (semicolonPos == NULL) {
            fprintf(stderr, "Error: ';' not found in the first line of the file\n");
            fclose(file);
            free(line);
            exit(EXIT_FAILURE);
        }

        // Calculate the length of the correct answer token
        size_t correctAnswerTokenLength = semicolonPos - line;

        // Allocate memory for the correct answer token
        *correctAnswerToken = (char*)malloc(correctAnswerTokenLength + 1);
        if (*correctAnswerToken == NULL) {
            perror("Error allocating memory");
            fclose(file);
            free(line);
            exit(EXIT_FAILURE);
        }

        // Copy the correct answer token
        strncpy(*correctAnswerToken, line, correctAnswerTokenLength);
        (*correctAnswerToken)[correctAnswerTokenLength] = '\0';

        // Read the rest of the file as the question token
        size_t questionTokenLength = strlen(line) + 1; // +1 for the newline character
        *questionToken = (char*)malloc(questionTokenLength + 1);
        if (*questionToken == NULL) {
            perror("Error allocating memory");
            fclose(file);
            free(line);
            free(*correctAnswerToken);
            exit(EXIT_FAILURE);
        }

        // Copy the line to the question token, excluding the correct answer token
        strcpy(*questionToken, semicolonPos + 1);  // Skip the ';' character
        strcat(*questionToken, "\n");  // Add newline after the correct answer token

        // Read the remaining lines of the file and append to the question token
        while (custom_getline(&line, &len, file) != EOF) {
            // Calculate the length of the question token
            questionTokenLength += strlen(line) + 1; // +1 for the newline character

            // Resize the question token buffer
            *questionToken = (char*)realloc(*questionToken, questionTokenLength + 1);
            if (*questionToken == NULL) {
                perror("Error reallocating memory");
                fclose(file);
                free(line);
                free(*correctAnswerToken);
                exit(EXIT_FAILURE);
            }

            // Append the line to the question token
            strcat(*questionToken, line);
            strcat(*questionToken, "\n");  // Add newline after each line
        }
    }
    else {
        fprintf(stderr, "Error reading the first line of the file\n");
        fclose(file);
        free(line);
        exit(EXIT_FAILURE);
    }

    // Free the memory for the line
    free(line);

    // Close the file
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
    int correctQuestionCount = 0; // ze zacatku je 0 spravn7ch odpovedi

    char* correctAnswerToken;
    char* questionToken;

    for (int questionPos = 0; questionPos < 6; questionPos++) { // loop pres vsechna cisla ve vektoru questionNumbers

        int currentQuestionNumber = questionNumbers[questionPos];

        // vytvoreni cesty se spravnym tematem cislem
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
        strcpy(correctAnswerReveal, correctAnswerToken);

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
    return correctQuestionCount;
}


// Function to store quiz results in a text file
void storeResults(char chosenTopic[], char nickname[], int numberOfCorrectAnswers) {
    // Construct the file path based on the chosen topic
    char filePath[50];
    snprintf(filePath, sizeof(filePath), "Stats/%sResults.txt", chosenTopic);

    // Open the file in append mode
    FILE* file = fopen(filePath, "a");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for writing: %s\n", filePath);
        exit(EXIT_FAILURE);
    }

    // Write the result to the file
    fprintf(file, "%s\t%d\n", nickname, numberOfCorrectAnswers);

    // Close the file
    fclose(file);
}

// Function to calculate the average score for a given topic
float calculateAverage(const char filePath[]) {
    // Open the file in read mode
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for reading: %s\n", filePath);
        exit(EXIT_FAILURE);
    }

    // Variables to store total score and the number of results
    float totalScore = 0;
    int count = 0;

    // Read results from the file and update totalScore and count
    while (!feof(file)) {
        char nickname[NAME_SIZE];
        int score;
        if (fscanf(file, "%s\t%d\n", nickname, &score) == 2) {
            totalScore += score;
            count++;
        }
    }

    // Close the file
    fclose(file);

    // Calculate and return the average score (in percentage)
    return count > 0 ? (totalScore / (count * 6)) * 100 : 0;
}

// Function to extract the filename from a given path
const char* getFilename(const char* path) {
    const char* filename = strrchr(path, '/');
    return filename ? filename + 1 : path;
}

// Function to print the leaderboard for a given topic
void printLeaderboard(const char filePath[]) {
    // Make a copy of the file path for string manipulation
    char* filePathCopy = strdup(filePath);
    if (filePathCopy == NULL) {
        fprintf(stderr, "Error duplicating file path\n");
        exit(EXIT_FAILURE);
    }

    // Open the file in read mode
    FILE* file = fopen(filePath, "r");
    if (file == NULL) {
        fprintf(stderr, "Error opening file for reading: %s\n", filePath);
        free(filePathCopy);
        exit(EXIT_FAILURE);
    }

    // Array to store quiz results
    // Assuming there are no more than 100 results
    typedef struct {
        char nickname[NAME_SIZE];
        int numberOfCorrectAnswers;
    } QuizResult;

    QuizResult results[100];
    int count = 0;

    // Read results from the file and update the results array
    while (!feof(file) && count < 100) {
        if (fscanf(file, "%s\t%d\n", results[count].nickname, &results[count].numberOfCorrectAnswers) == 2) {
            count++;
        }
    }

    // Close the file
    fclose(file);

    // Sort the results in descending order based on the number of correct answers
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (results[i].numberOfCorrectAnswers < results[j].numberOfCorrectAnswers) {
                // Swap results[i] and results[j]
                QuizResult temp = results[i];
                results[i] = results[j];
                results[j] = temp;
            }
        }
    }

    // Tokenize the copied file path
    char* topicStart = strstr(filePathCopy, "Stats\\");
    if (topicStart != NULL) {
        topicStart += strlen("Stats\\");

        char* topicEnd = strstr(topicStart, "Results");
        if (topicEnd != NULL) {
            *topicEnd = '\0';  // Null-terminate the topic string
        }

        // Print the leaderboard
        printf("Leaderboard tematu %s:\n", topicStart);
        for (int i = 0; i < (count < 3 ? count : 3); i++) {
            printf("%s\t%d/6\n", results[i].nickname, results[i].numberOfCorrectAnswers);
        }
        printf("\n");
    }
    else {
        fprintf(stderr, "Error extracting topic from file path\n");
    }

    // Free the dynamically allocated memory
    free(filePathCopy);
}

// Function to clear all "results" text files
void clearAllResultsFiles() {
    // Array of topic names
    const char* topics[] = { "hudba", "priroda", "dejiny" };

    // Iterate through each topic
    for (int i = 0; i < sizeof(topics) / sizeof(topics[0]); i++) {
        // Construct the file path for the current topic
        char filePath[50];
        snprintf(filePath, sizeof(filePath), "Stats\\%sResults.txt", topics[i]);

        // Open the file in write mode (truncate the file)
        FILE* file = fopen(filePath, "w");
        if (file == NULL) {
            fprintf(stderr, "Error opening file for writing: %s\n", filePath);
            exit(EXIT_FAILURE);
        }

        // Close the file
        fclose(file);

        printf("Cleared %sResults.txt\n", topics[i]);
    }
}
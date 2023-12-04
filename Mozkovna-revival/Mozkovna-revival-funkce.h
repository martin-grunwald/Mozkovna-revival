#pragma once

#include "iostream"

#define NAME_SIZE 32
#define QUESTIONS_PER_TEST 6
#define PLAYER_ANSWER_SIZE 32

void GenerateQuestionNumbers(int questionNumbers[6]);
void GetQuestion(const char* filename, char** correctAnswerToken, char** questionToken);
void makeAlphabeticalsLowercase(char* input);
int PlayQuestions(int questionNumbers[QUESTIONS_PER_TEST], const char* chosenTopic);

void storeResults(char chosenTopic[], char nickname[], int numberOfCorrectAnswers);
float calculateAverage(const char filePath[]);
void printLeaderboard(const char filePath[]);
void clearAllResultsFiles();
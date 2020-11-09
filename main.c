#pragma warning (disable : 4996)
#include <stdlib.h>
#include <ctype.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#include <errno.h>
#if defined (_WIN32)
#include <io.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <dirent.h>
#include <unistd.h>
#define GetCurrentDir getcwd
#endif
#define MAX_NAME_LENGTH 256
char name[MAX_NAME_LENGTH];
char* currentDirectory = "";
int makeDirectory(char* given)
{
#if defined(_WIN32)
    return _mkdir(given);
#else
    return mkdir(given, 0700);
#endif
}
bool isSymbol(char ch)
{
    if (ch == '<' || ch == '>' || ch == '{' || ch == '}' || ch == '/' || ch == '*')
        return (true);
    return (false);
}
bool isDelimiter(char ch)
{
    if (ch == ' ' || ch == '*' || ch == '/' || ch == ';' || ch == '{' || ch == '}' || ch == '<' || ch == '>' || ch == '\n')
        return (true);
    return (false);
}
bool isValidDirection(char* str)
{
    char invalid[10] = { '.', '?', ',','!',' ',';','+','!' };
    bool isInvalid = false;
    for (int i = 1; i < strlen(str); i++)
    {
        for (int a = 0; a < sizeof(invalid); a++)
        {
            if (str[i] == invalid[a])
            {
                isInvalid = true;
                break;
            }
        }
        if (isInvalid) { break; }
    }
    if (str[0] == '0' || str[0] == '1' || str[0] == '2' ||
        str[0] == '3' || str[0] == '4' || str[0] == '5' ||
        str[0] == '6' || str[0] == '7' || str[0] == '8' ||
        str[0] == '9' || str[0] == '.' || str[0] == '?' || str[0] == ',' || str[0] == '_' || str[0] == '!' || isInvalid || str[0] == ' ' || (str[0] == '*' && strlen(str) > 1) || str[0] == ';' || str[0] == '{' || str[0] == '}' || str[0] == '<' || str[0] == '>' || str[0] == '\n')
        return (false);
    return (true);
}
bool isKeyword(char* str)
{
    if ((!strcmp(str, "if")) || (!strcmp(str, "ifnot")) || (!strcmp(str, "make")) || (!strcmp(str, "go")))
    {
        return true;
    }
    else
    {
        return false;
    }
}
char* subString(char* str, int left, int right)
{
    int i;
    char* subStr = (char*)calloc(sizeof(char)+1,(right - left + 2));

    for (i = left; i <= right; i++)
        subStr[i - left] = str[i];
    subStr[right - left + 1] = '\0';
    return (subStr);
}
bool directoryExists(char* path) {
#if defined(_WIN32)
    if (_access(path, 0) != 0)
        return false;
    return true;
#else
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return true;
    }
    return false;
#endif
}
void parse(char* str, FILE* writingFile, int* parseNumber)
{
    int left = 0, right = 0;
    int len = strlen(str);
    while (right <= len && left <= right)
    {
        if (isDelimiter(str[right]) == false)
            right++;
        if (isDelimiter(str[right]) == true && left == right)
        {
            if (str[right] == ';') { if (str[right + 1] == '\n') { right++; } }
            right++;
            left = right;
        }
        else if (isDelimiter(str[right]) == true && left != right || (right == len && left != right))
        {
            char* subStr = subString(str, left, right - 1);
            if (isKeyword(subStr) == true)//Control of keyword
            {
                if (!strcmp(subStr, "if"))
                {
                    if (str[right] == ' ') { right++; }
                    int temp = 0;
                    char* newTempCurrent = "";
                    newTempCurrent = currentDirectory;
                    if (str[right] == '<')
                    {
                        bool isFinishedString = false;
                        for (int i = right + 1; i < len; i++)
                        {
                            if (str[i] == '>')
                            {
                                temp = i + 1;
                                int len5 = i - (right + 1);
                                int charCount = 0;
                                int starCount = 0;
                                int blankCountInCharArray = 0;
                                int slashCountInCharArray = 0;
                                int slashCount = 0;
                                char* charArray1 = calloc(len5+1,sizeof(char));
                                bool onlyStarCanGo = false;//sadece yıldız olacak ve klasore erisebilecek
                                for (int j = right + 1; j < i; j++)
                                {
                                    charArray1[charCount] = str[j];
                                    if (charArray1[charCount] == '*') { starCount++; }
                                    else if (charArray1[charCount] == '/') { slashCountInCharArray++; }
                                    else if (charArray1[charCount] == " ") {
                                        blankCountInCharArray++;
                                    }
                                    charCount++;
                                }
                                if (newTempCurrent != "")
                                {

                                    for (int m = 0; m < strlen(newTempCurrent); m++)
                                    {
                                        if (newTempCurrent[m] == '/') { slashCount++; }
                                    }
                                    if (slashCountInCharArray + starCount + blankCountInCharArray == len5)
                                    {
                                        if (starCount <= slashCount - 1)
                                        {
                                            onlyStarCanGo = true;
                                        }
                                    }
                                    if (starCount > 0 && !onlyStarCanGo)
                                    {
                                        if (starCount <= slashCount - 1)
                                        {
                                            for (int z = 0; z < strlen(charArray1); z++)
                                            {
                                                if (charArray1[z] == '*')
                                                {
                                                    int slashCount2 = 0;
                                                    for (int c = 0; c < strlen(newTempCurrent); c++)
                                                    {
                                                        if (newTempCurrent[c] == '/') { slashCount2++; }
                                                        if (slashCount2 == slashCount && (newTempCurrent[c] == '/' || c == strlen(newTempCurrent) - 1))
                                                        {
                                                            if (newTempCurrent[c] == '/')
                                                            {
                                                                char* newCurrent = calloc(c+1,sizeof(char));
                                                                for (int j = 0; j < c; j++)
                                                                {
                                                                    newCurrent[j] = newTempCurrent[j];
                                                                }
                                                                newTempCurrent = newCurrent;
                                                                slashCount--;
                                                            }
                                                            else
                                                            {
                                                                newTempCurrent = "";
                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                    }
                                }
                                if (!strcmp(newTempCurrent, charArray1) || onlyStarCanGo)
                                {
                                    bool isMultipleLine = false;
                                    if (str[i] == '{' || str[i + 1] == '{' || str[i + 2] == '{')
                                    {
                                        isMultipleLine = true;
                                        bool isBracketFinished = false;
                                        int j;
                                        int startingIndex;
                                        if (str[i] == '{')
                                        {
                                            j = i + 1;
                                            startingIndex = i + 1;
                                        }
                                        else if (str[i + 1] == '{')
                                        {
                                            j = i + 2;
                                            startingIndex = i + 2;
                                        }
                                        else
                                        {
                                            j = i + 3;
                                            startingIndex = i + 3;
                                        }
                                        for (j; j < len; j++)
                                        {
                                            if (str[j] == '}')
                                            {
                                                right = j + 1;
                                                left = j + 1;
                                                isBracketFinished = true;
                                                char* commandArray = calloc(j - (startingIndex + 1)+1, sizeof(char));
                                                int commandCount = 0;
                                                for (int k = startingIndex + 1; k < j; k++)
                                                {
                                                    commandArray[commandCount] = str[k];
                                                    commandCount++;
                                                }
                                                if(onlyStarCanGo)
                                                {
                                                    parse(commandArray, writingFile, 0);

                                                }
                                                else
                                                {
                                                    parse(commandArray, writingFile, 1);
                                                }
                                            }
                                        }
                                        if (!isBracketFinished)
                                        {
                                            fputs("Missing token: }\n", writingFile);
                                        }
                                    }
                                    if (!isMultipleLine)
                                    {
                                        bool isFinished = false;
                                        bool isBracketFalse = false;
                                        for (int j = temp; j < len; j++)
                                        {
                                            if (str[j] == ';')
                                            {
                                                right = j + 1;
                                                left = j + 1;
                                                char* commandArray = calloc(j - (temp)+1,sizeof(char));
                                                int commandCount = 0;
                                                int k;
                                                if (str[temp] == '\n') { k = temp + 1; }
                                                else { k = temp; }
                                                for (k; k < j; k++)//basimiza bela olacak
                                                {
                                                    commandArray[commandCount] = str[k];
                                                    commandCount++;
                                                }
                                                for (int z = 0; z < (j - (temp)); z++)
                                                {
                                                    if (commandArray[z] == '\n' || commandArray[z] == " ")
                                                    {
                                                        isBracketFalse = true;
                                                        break;
                                                    }
                                                }
                                                if (isBracketFalse)
                                                {
                                                    fputs("Eror: Missing Brackets...\n", writingFile);
                                                    break;
                                                }
                                                else
                                                {
                                                    if(onlyStarCanGo)
                                                    {
                                                        parse(commandArray, writingFile, 0);

                                                    }
                                                    else
                                                    {
                                                        parse(commandArray, writingFile, 1);
                                                    }
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                else//failed for some other reason
                                {
                                    bool isMultipleLine = false;
                                    for (int a = i; a < len; a++)
                                    {
                                        if (str[a] == '{')
                                        {
                                            isMultipleLine = true;
                                            bool isBracketFinished = false;
                                            for (int j = a + 1; j < len; j++)
                                            {
                                                if (str[j] == '}')
                                                {
                                                    right = j + 1;
                                                    left = j + 1;
                                                    isBracketFinished = true;
                                                    break;
                                                }
                                            }
                                            if (!isBracketFinished)
                                            {
                                                fputs("Missing token: }\n", writingFile);
                                                break;
                                            }
                                        }
                                    }
                                }
                                isFinishedString = true;
                                break;
                            }
                        }
                        if (!isFinishedString) { fputs("MissingToken >", writingFile); }
                    }
                }
                else if (!strcmp(subStr, "ifnot"))
                {
                    if (str[right] == ' ') { right++; }
                    int temp = 0;
                    char* newTempCurrent = "";
                    newTempCurrent = currentDirectory;
                    if (str[right] == '<')
                    {
                        bool isFinishedString = false;
                        for (int i = right + 1; i < len; i++)
                        {
                            if (str[i] == '>')
                            {
                                temp = i + 1;
                                int len5 = i - (right + 1);
                                int charCount = 0;
                                int starCount = 0;
                                int blankCountInCharArray = 0;
                                int slashCountInCharArray = 0;
                                int slashCount = 0;
                                char* charArray1 = calloc(len5+1, sizeof(char));
                                bool onlyStarCanGo = false;//sadece yıldız olacak ve klasore erisebilecek
                                for (int j = right + 1; j < i; j++)
                                {
                                    charArray1[charCount] = str[j];
                                    if (charArray1[charCount] == '*') { starCount++; }
                                    else if (charArray1[charCount] == '/') { slashCountInCharArray++; }
                                    else if (charArray1[charCount] == " ") {
                                        blankCountInCharArray++;
                                    }
                                    charCount++;
                                }
                                if (newTempCurrent != "")
                                {
                                    printf("%s", newTempCurrent);
                                    for (int m = 0; m < strlen(newTempCurrent); m++)
                                    {
                                        if (newTempCurrent[m] == '/') { slashCount++; }
                                    }
                                    if (slashCountInCharArray + starCount + blankCountInCharArray == len5)
                                    {
                                        if (starCount <= slashCount - 1)
                                        {
                                            onlyStarCanGo = true;
                                        }
                                    }
                                    if (starCount > 0 && !onlyStarCanGo)
                                    {
                                        if (starCount <= slashCount - 1)
                                        {
                                            for (int z = 0; z < strlen(charArray1); z++)
                                            {
                                                if (charArray1[z] == '*')
                                                {
                                                    int slashCount2 = 0;
                                                    for (int c = 0; c < strlen(newTempCurrent); c++)
                                                    {
                                                        if (newTempCurrent[c] == '/') { slashCount2++; }
                                                        if (slashCount2 == slashCount && (newTempCurrent[c] == '/' || c == strlen(newTempCurrent) - 1))
                                                        {
                                                            if (newTempCurrent[c] == '/')
                                                            {
                                                                char* newCurrent = calloc(c+1,sizeof(char));
                                                                for (int j = 0; j < c; j++)
                                                                {
                                                                    newCurrent[j] = newTempCurrent[j];
                                                                }
                                                                newTempCurrent = newCurrent;
                                                                slashCount--;
                                                            }
                                                            else { newTempCurrent = ""; }
                                                        }
                                                    }
                                                }
                                            }
                                        }

                                    }
                                }
                                if (onlyStarCanGo == true) { onlyStarCanGo = false; }
                                else { onlyStarCanGo = true; }
                                if (strcmp(newTempCurrent, charArray1) || onlyStarCanGo)
                                {
                                    bool isMultipleLine = false;
                                    if (str[i] == '{' || str[i + 1] == '{' || str[i + 2] == '{')
                                    {
                                        isMultipleLine = true;
                                        bool isBracketFinished = false;
                                        int j;
                                        int startingIndex;
                                        if (str[i] == '{')
                                        {
                                            j = i + 1;
                                            startingIndex = i + 1;
                                        }
                                        else if (str[i + 1] == '{')
                                        {
                                            j = i + 2;
                                            startingIndex = i + 2;
                                        }
                                        else
                                        {
                                            j = i + 3;
                                            startingIndex = i + 3;
                                        }
                                        for (j; j < len; j++)
                                        {
                                            if (str[j] == '}')
                                            {
                                                right = j + 1;
                                                left = j + 1;
                                                isBracketFinished = true;
                                                char* commandArray = calloc(j - (startingIndex + 1)+1,sizeof(char));
                                                int commandCount = 0;
                                                for (int k = startingIndex + 1; k < j; k++)
                                                {
                                                    commandArray[commandCount] = str[k];
                                                    commandCount++;
                                                }
                                                parse(commandArray, writingFile, 1);
                                            }
                                        }
                                        if (!isBracketFinished)
                                        {
                                            fputs("Missing token: }\n", writingFile);
                                        }
                                    }
                                    if (!isMultipleLine)
                                    {
                                        bool isFinished = false;
                                        bool isBracketFalse = false;
                                        for (int j = temp; j < len; j++)
                                        {
                                            if (str[j] == ';')
                                            {
                                                right = j + 1;
                                                left = j + 1;
                                                char* commandArray = calloc(j - (temp)+1,sizeof(char));
                                                int commandCount = 0;
                                                int k;
                                                if (str[temp] == '\n') { k = temp + 1; }
                                                else { k = temp; }
                                                for (k; k < j; k++)//basimiza bela olacak
                                                {
                                                    commandArray[commandCount] = str[k];
                                                    commandCount++;
                                                }
                                                for (int z = 0; z < (j - (temp)); z++)
                                                {
                                                    if (commandArray[z] == '\n' || commandArray[z] == " ")
                                                    {
                                                        isBracketFalse = true;
                                                        break;
                                                    }
                                                }
                                                if (isBracketFalse)
                                                {
                                                    fputs("Eror: Missing Brackets...\n", writingFile);
                                                    break;
                                                }
                                                else
                                                {
                                                    parse(commandArray, writingFile, 1);
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                                else//Error failed for some other reason
                                {
                                    bool isMultipleLine = false;
                                    for (int a = i; a < len; a++)
                                    {
                                        if (str[a] == '{')
                                        {
                                            isMultipleLine = true;
                                            bool isBracketFinished = false;
                                            for (int j = a + 1; j < len; j++)
                                            {
                                                if (str[j] == '}')
                                                {
                                                    right = j + 1;
                                                    left = j + 1;
                                                    isBracketFinished = true;
                                                    break;
                                                }
                                            }
                                            if (!isBracketFinished)
                                            {
                                                fputs("Missing token: }\n", writingFile);
                                                break;
                                            }
                                        }
                                    }
                                }
                                isFinishedString = true;
                                break;
                            }
                        }
                        if (!isFinishedString) { fputs("MissingToken >", writingFile); }
                    }
                }
                else if (!strcmp(subStr, "make"))
                {
                    if (str[right] == ' ') { right++; }
                    char* newTempCurrent2 = currentDirectory;
                    if (str[right] == '<')
                    {
                        bool isVerif = true;
                        bool isFinishedString = false;
                        for (int i = right + 1; i < len; i++)
                        {
                            if (str[i] == '>')
                            {
                                int len5 = 0;
                                if (newTempCurrent2 != "") { len5 = i - (right + 1) + strlen(newTempCurrent2) + 1; }
                                else { len5 = i - (right + 1); }
                                int charCount = 0;
                                char* charArraymake = calloc(len5 + 1, sizeof(char));
                                char* temp_arr = calloc(len5 + 1, sizeof(char));
                                if (newTempCurrent2 != "")
                                {
                                    int starCount = 0;
                                    int slashCount = 0;
                                    for (int j = right + 1; j < i; j++)
                                    {
                                        if (isupper(str[j])) { charArraymake[charCount] = tolower(str[j]); }
                                        else { charArraymake[charCount] = str[j]; }
                                        if (charArraymake[charCount] == '*') { starCount++; }
                                        charCount++;
                                    }
                                    int tempForValid = 0;
                                    int t;
                                    int lenVerif = 0;
                                    for (int p = 0; p < strlen(charArraymake); p++)
                                    {
                                        if (charArraymake[p] == '/' || p == strlen(charArraymake) - 1)
                                        {
                                            if (p == strlen(charArraymake) - 1)
                                            {
                                                t = p + 1;
                                                lenVerif = t - tempForValid;
                                            }
                                            else
                                            {
                                                t = p;
                                                lenVerif = p;
                                            }
                                            char* verifControl = calloc(lenVerif+1,sizeof(char));
                                            int verifControlCount = 0;
                                            for (int f = tempForValid; f < t; f++) { verifControl[verifControlCount++] = charArraymake[f]; }
                                            tempForValid = p + 1;
                                            if (!isValidDirection(verifControl))
                                            {
                                                isVerif = false;
                                                fputs("Invalid directory name (", writingFile);
                                                fputs(verifControl, writingFile);
                                                fputs(")\n", writingFile);
                                                break;
                                            }
                                        }
                                    }
                                    for (int a = 0; a < strlen(newTempCurrent2); a++) { if (newTempCurrent2[a] == '/') { slashCount++; } }
                                    if (starCount > 0)
                                    {
                                        if (starCount <= slashCount + 1)
                                        {
                                            for (int a = 0; a < charCount; a++)
                                            {
                                                if (charArraymake[a] == '*')
                                                {
                                                    int slashCount2 = 0;
                                                    for (int c = 0; c < strlen(newTempCurrent2); c++)
                                                    {
                                                        if (newTempCurrent2[c] == '/') { slashCount2++; }
                                                        if (slashCount2 == slashCount && (newTempCurrent2[c] == '/' || c == strlen(newTempCurrent2) - 1))
                                                        {
                                                            if (newTempCurrent2[c] == '/')
                                                            {
                                                                char* newCurrent = calloc(c+1,sizeof(char));
                                                                for (int j = 0; j < c; j++) { newCurrent[j] = newTempCurrent2[j]; }
                                                                newTempCurrent2 = newCurrent;
                                                                slashCount--;
                                                            }
                                                            else { newTempCurrent2 = ""; }
                                                        }
                                                    }
                                                }
                                            }
                                            char* tempCharArray = charArraymake;
                                            char* newString = "";
                                            for (int z = 0; z < strlen(tempCharArray); z++)
                                            {
                                                if (tempCharArray[z] == '*')
                                                {
                                                    char* newCharArray = calloc(strlen(tempCharArray) - (z + 2)+1,sizeof(char));
                                                    int newCharCount = 0;
                                                    for (int m = z + 2; m < strlen(tempCharArray); m++)
                                                    {
                                                        newCharArray[newCharCount] = tempCharArray[m];
                                                        newCharCount++;
                                                    }
                                                    newString = newCharArray;
                                                }
                                            }
                                            charArraymake = newString;
                                        }
                                    }
                                    char* lastDirection = calloc(strlen(newTempCurrent2) + strlen(charArraymake) + 2 ,sizeof(char));
                                    int lastDirectionCount = 0;
                                    for (int a = 0; a < strlen(newTempCurrent2); a++)
                                    {
                                        lastDirection[lastDirectionCount] = newTempCurrent2[a];
                                        lastDirectionCount++;
                                    }
                                    if (strlen((newTempCurrent2)) != 0)
                                    {
                                        lastDirection[lastDirectionCount++] = '/';
                                    }
                                    for (int j = 0; j < strlen(charArraymake); j++)
                                    {
                                        lastDirection[lastDirectionCount] = charArraymake[j];
                                        lastDirectionCount++;
                                    }
                                    charArraymake = lastDirection;
                                    charCount = strlen(newTempCurrent2) + strlen(charArraymake) + 1;
                                }
                                else
                                {
                                    for (int j = right + 1; j < i; j++)
                                    {
                                        charArraymake[charCount] = str[j];
                                        charCount++;
                                    }
                                }
                                if(isVerif)
                                {

                                    for (int c = 0; c < strlen(newTempCurrent2); c++)
                                    {
                                        temp_arr[c] = newTempCurrent2[c];
                                    }
                                    temp_arr[strlen(temp_arr)] = '/';
                                    for (int temp_arrCount = strlen(temp_arr); temp_arrCount < strlen(charArraymake); temp_arrCount++)
                                    {
                                        if (charArraymake[temp_arrCount] == '/')
                                        {
                                            int makeDirectoryIntMake = makeDirectory(temp_arr);
                                            temp_arr[temp_arrCount] = charArraymake[temp_arrCount];
                                        }
                                        else if (temp_arrCount == strlen(charArraymake) - 1)
                                        {
                                            temp_arr[temp_arrCount] = charArraymake[temp_arrCount];
                                            int makeDirectoryIntMake = makeDirectory(temp_arr);
                                            if (makeDirectoryIntMake == -1)
                                            {
                                                fputs("ERROR: This directory has already been created (", writingFile);
                                                fputs(temp_arr, writingFile);
                                                fputs(")\n", writingFile);
                                                break;
                                            }
                                        }
                                        else
                                        {
                                            temp_arr[temp_arrCount] = charArraymake[temp_arrCount];
                                        }
                                    }
                                }
                                right = i;
                                left = i;
                                isFinishedString = true;
                                break;
                            }
                        }
                        if (!isFinishedString) { fputs("MissingToken >", writingFile); }
                    }
                }
                else if (!strcmp(subStr, "go"))
                {
                    if (str[right] == ' ') { right++; }
                    if (str[right] == '<')
                    {
                        bool isFinishedString = false;
                        for (int i = right + 1; i < len; i++)
                        {
                            if (str[i] == '>')
                            {
                                int len5 = i - (right + 1);
                                int charCount = 0;
                                int starCount = 0;
                                int slashCount = 0;
                                char* charArray = calloc(len5+1,sizeof(char));
                                for (int j = right + 1; j < i; j++)
                                {
                                    charArray[charCount] = str[j];
                                    if (charArray[charCount] == '*') { starCount++; }
                                    charCount++;
                                }
                                if (currentDirectory == "")
                                {
                                    if (starCount > 0) {
                                        printf("%s", "ERROR");
                                    }//ERROR
                                   // DIR* dir = opendir(charArray);
                                    if (directoryExists(charArray)){
                                        currentDirectory = charArray;
                                    }
                                    else if (ENOENT == errno) {
                                        fputs("Directory is not created...", writingFile);
                                    }
                                }
                                else
                                {
                                    if (parseNumber == 1)
                                    {
                                        for (int m = 0; m < strlen(currentDirectory); m++) { if (currentDirectory[m] == '/') { slashCount++; } }
                                        if (starCount > 0)
                                        {
                                            if (starCount <= slashCount + 1)
                                            {
                                                for (int z = 0; z < strlen(charArray); z++)
                                                {
                                                    if (charArray[z] == '*')
                                                    {
                                                        int slashCount2 = 0;
                                                        for (int c = 0; c < strlen(currentDirectory); c++)
                                                        {
                                                            if (currentDirectory[c] == '/') { slashCount2++; }
                                                            if (slashCount2 == slashCount && (currentDirectory[c] == '/' || c == strlen(currentDirectory) - 1))
                                                            {
                                                                if (currentDirectory[c] == '/')
                                                                {
                                                                    char* newCurrent = calloc(c+1,sizeof(char));
                                                                    for (int j = 0; j < c; j++)
                                                                    {
                                                                        newCurrent[j] = currentDirectory[j];
                                                                    }
                                                                    currentDirectory = newCurrent;
                                                                    slashCount--;
                                                                }
                                                                else
                                                                {
                                                                    currentDirectory = "";//BURAYI UNUTMA
                                                                }

                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else { fputs("Program is trying to reach a file which is not created...", writingFile); }
                                        }
                                        else
                                        {
                                            if (starCount > 0) { printf("%s", "ERROR"); }//ERROR
                                            int totalLenght = strlen(charArray) + strlen(currentDirectory) + 1;
                                            char* newCurrentForIf = calloc(totalLenght+1,sizeof(char));
                                            int chararrayCount = 0;
                                            for (int m = 0; m < totalLenght; m++)
                                            {
                                                if (m < strlen(currentDirectory))
                                                {
                                                    newCurrentForIf[m] = currentDirectory[m];
                                                }
                                                else if (m == strlen(currentDirectory))
                                                {
                                                    newCurrentForIf[m] = '/';
                                                }
                                                else if (strlen(currentDirectory) < m && m < totalLenght)
                                                {
                                                    newCurrentForIf[m] = charArray[chararrayCount];
                                                    chararrayCount++;
                                                }

                                            }
                                            //DIR* dir = opendir(newCurrentForIf);
                                            if (directoryExists(newCurrentForIf)) { currentDirectory = newCurrentForIf; }
                                            else if (ENOENT == errno) { fputs("Directory is not created...", writingFile); }
                                            printf("%s\n", currentDirectory);
                                        }
                                    }
                                    else
                                    {
                                        for (int m = 0; m < strlen(currentDirectory); m++) { if (currentDirectory[m] == '/') { slashCount++; } }
                                        if (starCount > 0)
                                        {
                                            if (starCount <= slashCount + 1)
                                            {
                                                for (int z = 0; z < strlen(charArray); z++)
                                                {
                                                    if (charArray[z] == '*')
                                                    {
                                                        int slashCount2 = 0;
                                                        for (int c = 0; c < strlen(currentDirectory); c++)
                                                        {
                                                            if (currentDirectory[c] == '/') { slashCount2++; }
                                                            if (slashCount2 == slashCount && (currentDirectory[c] == '/' || c == strlen(currentDirectory) - 1))
                                                            {
                                                                if (currentDirectory[c] == '/')
                                                                {
                                                                    char* newCurrent = calloc(c+1,sizeof(char));
                                                                    for (int j = 0; j < c; j++)
                                                                    {
                                                                        newCurrent[j] = currentDirectory[j];
                                                                    }
                                                                    currentDirectory = newCurrent;
                                                                    slashCount--;
                                                                }
                                                                else
                                                                {
                                                                    currentDirectory = "";
                                                                }

                                                            }
                                                        }
                                                    }
                                                }
                                            }
                                            else { fputs("Program is trying to reach a file which is not created...", writingFile); }
                                        }
                                        else
                                        {
                                            if (starCount > 0) { printf("%s", "ERROR"); }//ERROR
                                            //DIR* dir = opendir(charArray);
                                            if (directoryExists(charArray)) { currentDirectory = charArray; }
                                            else if (ENOENT == errno) { fputs("Directory is not created...", writingFile); }
                                        }
                                    }
                                }
                                right = i;
                                left = i;
                                isFinishedString = true;
                                break;
                            }
                        }
                        if (!isFinishedString) { fputs("MissingToken >", writingFile); }
                    }
                }
            }
            else if (isValidDirection(subStr) == false && isDelimiter(str[right - 1]) == false)
            {
                fputs("Error: NonValidIdentifier(", writingFile);
                fputs(subStr, writingFile);
                fputs(")\n", writingFile);
            }
            left = right;
        }
    }
    return;
}
int main(void)
{
    currentDirectory = GetCurrentDir(NULL, 0);
    if (currentDirectory == NULL)
    {
        printf("Failed to get current directory.");
    }
    else { for (int i = 0; i < (int)strlen(currentDirectory); i++) { if (currentDirectory[i] == '\\') { currentDirectory[i] = '/'; } } }
    FILE* dosya;
    FILE* dosya2;
    printf("********************************************************************************\n");
    printf("You are in : ");
    printf("%s\n", currentDirectory);
    printf("********************************************************************************");
    printf("\nPlease write the name of file that you  want to be read : ");
    scanf("%s", name);
    int c;
    char file[1000];
    if ((dosya = fopen(name, "r")) == NULL)
        printf("dosya okumada hata var");
    else {
        int i = 0;
        while ((c = fgetc(dosya)) != EOF)
        {
            file[i] = c;

            i++;
        }
        file[i] = '\0';
        fclose(dosya);
    }
    dosya2 = fopen("code.lex", "w");
    parse(file, dosya2, 0);
    printf("%s", currentDirectory);

    fclose(dosya2);
    return 0;
}

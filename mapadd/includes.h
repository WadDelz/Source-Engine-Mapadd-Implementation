#include <string>

//for anyone like themaster974 who sees this. in the future (soon) I will implement a function where
//if you have a keyvalue with an int float or bool's name as its value i will make it so you can write
//the name of the bool float or int anywhere in the string and not just the entire string/keyvalue 
//being the int float or bool's name. so instead of me
//having the string having to be "val" (just pretend I have an int named val) I could have the string be
//"the players health is val" or something else where the entire string isn't just the name of the value
//I will also make probably a DynamicArray and store the array indexes of the bools floats and ints so multiple
//ints floats and bools can be in a keyvalue. I will also add for the actual int and float function I will
//also add a function that makes it so if you write like "player health" for the ints value then the ints value
//would be the players health.

#define DYNAMICINT(DoThis) 	\
int valnam = 99999999; \
int valnamfloat = 99999999; \
int valnamebool = 99999999; \
for (int i = 0; i < intNames.getSize(); i++) \
{ \
	if (!Q_strcmp(value->GetString(), intNames[i])) \
	{ \
		valnam = i; \
		break; \
	} \
} \
for (int i = 0; i < floatNames.getSize(); i++) \
{ \
	if (!Q_strcmp(value->GetString(), floatNames[i])) \
	{ \
			valnamfloat = i; \
			break; \
	} \
} \
for (int i = 0; i < boolNames.getSize(); i++) \
{ \
	if (!Q_strcmp(value->GetString(), boolNames[i])) \
	{ \
			valnamebool = i; \
			break; \
	} \
} \
if (valnamfloat != 99999999) \
{ \
	char intValChar[FILENAME_MAX]; \
	Q_snprintf(intValChar, sizeof(intValChar), "%f", floatValues[valnamfloat]); \
	DoThis; \
} \
else if (valnam != 99999999) \
{ \
	char intValChar[FILENAME_MAX]; \
	Q_snprintf(intValChar, sizeof(intValChar), "%d", intValues[valnam]); \
	DoThis; \
} \
else if (valnamebool != 99999999) \
{ \
	std::string str = std::to_string(boolValues[valnamebool]); \
	const char* intValChar = str.c_str(); \
	DoThis; \
} 

#define PARSELABEL \
	char entities[FILENAME_MAX]; \
	Q_snprintf(entities, sizeof(entities), "entities:%s", value->GetString()); \
	KeyValues* labelnameabs = mapaddFile->FindKey(entities); \
	if (labelnameabs) \
	{ \
		ParseEntities(labelnameabs); \
		break; \
	} \

std::string getFirstWord(const char* input);
std::string getSecondWord(const char* input);
std::string removeNonIntCharacters(const char* input);
std::string removeNonFloatCharacters(const char* input);
std::string removeFunctionCharacters(const char* input);
bool IsBool(const char* str);
bool IsNumber(const char* str);
bool IsInt(const char* str);
bool hasTwoWords(const char* str);
bool hasBrackets(const char* input);
bool StartsWith(const char* word, const char* find);
int binaryToInt(int binaryInt);
//i will add more to this file in the future

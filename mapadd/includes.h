#include <string>

//for anyone who sees this. in the future (soon) I will implement a function where
//if you have a keyvalue with an int float or bool's name as its value i will make it so you can write
//the name of the bool float or int anywhere in the string and not just the entire string/keyvalue 
//being the int float or bool's name. so instead of me
//having the string having to be "val" (just pretend I have an int named val) I could have the string be
//"the players health is val" or something else where the entire string isn't just the name of the value
//I will also make probably a DynamicArray and store the array indexes of the bools floats and ints so multiple
//ints floats and bools can be in a keyvalue. I will also add for the actual int and float function I will
//also add a function that makes it so if you write like "player health" for the ints value then the ints value
//would be the players health.

//Edit this is the code with that above implemented. you can also have multiple bools floats or ints in a 
//key's value

#define DYNAMICINT(DoThis) 	\
/*this allows for " to be used in strings as ^ */ \
const char* str_nonfunc = ReplaceWordBrackets(value->GetString(), "^", "\""); \
DynamicArray<int> intNamVal;\
DynamicArray<int> floatNamVal;\
DynamicArray<int> boolNamVal; \
DynamicArray<int> stringNamVal; \
for (int i = 0; i < intNames.getSize(); i++)\
{\
	if (HasWord(value->GetString(), intNames[i]))\
	{\
		intNamVal.addElement(i);\
	}\
}\
for (int i = 0; i < floatNames.getSize(); i++)\
{\
	if (HasWord(value->GetString(), floatNames[i]))\
	{\
		floatNamVal.addElement(i);\
	}\
}\
for (int i = 0; i < boolNames.getSize(); i++)\
{\
	if (HasWord(value->GetString(), boolNames[i]))\
	{\
		boolNamVal.addElement(i);\
	}\
}\
for (int i = 0; i < stringNames.getSize(); i++)\
{\
	if (HasWord(value->GetString(), stringNames[i]))\
	{\
		stringNamVal.addElement(i);\
	}\
}\
if (intNamVal.getSize() != 0 || floatNamVal.getSize() != 0 || boolNamVal.getSize() != 0 || stringNamVal.getSize() != 0)\
{\
	const char* absstr = value->GetString();\
	for (int i = 0; i < intNamVal.getSize(); i++)\
	{\
		char add[FILENAME_MAX];\
		Q_snprintf(add, sizeof(add), "%d", intValues[intNamVal[i]]);\
		absstr = ReplaceWord(absstr, intNames[intNamVal[i]], add);\
	}\
	for (int i = 0; i < floatNamVal.getSize(); i++)\
	{\
		char add[FILENAME_MAX];\
		Q_snprintf(add, sizeof(add), "%f", floatValues[floatNamVal[i]]);\
		absstr = ReplaceWord(absstr, floatNames[floatNamVal[i]], add);\
	}\
	for (int i = 0; i < stringNamVal.getSize(); i++)\
	{\
		absstr = ReplaceWord(absstr, stringNames[stringNamVal[i]], stringValues[stringNamVal[i]]); \
	}\
	for (int i = 0; i < boolNamVal.getSize(); i++)\
	{\
		std::string add = std::to_string(boolValues[boolNamVal[i]]); \
		absstr = ReplaceWord(absstr, boolNames[boolNamVal[i]], add.c_str());\
	}\
	absstr = ReplaceWordBrackets(absstr, "^", "\""); \
	DoThis;\
} \
	else\

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
bool HasWord(const char* text, const char* word);
int binaryToInt(int binaryInt);
const char* ReplaceWord(const char* text, const char* wordToReplace, const char* replacementWord);
const char* ReplaceWordBrackets(const char* text, const char* wordToReplace, const char* replacementWord);
//i will add more to this file in the future

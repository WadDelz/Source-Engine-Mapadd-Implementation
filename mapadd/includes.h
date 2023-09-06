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
bool hasTwoWords(const char* str);
bool IsNumber(const char* str);
bool IsBool(const char* str);
bool StartsWith(const char* word, const char* find);
//i will add more to this file in the future
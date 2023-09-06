#include "cbase.h"	
#include <iostream>
#include <string>
#include "includes.h"

std::string getFirstWord(const char* input) {
	std::string result;

	// Iterate through the characters in the input until a space or null character is encountered
	while (*input != '\0' && *input != ' ') {
		result += *input;
		++input;
	}

	return result;
}

std::string getSecondWord(const char* input) {
	std::string result;
	int wordCount = 0;

	// Skip leading spaces
	while (*input != '\0' && *input == ' ') {
		++input;
	}

	// Iterate through the characters in the input
	while (*input != '\0') {
		if (*input == ' ') {
			++wordCount;
			// Check if this is the second word
			if (wordCount == 2) {
				// Skip spaces after the first word
				while (*input != '\0' && *input == ' ') {
					++input;
				}
				// Extract the second word
				while (*input != '\0' && *input != ' ') {
					result += *input;
					++input;
				}
				break;
			}
		}
		else {
			++input;
		}
	}

	return result;
}

std::string removeNonIntCharacters(const char* input) {
	std::string result;

	for (int i = 0; input[i] != '\0'; ++i) {
		if (input[i] >= '0' && input[i] <= '9') {
			result += input[i];
		}
	}

	return result;
}

std::string removeNonFloatCharacters(const char* input) {
	std::string result;

	for (int i = 0; input[i] != '\0'; ++i) {
		if ((input[i] >= '0' && input[i] <= '9') || input[i] == '.') {
			result += input[i];
		}
	}

	return result;
}

std::string removeFunctionCharacters(const char* input) {
	std::string result;

	for (int i = 0; input[i] != '\0'; ++i) {
		if (input[i] != '<' && input[i] != '>' && input[i] != '=' && input[i] != '!')   {
			result += input[i];
		}
	}

	return result;
}

bool StartsWith(const char* word, const char* find)
{
	const char* entPrefix = find;
	int i = 0;

	while (entPrefix[i] != '\0')
	{
		if (entPrefix[i] != word[i])
		{
			return false;
		}
		i++;
	}

	return true;
}

bool hasTwoWords(const char* str) {
	// Initialize a word count to 0
	int wordCount = 0;

	// Iterate through the characters in the string
	for (int i = 0; str[i] != '\0'; i++) {
		// Check if the current character is not a space or a tab
		if (str[i] != ' ' && str[i] != '\t') {
			// If the previous character was a space or the beginning of the string,
			// increment the word count
			if (i == 0 || str[i - 1] == ' ' || str[i - 1] == '\t') {
				wordCount++;
			}
		}
	}

	// Check if the word count is exactly 2
	return wordCount == 2;
}

bool IsNumber(const char* str) {
	if (str == nullptr || *str == '\0') {
		// Handle empty string or null pointer
		return false;
	}

	bool hasDigit = false;
	bool hasSign = false;

	for (const char* p = str; *p != '\0'; ++p) {
		if (!isdigit(*p)) {
			// Check for allowed characters '<', '>', '=', and '!'
			if (*p != '<' && *p != '>' && *p != '=' && *p != '!' && *p != '.') {
				return false; // Invalid character
			}
		}
		else {
			hasDigit = true;
		}
		if ((p == str && (*p == '+' || *p == '-')) || (p != str && (*p == '+' || *p == '-'))) {
			hasSign = true;
		}
	}

	// Ensure there's at least one digit in the string
	return hasDigit || hasSign;
}

bool IsBool(const char* str) {
	if (str == nullptr || *str == '\0') {
		return false;
	}

	for (const char* p = str; *p != '\0'; ++p) {
		if (!Q_strcmp(str, "true") || !Q_strcmp(str, "false"))
			return true;
	}

	return false;
}
#include <iostream>
#include <fstream>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <cctype>
#include <set>
#include <limits> //Correction
#include <execution>
#include <unordered_map>

// Function to check if a string is a number
bool isNumber(const std::string& s) {
    return !s.empty() && std::all_of(s.begin(), s.end(), ::isdigit);
}

// Function to calculate the value of a word based on the provided letter values
int calculateWordValue(std::string word, std::map<char, int>& letterValues) {
    int value = 0;  // Initialize the word value to 0

    // Iterate through each character in the input word
    for (char c : word) {
        c = std::tolower(c); // Convert the character to lowercase

        // Check if the lowercase character is present in the letterValues map
        if (letterValues.find(c) != letterValues.end()) {
            // If the character is present, add its value to the total word value
            value += letterValues[c];
        }
    }

    return value;  // Return the calculated word value
}

// Function to calculate Levenshtein distance between two words with memoization
int levenshteinDistance(const std::string& s1, const std::string& s2, std::vector<std::vector<int>>& memo) {
    const size_t len1 = s1.size(), len2 = s2.size();

    if (memo[len1][len2] != -1) {
        return memo[len1][len2];
    }

    if (len1 == 0) {
        memo[len1][len2] = len2;
    } else if (len2 == 0) {
        memo[len1][len2] = len1;
    } else {
        int substitutionCost = (s1[len1 - 1] == s2[len2 - 1]) ? 0 : 1;

        memo[len1][len2] = std::min({
            levenshteinDistance(s1.substr(0, len1 - 1), s2, memo) + 1,
            levenshteinDistance(s1, s2.substr(0, len2 - 1), memo) + 1,
            levenshteinDistance(s1.substr(0, len1 - 1), s2.substr(0, len2 - 1), memo) + substitutionCost
        });
    }

    return memo[len1][len2];
}

// Function to find the most similar word from a list with memoization
std::string findMostSimilarWord(const std::string& target, const std::vector<std::string>& wordList) {
    std::cout << target << std::endl;
    // Handle if the word list is empty
    if (wordList.empty()) {
        return "";
    }

    std::string mostSimilarWord;  // Variable to store the most similar word
    int minDistance = std::numeric_limits<int>::max();  // Initialize the minimum Levenshtein distance to the maximum possible value
    
    std::vector<std::vector<int>> memo(target.size() + 1, std::vector<int>(target.size() + 1, -1));

    // Iterate through each word in the word list
    for (const auto& word : wordList) {
        int distance = levenshteinDistance(target, word, memo);  // Calculate the Levenshtein distance between the target word and the current word

        // If the current word has a smaller Levenshtein distance, update the minimum distance and the most similar word
        if (distance < minDistance) {
            minDistance = distance;
            mostSimilarWord = word;
        }
    }
    std::cout << mostSimilarWord << std::endl;

    return mostSimilarWord;  // Return the most similar word found in the word list
}



int main() {
    // Open the input file
    std::ifstream inputFile("input_words - Main.txt");

    // Check if the file is open
    if (!inputFile.is_open()) {
        std::cerr << "Error opening the file!" << std::endl;
        return 1;
    }

    // Read the entire content of the file into a single string
    std::string fileContent((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());

    // Vectors to store words, numbers, and mixed words
    std::vector<std::string> words, numbers, mixedWords;

    // Create an istringstream to tokenize the file content
    std::istringstream buffer(fileContent);
    std::string token;

    // Tokenize the content of the file
    while (buffer >> token) {
        // Categorize the token based on whether it's a number, mixed word, or regular word
        if (isNumber(token)) {
            numbers.emplace_back(std::move(token));
        } else if (std::any_of(token.begin(), token.end(), ::isdigit)) {
            mixedWords.emplace_back(std::move(token));
        } else {
            words.emplace_back(std::move(token));
        }
    }
    inputFile.close();

    std::cout << "Read from Input Words" << std::endl;
    // Read letter values from the values file
    std::ifstream valuesFile("letter_values.txt");
    std::map<char, int> letterValues;
    std::string line;
    while (std::getline(valuesFile, line)) {
        std::istringstream iss(line);
        char letter;
        char equalSign;
        int value;
        if (iss >> letter >> equalSign >> value) {
            if (equalSign == '=') {
                letterValues[std::tolower(letter)] = value; // Convert letter to lowercase
                letterValues[std::toupper(letter)] = value; // Add the uppercase equivalent
            }
        }
    }
    valuesFile.close();
    std::cout << "Read from ValueFile Words" << std::endl;


    // Calculate the values of the words and store unique values
    std::map<int, std::vector<std::string>> wordValueMap;
    std::map<std::string, int> wordOccurrenceMap;
    for (const auto& w : words) {
        int value = calculateWordValue(w, letterValues);
        wordValueMap[value].push_back(w);
        wordOccurrenceMap[w]++;
    }
    std::cout << "Calculate Values" << std::endl;

    // Write the numbers and corresponding words to the output file
    std::ofstream uniqueSortedOutputFile("output_sorted_words_with_values.txt");
    for (const auto& entry : wordValueMap) {
        uniqueSortedOutputFile << entry.first << " - ";
        for (const auto& word : entry.second) {
            uniqueSortedOutputFile << word << " ";
        }
        uniqueSortedOutputFile << "\n";
    }
    uniqueSortedOutputFile.close();
    std::cout << "Write the numbers and corresponding words to the output file" << std::endl;

    // Count occurrences of each number
    std::map<std::string, int> numberOccurrences;
    for (const auto& num : numbers) {
        if (numberOccurrences.find(num) != numberOccurrences.end()) {
            numberOccurrences[num]++;
        }
        else {
            numberOccurrences[num] = 1;
        }
    }
    std::cout << "Count occurrences of each number" << std::endl;

    // Sort numbers based on occurrences in descending order
    std::vector<std::pair<std::string, int>> sortedNumberOccurrences(numberOccurrences.begin(), numberOccurrences.end());
    std::sort(sortedNumberOccurrences.begin(), sortedNumberOccurrences.end(),
        [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
            return a.second > b.second;
        });

    std::cout << "Sort numbers based on occurrences in descending order" << std::endl;


    // Write the sorted numbers with occurrences to the output file
    std::ofstream numbersFile("output_sorted_numbers.txt");
    for (const auto& entry : sortedNumberOccurrences) {
        numbersFile << entry.first << " - Occurrences: " << entry.second << "\n";
    }
    numbersFile.close();
    std::cout << "Write the sorted numbers with occurrences to the output file" << std::endl;

    // Find and correct mixed words
    std::ofstream correctedMixedWordsFile("output_corrected_mixed_words.txt");

    std::for_each(std::execution::par, mixedWords.begin(), mixedWords.end(), [&](const auto& mixedWord) {
        std::string correctedWord = findMostSimilarWord(mixedWord, words);
        correctedMixedWordsFile << mixedWord << " - " << correctedWord << "\n";
    });

    correctedMixedWordsFile.close();
    std::cout << "Finished" << std::endl;

    return 0;
}

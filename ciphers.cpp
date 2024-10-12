#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& sentence) {
  double bestEnglishScore = scoreString(scorer, sentence);
  vector<char> bestCipher = genRandomSubstCipher();

  for (int outer = 0; outer < 20; outer++) {
    vector<char> cipher = genRandomSubstCipher();
    string outerSentence = applySubstCipher(cipher, sentence);
    double currEnglishScore = scoreString(scorer, outerSentence);

    int test = 0;
    while (test < 1500) {
      vector<char> testCipher = cipher;
      // get EnglishNessScore

      int randInt1 = Random::randInt(25);
      int randInt2 = Random::randInt(25);

      while (randInt1 == randInt2) {
        randInt1 = Random::randInt(25);
        randInt2 = Random::randInt(25);
      }

      // swap
      char temp = cipher[randInt1];
      testCipher[randInt1] = testCipher[randInt2];
      testCipher[randInt2] = temp;

      string newSentence = applySubstCipher(testCipher, sentence);
      double newEnglishScore = scoreString(scorer, newSentence);

      if (newEnglishScore > currEnglishScore) {
        test = 0;
        cipher = testCipher;
        currEnglishScore = newEnglishScore;
      } else {
        test++;
      }
    }
    if (bestEnglishScore < currEnglishScore) {
      bestCipher = cipher;
      bestEnglishScore = currEnglishScore;
    }
  }
  return bestCipher;
}

/**
 * Print instructions for using the program.
 */
void printMenu() {

  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "F - Decrypt Substitution Cipher from File" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

int main() {
  Random::seed(time(NULL));
  string command;
  ifstream dictFile;
  ifstream quadFile;

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;

    if (command == "C" || command == "c") {
      runCaesarEncrypt();
    }

    if (command == "D" || command == "d") {
      dictFile.open("dictionary.txt");
      vector<string> dict;
      string word;

      while (!dictFile.eof()) {
        getline(dictFile, word);
        dict.push_back(word);
      }

      runCaesarDecrypt(dict);
      dictFile.close();
    }

    if (command == "A" || command == "a") {
      applyRandSubstCipherCommand();
    }

    if (command == "e" || command == "E") {
      quadFile.open("english_quadgrams.txt");
      vector<string> quadgrams;
      vector<int> counts;

      string word;
      string quadword;
      int quadNum;

      while (!quadFile.eof()) {
        getline(quadFile, word);
        int commaPos = word.find(",");
        if (commaPos != string::npos) {
          quadword = word.substr(0, commaPos);
          quadNum = stoi(word.substr(commaPos + 1, word.size() - 1));
          quadgrams.push_back(quadword);
          counts.push_back(quadNum);
        }
      }

      QuadgramScorer scorer(quadgrams, counts);
      computeEnglishnessCommand(scorer);
      quadFile.close();
    }
    if (command == "s" || command == "S") {
      quadFile.open("english_quadgrams.txt");
      vector<string> quadgrams;
      vector<int> counts;

      string word;
      string quadword;
      int quadNum;

      while (!quadFile.eof()) {
        getline(quadFile, word);
        int commaPos = word.find(",");
        if (commaPos != string::npos) {
          quadword = word.substr(0, commaPos);
          quadNum = stoi(word.substr(commaPos + 1, word.size() - 1));
          quadgrams.push_back(quadword);
          counts.push_back(quadNum);
        }
      }

      QuadgramScorer scorer(quadgrams, counts);

      decryptSubstCipherCommand(scorer);

      quadFile.close();
    }

    if (command == "F" || command == "f") {
      quadFile.open("english_quadgrams.txt");
      vector<string> quadgrams;
      vector<int> counts;

      string word;
      string quadword;
      int quadNum;

      while (!quadFile.eof()) {
        getline(quadFile, word);
        int commaPos = word.find(",");
        if (commaPos != string::npos) {
          quadword = word.substr(0, commaPos);
          quadNum = stoi(word.substr(commaPos + 1, word.size() - 1));
          quadgrams.push_back(quadword);
          counts.push_back(quadNum);
        }
      }

      QuadgramScorer scorer(quadgrams, counts);

      string fileName;
      string fileNameTo;
      cout << "Enter filename with text to substitution-cipher decrypt: ";
      getline(cin, fileName);
      // cout << endl;
      cout << "Enter filename to write results to: ";
      getline(cin, fileNameTo);
      cout << endl;
      ofstream newFile(fileNameTo);
      ifstream cipherFile(fileName);

      string totalSentence = "";
      string sentence;
      vector<string> finalText;
      while (!cipherFile.eof()) {
        getline(cipherFile, sentence);
        totalSentence += sentence;
        finalText.push_back(sentence);
      }
      // cout << "before run" << endl;
      string cleanSentence = clean(totalSentence);
      // cout << cleanSentence << endl;
      vector<char> bestCipher = decryptSubstCipher(scorer, cleanSentence);
      // cout << "running" << endl;
      for (int t = 0; t < finalText.size(); t++) {
        if (t == finalText.size() - 1) {
          newFile << applySubstCipher(bestCipher, finalText[t]);
        } else {
          finalText[t] += "\n";
          newFile << applySubstCipher(bestCipher, finalText[t]);
        }
      }

      newFile.close();
      cipherFile.close();
      quadFile.close();
    }

    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc

char rot(char c, int amount) {
  // Translates c character to a number
  int numChar = ALPHABET.find(c);

  // Encrypts the letter by incrementing the number of rotations
  char newCharacter = ALPHABET[(numChar + amount) % 26];

  return newCharacter;
}

string rot(const string& line, int amount) {
  char upperChar;
  string newLine = "";

  // for loop to go through each letter in the sentence
  for (size_t i = 0; i < line.size(); i++) {
    upperChar = toupper(line[i]);
    if (isspace(upperChar)) {
      newLine.push_back(' ');
    } else if (ALPHABET.find(upperChar) == -1) {
    } else {
      newLine.push_back(rot(upperChar, amount));
    }
  }
  return newLine;
}

void runCaesarEncrypt() {
  string sentence;
  int rotateNum;

  cout << "\nEnter the text to Caesar encrypt: " << endl;
  getline(cin, sentence);

  cout << "\nEnter the number of characters to rotate by: " << endl;
  cin >> rotateNum;

  cout << rot(sentence, rotateNum) << endl;

  cout << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec

void rot(vector<string>& sentence, int amount) {
  for (size_t word = 0; word < sentence.size(); word++) {
    for (size_t letter = 0; letter < sentence[word].size(); letter++) {
      int numChar = ALPHABET.find(sentence[word][letter]);
      char newCharacter = ALPHABET[(numChar + amount) % 26];
      sentence[word][letter] = newCharacter;
      // cout << newCharacter;
    }
    // cout << sentence[word] << " ";
  }
  // cout << endl;
}

// CLEAN IS AN ISSUE WITH PUSHING SPACES
string clean(const string& s) {
  // make each letter upper
  string upperLine;

  for (size_t i = 0; i < s.size(); i++) {
    if ((ALPHABET.find(toupper(s[i])) != -1)) {
      upperLine.push_back(toupper(s[i]));
    }
  }

  return upperLine;
}

vector<string> splitBySpaces(const string& line) {
  // Enter line to output vector of sentence
  vector<string> sentence;
  string word = "";

  for (size_t i = 0; i < line.size(); i++) {
    // char upperChar = toupper(line[i]);

    if ((i + 1 == line.size()) && (ALPHABET.find(line[i]) != -1)) {
      // cout << upperChar;
      word.push_back(line[i]);
      sentence.push_back(word);
      // cout << line[i] << " ";
    } else if (!isspace(line[i]) && ((ALPHABET.find(line[i]) != -1))) {
      word.push_back(line[i]);
      // cout << line[i];
    } else if (isspace(line[i])) {
      sentence.push_back(word);
      word = "";
      // cout << " ";
    }
  }

  vector<string> newSentence;

  for (size_t word = 0; word < sentence.size(); word++) {
    if (sentence[word] != "") {
      newSentence.push_back(sentence[word]);
    }
  }

  return newSentence;
}

string joinWithSpaces(const vector<string>& sentence) {
  // End function

  string stringSentence = "";
  for (int word = 0; word < sentence.size(); word++) {
    for (int letter = 0; letter < sentence[word].size(); letter++) {
      stringSentence.push_back(sentence[word][letter]);
    }
    if (word + 1 != sentence.size()) {
      stringSentence.push_back(' ');
    }
  }
  return stringSentence;
}

int numWordsIn(const vector<string>& sentence, const vector<string>& dict) {
  int englishWords = 0;

  for (size_t l = 0; l < sentence.size(); l++) {
    // cout << sentence[0] << "word" << endl;

    for (size_t o = 0; o < dict.size(); o++) {
      if (sentence[l] == dict[o]) {
        englishWords++;
      }
    }
  }

  return englishWords;
}

void runCaesarDecrypt(const vector<string>& dict) {
  string line;
  int decryptCount = 0;

  cout << "Enter the text to Caesar decrypt:" << endl;
  getline(cin, line);

  string dummy = clean(line);

  string upperLine;

  for (size_t i = 0; i < line.size(); i++) {
    if ((ALPHABET.find(toupper(line[i])) != -1) || isspace(toupper(line[i]))) {
      upperLine.push_back(toupper(line[i]));
    }
  }
  line = upperLine;

  for (size_t letterRotate = 0; letterRotate < 26; letterRotate++) {
    vector<string> sentence = splitBySpaces(line);

    rot(sentence, letterRotate);

    if (numWordsIn(sentence, dict) > (sentence.size() / 2)) {
      cout << joinWithSpaces(sentence) << endl;
      decryptCount++;
    }
  }
  if (decryptCount == 0) {
    cout << "No good decryptions found" << endl;
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc

string applySubstCipher(const vector<char>& cipher, const string& s) {
  string newSentence = "";

  for (size_t letter = 0; letter < s.size(); letter++) {
    char upperLetter = toupper(s[letter]);

    if ((ALPHABET.find(upperLetter) != -1)) {
      newSentence.push_back(cipher[ALPHABET.find(upperLetter)]);
    } else if ((ALPHABET.find(upperLetter) == -1)) {
      newSentence.push_back(s[letter]);
    } else if (isspace(s[letter])) {
      newSentence.push_back(' ');
    }
  }

  return newSentence;
}

void applyRandSubstCipherCommand() {
  string userInput;
  cout << "Enter the text to substitution-cipher encrypt: ";
  getline(cin, userInput);
  cout << endl;
  vector<char> cipher = genRandomSubstCipher();

  cout << applySubstCipher(cipher, userInput) << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec

double scoreString(const QuadgramScorer& scorer, const string& s) {
  double totalSum = 0.0;
  clean(s);
  for (int sentenceIndex = 0; sentenceIndex < s.size(); sentenceIndex++) {
    if (ALPHABET.find(s[sentenceIndex + 3]) != -1) {
      string fourLetters = "";

      int currentIndex = sentenceIndex;
      for (int i = sentenceIndex; i < (currentIndex + 4); i++) {
        fourLetters.push_back(s[i]);
      }
      // if (fourLetters.size() == 4) {
      totalSum += scorer.getScore(fourLetters);
      // }
    }
  }
  return totalSum;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  //   Runs the command to score the "Englishness" of input text. Prompts from
  //   the
  //  * console input (cin) once to get text. Outputs the Englishness score as
  //  * determined by `scorer`.
  string sentence;
  cout << "Enter a string to score: " << endl;
  getline(cin, sentence);

  sentence = clean(sentence);
  // scoreString(fourLetters)

  cout << scoreString(scorer, sentence) << endl;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  string sentence;
  cout << "Enter text to substitution-cipher decrypt: " << endl;
  getline(cin, sentence);

  string cleanSentence = clean(sentence);
  vector<char> bestCipher = decryptSubstCipher(scorer, cleanSentence);

  cout << applySubstCipher(bestCipher, sentence);
}

#pragma endregion SubstDec

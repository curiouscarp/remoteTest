// File name: WordCount.cpp
// Author: Zachary Gruber
// Userid: gruberzd
// Email: zachary.d.gruber@vanderbilt.edu
// Class: cs2201
// Assignment Number: project 8
// Honor Statement: i adhere to it
// Description: computes vector score for two texts
// Last Changed: 11/11/2019


#include <cstdlib>
#include <cmath>
#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>


const double MINIMUM_APPEARANCE = 0.001;
const int STEM_LENGTH = 6;

//DO NOT CHANGE THESE
typedef std::set<std::string> WordSet;
typedef std::map<std::string, int> WordCountMap;


// function prototypes
// you are not allowed to change these function headers,
// but you can add helper functions as needed.
WordSet readCommonWords(const std::string &fname);

WordCountMap processFile(const std::string &fname, const WordSet &common);

double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname);

std::string adjust(std::string word);

double totalWords(WordCountMap map);

/* main */
int main() {
    std::string commonFile = "texts/common.txt";
    std::string fin1, fin2, fout, str;

    do {
        std::cout << "Enter name of the first input file: ";
        std::cin >> fin1;
        fin1 = "texts/" + fin1;

        std::cout << "Enter name of the second input file: ";
        std::cin >> fin2;
        fin2 = "texts/" + fin2;

        std::cout << "Enter name of the output file: ";
        std::cin >> fout;
        fout = "texts/" + fout;

        WordSet common = readCommonWords(commonFile);

        WordCountMap first = processFile(fin1, common);
        WordCountMap second = processFile(fin2, common);

        double dist = compareTexts(first, second, fout);

        std::cout << "Vector Distance: " << dist << std::endl;

        std::cout << "Would you like to run the program again (Y|N): ";
        std::cin >> str;
    } while (tolower(str[0]) == 'y');


    return 0;
}

/* readCommonWords
 * This method reads words from a given file and places them
 * into a WordSet, which it returns.
 *
 * pre:  a valid file name with 1 word per line, words all in lower case
 * post: all words int the file are placed in the WordSet
 */
WordSet readCommonWords(const std::string &fname) {
    WordSet ret;

    std::string str;
    std::ifstream file(fname.c_str());
    // Do error checking here
    if (!file) {
        std::cout << "Unable to open file. Press enter to exit program.";
        getline(std::cin, str);  // consume existing line
        std::cin.get();  // get the key press
        exit(1);
    }
    std::string words;
    while(file >> words){
        ret.insert(words);
    }
    file.close();
    return ret;
}


/* processFile
 * This function reads in all words from the given file
 * after reading a word it converts it to lower case,
 * removes non alphabetic characters and stems it to STEM_LENGTH.
 * If the resulting word is considered common it is ignored.
 * Otherwise, the count in the map that matches the word
 * is incremented.
 *
 * pre:  the name of a text file and a set of words to be ignored.
 * post: The file has been read; a map of cleansed words is created and returned
 *
 */
WordCountMap processFile(const std::string &fname, const WordSet &common) {
    WordCountMap ret;

    std::string str;
    std::ifstream file(fname.c_str());
    // Do error checking here
    if (!file) {
        std::cout << "Unable to open file. Press enter to exit program.";
        getline(std::cin, str);  // consume existing line
        std::cin.get();  // get the key pressk
        exit(1);
    }
    std::string word;
    while(file >> word){
        //function call to convert letter to lowercase and
        // remove punctuation and nonalphabetic characters
        word = adjust(word);
        //stem word to 6 characters if longer than 6 characters
        if(word.size() > STEM_LENGTH){
            word = word.substr(0,STEM_LENGTH);
        }
        //increment count on map if not common and not empty
        if(common.find(word) == common.end() && !word.empty()){
            ret[word]++;
        }
    }
    file.close();
    return ret;
}


/* compareTexts
 * Compares the count maps of 2 texts.
 * The result returned is the euclidean distance
 * where each word with a percentile appearance greater than
 * MINIMUM_APPEARANCE counts as a dimesion of the vectors.
 * (they still count toward the full sum used to calculate
 *  percentile appearance)
 *
 * This method also prints out the results to a file using the following
 * format (start and end tags added for clarity):
 * / -- start example output --/
 * word1:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * word2:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 * ...
 * wordN:	distance = x.xxxxxe-xxx	(score1 = y.yyyyyyyy, score2 = z.zzzzzzz)
 *
 * Counted words in document 1 = xxxx
 * Counted words in document 2 = xxxx
 *
 * Vector Distance: dist
 * /-- end example output --/
 *
 * word1-wordN are all words whose comparison is include in the final sum.
 * distance values are the square of the difference of the percentile
 *                         scores from the two maps.
 * score1 & score2 values are the scores from the two maps.
 * dist is the final distance that is also returned,
 *    it is the square root of the sum of all the individual distances.
 * To help line things up, there is a tab character after the ":" and another
 *    before the "(" on the line for each word.
 *
 * pre:  two maps of texts to be compared and the name of an output file
 * post: returns the euclidean distance of the two files
 *       if the output file cannot be opened, the method prints
 *       an error mesage and exits the program.
 */
double compareTexts(WordCountMap &first,
                    WordCountMap &second,
                    const std::string &fname) {
    //opens output file
    std::ofstream outfile;
    outfile.open(fname.c_str());
    if (outfile.fail()) {
        std::cout << "Error opening output data file" << std::endl;
        std::cout << "press enter to exit";
        std::cin.get();
        std::cin.get();
        exit(1);
    }

    double vectorDistance = 0.0;
    double distance;
    //stores total amount of words in first map
    double countFirst = totalWords(first);
    //stores total amount of words in second map
    double countSecond = totalWords(second);

    WordCountMap::iterator p;
    //computes scores for a given word that is in both maps (with sufficient appearance)
    for(p = first.begin(); p!= first.end(); ++p){
        if(second.count(p->first) == 1 && (p->second / countFirst > MINIMUM_APPEARANCE)
        && (second[p->first] / countSecond > MINIMUM_APPEARANCE)){
            distance = (((p->second / countFirst) - (second[p->first] / countSecond)) *
                        ((p->second / countFirst) - (second[p->first] / countSecond)));
            outfile << (p->first) << ":\tdistance = " << distance << "\t(score1 = "
            << p->second / countFirst << ", score2= " <<
            (second[p->first] / countSecond) <<std::endl;
            vectorDistance += distance;
        }
    }
    outfile << std::endl;
    outfile << "Counted words in document 1 = " << countFirst << std::endl;
    outfile << "Counted words in document 2 = " << countSecond << std::endl;
    outfile << std::endl;
    outfile << "Vector Distance: " << sqrt(vectorDistance);

    //closes output file
    outfile.close();
    return sqrt(vectorDistance);
}

std::string adjust(std::string word){
    for (int i=0, len = word.size(); i < len; ++i){
        if(ispunct(word[i]) || isdigit(word[i])){
            word.erase(i--,1);
            len = word.size();
        }
        word[i] = std::tolower(word[i]);
    }
    return word;
}

double totalWords(WordCountMap map){
    double count = 0.0;
    WordCountMap::iterator p;
    for(p = map.begin(); p != map.end(); ++p){
        count += p->second;
    }
    return count;
}


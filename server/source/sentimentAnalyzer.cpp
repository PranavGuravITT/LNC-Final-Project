
#include "sentimentAnalyzer.h"
#include <sstream>
#include <cctype>

SentimentAnalyzer::SentimentAnalyzer()
{
    positiveWords = {"good", "great", "excellent", "amazing", "delicious", "tasty", "love", 
        "awesome", "fantastic", "wonderful", "superb", "outstanding", "perfect", 
        "pleasant", "enjoyable", "satisfying", "impressive", "incredible", 
        "marvelous", "fabulous", "positive", "beautiful", "happy", "joyful", 
        "cheerful", "brilliant", "charming", "graceful", "refreshing", "radiant"};

    negativeWords = {"bad", "terrible", "awful", "disgusting", "horrible", "poor", "hate", 
        "disappointing", "unpleasant", "horrific", "nasty", "dreadful", "atrocious", 
        "abysmal", "unbearable", "unacceptable", "distasteful", "unappealing", 
        "miserable", "sad", "unhappy", "ugly", "horrendous", "disgusted", 
        "annoyed", "angry", "frustrated", "boring", "dull", "tedious"};
}

double SentimentAnalyzer::analyze(const std::string &text)
{
    int positiveCount = 0;
    int negativeCount = 0;

    std::istringstream stream(text);
    std::string word;
    while (stream >> word)
    {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);

        if (positiveWords.find(word) != positiveWords.end())
        {
            positiveCount++;
        }
        else if (negativeWords.find(word) != negativeWords.end())
        {
            negativeCount++;
        }
    }

    int totalCount = positiveCount + negativeCount;
    if (totalCount == 0)
    {
        return 1.0;
    }
    else
    {
        return 1.0 + (static_cast<double>(positiveCount - negativeCount) / totalCount);
    }
}

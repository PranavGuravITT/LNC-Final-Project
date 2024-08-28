#ifndef SENTIMENT_ANALYZER_H
#define SENTIMENT_ANALYZER_H

#include <string>
#include <unordered_set>
#include <algorithm>

class SentimentAnalyzer
{
private:
    static std::unordered_set<std::string> positiveWords;
    static std::unordered_set<std::string> negativeWords;

public:
    SentimentAnalyzer();
    static double analyze(const std::string &text);
};

#endif

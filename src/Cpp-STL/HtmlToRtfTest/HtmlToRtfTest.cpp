// HtmlToRtfTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <string>
#include <algorithm>
#include <memory>
#include <regex>

using namespace std;

void ConvertToRtf(wstring& msg, wstring htmlStartTag, wstring htmlStopTag, wstring rtfStartTag, wstring rtfStopTag)
{
    // Convert tags to lowercase
    transform(htmlStartTag.begin(), htmlStartTag.end(), htmlStartTag.begin(), ::tolower);
    transform(htmlStopTag.begin(), htmlStopTag.end(), htmlStopTag.begin(), ::tolower);

    // Remove whitespaces if any
    std::wstring::iterator end_pos = std::remove(htmlStartTag.begin(), htmlStartTag.end(), ' ');
    htmlStartTag.erase(end_pos, htmlStartTag.end());
    end_pos = std::remove(htmlStopTag.begin(), htmlStopTag.end(), ' ');
    htmlStopTag.erase(end_pos, htmlStopTag.end());

    wstring tag;
    for (int itr = 1; itr < htmlStartTag.size(); ++itr) {
        if ('>' == htmlStartTag[itr]) {
            break;
        }
        tag += htmlStartTag[itr];
    }

    // Include spaces in regex
    htmlStartTag.insert(1, L"\\s*(");
    transform(tag.begin(), tag.end(), tag.begin(), ::toupper);
    htmlStartTag.insert(htmlStartTag.size() - 1, wstring(L"|") + tag + wstring(L")") + wstring(L"\\s*"));

    htmlStopTag.insert(1, L"\\s*");
    htmlStopTag.insert(5, L"\\s*(");
    htmlStopTag.insert(htmlStopTag.size() - 1, wstring(L"|") + tag + wstring(L")") + wstring(L"\\s*"));

    // Create regex from the tags and perform regex search and replace the matches with rtf Tags
    std::wregex tagStart_Regex(htmlStartTag, std::wregex::icase);
    std::wregex tagStop_Regex(htmlStopTag, std::wregex::icase);

    while (regex_search(msg, tagStart_Regex)) {
        msg = std::regex_replace(msg, tagStart_Regex, rtfStartTag + wstring(L" "));
    }

    while (regex_search(msg, tagStop_Regex)) {
        msg = std::regex_replace(msg, tagStop_Regex, rtfStopTag + wstring(L" "));
    }
}
void ConvertToRtf(string& msg, string htmlStartTag, string htmlStopTag, string rtfStartTag, string rtfStopTag)
{
    // Convert tags to lowercase
    transform(htmlStartTag.begin(), htmlStartTag.end(), htmlStartTag.begin(), ::tolower);
    transform(htmlStopTag.begin(), htmlStopTag.end(), htmlStopTag.begin(), ::tolower);

    // Remove whitespaces if any
    std::string::iterator end_pos = std::remove(htmlStartTag.begin(), htmlStartTag.end(), ' ');
    htmlStartTag.erase(end_pos, htmlStartTag.end());
    end_pos = std::remove(htmlStopTag.begin(), htmlStopTag.end(), ' ');
    htmlStopTag.erase(end_pos, htmlStopTag.end());

    string tag;
    for (int itr = 1; itr < htmlStartTag.size(); ++itr) {
        if ('>' == htmlStartTag[itr]) {
            break;
        }
        tag += htmlStartTag[itr];
    }

    // Include spaces in regex
    htmlStartTag.insert(1, "\\s*(");
    transform(tag.begin(), tag.end(), tag.begin(), ::toupper);
    htmlStartTag.insert(htmlStartTag.size() - 1, string("|") + tag + string(")") + string("\\s*"));

    htmlStopTag.insert(1, "\\s*");
    htmlStopTag.insert(5, "\\s*(");
    htmlStopTag.insert(htmlStopTag.size() - 1, string("|") + tag + string(")") + string("\\s*"));

    // Create regex from the tags and perform regex search and replace the matches with rtf Tags
    std::regex tagStart_Regex(htmlStartTag, std::wregex::icase);
    std::regex tagStop_Regex(htmlStopTag, std::wregex::icase);

    while (regex_search(msg, tagStart_Regex)) {
        msg = std::regex_replace(msg, tagStart_Regex, rtfStartTag + string(" "));
    }

    while (regex_search(msg, tagStop_Regex)) {
        msg = std::regex_replace(msg, tagStop_Regex, rtfStopTag + string(" "));
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        printf("Please input a string as argument! embrace it with double quote!!\n");
        return -1;
    }

    std::string msg = argv[1];

    ConvertToRtf(msg, "<u>", "</u>", "\\u", "\\ul0");
    ConvertToRtf(msg, "<b>", "</b>", "\\b", "\\b0");
    ConvertToRtf(msg, "<p>", "</p>", "\\par", "\\par0");
    ConvertToRtf(msg, "<i>", "</i>", "\\i", "\\i0");
    ConvertToRtf(msg, "<ul>", "</ul>", "\\par", "\\par0");
    ConvertToRtf(msg, "<li>", "</li>", "(\Line    \bullet    )", "( )");
    ConvertToRtf(msg, "&lt;", "&gt;", "<", ">");

    getchar();

    //ConvertToRtf(msg, L"<u>", L"</u>", L"\\ul", L"\\ul0");
    //ConvertToRtf(msg, L"<b>", L"</b>", L"\\b", L"\\b0");
    //ConvertToRtf(msg, L"<p>", L"</p>", L"\\par", L"\\par0");
    //ConvertToRtf(msg, L"<i>", L"</i>", L"\\i", L"\\i0");
    //ConvertToRtf(msg, L"<ul>", L"</ul>", L"\\par", L"\\par0");
    //ConvertToRtf(msg, L"<li>", L"</li>", LR"(\Line    \bullet    )", LR"( )");
    //ConvertToRtf(msg, L"&lt;", L"&gt;", L"<", L">");

    //htmlEmptyTagToRtf(message, L"<br>", L"\\Line");
    //newLineToRtf(message);
    //maskHyperlink(message);


}

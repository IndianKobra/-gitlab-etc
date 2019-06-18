//g++ sortstation.cpp --std=c++11
#include<iostream>
#include<vector>
#include<set>
#include<map>
#include<stack>
#include<cmath>
#include <stdlib.h>
using namespace std;

set<char> Numbers{'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '.'};//char Numbers[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};
set<char> Parentheses{'(', ')'};//c++11standarts
set<char> Operators{'+', '-', '*', '/', '^', '%'};
set<char> Letters{'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'g', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z'};
set<string> UnPrefOpers{"+", "-"};
set<string> UnPostfOpers{};
map<string, short> Opers{{"+", 3}, { "-", 3}, {"*", 5}, { "/", 5}, {"^", 8}, {"**", 8}, {"+pre", 9}, { "-pre", 9}};//priorete % 2 - leftassosiativna
set<string> Funcs{"sin", "cos"};

void ExitWithError(string S)
{
    cout << S << endl;
    exit(0);
}

vector<string> SplitOnTokens(string &S)
{
    vector<string> Answer;
    for(int i = 0; i < S.size(); i++)
    {
        int b = i;//begin
        if(Numbers.count(S[b]))   while(i+1 < S.size() && (Letters.count(S[i+1])||Numbers.count(S[i+1]))) i++;
        else if(Letters.count(S[b]))   while(i+1 < S.size() && (Numbers.count(S[i+1])||Letters.count(S[i+1]))) i++;
        else if(Operators.count(S[b])) while(i+1 < S.size() && Operators.count(S[i+1])) i++;
        else if(Parentheses.count(S[b])) b = b;
        else continue;
        Answer.push_back(S.substr(b, i-b+1));
    }
    return Answer;
}
vector<string> SortStation(vector<string>& S)
{
    vector<string> Answer;
    stack<string> Sta;
    for(int i = 0; i < S.size(); i++)
    {
        if(S[i] == ")")
        {
            while(!Sta.empty()&&(Sta.top()!="("))/* !Parentheses.count((Sta.top())[0])*/
            {
                Answer.push_back(Sta.top());
                Sta.pop();
            }
            if(Sta.empty()) ExitWithError("wrong parentheses");
            Sta.pop();
        }
        else if(S[i] == "(") Sta.push(S[i]);
        else if(UnPrefOpers.count(S[i]) && (i==0 || S[i-1] == "(" || Operators.count(S[i-1][0]) || Funcs.count(S[i-1]))) Sta.push(S[i]+"pre"); //предполагае, что у унарных опрераторов приоретет выше
        else if(UnPostfOpers.count(S[i]) && (i==S.size()-1 || S[i+1] == ")" || Operators.count(S[i+1][0]) || Funcs.count(S[i+1]))) Sta.push(S[i]+"post");
        else if(Opers.count(S[i]))
        {
            while(!Sta.empty() && Opers.count(Sta.top()) && (Opers[S[i]] < Opers[Sta.top()] || (Opers[S[i]] == Opers[Sta.top()] && Opers[S[i]] % 2)))
            {
                Answer.push_back(Sta.top());
                Sta.pop();
            }
            Sta.push(S[i]);
        }
        else if(Funcs.count(S[i])) Sta.push(S[i]);
        else if(Numbers.count(S[i][0])) Answer.push_back(S[i]);
        else ExitWithError("Unknow name:"+S[i]);//!! error
    }
    while(!Sta.empty())
    {
        Answer.push_back(Sta.top());
        Sta.pop();
    }
    return Answer;
}
int Str2Int(string S)
{
    int Ans = 0;
    for(int i = 0; i < S.size(); i++)
    {
        if(S[i] >= '0' && S[i] <= '9') Ans = Ans*10+S[i]-'0';
    }
    return Ans;
}
float Str2float(string S)
{
    int i;
    float Ans = 0, k = 1;
    for(i = 0; i < S.size(); i++)
    {
        if(S[i] >= '0' && S[i] <= '9') Ans = Ans*10+S[i]-'0';
        if(S[i] == '.' || S[i] == 'e') break;
    }
    if(S[i] == '.')
    {
        for(i = i + 1; i < S.size(); i++)
        {
            k/=10;
            Ans += (S[i]-'0')*k;
        }
    }
    if(S[i] == 'e')
    {
        i = Str2Int(S.substr(i+1));
        if(i > 0) for(i = i; i > 0; i--) Ans *= 10;
        if(i < 0) for(i = i; i < 0; i++) Ans /= 10;
    }
    return Ans;
}

float calc(vector<string>& Polish)
{
    stack<float> S;
    for(int i = 0; i < Polish.size(); i++)
    {
        float a, b;
        if(Numbers.count(Polish[i][0])) S.push(Str2float(Polish[i]));
        if(Opers.count(Polish[i]) && Operators.count(Polish[i][Polish[i].size()-1]))
        {
            if(S.empty()) ExitWithError("no args for:" + Polish[i]);
            a = S.top();
            S.pop();
            if(S.empty()) ExitWithError("no args for:" + Polish[i]);
            b = S.top();
            S.pop();
        }
        else if(Opers.count(Polish[i]) || Funcs.count(Polish[i]))
        {
            if(S.empty()) ExitWithError("no args for:" + Polish[i]);
            a = S.top();
            S.pop();
        }
        if (Polish[i] == "+") S.push(b+a);
        if (Polish[i] == "-") S.push(b-a);
        if (Polish[i] == "*") S.push(b*a);
        if (Polish[i] == "/") S.push(b/a);
        if (Polish[i] == "**") S.push(pow(b, a));
        if (Polish[i] == "^") S.push(pow(b, a));
        if (Polish[i] == "+pre") S.push(+a);
        if (Polish[i] == "-pre") S.push(-a);
        if (Polish[i] == "cos") S.push(cos(a));
        if (Polish[i] == "sin") S.push(sin(a));
    }
    return S.top();
}
int main()
{
    string S;
    getline (cin, S);
    if(S == "") S = "(2^3^5^(4-2*5+12/2))^2/4-32*(-1)-(-2)+1";
    vector<string> Tokens = SplitOnTokens(S);
    vector<string> Polish = SortStation(Tokens);
    cout << S << "=" << calc(Polish);
    return 0;
}

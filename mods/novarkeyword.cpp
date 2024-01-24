#include <iostream>
#include <string>
#include <vector>
#include <functional>

#define FLOAT_MAX 3.402823466e+38F

enum TokenType {
    FUNCTION,
    OPENBRAC,
    CLOSEBRAC,
    STRING,
    NUMBER,
    SEMI,
    COMMA,
    VAR,
    EQ,
    IF,
    THEN
} ;

struct Token
{
    TokenType type;
    std::string value;
};

struct Args
{
    void* ptrs;
    std::string strings;
    std::string types;
};

typedef std::function <void*(std::vector<Args>)> voidptr;

struct ptrs
{
    std::string ptrname;
    voidptr ptr;
};

typedef std::function<double(std::vector<Args>)> numfunc;

struct  number
{
    std::string numname;
    numfunc func;
};

typedef std::function<std::string(std::vector<Args>)> strfunc;

struct str
{
    std::string strname;
    strfunc func;
};

typedef  std::function<void(std::vector<Args>)> Function;

struct Funcs
{
    std::string FuncName;
    Function func;
};

struct var
{
    std::string name;
    Args args;
};

std::vector<var> vars;

std::vector<Token> GetToken(const std::string& file)
{
    std::string buffer;
    std::vector<Token> tokens;
    bool InString = false;
    bool InFunc = false;
    bool InNum = false;
    for(int i = 0; i < file.length(); i++)
    {
        char c = file.at(i);
        if (InString || InFunc|| InNum)
        {
            if (InString)
            {
                if (c == '"') {
                    tokens.push_back({TokenType::STRING, buffer});
                    buffer.clear();
                    InString= false;
                }
                else {
                    buffer.push_back(c);
                }
            }
            else if (InFunc)
            {
                if (isspace(c) || c == '(' || c == '"' || c == ')' || c == ',' || c == '=' || c == ';')
                {
                    if (buffer == "var")
                        tokens.push_back({TokenType::VAR, buffer});
                    else if (buffer == "if") {
                        tokens.push_back({TokenType::IF, buffer});
                    }
                    else if (buffer == "then") {
                        tokens.push_back({TokenType::THEN, buffer});
                    }
                    else
                        tokens.push_back({TokenType::FUNCTION, buffer});

                    if (c == '(')
                        tokens.push_back({TokenType::OPENBRAC, "("});
                    else if (c == '"')
                        InString = true;
                    else if (c == ')')
                        tokens.push_back({TokenType::CLOSEBRAC, ")"});
                    else if (c == ',')
                        tokens.push_back({TokenType::COMMA, ","});
                    else if (c == '=') {
                        tokens.push_back({TokenType::EQ, "="});
                    }
                    else if (c == ';') {
                        tokens.push_back({TokenType::SEMI, ";"});
                    }
                    buffer.clear();
                    InFunc = false;
                }
                else {
                    if (c != ';')
                        buffer.push_back(c);
                    else
                    {
                        std::cerr<<"Error unexpected " << c << " After " << buffer << '\n';
                        return {};
                    }
                }
            }
            else
            {
                if (isspace(c) || c == '(' || c == '"' || c == ')' || c == ','  || c == ';')
                {
                    tokens.push_back({TokenType::NUMBER, buffer});

                    if (atof(buffer.c_str()) == FLOAT_MAX)
                        buffer = "0";

                    if (c == '(')
                        tokens.push_back({TokenType::OPENBRAC, "("});
                    else if (c == '"')
                        InString = true;
                    else if (c == ')')
                        tokens.push_back({TokenType::CLOSEBRAC, ")"});
                    else if (c == ',')
                        tokens.push_back({TokenType::COMMA, ","});
                    else if (c == ';') {
                        tokens.push_back({TokenType::SEMI, ";"});
                    }

                    buffer.clear();
                    InNum = false;
                }
                else {
                    if (isdigit(c) || c == '.')
                        buffer.push_back(c);
                    else
                    {
                        std::cerr<<"Error unexpected " << c << " After " << buffer << '\n';
                        return {};
                    }
                }
            }
        }
        else
        {
            if (c == '"') {
                InString = true;
            }
            else if (c == '(') {
                tokens.push_back({TokenType::OPENBRAC, "("});
            }
            else if (c == ')') {
                tokens.push_back({TokenType::CLOSEBRAC, ")"});
            }
            else if (c == ',') {
                tokens.push_back({TokenType::COMMA, ","});
            }
            else if (c == ';') {
                tokens.push_back({TokenType::SEMI, ";"});
            }
            else if (c == '=') {
                tokens.push_back({TokenType::EQ, "="});
            }
            else if (isalpha(c)) {
                buffer.push_back(c);
                InFunc = true;
            }
            else if (isdigit(c)) {
                buffer.push_back(c);
                InNum = true;
            }
        }
    }
    if (buffer.length() != 0) {
        if(InNum) {
            tokens.push_back({TokenType::NUMBER, buffer});
        }
        else if (InFunc)
        {
            tokens.push_back({TokenType::FUNCTION, buffer});
        }
        else
            return {};
    }
    return tokens;
}

std::vector<Token> GetArgTokens(int& o, std::vector<Token> tokens)
{
    std::vector<Token> rettokens;
    int scope = 0, numopen = 0, numclose = 0;
    if ( o + 2 < tokens.size())
    {
        if (tokens[o +1].type != TokenType::OPENBRAC) {
            std::cerr << "Error: expected ( after: " << tokens[0].value << " but got " << tokens[o + 1].value << '\n';
            return {};
        }
        else if (tokens[o +1].type == TokenType::OPENBRAC && tokens[o + 2].type == TokenType::COMMA) {
            std::cerr << "Error: Unexpected , after: " << tokens[0].value << '\n';
            return {};
        }
        else if (tokens[o +1].type == TokenType::CLOSEBRAC && tokens[o + 2].type == TokenType::OPENBRAC) {
            std::cerr << "Error: Unexpected ( after: " << tokens[0].value << "(" << '\n';
            return {};
        }
    }

    for(int j = o; j < tokens.size(); j++) {
        if (tokens[j].type == TokenType::OPENBRAC)
        {
            scope++;
            numopen++;
        }
        else if (tokens[j].type == TokenType::CLOSEBRAC && scope != 1) {
            numclose++;
            scope--;
        }
        else if (tokens[j].type == TokenType::CLOSEBRAC && scope == 1) {
            rettokens.push_back({tokens[j].type, tokens[j].value});
            scope--;
            numclose++;
            o = j;
            break;
        }
        rettokens.push_back({tokens[j].type, tokens[j].value});
    }

    if (numclose != numopen || numopen == 0 || numclose == 0)
    {
        std::cerr << "Error Expected: " << '\n';
        return {};
    }

    if (scope != 0)
        return {};

    return rettokens;
}

std::string GetStr(std::vector<Token> tokens, std::vector<Funcs> fncs, std::vector<ptrs> Ptrs,
                   std::vector<number> nums, std::vector<str> strings);

void* GetPtr(std::vector<Token> tokens, std::vector<Funcs> fncs, std::vector<ptrs> Ptrs,
             std::vector<number> nums, std::vector<str> strings);

double GetNum(std::vector<Token> tokens, std::vector<Funcs> fncs = {}, std::vector<ptrs> Ptrs = {},
              std::vector<number> nums = {}, std::vector<str> strings = {})
{
    for(int o = 0; o < tokens.size(); o++ ) {
        if (tokens[o].type == TokenType::FUNCTION) {
            numfunc func;
            bool found = false;
            for(int j =0; j<nums.size(); j++) {
                if (tokens[o].value  == nums[j]. numname) {
                    func = nums[j].func;
                    found = true;
                    break;
                }
            }
            if (!found)
                return FLOAT_MAX;

            o++;

            if (o < tokens.size()) {
                if (tokens[o].type == TokenType::OPENBRAC)  {
                    std::vector<Args> args;

                    o++;
                    int it = 0;
                    while (o < tokens.size()) {
                        if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                && tokens[o].type != TokenType::CLOSEBRAC) {
                            std::cerr << "Error: Found " << tokens[o].value << " Instead Of String\n";
                            return FLOAT_MAX;
                        }
                        else if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                 && tokens[o].type == TokenType::CLOSEBRAC) {
                            if (it != 0) {
                                return FLOAT_MAX;
                            }

                            args = {};
                            break;
                        }
                        else {
                            it++;
                            if (tokens[o] .type== TokenType::STRING)
                            {
                                args.push_back({NULL, tokens[o].value,  "string"});
                            }
                            else if  (tokens[o].type == TokenType::FUNCTION) {
                                bool isv = false;
                                for(int u =0; u<vars.size(); u++) {
                                    if (tokens[o].value == vars[u].name) {
                                        args.push_back (vars[u].args);
                                        isv = true;
                                        break;
                                    }
                                }

                                if (!isv)
                                {
                                    std::vector<Token> tmptokens = GetArgTokens(o, tokens);
                                    if (tmptokens.size() == 0)
                                        return FLOAT_MAX;

                                    bool DidPrev = false;
                                    for(int j = 0; j < nums.size(); j++) {
                                        if (tmptokens[0].value == nums[j].numname)
                                        {
                                            double add = GetNum(tmptokens, fncs, Ptrs, nums, strings);
                                            if (add == FLOAT_MAX)
                                            {
                                                std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                return FLOAT_MAX;
                                            }

                                            DidPrev = true;
                                            args.push_back({NULL, std::to_string(add), "number"});
                                            break;
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        for(int j = 0; j < strings.size(); j++) {
                                            if (tmptokens[0].value == strings[j].strname)
                                            {
                                                std::string add = GetStr(tmptokens, fncs, Ptrs, nums, strings);
                                                if (add == "apentb3labfox9qu2psv")
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return FLOAT_MAX;
                                                }

                                                DidPrev = true;
                                                args.push_back({NULL, add, "string"});
                                                break;
                                            }
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        for(int j = 0; j < Ptrs.size(); j++) {
                                            if (tmptokens[0].value == Ptrs[j].ptrname)
                                            {
                                                void* p = GetPtr(tmptokens, fncs, Ptrs, nums, strings);
                                                if (p == nullptr)
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return FLOAT_MAX;
                                                }

                                                DidPrev = true;
                                                args.push_back({p, "", "ptr"});
                                                break;
                                            }
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        std::cerr<< "Error: Unknown Function " << tmptokens[0].value << '\n';
                                        return FLOAT_MAX;
                                    }
                                }
                            }
                            else
                            {
                                args.push_back({NULL, tokens[o].value,  "number"});
                            }

                            o++;

                            if (o >= tokens.size()) {
                                return FLOAT_MAX;
                            }
                            if (tokens[o].type == TokenType::COMMA) {
                                o++;
                                continue;
                            }
                            else if (tokens[o].type == TokenType::CLOSEBRAC)
                            {
                                break;
                            }
                            else {
                                std::cerr << "Error expected , Or ) After " << tokens[o - 1].value << " But Got " << tokens[o].value<<"\n";
                                return FLOAT_MAX;
                            }
                        }
                    }
                    double rr = func(args);
                    return rr;
                }
                else {
                    std::cerr << "Error: Function " << tokens[o - 1].value << " Without '('\n";
                    return FLOAT_MAX;
                }
            }
            else {
                std::cerr << "Error: Function " << tokens[o].value << " Without '('\n";
                return FLOAT_MAX;
            }
        }
    }
}


std::string GetStr(std::vector<Token> tokens, std::vector<Funcs> fncs = {}, std::vector<ptrs> Ptrs = {},
                   std::vector<number> nums = {}, std::vector<str> strings = {})
{
    for(int o = 0; o < tokens.size(); o++ ) {
        if (tokens[o].type == TokenType::FUNCTION) {
            strfunc func;
            bool found = false;
            for(int j =0; j<strings.size(); j++) {
                if (tokens[o].value  == strings[j]. strname) {
                    func = strings[j].func;
                    found = true;
                    break;
                }
            }
            if (!found)
                return "apentb3labfox9qu2psv";

            o++;

            if (o < tokens.size()) {
                if (tokens[o].type == TokenType::OPENBRAC)  {
                    std::vector<Args> args;

                    o++;
                    int it = 0;
                    while (o < tokens.size()) {
                        if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                && tokens[o].type != TokenType::CLOSEBRAC) {
                            std::cerr << "Error: Found " << tokens[o].value << " Instead Of String\n";
                            return "apentb3labfox9qu2psv";
                        }
                        else if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                 && tokens[o].type == TokenType::CLOSEBRAC) {
                            if (it != 0) {
                                return "apentb3labfox9qu2psv";
                            }

                            args = {};
                            break;
                        }
                        else {
                            it++;
                            if (tokens[o] .type== TokenType::STRING)
                            {
                                args.push_back({NULL, tokens[o].value,  "string"});
                            }
                            else if  (tokens[o].type == TokenType::FUNCTION) {
                                bool isv = false;
                                for(int u =0; u<vars.size(); u++) {
                                    if (tokens[o].value == vars[u].name) {
                                        args.push_back (vars[u].args);
                                        isv = true;
                                        break;
                                    }
                                }

                                if (!isv)
                                {
                                    std::vector<Token> tmptokens = GetArgTokens(o, tokens);
                                    if (tmptokens.size() == 0)
                                        return "apentb3labfox9qu2psv";

                                    bool DidPrev = false;
                                    for(int j = 0; j < nums.size(); j++) {
                                        if (tmptokens[0].value == nums[j].numname)
                                        {
                                            double add = GetNum(tmptokens, fncs, Ptrs, nums, strings);
                                            if (add == FLOAT_MAX)
                                            {
                                                std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                return "apentb3labfox9qu2psv";
                                            }

                                            DidPrev = true;
                                            args.push_back({NULL, std::to_string(add), "number"});
                                            break;
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        for(int j = 0; j < strings.size(); j++) {
                                            if (tmptokens[0].value == strings[j].strname)
                                            {
                                                std::string add = GetStr(tmptokens, fncs, Ptrs, nums, strings);
                                                if (add == "apentb3labfox9qu2psv")
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return "apentb3labfox9qu2psv";
                                                }

                                                DidPrev = true;
                                                args.push_back({NULL, add, "string"});
                                                break;
                                            }
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        for(int j = 0; j < Ptrs.size(); j++) {
                                            if (tmptokens[0].value == Ptrs[j].ptrname)
                                            {
                                                void* p = GetPtr(tmptokens, fncs, Ptrs, nums, strings);
                                                if (p == nullptr)
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return "apentb3labfox9qu2psv";
                                                }

                                                DidPrev = true;
                                                args.push_back({p, "", "ptr"});
                                                break;
                                            }
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        std::cerr<< "Error: Unknown Function " << tmptokens[0].value << '\n';
                                        return "apentb3labfox9qu2psv";
                                    }
                                }
                            }
                            else
                            {
                                args.push_back({NULL, tokens[o].value,  "number"});
                            }

                            o++;

                            if (o >= tokens.size()) {
                                return "apentb3labfox9qu2psv";
                            }
                            if (tokens[o].type == TokenType::COMMA) {
                                o++;
                                continue;
                            }
                            else if (tokens[o].type == TokenType::CLOSEBRAC)
                            {
                                break;
                            }
                            else {
                                std::cerr << "Error expected , Or ) After " << tokens[o - 1].value << " But Got " << tokens[o].value<<"\n";
                                return "apentb3labfox9qu2psv";
                            }
                        }
                    }
                    std::string rr = func(args);
                    return rr;
                }
                else {
                    std::cerr << "Error: Function " << tokens[o - 1].value << " Without '('\n";
                    return "apentb3labfox9qu2psv";
                }
            }
            else {
                std::cerr << "Error: Function " << tokens[o].value << " Without '('\n";
                return "apentb3labfox9qu2psv";
            }
        }
    }
}

void* GetPtr(std::vector<Token> tokens, std::vector<Funcs> fncs = {}, std::vector<ptrs> Ptrs = {},
             std::vector<number> nums = {}, std::vector<str> strings = {})
{
    for(int o = 0; o < tokens.size(); o++ ) {
        if (tokens[o].type == TokenType::FUNCTION) {
            voidptr func;
            bool found = false;
            for(int j =0; j<Ptrs.size(); j++) {
                if (tokens[o].value  == Ptrs[j]. ptrname) {
                    func = Ptrs[j].ptr;
                    found = true;
                    break;
                }
            }
            if (!found)
                return nullptr;

            o++;

            if (o < tokens.size()) {
                if (tokens[o].type == TokenType::OPENBRAC)  {
                    std::vector<Args> args;

                    o++;
                    int it = 0;
                    while (o < tokens.size()) {
                        if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                && tokens[o].type != TokenType::CLOSEBRAC) {
                            std::cerr << "Error: Found " << tokens[o].value << " Instead Of String\n";
                            return nullptr;
                        }
                        else if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                 && tokens[o].type == TokenType::CLOSEBRAC) {
                            if (it != 0) {
                                return nullptr;
                            }

                            args = {};
                            break;
                        }
                        else {
                            it++;
                            if (tokens[o] .type== TokenType::STRING)
                            {
                                args.push_back({NULL, tokens[o].value,  "string"});
                            }
                            else if  (tokens[o].type == TokenType::FUNCTION) {
                                bool isv = false;
                                for(int u =0; u<vars.size(); u++) {
                                    if (tokens[o].value == vars[u].name) {
                                        args.push_back (vars[u].args);
                                        isv = true;
                                        break;
                                    }
                                }

                                if (!isv)
                                {
                                    std::vector<Token> tmptokens = GetArgTokens(o, tokens);
                                    if (tmptokens.size() == 0)
                                        return nullptr;

                                    bool DidPrev = false;
                                    for(int j = 0; j < nums.size(); j++) {
                                        if (tmptokens[0].value == nums[j].numname)
                                        {
                                            double add = GetNum(tmptokens, fncs, Ptrs, nums, strings);
                                            if (add == FLOAT_MAX)
                                            {
                                                std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                return nullptr;
                                            }

                                            DidPrev = true;
                                            args.push_back({NULL, std::to_string(add), "number"});
                                            break;
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        for(int j = 0; j < strings.size(); j++) {
                                            if (tmptokens[0].value == strings[j].strname)
                                            {
                                                std::string add = GetStr(tmptokens, fncs, Ptrs, nums, strings);
                                                if (add == "apentb3labfox9qu2psv")
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return nullptr;
                                                }

                                                DidPrev = true;
                                                args.push_back({NULL, add, "string"});
                                                break;
                                            }
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        for(int j = 0; j < Ptrs.size(); j++) {
                                            if (tmptokens[0].value == Ptrs[j].ptrname)
                                            {
                                                void* p = GetPtr(tmptokens, fncs, Ptrs, nums, strings);
                                                if (p == nullptr)
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return nullptr;
                                                }

                                                DidPrev = true;
                                                args.push_back({p, "", "ptr"});
                                                break;
                                            }
                                        }
                                    }
                                    if (!DidPrev)
                                    {
                                        std::cerr<< "Error: Unknown Function " << tmptokens[0].value << '\n';
                                        return nullptr;
                                    }
                                }
                                else
                                {
                                    args.push_back({NULL, tokens[o].value,  "number"});
                                }
                                o++;

                                if (o >= tokens.size()) {
                                    return nullptr;
                                }
                                if (tokens[o].type == TokenType::COMMA) {
                                    o++;
                                    continue;
                                }
                                else if (tokens[o].type == TokenType::CLOSEBRAC)
                                {
                                    break;
                                }
                                else {
                                    std::cerr << "Error expected , Or ) After " << tokens[o - 1].value << " But Got " << tokens[o].value<<"\n";
                                    return nullptr;
                                }
                            }
                        }
                    }
                    void* rr = func(args);
                    return rr;
                }
                else {
                    std::cerr << "Error: Function " << tokens[o - 1].value << " Without '('\n";
                    return nullptr;
                }
            }
            else {
                std::cerr << "Error: Function " << tokens[o].value << " Without '('\n";
                return nullptr;
            }
        }
    }
}

void Parse(std::vector<Token> tokens, std::vector<Funcs> fncs = {}, std::vector<ptrs> Ptrs = {},
           std::vector<number> nums = {}, std::vector<str> strings = {}) {
    for(int o = 0; o < tokens.size(); o++ ) {
        if (tokens[o].type == TokenType::FUNCTION) {
            o++;
            if (o >= tokens.size())
            return;
            
            if (tokens[o].type == TokenType::EQ)
            {
                std::string name = tokens[o-1].value;
                bool isva = false;
                var *dovar = nullptr;
                for (int p = 0; p<vars.size(); p++) {
                    if (vars[p].name == name)
                    {
                        dovar = &vars[p];
                        isva = true;
                        break;
                    }
                }

                o++;
                Args args;
                if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type !=  TokenType::NUMBER) {
                    std::cerr << "Error: Found " << tokens[o].value << " Instead Of st\n";
                    return;
                }
                else {
                    if (tokens[o] .type== TokenType::STRING)
                    {
                        args = {NULL, tokens[o].value,  "string"};
                    }
                    else if  (tokens[o].type == TokenType::FUNCTION) {
                        bool isv = false;
                        for(int u =0; u<vars.size(); u++) {
                            if (tokens[o].value == vars[u].name) {
                                args = (vars[u].args);
                                isv = true;
                                break;
                            }
                        }

                        if (!isv)
                        {
                            std::vector<Token> tmptokens = GetArgTokens(o, tokens);
                            if (tmptokens.size() == 0)
                                return;

                            bool DidPrev = false;
                            for(int j = 0; j < nums.size(); j++) {
                                if (tmptokens[0].value == nums[j].numname)
                                {
                                    double add = GetNum(tmptokens, fncs, Ptrs, nums, strings);
                                    if (add == FLOAT_MAX)
                                    {
                                        std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                        return;
                                    }

                                    DidPrev = true;
                                    args = {NULL, std::to_string(add), "number"};
                                    break;
                                }
                            }
                            if (!DidPrev)
                            {
                                for(int j = 0; j < strings.size(); j++) {
                                    if (tmptokens[0].value == strings[j].strname)
                                    {
                                        std::string add = GetStr(tmptokens, fncs, Ptrs, nums, strings);
                                        if (add == "apentb3labfox9qu2psv")
                                        {
                                            std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                            return;
                                        }

                                        DidPrev = true;
                                        args = {NULL, add, "string"};
                                        break;
                                    }
                                }
                            }
                            if (!DidPrev)
                            {
                                for(int j = 0; j < Ptrs.size(); j++) {
                                    if (tmptokens[0].value == Ptrs[j].ptrname)
                                    {
                                        void* p = GetPtr(tmptokens, fncs, Ptrs, nums, strings);
                                        if (p == nullptr)
                                        {
                                            std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                            return;
                                        }

                                        DidPrev = true;
                                        args = {p, "", "ptr"};
                                        break;
                                    }
                                }
                            }
                            if (!DidPrev)
                            {
                                std::cerr<< "Error: Unknown Function " << tmptokens[0].value << '\n';
                                return;
                            }
                        }
                    }
                    else
                    {
                        args = {NULL, tokens[o].value,  "number"};
                    }
                }

                if (o + 1 < tokens.size())
                {
                    o++;
                    if (tokens[o].type != TokenType::SEMI)
                        return;

                    o++;
                    if (isva)
                        dovar->args = args;
                    else
                        vars.push_back({name, args});

                    std::vector<Token> tmp;
                    for (int j = o; j<tokens.size(); j++) {
                        tmp.push_back({tokens[j].type, tokens[j].value});
                    }
                    Parse(tmp, fncs, Ptrs, nums, strings);
                    return;
                }
                else
                    return;

            }
            else
            {
            o--;
                Function func;
                bool IsFnc = false;
                for(int j =0; j<fncs.size(); j++) {
                    if (tokens[o].value  == fncs[j]. FuncName) {
                        func = fncs[j].func;
                        IsFnc = true;
                        break;
                    }
                }
                if(!IsFnc)
                    return;

                o++;
                if (o < tokens.size()) {
                    if (tokens[o].type == TokenType::OPENBRAC)  {
                        std::vector<Args> args;

                        o++;
                        int it = 0;
                        while (o < tokens.size()) {
                            if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                    && tokens[o].type != TokenType::CLOSEBRAC) {
                                std::cerr << "Error: Found " << tokens[o].value << " Instead Of String\n";
                                return;
                            }
                            else if (tokens[o] .type!= TokenType::STRING && tokens[o] .type != TokenType::FUNCTION && tokens[o] .type != TokenType::NUMBER
                                     && tokens[o].type == TokenType::CLOSEBRAC) {
                                if (it != 0)
                                    return;

                                o++;
                                if (o >= tokens.size())
                                    return;

                                if (tokens[o].type !=TokenType::SEMI)
                                    return;

                                args = {};
                                break;
                            }
                            else {
                                it++;
                                if (tokens[o] .type== TokenType::STRING)
                                {
                                    args.push_back({NULL, tokens[o].value,  "string"});
                                }
                                else if  (tokens[o].type == TokenType::FUNCTION) {
                                    bool isv = false;
                                    for(int u =0; u<vars.size(); u++) {
                                        if (tokens[o].value == vars[u].name) {
                                            args.push_back (vars[u].args);
                                            isv = true;
                                            break;
                                        }
                                    }

                                    if (!isv)
                                    {
                                        std::vector<Token> tmptokens = GetArgTokens(o, tokens);
                                        if (tmptokens.size() == 0)
                                            return;

                                        bool DidPrev = false;
                                        for(int j = 0; j < nums.size(); j++) {
                                            if (tmptokens[0].value == nums[j].numname)
                                            {
                                                double add = GetNum(tmptokens, fncs, Ptrs, nums, strings);
                                                if (add == FLOAT_MAX)
                                                {
                                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                    return;
                                                }

                                                DidPrev = true;
                                                args.push_back({NULL, std::to_string(add), "number"});
                                                break;
                                            }
                                        }
                                        if (!DidPrev)
                                        {
                                            for(int j = 0; j < strings.size(); j++) {
                                                if (tmptokens[0].value == strings[j].strname)
                                                {
                                                    std::string add = GetStr(tmptokens, fncs, Ptrs, nums, strings);
                                                    if (add == "apentb3labfox9qu2psv")
                                                    {
                                                        std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                        return;
                                                    }

                                                    DidPrev = true;
                                                    args.push_back({NULL, add, "string"});
                                                    break;
                                                }
                                            }
                                        }
                                        if (!DidPrev)
                                        {
                                            for(int j = 0; j < Ptrs.size(); j++) {
                                                if (tmptokens[0].value == Ptrs[j].ptrname)
                                                {
                                                    void* p = GetPtr(tmptokens, fncs, Ptrs, nums, strings);
                                                    if (p == nullptr)
                                                    {
                                                        std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                                        return;
                                                    }

                                                    DidPrev = true;
                                                    args.push_back({p, "", "ptr"});
                                                    break;
                                                }
                                            }
                                        }
                                        if (!DidPrev)
                                        {
                                            std::cerr<< "Error: Unknown Function " << tmptokens[0].value << '\n';
                                            return;
                                        }
                                    }
                                }
                                else
                                {
                                    args.push_back({NULL, tokens[o].value,  "number"});
                                }

                                o++;

                                if (o >= tokens.size())
                                    return;

                                if (tokens[o].type == TokenType::COMMA) {
                                    o++;
                                    continue;
                                }
                                else if (tokens[o].type == TokenType::CLOSEBRAC)
                                {
                                    o++;
                                    if (o < tokens.size()) {
                                        if (tokens[o].type == TokenType::SEMI) {
                                            break;
                                        }
                                        else {
                                            std::cerr << "Error Expecting ;\n";
                                            return;
                                        }
                                    }
                                    else {
                                        std::cerr << "Error ; without )\n";
                                        return;
                                    }
                                }
                                else if (tokens[o].type == TokenType::SEMI) {
                                    std::cerr << "Error ; without )\n";
                                    return;
                                }
                                else {
                                    std::cerr << "Error expected , Or ) After " << tokens[o - 1].value << " But Got " << tokens[o].value<<"\n";
                                    return;
                                }
                            }
                        }
                        func(args);

                        o++;
                        std::vector<Token> tmp;
                        for (int j = o; j<tokens.size(); j++) {
                            tmp.push_back({tokens[j].type, tokens[j].value});
                        }
                        Parse(tmp, fncs, Ptrs, nums, strings);
                        return;
                    }
                    else {
                        std::cerr << "Error: Function " << tokens[o - 1].value << " Without '('\n";
                        return;
                    }
                }
                else {
                    std::cerr << "Error: Function " << tokens[o].value << " Without '('\n";
                    return;
                }
            }
        }
        else if (tokens[o].type == TokenType::IF) {
            o++;
            if (o >= tokens.size ())
                return;

            int stmt = 0;

            if (tokens[o] .type != TokenType::FUNCTION && tokens[o] .type !=  TokenType::NUMBER) {
                std::cerr << "Error: Found " << tokens[o].value << " Instead Of st\n";
                return;
            }
            else
            {
                if  (tokens[o].type == TokenType::FUNCTION) {
                    bool isv = false;

                    for(int u =0; u<vars.size(); u++) {
                        if (tokens[o].value == vars[u].name) {
                            if (vars[u].args.types == "ptr")
                            {
                                if (vars[u].args.ptrs)
                                    stmt = true;
                                else
                                    stmt=false;
                            }
                            else if (vars[u].args.types == "number")
                            {
                                if ((int)atof(vars[u].args.strings.c_str()))
                                    stmt = true;
                                else
                                    stmt=false;
                            }
                            else
                                return;
                            isv = true;
                            break;
                        }
                    }

                    if (!isv)
                    {
                        std::vector<Token> tmptokens = GetArgTokens(o, tokens);
                        if (tmptokens.size() == 0)
                            return;

                        bool DidPrev = false;
                        for(int j = 0; j < nums.size(); j++) {
                            if (tmptokens[0].value == nums[j].numname)
                            {
                                double add = GetNum(tmptokens, fncs, Ptrs, nums, strings);
                                if (add == FLOAT_MAX)
                                {
                                    std::cerr << "Error Calling Function " <<tmptokens[0].value << '\n';
                                    return;
                                }

                                DidPrev = true;
                                if ((int)add)
                                    stmt = true;
                                else
                                    stmt=false;
                                break;
                            }
                        }
                        if (!DidPrev)
                        {
                            for(int j = 0; j < Ptrs.size(); j++) {
                                if (tmptokens[0].value == Ptrs[j].ptrname)
                                {
                                    void* p = GetPtr(tmptokens, fncs, Ptrs, nums, strings);

                                    DidPrev = true;
                                    if (p)
                                        stmt = true;
                                    else
                                        stmt=false;
                                    break;
                                }
                            }
                        }
                        if (!DidPrev)
                        {
                            std::cerr<< "Error: Unknown Function " << tmptokens[0].value << '\n';
                            return;
                        }
                    }
                }
                else
                {
                    if ((int)atof(tokens[o].value.c_str()))
                        stmt = true;
                    else
                        stmt = false;
                }
            }
            o++;
            if (o >=tokens.size())
                return;

            if (tokens[o].type != TokenType ::THEN)
                return;

            o++;
            if(o >= tokens.size())
                return;

            if (stmt)
            {
                std::vector<Token> tmp;
                int scope = 0;
                bool FEnd = false;
                bool did = false;
                bool d = false;
                bool InElse = false;
                bool DidElse = false;
                for (int j = o; j<tokens.size(); j++) {
                    if (d) {
                        FEnd = false;
                        d=false;
                        InElse = false;
                    }
                    if (!did) {
                        if(tokens[j].type == TokenType::THEN)
                            scope++;
                        else if (tokens[j].value == "end" && scope != 0) {
                            scope--;
                        }
                        else if (tokens[j].value == "end" && scope == 0) {
                            FEnd = true;
                            did= true;
                            d = true;
                        }
                        else if  (tokens[j].value == "else" && scope == 0) {
                            if (!DidElse)
                            {
                                InElse = true;
                                DidElse = true;
                            }
                        }
                    }
                    if (!FEnd && !InElse) {
                        tmp.push_back({tokens[j].type, tokens[j].value});
                    }
                }
                if (!did || scope != 0)
                    return;

                Parse(tmp, fncs, Ptrs, nums, strings);
            }
            else
            {
                std::vector<Token> tmp;
                int scope = 0;
                bool FEnd = false;
                bool did = false;
                bool d = false;
                bool d2 = false;
                bool InElse = false;
                bool DidElse = false;
                for (int j = o; j<tokens.size(); j++) {
                    if (d) {
                        FEnd = true;
                        d=false;
                        InElse = true;
                    }
                    if (d2) {
                        InElse = true;
                        d2 = false;
                    }
                    if (!did) {
                        if(tokens[j].type == TokenType::THEN && !FEnd)
                            scope++;
                        else if (tokens[j].value == "end" && scope != 0 &&!FEnd) {
                            scope--;
                        }
                        else if (tokens[j].value == "end" && scope == 0 && !FEnd) {
                            FEnd  = false;
                            InElse = false;
                            did= true;
                            d = true;
                        }
                        else if  (tokens[j].value == "else" && scope == 0) {
                            if (!DidElse)
                            {
                                d2 = true;
                                DidElse = true;
                            }
                        }
                    }
                    if (FEnd || InElse) {
                        tmp.push_back({tokens[j].type, tokens[j].value});
                    }
                }
                if (!did || scope != 0)
                    return;

                Parse(tmp, fncs, Ptrs, nums, strings);
            }
        }
        else
        {
            return;
        }
    }
}

#define l_nArgs(ags) ags.size()
#define l_getString(ags, index) ((index < ags.size()) ? (ags[index].types == "string") ? ags[index].strings : "" : "")
#define l_InvalidString ""
#define l_getNumber(ags, index) ((index < ags.size()) ? (ags[index].types == "number") ? atof(ags[index].strings.c_str()) : FLOAT_MAX : FLOAT_MAX)
#define l_getInt(ags, index) ((index < ags.size()) ? (ags[index].types == "number") ? atoi(ags[index].strings.c_str()) : FLOAT_MAX : FLOAT_MAX)
#define l_InvalidNum FLOAT_MAX
#define l_getPtr(ags, index) ((index < ags.size()) ? (ags[index].types == "ptr") ? ags[index].ptrs : nullptr : nullptr)
#define l_isPtr(ags, index) ((index < ags.size()) ? (ags[index].types == "ptr") ? true : false : false)
#define l_isString(ags, index) ((index < ags.size()) ? (ags[index].types == "string") ? true : false : false)
#define l_isNumber(ags, index) ((index < ags.size()) ? (ags[index].types == "number") ? true : false : false)
#define l_invalidPtr nullptr
#define l_get(ags, index) ((index < ags.size()) ? ags[index].strings  : "")
#define largs std::vector<Args> args

#define VoidFuncs std::vector<Funcs>
#define NumFuncs std::vector<number>
#define PtrFuncs std::vector<ptrs>
#define StringFuncs std::vector<str>

void* g(largs)
{   int* b = new int;
    return  b;
}
void print(largs)
{
    std::cout << l_get (args, 0);
}

double mn(largs)
{
    return !l_getNumber(args, 0);
}

int main()
{
    VoidFuncs s = {
        {"print", print}
    };
    NumFuncs d = {
        {"not", mn},
    };
    PtrFuncs  sz= {{"t", g}};
    auto t = GetToken("if not(0) then print(\"true\"); x = 2; print(x); x = not(not(1)); print(x); else print(\"false\"); end print(199);");
    Parse(t,s, sz,d);
}
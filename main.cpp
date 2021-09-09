#include <iostream>
#include <string>
#include <sstream>
#include <list>
#include <vector>
#include <iomanip>

using namespace std;

class Variable {
    char varName{};
    float coefficient{};
    double power{};

public:

    ///Setters
    void setVarName(char varNam) {
        this->varName = varNam;
    }

    void setCoefficient(float coEff) {
        this->coefficient = coEff;
    }

    void setPower(double pow) {
        this->power = pow;
    }
    ///End of Setters

    static int getPrecision(double value) {
        if ((value / (int) value) != 1) {
            return 2;
        } else {
            return 0;
        }
    }

    ///Overloading operator << for passing class object to cout
    friend std::ostream &operator<<(std::ostream &os, Variable const &var) {
        string nonCoEffStr;
        string coEffStr;
        int precision;
        stringstream ss;
        if (var.varName == ' ') {
            nonCoEffStr.append("");
        } else {
            nonCoEffStr.append(1, var.varName);
            nonCoEffStr.append(1, '^');
            precision = getPrecision(var.getPower());
            ss << fixed << setprecision(precision) << var.getPower();
            nonCoEffStr += ss.str();
            ss.str(""); ///Clearing the Stream
        }

        if (var.coefficient == 1) {
            coEffStr = "";
        } else if (var.coefficient == -1) {
            coEffStr = "-";
        } else {
            precision = getPrecision(var.getCoefficient());
            ss << fixed << setprecision(precision) << var.getCoefficient();
            coEffStr += ss.str();
            ss.str(""); ///Clearing the Stream
        }

        return os << coEffStr << nonCoEffStr << " ";
    }

    ///getters
    char getVarName() const {
        return varName;
    }

    float getCoefficient() const {
        return coefficient;
    }

    double getPower() const {
        return power;
    }
    ///End of getters

    ///Parameterized constructor
    Variable(char varName, float coefficient, double power) {
        this->varName = varName;
        this->coefficient = coefficient;
        this->power = power;
    }

    Variable() = default; ///Default Empty Constructor
};

///Tokenize string method to split string based on delimiters
vector<string> simple_tokenizer(const string &s, const char delim) {
    stringstream ss(s);
    vector<string> result;
    string word;
    while (getline(ss, word, delim)) {
        result.push_back(word);
    }

    return result;
}

///Parses expression to build an object of Variable type
Variable makeVariable(string expression, int &i, bool isNegative) {
    Variable var{};
    i = i + 1;
    string coEff;

    while (expression[i] == '/' || (expression[i] >= '0' && expression[i] <= '9')) {
        coEff += expression[i];
        i = i + 1;
    }

    ///Coefficient empty means it's either -1 or 1 based on sign
    if (coEff.empty()) {
        var.setCoefficient(isNegative ? -1 : 1);
    }

        /// If / found  -> There is a fractional coefficient
    else if (coEff.find('/') != string::npos) {
        vector<string> splitStr = simple_tokenizer(coEff, '/');
        float co = 1;
        int j = 0;
        for (const auto &val: splitStr) {
            if (j == 0) {
                co *= stof(val);
            } else {
                co /= stof(val);
            }
            j++;
        }

        var.setCoefficient(isNegative ? -co : co);
    } else {
        var.setCoefficient(isNegative ? -stof(coEff) : stof(coEff));
    }

    ///Handling constants
    if (expression[i] == '\0' || !isalpha(expression[i])) {
        var.setVarName(' ');
        var.setPower(0);
        return var;
    } else {
        var.setVarName(expression[i]);
    }

    ///variable has power
    if (expression[i + 1] == '^') {
        i += 2;
        string pow;
        while (expression[i] == '/' || (expression[i] >= '0' && expression[i] <= '9')) {
            pow += expression[i];

            i = i + 1;
        }

        ///Fractional power
        if (pow.find('/') != string::npos) {
            vector<string> splitStr = simple_tokenizer(pow, '/');
            double power = 1;
            int j = 0;
            for (const auto &val: splitStr) {
                if (j == 0) {
                    power *= stod(val);
                } else {
                    power /= stod(val);
                }
                j++;
            }

            var.setPower(power);
        }
            ///integer power
        else {
            var.setPower(stof(pow));
        }
    }
        ///No power supplied, using 1
    else {
        var.setPower(1);
    }

    ///Return the built object
    return var;
}

int main() {
    string polyExpression;
    cout << "Enter a polynomial\n(Use ^ to denote to the power of a variable)\n";
    getline(cin, polyExpression);

    ///Remove spaces
    polyExpression.erase(remove_if(polyExpression.begin(), polyExpression.end(), isspace), polyExpression.end());

    ///List to store objects of parsed variables
    list<Variable> varList;

    ///Call function make variable based on signedness
    for (int i = 0; i < polyExpression.length();) {
        if (polyExpression[i] == '-') {
            varList.push_back(makeVariable(polyExpression, i, true));
        } else if (polyExpression[i] == '+' || isalnum(polyExpression[i])) {
            varList.push_back(makeVariable(polyExpression, i, false));
        } else {
            cout << "Invalid expression!";
            return -1;
        }
    }

    varList.sort([](const Variable &f, const Variable &s) { return f.getPower() > s.getPower(); });

    ///list to store solved objects
    list<Variable> solvedList;

    ///Traversing variable list to get final result
    for (int i = 0; i < varList.size(); i++) {
        auto it = varList.begin();

        ///Solved list is empty, passing the Variable object
        if (solvedList.empty()) {
            Variable var = {it->getVarName(), it->getCoefficient(), it->getPower()};
            solvedList.push_back(var);
            continue;
        }

        ///Access i th index of varList
        advance(it, i);

        int k = 0;

        ///bool to check if the coefficient value of the
        /// variable in final result was updated
        bool wasUpdated = false;

        for (auto j: solvedList) {
            auto itSolved = solvedList.begin();
            advance(itSolved, k);

            ///Checking if varName and Power are same
            ///Break on found, otherwise for loop will
            /// encounter error of changing list that is being iterated
            if ((j.getVarName() == it->getVarName()) && (j.getPower() == it->getPower())) {
                Variable var = {it->getVarName(), it->getCoefficient() + j.getCoefficient(), it->getPower()};
                ///Erasing previous object at index
                itSolved = solvedList.erase(itSolved);
                ///Inserting updated variable
                solvedList.insert(itSolved, var);
                wasUpdated = true;
                break;
            }

            k++;
        }

        ///Not updated case :
        ///Pushing new variable to solvedList
        if (!wasUpdated) {

            Variable var = {it->getVarName(), it->getCoefficient(), it->getPower()};
            solvedList.push_back(var);
        }
    }

    int i = 0;

    ///Printing result
    for (Variable var: solvedList) {
        if (var.getCoefficient() > 0 && i != 0) {
            cout << "+ " << var;
        } else {
            cout << var;
        }
        i++;
    }

    return 0;
}

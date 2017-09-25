#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>

using namespace std;

//formula class prototype
class Formula {
	public:
		Formula(string equation) {
			Initialization(equation);
		}
		string All_Calculations();
	private:
		vector<string> elements;
		void Initialization(string _equation);
		string Calculations(vector<string>& _elements);
		void Operator(vector<string>& _elements, char _sign);
		void Plus_Minus_Sign();
};

bool HasErrors(string _equation);

int main() {
	//obtaining text file (without error)
	ifstream file;
	file.open("data.txt");
	
	if (file.fail()) {
		cerr << "File Error" << endl;
		exit(1);
	}
	
	string line;
	int count(0);
	vector<Formula> terms;
	
	//while text file still has lines
	while (getline(file, line)) {
		++count;
		//if line doesn't start with a # and line isn't empty
		if (line[0] != '#' && !line.empty()) {
			//and doesn't have syntax errors
			if (HasErrors(line) == false) {
				//instantiate new formula object
				Formula temporary(line);
				terms.push_back(temporary);
				//printing results
				cout << temporary.All_Calculations() << endl;
			}
			else {
				cout << "Syntax error in line " << count << endl;
			}
		}
	}
	
	return 0;
}

bool HasErrors(string _equation) {
	//finding if amount of ( is equal to amount of ), if not, there is a syntax error
	int _amount_open(count(_equation.begin(), _equation.end(), '('));
	int _amount_close(count(_equation.begin(), _equation.end(), ')'));
	
	if (_amount_open != _amount_close) {
		return true;
	}
	else {
		//regex expression to find any syntax error
		regex _expression("[^-.+*()\\/0-9]|[-.+][-+]|[*.\\/][*\\/]|[.][0-9]*[.]|[0-9.][(]|[-.+*\\/][)]|[(][)\\/*]|[)][0-9.(]");
		
		//to iterate over different matches of a same regex pattern in a string
		sregex_iterator _iter(_equation.begin(), _equation.end(), _expression);
		// declaring iterator without initializing it automatically creates an ending condition
		sregex_iterator _iter_end;
		// match type to save the possible matches found by the iterator
		smatch _match;
				
		//if iterator has matches
		if(_iter != _iter_end) {
			return true;
		}
		else {
			return false;
		}
	}
}

void Formula::Initialization(string _equation) {
	//expression to match numbers, +, -, *, / and parenthesis
	regex _expression("^[-+](?:[0-9]*[.])?[0-9]+|(?:[0-9]*[.])?[0-9]+|[-+*\\/()]");

	//to iterate over different matches of a same regex pattern in a string
	sregex_iterator _iter(_equation.begin(), _equation.end(), _expression);
	// declaring iterator without initializing it automatically creates an ending condition
	sregex_iterator _iter_end;
	// match type to save the possible matches found by the iterator
	smatch _match;
			
	//if iterator has matches
	while(_iter != _iter_end) {
		//dereferencing the iterator to obtain a matched value
		_match = *_iter;
		//saving it into the vector 
		elements.push_back(_match.str());
		//advancing the regex_iterator to the next match
		++_iter;
	}
}

string Formula::All_Calculations() {
	//amount of open parenthesis (which we already know it's equal to closed ones)
	int _amount_open(count(elements.begin(), elements.end(), "("));
	
	//if it's greater than 0
	if (_amount_open > 0) {
		vector<string> _parenthesis_pair = {"(", ")"}, _sub_elements;
		vector<string>::iterator _start, _end;
		
		//for each parenthesis
		for (int i = 0; i < _amount_open; ++i) {
			//modifying signs
			Plus_Minus_Sign();
			//finding an opening parenthesis
			_start = find(elements.begin(), elements.end(), "(");
			_end = _start;
			
			//keep finding parenthesis until start has a (, and end has a ), which means we are inside the most inner parenthesis couple 
			while(*_end != ")") {
				_start = _end;
				//finding next parenthesis
				_end = find_first_of(next(_start), elements.end(), _parenthesis_pair.begin(), _parenthesis_pair.end());
			}
			//obtaining elements between those two parenthesis
			_sub_elements = vector<string>(next(_start), _end);
			//doing calculations
			*_end = Calculations(_sub_elements);
			//erasing elements between parenthesis including them, and leaving only the result
			elements.erase(_start, _end);
		}
	}
	
	//modifying signs
	Plus_Minus_Sign();
	//returning calculations value
	return Calculations(elements);
}

string Formula::Calculations(vector<string>& _elements) {
	vector<char> signs = {'/', '*', '+', '-'};
	
	//do calculations for every sign 
	for (char sign : signs) {
		//unless elements size is already one (avoinding entering on operations unnecessarily
		if (_elements.size() == 1) {
			break; 
		}
		else {
			Operator(_elements, sign);
		}
	}
	
	//returning operations value (after operations, only one item remains)
	return _elements[0];
}

void Formula::Operator(vector<string>& _elements, char _sign) {
	vector<double> _values = {0, 0};
	// iterator to find signs between elements
	vector<string>::iterator _iter = find(_elements.begin(), _elements.end(), string(1, _sign));
	vector<string>::iterator _result;
	
	//while the iterator isn't empty
	while (_iter != _elements.end() && _elements.size() > 1) {
		//values on both sides of the sign 
		_values[0] = atof((*prev(_iter)).c_str());
		_values[1] = atof((*next(_iter)).c_str());
		
		//new iterator result will be the element before the sign			
		_result = prev(_iter);
		
		//do calculation and change value before the sign to the calculation result
		switch (_sign) {
			case '+': *_result = to_string(_values[0] + _values[1]); break;
			case '-': *_result = to_string(_values[0] - _values[1]); break;
			case '*': *_result = to_string(_values[0] * _values[1]); break;
			case '/': *_result = to_string(_values[0] / _values[1]); break;
		}
		
		//removes from the vector the element next to the sign, and the sign
		_elements.erase(next(_iter)); _elements.erase(_iter);
		
		//finds next sign in vector
		_iter = find(_result, _elements.end(), string(1, _sign));	
	}	
}

void Formula::Plus_Minus_Sign() {
	vector<char> _signs = {'+', '-'};
	char* _p;
	double _value(0);

	for (char _sign : _signs) {
		// iterator to find signs between elements
		vector<string>::iterator _iter = find(elements.begin(), elements.end(), string(1, _sign));
		
		//while the iterator isn't empty
		while (_iter != elements.end() && elements.size() > 1) {	
			//to add a +- sign to a number, if signs are preceded by a ( / *
			if (*prev(_iter) == "(" || *prev(_iter) == "/" || *prev(_iter) == "*") {
				//value after the sign 
				_value = strtod((*next(_iter)).c_str(), &_p);
				//if value after the sign is a number
				if (*_p == '\0') {
					//puts number on sign
					switch (_sign) {
						case '+': *_iter = to_string(_value * +1.0); break;
						case '-': *_iter = to_string(_value * -1.0); break;
					}
					//removes from the vector the number alone
					elements.erase(next(_iter));
					//finds next sign in vector
					_iter = find(_iter, elements.end(), string(1, _sign));	
				}
				else {
					//finds next sign in vector
					_iter = find(next(_iter), elements.end(), string(1, _sign));
				}
			}
			else {
				//finds next sign in vector
				_iter = find(next(_iter), elements.end(), string(1, _sign));
			}
		}
	}
}

/*
	Fast token reader for files

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef TOKEN_READER_H_
#define TOKEN_READER_H_

#include <iostream>
#include <fstream>

class token_reader {
	public:		
		token_reader(std::istream* in);
		~token_reader();
		
		bool is_missing;
	
		int skipValue();
		std::string readString();
		double readFloat();
		long long readInt();
		void gotoNextLine();
		char ch;


		template<typename T> inline void read(T& x) {
			throw "not implemented";
		}   
	
		inline bool isNewLine(char ch) {
			return ((ch == '\n') || (ch == 0) || (ch == 13) || (ch == 10));	
		}
	private:
  		const static int DEFAULT_BUFFER_SIZE = 1024;
				
		std::istream* in;
			
		int buffer_size;
		char* buffer;
		long long buffer_position;
		int buffer_length;
		
		
		inline char readChar() {
			buffer_position++;
			if (buffer_position >= buffer_length) {
				buffer_length = (*in).readsome(buffer, buffer_size);
				buffer_position = 0;
				if (buffer_length == 0) {
					return 0;	
				}					
			}
			return buffer[buffer_position];
		}
		inline bool isSeparator(char ch) {
			return ((ch == '\t') || (ch == ' '));
		}
		
};

template<> inline void token_reader::read<int>(int& x) {
	x = readInt();
}		   
template<> inline void token_reader::read<double>(double& x) {
	x = readFloat();
}    

void token_reader::gotoNextLine() {
	do {
		ch = readChar();
	} while (! isNewLine(ch));
}

int token_reader::skipValue() {
	do {
		ch = readChar();
		if (ch == 0) {
			return 0; 			
		} else if (isNewLine(ch)) {
			return 1; 
		} else if (isSeparator(ch)) {
			return 2;
		}
	} while (true);	
}


std::string token_reader::readString() {
	std::string result;
	do {
		ch = readChar();
		if ((ch == 0) || (isNewLine(ch)) || (isSeparator(ch))) {
			return result;		
		} else {
			result += ch;
		}
	} while (true);			
}
		
long long token_reader::readInt() {
	is_missing = false;
	bool is_beginning = true;
	bool is_negative = false;
	long long result = 0;
	do {
		ch = readChar();
		if (is_beginning && (isSeparator(ch) || isNewLine(ch))) {
			is_missing = true;
			return 0;
		} else if (is_beginning && (ch == '-')) {
			is_negative = true;
		} else if ((ch >= '0') && (ch <= '9')) {
			result = result * 10 + (ch-'0');	
		} else if (isSeparator(ch) || isNewLine(ch) || true) { // remark: this becomes the default case because every character e.g. ":" can be the separator for ints
			if (is_negative) {
				return -result;
			} else {
				return result;
			}
		} else {
			std::cout << "cannot read an integer. found character: " << ch << std::endl;
			throw;	
		}
		is_beginning = false;
	} while (true);	
}
	
double token_reader::readFloat() {
	is_missing = false;
	bool is_beginning = true;
	bool is_negative = false;
	double result = 0;
	long long number = 0;
	long long comma_number = 0;
	long long comma_divisor = 1;
	long long exp_number = 0;
	bool exp_is_negative = false;
	bool exp_beginning = false;
	const int PARSING_STATE_STD = 0;
	const int PARSING_STATE_COMMA = 1;
	const int PARSING_STATE_EXP = 2;
	int parsing_state = PARSING_STATE_STD;
	
	do {
		ch = readChar();
		if (is_beginning && (isSeparator(ch) || isNewLine(ch))) {
			is_missing = true;
			return 0;
		} else if (is_beginning && (ch == '-')) {
			is_negative = true;
		} else if (is_beginning && (ch == '+')) {
			is_negative = false;
		} else if (exp_beginning && (ch == '-')) {
			exp_is_negative = true;
			exp_beginning = false;
		} else if (exp_beginning && (ch == '+')) {
			exp_beginning = false;
		} else if ((ch >= '0') && (ch <= '9')) {
			if (parsing_state == PARSING_STATE_STD) {
				number = number * 10 + (ch-'0');
			} else if (parsing_state == PARSING_STATE_COMMA) {
				comma_number = comma_number * 10 + (ch-'0');
				comma_divisor *= 10;
			} else if (parsing_state == PARSING_STATE_EXP) {
				exp_number = exp_number * 10 + (ch-'0');
				exp_beginning = false;
			}
		} else if ((ch == '.') && (parsing_state == PARSING_STATE_STD)) {
			parsing_state = PARSING_STATE_COMMA;
		} else if (((ch == 'e') || (ch == 'E')) && (parsing_state == PARSING_STATE_COMMA)) {
			parsing_state = PARSING_STATE_EXP;
			exp_beginning = true;
		} else if (isSeparator(ch) || isNewLine(ch)) {
			result =  (double) comma_number/comma_divisor;
			result += number;
			if (parsing_state == PARSING_STATE_EXP) {
				for (int i = 0; i < exp_number; i++) {
					if (exp_is_negative) {
						result /= 10;
					} else {
						result *= 10;
					}	
				}	
			}
			if (is_negative) {
				return -result;
			} else {
				return result;
			}
		} else {
			std::cout << "cannot read this double. found character: " << ch << "(" << (int)ch << ") in state " << parsing_state << std::endl;
			throw;	
		}
		is_beginning = false;
	} while (true);	
}
		

token_reader::token_reader(std::istream* in) {
	this->in = in;
	buffer_size = DEFAULT_BUFFER_SIZE;
	buffer_length = 0;
	buffer_position = 0;
	buffer = new char[buffer_size];
}


token_reader::~token_reader() {
	if (buffer != NULL) {
		delete[] buffer;
	}
}


#endif /*TOKEN_READER_H_*/

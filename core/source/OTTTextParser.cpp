#include "OTTTextParser.hpp"

#include <core/OTTStringOps.hpp>

#include <iostream>
#include <sstream>
#include <algorithm>

#define debugStr std::cout << " OTTTextParser: [debug] "
#define errorStr std::cout << " OTTTextParser: [error] "
#define fatalErrorStr std::cout << " OTTTextParser: [fatal error!] "

namespace ott {

	unsigned short getUShort(const unsigned char& h, const unsigned char& l) {
		return (((0xFFFF & h) << 8) + l);
	}

	bool bitTest(const unsigned char& input, const unsigned char& bit) {
		return ((input & (0x1 << bit)) == (0x1 << bit));
	}

	bool bitTest(const unsigned short& input, const unsigned char& bit) {
		return ((input & (0x1 << bit)) == (0x1 << bit));
	}

	bool bitTest(const unsigned int& input, const unsigned char& bit) {
		return ((input & (0x1 << bit)) == (0x1 << bit));
	}

	void bitSet(unsigned char& input, const unsigned char& bit) {
		input |= (0x1 << bit);
	}

	void bitSet(unsigned short& input, const unsigned char& bit) {
		input |= (0x1 << bit);
	}

	void bitSet(unsigned int& input, const unsigned char& bit) {
		input |= (0x1 << bit);
	}

	void bitReset(unsigned char& input, const unsigned char& bit) {
		input &= ~(0x1 << bit);
	}

	void bitReset(unsigned short& input, const unsigned char& bit) {
		input &= ~(0x1 << bit);
	}

	void bitReset(unsigned int& input, const unsigned char& bit) {
		input &= ~(0x1 << bit);
	}

	unsigned char getBitmask(const unsigned char& low, const unsigned char& high) {
		unsigned char mask = 0;
		for (unsigned char i = low; i <= high; i++)
			mask |= (1 << i);
		return mask;
	}

	unsigned char CompareOperatorStrings(const std::string& str1, const std::string& str2);
}

ott::NumericalString::NumericalString(const unsigned int& val) :
	str(ott::UIntToString(val)),
	result(val),
	op(0x0),
	type(NUMTYPE::INTEGER),
	computed(false)
{
}

ott::NumericalString::NumericalString(const std::string& s, OperatorType_t* op_/*=0x0*/) :
	str(s),
	result(ott::GetUserInputUInt(s)),
	op(op_),
	type(NUMTYPE::INTEGER),
	computed(false)
{
}

std::string ott::NumericalString::Str() const {
	return (computed ? ott::UIntToString(result) : str);
}

void ott::NumericalString::SetResult(const NumericalString& res, const NUMTYPE& newType/* = NUMTYPE::NONE*/) {
	result = res.GetUInt();
	computed = true;
	if (newType != NUMTYPE::NONE)
		type = newType;
}

ott::NumericalString ott::ExternalVariable::Get() {
	NumericalString retval;
	switch (dtype) {
	case CPPTYPE::BOOL:
		retval.result = *(static_cast<bool*>(ptr));
		retval.type = NUMTYPE::BOOLEAN;
		break;
	case CPPTYPE::UINT8:
		retval.result = *(static_cast<unsigned char*>(ptr));
		break;
	case CPPTYPE::UINT16:
		retval.result = *(static_cast<unsigned short*>(ptr));
		break;
	case CPPTYPE::UINT32:
		retval.result = *(static_cast<unsigned int*>(ptr));
		break;
	case CPPTYPE::INT8:
		retval.result = *(static_cast<char*>(ptr));
		break;
	case CPPTYPE::INT16:
		retval.result = *(static_cast<short*>(ptr));
		break;
	case CPPTYPE::INT32:
		retval.result = *(static_cast<int*>(ptr));
		break;
	default:
		break;
	}
	return retval;
}

ott::NumericalString ott::ExternalVariable::Set(const NumericalString& rhs) {
	switch (dtype) {
	case CPPTYPE::BOOL:
		*(static_cast<bool*>(ptr)) = static_cast<bool>(rhs.result);
		break;
	case CPPTYPE::UINT8:
		*(static_cast<unsigned char*>(ptr)) = static_cast<unsigned char>(rhs.result);
		break;
	case CPPTYPE::UINT16:
		*(static_cast<unsigned short*>(ptr)) = static_cast<unsigned short>(rhs.result);
		break;
	case CPPTYPE::UINT32:
		*(static_cast<unsigned int*>(ptr)) = static_cast<unsigned int>(rhs.result);
		break;
	case CPPTYPE::INT8:
		*(static_cast<char*>(ptr)) = static_cast<char>(rhs.result);
		break;
	case CPPTYPE::INT16:
		*(static_cast<short*>(ptr)) = static_cast<short>(rhs.result);
		break;
	case CPPTYPE::INT32:
		*(static_cast<int*>(ptr)) = static_cast<int>(rhs.result);
		break;
	default:
		break;
	}
	return (*this);
}

ott::TextParser::TextParser() :
	debugMode(false),
	fatalError(false),
	maxOperatorLength(0),
	lastResult()
{
	// Add operators to the list, following order of precedence
	// Arithmetic operators
	AddArithmeticOperator("*", OPERATOR::MULTIPLY);
	AddArithmeticOperator("/", OPERATOR::DIVIDE);
	AddArithmeticOperator("%", OPERATOR::MODULUS);
	AddArithmeticOperator("+", OPERATOR::ADD);
	AddArithmeticOperator("-", OPERATOR::SUBTRACT);
	AddArithmeticOperator("<<", OPERATOR::BITSHIFT_LEFT);
	AddArithmeticOperator(">>", OPERATOR::BITSHIFT_RIGHT);
	AddArithmeticOperator("&", OPERATOR::BITWISE_AND);
	AddArithmeticOperator("^", OPERATOR::BITWISE_XOR);
	AddArithmeticOperator("|", OPERATOR::BITWISE_OR);

	// Logical operators
	AddLogicalOperator("<", OPERATOR::LESS_THAN);
	AddLogicalOperator(">", OPERATOR::GREATER_THAN);
	AddLogicalOperator("<=", OPERATOR::LESS_THAN_EQUAL);
	AddLogicalOperator(">=", OPERATOR::GREATER_THAN_EQUAL);
	AddLogicalOperator("==", OPERATOR::EQUALITY);
	AddLogicalOperator("!=", OPERATOR::NOT_EQUAL);
	AddLogicalOperator("&&", OPERATOR::LOGICAL_AND, NUMTYPE::BOOLEAN);
	AddLogicalOperator("||", OPERATOR::LOGICAL_OR, NUMTYPE::BOOLEAN);

	// Assignment operators
	AddAssignmentOperator("=", OPERATOR::ASSIGNMENT);
	AddAssignmentOperator("*=", OPERATOR::MULTIPLY);
	AddAssignmentOperator("/=", OPERATOR::DIVIDE);
	AddAssignmentOperator("%=", OPERATOR::MODULUS);
	AddAssignmentOperator("+=", OPERATOR::ADD);
	AddAssignmentOperator("-=", OPERATOR::SUBTRACT);
	AddAssignmentOperator("<<=", OPERATOR::BITSHIFT_LEFT);
	AddAssignmentOperator(">>=", OPERATOR::BITSHIFT_RIGHT);
	AddAssignmentOperator("&=", OPERATOR::BITWISE_AND);
	AddAssignmentOperator("^=", OPERATOR::BITWISE_XOR);
	AddAssignmentOperator("|=", OPERATOR::BITWISE_OR);

	// Number type name map for nicer error message output
	numTypes[NUMTYPE::NONE] = "none";
	numTypes[NUMTYPE::BOOLEAN] = "bool";
	numTypes[NUMTYPE::INTEGER] = "int";
}

ott::TextParser::TextParser(const std::string& str) :
	TextParser()
{
	this->Parse(str, lastResult);
}

bool ott::TextParser::FindOperatorHere(const std::string& input, const size_t& loc, OperatorType_t*& op) {
	std::string dummy = input.substr(loc, maxOperatorLength);
	unsigned char matchStrength;
	std::vector<std::pair<unsigned char, OperatorType_t*> > matches;
	for (auto iter = operators.begin(); iter != operators.end(); iter++) {
		matchStrength = ott::CompareOperatorStrings(dummy, iter->sName);
		if (matchStrength) {
			matches.push_back(std::pair<unsigned char, OperatorType_t*>(matchStrength, iter->Get()));
		}
	}
	unsigned char maxStrength = 0;
	for (auto match = matches.begin(); match != matches.end(); match++) {
		if (match->first > maxStrength) {
			op = match->second;
			maxStrength = match->first;
		}
	}
	if (debugMode && maxStrength > 1)
		debugStr << " found long operator at index=" << loc << " in input string, strength=" << (int)maxStrength << std::endl;
	return (maxStrength > 0);
}

void ott::TextParser::SplitByOperators(const std::string& str, std::vector<NumericalString>& parts) {
	std::string current = "";
	OperatorType_t* op = 0x0;
	for (size_t i = 0; i < str.length(); i++) {
		if (FindOperatorHere(str, i, op)) {
			parts.push_back(NumericalString(current));
			parts.push_back(NumericalString(op->sName, op)); // operator
			current = "";
			if (op->sName.length() > 1) {
				i += op->sName.length() - 1;
			}
		}
		else
			current += str[i];
	}
	parts.push_back(NumericalString(current));
}

bool ott::TextParser::GetInput(const NumericalString& name, NumericalString& val) {
	if (name.computed) { // Value was computed in a previous operation
		val = name;
	}
	else if (ott::StringIsNumeric(name.str) == true) {
		val = name.result;
	}
	else {
		auto def = defines.find(name.str);
		if (def != defines.end()) {
			val = def->second;
		}
		else if ((def = tempDefines.find(name.str)) != tempDefines.end()) {
			val = def->second;
		}
		else {
			auto ext = externalDefines.find(name.str);
			if (ext != externalDefines.end()) {
				val = ext->second.Get();
			}
			else {
				errorStr << " variable (" << name.str << ") is not defined!\n";
				fatalError = true;
				return false;
			}
		}
	}
	return true;
}

bool ott::TextParser::Compute(NumericalString& lhs, const NumericalString& rhs, const NumericalString& op) {
	if (!lhs.CheckTypes(op, rhs)) {
		errorStr << " operand type mismatch for operator " << op.str << ", \'" << numTypes[lhs.type] << "\' and \'" << numTypes[rhs.type] << "\'" << std::endl;
		fatalError = true;
		return false;
	}
	NumericalString* ptr = 0x0;
	NumericalString valL = 0;
	NumericalString valR = 0;
	if (op.IsAssignmentOperator()) {
		if (lhs.computed || ott::StringIsNumeric(lhs.str) == true) {
			errorStr << " cannot assign to numerical value (" << lhs.str << ")!\n";
			fatalError = true;
			return false;
		}
		else {
			auto def = defines.find(lhs.str);
			if (def != defines.end()) {
				valL = def->second;
				ptr = &def->second;
			}
			else {
				auto ext = externalDefines.find(lhs.str);
				if (ext != externalDefines.end()) {
					valL = ext->second.Get();
					ptr = &ext->second;
				}
				else if (op.Operator() == OPERATOR::ASSIGNMENT) { // New definition
					this->AddDefinition(lhs.str, 0);
					def = defines.find(lhs.str);
					valL = def->second;
					ptr = &def->second;
				}
				else {
					errorStr << " variable (" << lhs.str << ") is not defined!\n";
					fatalError = true;
					return false;
				}
			}
		}
	}
	else if (this->GetInput(lhs, valL) == false) { // Get left hand value
		errorStr << " variable (" << lhs.str << ") is not defined!\n";
		fatalError = true;
		return false;
	}
	if (this->GetInput(rhs, valR) == false) { // Get right hand value
		errorStr << " variable (" << rhs.str << ") is not defined!\n";
		fatalError = true;
		return false;
	}
	NumericalString result = 0;
	switch (op.Operator()) {
	case OPERATOR::NONE: // No operation
		break;
	case OPERATOR::MULTIPLY:
		result = valL * valR;
		break;
	case OPERATOR::DIVIDE:
		result = valL / valR;
		break;
	case OPERATOR::MODULUS:
		result = valL % valR;
		break;
	case OPERATOR::ADD:
		result = valL + valR;
		break;
	case OPERATOR::SUBTRACT:
		result = valL - valR;
		break;
	case OPERATOR::BITSHIFT_LEFT:
		result = valL << valR;
		break;
	case OPERATOR::BITSHIFT_RIGHT:
		result = valL >> valR;
		break;
	case OPERATOR::LESS_THAN:
		result = (valL < valR);
		break;
	case OPERATOR::GREATER_THAN:
		result = (valL > valR);
		break;
	case OPERATOR::LESS_THAN_EQUAL:
		result = (valL <= valR);
		break;
	case OPERATOR::GREATER_THAN_EQUAL:
		result = (valL >= valR);
		break;
	case OPERATOR::EQUALITY:
		result = (valL == valR);
		break;
	case OPERATOR::NOT_EQUAL:
		result = (valL != valR);
		break;
	case OPERATOR::BITWISE_AND:
		result = valL & valR;
		break;
	case OPERATOR::BITWISE_XOR:
		result = valL ^ valR;
		break;
	case OPERATOR::BITWISE_OR:
		result = valL | valR;
		break;
	case OPERATOR::LOGICAL_AND:
		result = ((valL != 0) && (valR != 0));
		break;
	case OPERATOR::LOGICAL_OR:
		result = ((valL != 0) || (valR != 0));
		break;
	case OPERATOR::ASSIGNMENT:
		result = valR;
		break;
	default:
		break;
	}
	if (op.IsAssignmentOperator()) { // Assignment operators
		ptr->Set(result);
	}
	if (debugMode)
		debugStr << "  >compute(): l=" << valL.GetUInt() << ", r=" << valR.GetUInt() << ", op=" << op.str << ", result=" << result.GetUInt() << std::endl;
	lhs.SetResult(result, op.ResultType()); // store the result into the left operand
	return true;
}

bool ott::TextParser::Evaluate(const unsigned int& index, const std::string& str) {
	if (debugMode) {
		debugStr << " [in] @[" << index << "]=\"" << str << "\"" << std::endl;
	}
	std::vector<NumericalString> parts;
	std::vector<NumericalString>::iterator iter;
	this->SplitByOperators(str, parts);
	if (parts.size() % 2 == 0) {
		fatalErrorStr << " OTTTextParser: invalid number of arguments passed to eval()" << std::endl;
		fatalError = true;
		return false;
	}
	for (auto op = operators.begin(); op != operators.end(); op++) { // Order of operations
		if (fatalError) {
			fatalErrorStr << " OTTTextParser: encountered fatal error while parsing string" << std::endl;
			fatalError = true;
			return false;
		}
		while ((iter = std::find(parts.begin(), parts.end(), op->sName)) != parts.end()) {
			if (debugMode) {
				std::string strToEvaluate = (iter - 1)->Str() + iter->Str() + (iter + 1)->Str();
				debugStr << "  eval(): compute(\"" << strToEvaluate << "\")" << std::endl;
			}
			if (this->Compute(*(iter - 1), *(iter + 1), *iter) == false) { // compute the result and store it into (iter-1)
				break;
			}
			parts.erase(iter, iter + 2); // erase iter and iter+1
		}
	}
	lastResult = parts.front();
	if (debugMode)
		debugStr << " [out] @[" << index << "]=" << lastResult.result << "" << std::endl;
	this->AddTempDefinition(index, lastResult);
	return true;
}

void ott::TextParser::AddTempDefinition(const unsigned int& index, const NumericalString& value) {
	std::stringstream repstr;
	repstr << "@[" << index << "]";
	tempDefines[repstr.str()] = value;
}

void ott::TextParser::AddArithmeticOperator(const std::string& name, const OPERATOR& op, const NUMTYPE& type) {
	operators.push_back(OperatorType_t(name, op, OPERATION::ARITHMETIC));
	operators.back().operands = type;
	if (name.length() > maxOperatorLength)
		maxOperatorLength = (unsigned int)name.length();
}

void ott::TextParser::AddLogicalOperator(const std::string& name, const OPERATOR& op, const NUMTYPE& type) {
	operators.push_back(OperatorType_t(name, op, OPERATION::LOGICAL));
	operators.back().operands = type;
	operators.back().result = NUMTYPE::BOOLEAN;
	if (name.length() > maxOperatorLength)
		maxOperatorLength = (unsigned int)name.length();
}

void ott::TextParser::AddAssignmentOperator(const std::string& name, const OPERATOR& op, const NUMTYPE& type) {
	operators.push_back(OperatorType_t(name, op, OPERATION::ASSIGNMENT));
	operators.back().operands = type;
	if (name.length() > maxOperatorLength)
		maxOperatorLength = (unsigned int)name.length();
}

bool ott::TextParser::StringIsExpression(const std::string& str) {
	OperatorType_t* op = 0x0;
	for (size_t i = 0; i < str.length(); i++) {
		if (this->FindOperatorHere(str, i, op)) {
			return true;
		}
	}
	return false;
}

bool ott::TextParser::Parse(const std::string& str, NumericalString& result) {
	tempDefines.clear();
	fatalError = false;
	unsigned int c1 = ott::OccurancesOfString(str, '(');
	unsigned int c2 = ott::OccurancesOfString(str, ')');
	if (c1 < c2) {
		errorStr << " extraneous ')' encountered" << std::endl;
		fatalError = true;
		return false;
	}
	if (c1 > c2) {
		errorStr << " expected ')' at end of input" << std::endl;
		fatalError = true;
		return false;
	}
	std::string retval = ott::StripAllWhitespace(str);
	int count = 0;
	while (!fatalError) {
		std::stringstream repstr;
		repstr << "@[" << count << "]";
		std::string enc = ott::ExtractString(retval, '(', ')', repstr.str());
		if (enc.empty())
			break;
		if (debugMode)
			debugStr << " eval(\"" << enc << "\")" << std::endl;
		this->Evaluate(count++, enc);
	}
	if (!fatalError) {
		if (debugMode)
			debugStr << " eval(\"" << retval << "\")" << std::endl;
		this->Evaluate(count, retval);
		result = lastResult;
		return true;
	}
	return false;
}

/*
 * Private functions
 */

unsigned char ott::CompareOperatorStrings(const std::string& str1, const std::string& str2) {
	unsigned char retval = 0;
	bool mismatch = false;
	for (size_t i = 0; i < str2.length(); i++) {
		if (str1[i] != str2[i]) {
			mismatch = true;
			break;
		}
	}
	if (!mismatch) {
		ott::bitSet(retval, (unsigned char)(str2.length() - 1));
	}
	return retval;
}

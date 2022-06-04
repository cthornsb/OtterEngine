#include <cmath>

#include "PianoKeys.hpp"

float PianoKeys::Keyboard::getFrequency(const Key& key, const Modifier& mod/*=Modifier::NONE*/, const int32_t& octave/*=4*/){
	const float twelfthRootOfTwo = 1.0594631f;
	const float A0 = 27.5f; // Frequency of A0 key, in Hz

	// Octaves are counted from C
	// But the standard frequency is computed from A
	// So we subtract one from the octave to get the previous A key
	float An = A0 * std::pow(2.f, octave - 1);
	int32_t deltaKey;
	
	// Compute how many steps target key is from the A key
	switch(key){
	case Key::A:
		deltaKey = 0;
		break;
	case Key::B:
		deltaKey = 2;
		break;
	case Key::C:
		deltaKey = 3;
		break;
	case Key::D:
		deltaKey = 5;
		break;
	case Key::E:
		deltaKey = 7;
		break;
	case Key::F:
		deltaKey = 8;
		break;
	case Key::G:
		deltaKey = 10;
		break;
	default:
		return 0.f;	
	}
	
	// Sharpen or flatten the key
	switch(mod){
	case Modifier::NONE:
		break;
	case Modifier::FLAT:
		deltaKey -= 1;
		break;
	case Modifier::SHARP:
		deltaKey += 1;
		break;
	default:
		return 0.f;
	}
	
	// Compute the frequency
	if(deltaKey > 0){ // Ascending (from A)
		return (An * std::pow(twelfthRootOfTwo, deltaKey));
	}
	else if(deltaKey < 0){ // Descending (from A)
		return (An / std::pow(twelfthRootOfTwo, -deltaKey));
	}
	return An; // A key
}

PianoKeys::Keyboard::Keyboard(){
	// 128 notes, C(-1) to G9
	// Standard 88 key keyboard: 0x15 to 0x6c (A0 to C8)
	// One octave: C, C#, D, D#, E, F, F#, G, G#, A, A#, B
	const std::string octaves[11] = { "-1", "0", "1", "2", "3", "4", "5", "6", "7", "8", "9" };
	for (int32_t i = 0; i < 11; i++) { // Over 9 octaves
		frequencies.push_back(std::make_pair("C" + octaves[i], getFrequency(Key::C, Modifier::NONE, i - 1)));
		frequencies.push_back(std::make_pair("C#" + octaves[i], getFrequency(Key::C, Modifier::SHARP, i - 1)));
		frequencies.push_back(std::make_pair("D" + octaves[i], getFrequency(Key::D, Modifier::NONE, i - 1)));
		frequencies.push_back(std::make_pair("D#" + octaves[i], getFrequency(Key::D, Modifier::SHARP, i - 1)));
		frequencies.push_back(std::make_pair("E" + octaves[i], getFrequency(Key::E, Modifier::NONE, i - 1)));
		frequencies.push_back(std::make_pair("F" + octaves[i], getFrequency(Key::F, Modifier::NONE, i - 1)));
		frequencies.push_back(std::make_pair("F#" + octaves[i], getFrequency(Key::F, Modifier::SHARP, i - 1)));
		frequencies.push_back(std::make_pair("G" + octaves[i], getFrequency(Key::G, Modifier::NONE, i - 1)));
		if (i < 10) {
			frequencies.push_back(std::make_pair("G#" + octaves[i], getFrequency(Key::G, Modifier::SHARP, i - 1)));
			frequencies.push_back(std::make_pair("A" + octaves[i], getFrequency(Key::A, Modifier::NONE, i)));
			frequencies.push_back(std::make_pair("A#" + octaves[i], getFrequency(Key::A, Modifier::SHARP, i)));
			frequencies.push_back(std::make_pair("B" + octaves[i], getFrequency(Key::B, Modifier::NONE, i)));
		}
	}
}

float PianoKeys::Keyboard::getFrequency(const std::string& key){
	return 0.f;
}

std::string PianoKeys::Keyboard::getKeyName(const float& freq){
	std::string retval;
	getKeyName(freq, retval);
	return retval;
}

float PianoKeys::Keyboard::getKeyName(const float& freq, std::string& key){
	size_t closestMatch = 0;
	float delta = 999.f; // Percentage difference between input frequency
	for(size_t i = 0; i != frequencies.size(); i++){
		if(frequencies[i].second == freq){ // Exact match is unlikely but possible
			key = frequencies[i].first;
			return 0.f;
		}
		float df = std::abs(2.f * std::abs(frequencies[i].second - freq) / (frequencies[i].second + freq));
		if(df < delta){
			closestMatch = i;
			delta = df;
		}
	}
	key = frequencies[closestMatch].first;
	return delta;
}

uint8_t PianoKeys::Keyboard::getKeyNumber(const float& freq) const {
	uint8_t retval;
	getKeyNumber(freq, retval);
	return retval;
}

float PianoKeys::Keyboard::getKeyNumber(const float& freq, uint8_t& key) const {
	size_t closestMatch = 0;
	float delta = 999.f; // Percentage difference between input frequency
	for (size_t i = 0; i != frequencies.size(); i++) {
		if (frequencies[i].second == freq) { // Exact match is unlikely but possible
			key = (uint8_t)i;
			return 0.f;
		}
		float df = std::abs(2.f * std::abs(frequencies[i].second - freq) / (frequencies[i].second + freq));
		if (df < delta) {
			closestMatch = i;
			delta = df;
		}
	}
	key = (uint8_t)closestMatch;
	return delta;
}
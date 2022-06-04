#ifndef PIANO_KEYS_HPP
#define PIANO_KEYS_HPP

#include <vector>
#include <string>

namespace PianoKeys {
	enum class Key {
		NONE,
		A,
		B,
		C,
		D,
		E,
		F,
		G,
	};
	
	enum class Modifier{
		NONE,
		FLAT, // Step down
		SHARP // Step up
	};

	class Keyboard{
	public:
		/** Default constructor
		  */
		Keyboard();
		
		/** Get the midi note number whose ideal frequency is closest to the input frequency
		  */
		uint8_t operator () (const float& freq) const {
			uint8_t retval;
			getKeyNumber(freq, retval);
			return getKeyNumber(freq);
		}

		/** Get the ideal frequency for the key matching the input string
		  * Input string should have the format "kom" where 'k' is the key (a to g),
		  * 'o' is the octave (-1 to 9, 4 assumed if omitted), and 'm' is the modifier (# or b or none if omitted).
		  * Not implemented.
		  */
		float getFrequency(const std::string& key);

		/** Get the frequency of a midi keyboard key (0-127, C[-1] to G9)
		  */
		float getFrequency(const uint8_t& key) {
			return (key < 128 ? frequencies[key].second : -1.f);
		}		

		/** Get the key string whose ideal frequency is closest to the input frequency
		  */
		std::string getKeyName(const float& freq);

		/** Get the key string whose ideal frequency is closest to the input frequency
		  * @return The absolute difference between the input frequency and the ideal frequency of the returned key
		  */
		float getKeyName(const float& freq, std::string& key);

		/** Get the midi note number whose ideal frequency is closest to the input frequency
		  */
		uint8_t getKeyNumber(const float& freq) const;

		/** Get the midi note number whose ideal frequency is closest to the input frequency
		  * @return The absolute difference between the ideal frequency and the input frequency
		  */
		float getKeyNumber(const float& freq, uint8_t& key) const;

		/** Get the real frequency of a piano key
		  */
		static float getFrequency(const Key& key, const Modifier& mod = Modifier::NONE, const int32_t& octave = 4);

	private:
		std::vector<std::pair<std::string, float> > frequencies; ///< Standard keyboard has 88 keys, A0 to C8 (27.5 Hz ~ 4186 Hz)
	};
}

#endif

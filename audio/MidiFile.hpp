#ifndef MIDI_FILE_HPP
#define MIDI_FILE_HPP

#include <fstream>
#include <vector>
#include <deque>
#include <queue>

namespace ott {
	enum class MidiStatusType{
		NONE,
		RELEASED,      // 000 Note released
		PRESSED,       // 001 Note pressed
		POLYPRESSURE,  // 010 Aftertouch?
		CONTROLCHANGE, // 011 Control change
		PROGRAMCHANGE, // 100 Program change (midi instrument)
		CHANPRESSURE,  // 101 Channel pressure
		PITCHCHANGE,   // 110 Pitch wheel change
		CHANMESSAGE    // 011 Channel mode message
	};

	struct NoteData {
		bool bPressed;

		uint8_t nChannel;

		uint8_t nNote;

		uint8_t nVelocity;

		double dTime;
	};

	class MidiChunk {
	public:
		/** Default constructor
		  */
		MidiChunk(const std::string& format = "    ") :
			sType(format),
			nLength(0),
			nIndex(0)
		{
		}

		/** Read a midi chunk from an input stream
		  */
		MidiChunk(std::ifstream& f) :
			MidiChunk()
		{
			readMidiChunk(f);
		}

		/** Equality operator
		  */
		bool operator == (const std::string& name) const {
			return (name == sType);
		}

		/** Inequality operator
		  */
		bool operator != (const std::string& name) const {
			return (name != sType);
		}

		/** Direct chunk payload access
		  */
		uint8_t& operator [] (const size_t& index) {
			return data[index];
		}

		/** Direct chunk payload access
		  */
		uint8_t operator [] (const size_t& index) const {
			return data[index];
		}

		/** Get the type string of the chunk
		  */
		std::string getType() const {
			return sType;
		}

		/** Return true if chunk payload is empty
		  */
		bool empty() const {
			return (nLength == 0);
		}

		/** Get total length of chunk data (in bytes)
		  */
		uint16_t getLength() const {
			return nLength;
		}

		/** Get current byte index which will be read
		  */
		uint16_t getIndex() const {
			return nIndex;
		}

		/** Get number of bytes remaining to be read
		  */
		uint16_t getBytesRemaining() const {
			return (nLength - nIndex);
		}

		/** Skip the next N bytes
		  */
		void skipBytes(const uint32_t& N = 1) {
			nIndex += N;
		}

		/** Peek at the next byte but do not advance the byte index
		  */
		uint8_t peekByte() const {
			return data[nIndex];
		}

		/** Get an uint8_t from chunk data
		  * @return True if data retrieved successfully and return false if there was not enough data to read
		  */
		bool getUChar(uint8_t& val);

		/** Get an uint16_t from chunk data
		  * @return True if data retrieved successfully and return false if there was not enough data to read
		  */
		bool getUShort(uint16_t& val);

		/** Get an uint32_t from chunk data
		  * @return True if data retrieved successfully and return false if there was not enough data to read
		  */
		bool getUInt(uint32_t& val);

		/** Get a string of length len
		  * @return True if string retrieved successfully and return false if there were not enough bytes
		  */
		bool getString(std::string& str, const uint32_t& len);

		/** Copy chunk data into a generic destination pointer
		  * @return True if data retrieved successfully and return false if there were not enough bytes
		  */
		bool copyMemory(void* dest, const uint32_t& len);

		/** Set the midi chunk type string
		  */
		void setType(const std::string& type) {
			sType = type;
		}

		/** Push a byte onto the chunk
		  */
		void pushUChar(const uint8_t& val);

		/** Push a int16_t onto the chunk
		  * Bytes are reversed automatically before being pushed
		  */
		void pushUShort(const uint16_t& val);

		/** Push an int32_t onto the chunk
		  * Bytes are reversed automatically before being pushed
		  */
		void pushUInt(const uint32_t& val);

		/** Push a string onto the chunk
		  */
		void pushString(const std::string& str);

		/** Push 1-4 bytes onto the chunk
		  * Bytes are reversed automatically before being pushed
		  */
		void pushMemory(const uint32_t& src, const uint32_t& len);

		/** Push a variable size integer onto the chunk
		  * Between one and four 7-bit bytes will be pushed onto the chunk, depending on
		  * size of the input integer value. Maximum size of input is 28 bits (0x0fffffff).
		  */
		void pushVariableSize(const uint32_t& val);

		/** Read a midi chunk from an input stream
		  */
		bool readMidiChunk(std::ifstream& f);

		/** Write a midi chunk t an ouptut stream
		  */
		bool writeMidiChunk(std::ofstream& f);

		/** Clear chunk properties
		  */
		void clear();

		/** Read a variable size integer from the chunk
		  * Between one and four 7-bit bytes will be read, depending on the size of the
		  * integer value in the chunk. The maximum size which can be read is 28 bits (0x0fffffff).
		  */
		static uint32_t readVariableLength(MidiChunk& chunk);

	private:
		std::string sType; ///< Midi chunk type

		uint32_t nLength; ///< Midi chunk length (in bytes, excluding 8 byte header)

		uint32_t nIndex; ///< Current byte read index in chunk payload

		std::vector<uint8_t> data; ///< Midi chunk payload
	}; // class MidiChunk

	class MidiKey{
	public:
		/** Default constructor
		  */
		MidiKey() :
			bPressed(false),
			nChannel(0),
			nKeyNumber(0),
			nVelocity(0x40),
			nTime(0)
		{
		}

		/** Midi channel specifier constructor
		  */
		MidiKey(const uint8_t& ch) :
			bPressed(false),
			nChannel(ch & 0x0f),
			nKeyNumber(0),
			nVelocity(0x40),
			nTime(0)
		{
		}

		/** Pressed key event constructor
		  */
		MidiKey(const uint8_t& ch, const uint8_t& key, const uint8_t& velocity = 0x40) :
			bPressed(true),
			nChannel(ch & 0x0f),
			nKeyNumber(key & 0x7f),
			nVelocity(velocity & 0x7f),
			nTime(0)
		{
		}

		/** Timestamped pressed key event constructor
		  */
		MidiKey(const uint32_t& t, const uint8_t& ch, const uint8_t& key, const uint8_t& velocity = 0x40) :
			bPressed(true),
			nChannel(ch & 0x0f),
			nKeyNumber(key & 0x7f),
			nVelocity(velocity & 0x7f),
			nTime(t)
		{
		}

		/** Return true if the channel number and key number of this key event matches another key event
		  */
		bool operator == (const MidiKey& rhs) const;

		/** Equality operator
		  */
		bool operator == (bool rhs) const {
			return (bPressed == rhs);
		}

		/** Equality operator
		  */
		bool operator != (bool rhs) const {
			return (bPressed != rhs);
		}

		/** Return true if the midi key is pressed down
		  */
		bool isPressed() const {
			return bPressed;
		}

		/** Get the time difference between this event's timestamp and an input time.
		  * Assumes that the specified time is greater than or equal to the timestamp of this event.
		  */
		uint32_t deltaTime(const uint32_t& t) const {
			return (t - nTime);
		}
		
		/** Get the current midi channel number (0 to 15)
		  */
		uint8_t getChannel() const {
			return nChannel;
		}
		
		/** Get the current midi key number (0 to 127)
		  */
		uint8_t getKeyNumber() const {
			return nKeyNumber;
		}
		
		/** Get the current velocity of a midi key press (0 to 127, 64 is default)
		  */
		uint8_t getKeyVelocity() const {
			return nVelocity;
		}
		
		/** Get the current midi clock time
		  */
		uint32_t getTime() const {
			return nTime;
		}
		
		/** Set to note press event
		  */
		virtual void press(){
			bPressed = true;
		}

		/** Set to note release event
		  */
		virtual void release(){
			bPressed = false;
		}
		
		/** Set the current midi channel number (0 to 15)
		  */ 
		void setChannel(const uint8_t& channel){
			nChannel = (channel & 0xf); // 0 to 15
		}
		
		/** Set the current midi key number (0 to 127)
		  */
		void setKeyNumber(const uint8_t& key){
			nKeyNumber = key;
		}
		
		/** Set the current velocity of a midi key press (0 to 127, 64 is default)
		  */
		void setKeyVelocity(const uint8_t& velocity){
			nVelocity = velocity;
		}		
		
		/** Set the event time in midi clock ticks
		  */
		void setTime(const uint32_t& t){
			nTime = t;
		}

		/** Set the midi channel, key number, velocity, and event time.
		  * If note velocity is not specified, the default velocity of 64 is used.
		  */
		void setNote(const uint32_t& t, const uint8_t& ch, const uint8_t& key, const uint8_t& velocity = 0x40);

		/** Copy midi channel, key number, velocity, and event time
		  */
		void setNote(const MidiKey& key);

		/** Write midi note event to midi track chunk.
		  * If bPressed is set, a Note On event is written, otherwise a Note Off event is written.
		  * @param trk Midi file track chunk to write midi event to
		  * @param t Midi time of previous event
		  */
		void writeTrackEvent(MidiChunk& trk, const uint32_t& t);

	protected:
		bool bPressed; ///< Flag indiciating whether or not this event represents a key being (1) pressed or (0) released
	
		uint8_t nChannel; ///< Midi channel (0 to 15)
	
		uint8_t nKeyNumber; ///< Midi key number (0 to 127)
		
		uint8_t nVelocity; ///< Midi key velocity (0 to 127)
		
		uint32_t nTime; ///< Midi event time in midi clock ticks
	};

	class MidiEvent {
	public:
		MidiEvent() :
			nType(0),
			nLength(0)
		{
		}

		/** Read a midi message event from an input track chunk.
		  * The event status byte will be read from the stream first.
		  */
		bool read2(MidiChunk& chunk) {
			uint8_t nRunningStatus;
			chunk.getUChar(nRunningStatus);
			return this->read(chunk, nRunningStatus);
		}

		/** Read a midi message event from an input track chunk
		  */
		virtual bool read(MidiChunk& chunk, const uint8_t& nRunningStatus) {
			return false;
		}

		/** Write a midi message event to an output track chunk
		  */
		virtual bool write(MidiChunk&) {
			return false;
		}

	protected:
		uint8_t nType; ///< Event type

		uint32_t nLength; ///< Variable length
	};

	class MidiMessage : public MidiEvent, public MidiKey {
	public:
		/** Default constructor
		  */
		MidiMessage() :
			MidiEvent(),
			MidiKey(),
			nDeltaTime(0),
			nStatus(MidiStatusType::NONE)
		{
		}

		/** Pressed or released key event
		  */
		MidiMessage(const MidiKey& key, const uint32_t& prevTime) :
			MidiEvent(),
			MidiKey(key),
			nDeltaTime(nTime - prevTime),
			nStatus(key.isPressed() ? MidiStatusType::PRESSED : MidiStatusType::RELEASED)
		{
		}

		/** Chunk constructor (read)
		  */
		MidiMessage(MidiChunk& chunk) :
			MidiEvent(),
			MidiKey(),
			nDeltaTime(0),
			nStatus(MidiStatusType::NONE)
		{
			read2(chunk);
		}

		/** Get the current midi message status
		  */
		MidiStatusType getStatus() const {
			return nStatus;
		}

		/** Get the delta-time of the midi message
		  */
		uint32_t getDeltaTime() const {
			return nDeltaTime;
		}
		
		/** Set the current midi message status
		  */
		void setStatus(const MidiStatusType& status){
			nStatus = status;
		}
		
		/** Set midi event delta-time
		  * @param t Time of previous event
		  */
		void setDeltaTime(const uint32_t& t){
			nDeltaTime = nTime - t;
		}

		/** Set to note press event
		  */
		void press() override {
			bPressed = true;
			nStatus = MidiStatusType::PRESSED;
		}

		/** Set to note release event
		  */
		void release() override {
			bPressed = false;
			nStatus = MidiStatusType::RELEASED;
		}

		/** Read a midi message event from an input track chunk
		  */
		bool read(MidiChunk& chunk, const uint8_t& nRunningStatus) override;
		
		/** Write a midi message event to an output track chunk
		  */
		bool write(MidiChunk& chunk) override;
		
	private:
		uint32_t nDeltaTime; ///< Number of midi clock ticks since last midi message
	
		MidiStatusType nStatus; ///< Midi status [0, 15]
	}; // class MidiMessage
	
	class MidiSysExclusive : public MidiEvent {
	public:
		/** Default constructor
		  */
		MidiSysExclusive() :
			MidiEvent()
		{
		}
		
		/** Chunk constructor
		  */
		MidiSysExclusive(MidiChunk& chunk) :
			MidiEvent()
		{
			read2(chunk);
		}
		
		/** Read a sys-exclusive event from an input track chunk (0xf0)
		  */
		bool read(MidiChunk& chunk, const uint8_t& nRunningStatus) override;
	}; // class MidiSysExclusive
	
	class MidiMetaEvent : public MidiEvent {
	public:
		/** Default constructor
		  */
		MidiMetaEvent() :
			MidiEvent(),
			nTempo(0)
		{
		}
		
		/** Chunk constructor
		  */
		MidiMetaEvent(MidiChunk& chunk) :
			MidiEvent(),
			nTempo(0)
		{
			read2(chunk);
		}

		/** Get the current midi track tempo
		  */
		uint32_t getTempo() const {
			return nTempo;
		}

		/** Read a meta event from a midi chunk (0xff)
		  */
		bool read(MidiChunk& chunk, const uint8_t& nRunningStatus) override;

	private:
		uint32_t nTempo;
	}; // class MidiMetaEvent
	
	class TrackEvent{
	public:
		/** Default constructor
		  */
		TrackEvent() :
			nStatusByte(0),
			nProgramNumber(0),
			nStartTime(0xffffffff),
			nPrevTime(0),
			nDeltaTime(0),
			dSecondsPerTick(1),
			mMeta(),
			mSys(),
			mMsg(),
			messages()
		{
		}
		
		/** Chunk constructor
		  */
		TrackEvent(MidiChunk& chunk) :
			TrackEvent()
		{
			read(chunk, 0);
		}
		
		/** Get a pointer to the front of the note data buffer, or a null pointer if the buffer is empty
		  */
		NoteData* operator () () {
			return (!messages.empty() ? &messages.front() : 0x0);
		}

		/** Return true if note data buffer is empty
		  */
		bool empty() const {
			return messages.empty();
		}

		/** Get the current midi program number (instrument type)
		  */
		uint8_t getProgramNumber() const {
			return nProgramNumber;
		}
		
		/** Get the midi clock time at the start of event
		  */
		uint32_t getStartTime() const {
			return nStartTime;
		}

		/** Get the midi clock delta time since the last event
		  */
		uint32_t getDeltaTime() const {
			return nDeltaTime; 
		}
		
		/** Set midi program number (instrument type)
		  */
		void setProgramNumber(const uint8_t& prog){
			nProgramNumber = prog;
		}

		/** Set the midi clock tick conversion factor (dt) such that T' = Tmidi * dt.
		  * Conversion factor should have units of seconds per clock tick.
		  */
		void setClockConversion(const double& dt) {
			dSecondsPerTick = dt;
		}

		/** Read a midi track event from an input track chunk
		  * @return True if at least two bytes remained in the chunk and return false otherwise
		  */
		bool read(MidiChunk& chunk, const uint32_t& t);

		/** Pop off the front of the note data buffer
		  */
		void pop() {
			messages.pop();
		}

	private:
		uint8_t nStatusByte;

		uint8_t nProgramNumber; ///< Midi program number (instrument)
	
		uint32_t nStartTime; ///< Start of midi time
	
		uint32_t nPrevTime; ///< Time of most recent note
		
		uint32_t nDeltaTime; ///< Midi event delta-time read from midi file

		double dSecondsPerTick; ///< Wall clock time per midi clock tick (seconds)

		MidiMetaEvent mMeta;

		MidiSysExclusive mSys;

		MidiMessage mMsg;

		std::queue<NoteData> messages;
	}; // class TrackEvent

	class MidiFile {
	public:
		MidiFile() :
			bSMPTE(false),
			nTime(0),
			nPrevTime(0),
			nFormat(0), // Single track midi format
			nTracks(0),
			nDivision(24), // 24 midi clock ticks per quarter note
			nDeltaTicksPerQuarter(24),
			nTicksPerFrame(40),
			nTimeFormat(25),
			header("MThd"), // Header chunk
			tracks() // Track chunks
		{
		}

		/** Print midi file header information
		  */
		void print();

	protected:
		bool bSMPTE; ///< Set if exact time specifications are used (SMPTE mode)

		uint32_t nTime; ///< Global midi clock tick counter

		uint32_t nPrevTime; ///< Midi clock of previous event

		uint16_t nFormat; ///< Midi file format

		uint16_t nTracks; ///< Number of midi track chunks in file

		uint16_t nDivision; ///< Meaning of delta-times (number of midi clock ticks per quarter note)

		uint16_t nDeltaTicksPerQuarter; ///< Number of delta-time ticks per quarter note (non-SMPTE mode only)

		uint8_t nTicksPerFrame; ///< Number of midi clock ticks within a single frame (SMPTE mode only)

		uint8_t nTimeFormat; ///< SMPTE midi time code format (represents the number of frames per second in SMPTE mode)

		MidiChunk header; ///< Midi header chunk

		std::vector<MidiChunk> tracks; ///< Midi track chunk
	}; // class MidiFile

	class MidiFileRecorder : public MidiFile {
	public:
		/** Default file constructor
		  */
		MidiFileRecorder();

		/** Midi filename constructor
		  */
		MidiFileRecorder(const std::string& filename, const std::string& title = "Track");

		/** Destructor
		  */	
		~MidiFileRecorder() { 
		}
		
		/** Increment the global midi clock counter by one
		  */
		uint32_t operator ++ () {
			return (++nTime);
		}

		/** Return true if
		  */
		bool findNote(const MidiKey& key) const;

		/** Set the input clock to output midi clock conversion factor (M) such that Tin = Tmidi * M
		  */
		void setClockMultiplier(const float& clk) {
			fClockMultiplier = clk;
		}

		/** Set the minimum midi note length to 1/Nth notes (default is 32).
		  */
		void setMinimumNoteLength(const uint32_t& N) {
			// By default, there are 24 midi ticks per quarter note and thus 96 ticks per whole note
			nMinimumDuration = 96 / N;
		}

		/** Set midi file format and timing information
		  * @param format Midi file format (0, 1, or 2)
		  * @param division 
		  */
		void setMidiHeader(const uint16_t& format, const uint16_t& division);

		/** Update the global midi clock counter
		  */
		void updateMidiClock(const uint32_t& t);

		/** Press a key
		  */
		void press(MidiKey& key);
		
		/** Release a held key.
		  * If no key is pressed on the specified midi channel, nothing happens.
		  */
		void release(MidiKey& key);

		/** Set the midi program number (instrument)
		  * @param ch Midi channel [0, 15]
		  * @param nPC Midi program number [0, 127]
		  */
		void setMidiInstrument(const uint8_t& ch, const uint8_t& nPC);

		/** Finalize the midi track chunk and prepare it for writing to disk
		  * @param t Absolute final time in midi ticks
		  */
		void finalize(const uint32_t& t);

		/** Finalize midi file and write it to disk with a specified filename
		  * @param filename Path to midi file (uses sFilename if filename not specified)
		  */
		bool write(const std::string& filename);

		/** Add a new midi track and make it the currently active track
		  */
		void addNewTrack(const std::string& title);

	private:
		bool bFirstNote; ///< The next midi note event will be the first

		bool bFinalized; ///< The midi track chunk has been finalized and is ready for writing

		bool bOneNotePerChannel; ///< Set if simultaneous notes on a given midi channel will not be allowed

		uint32_t nMinimumDuration; ///< Shortest possible note duration (in midi clock ticks)

		float fClockMultiplier; ///< Input clock tick to midi clock conversion factor

		MidiChunk* currentTrack; ///< Pointer to currently active midi track chunk

		std::deque<MidiKey> vKeyBuffer; ///< 

		/** Set midi header
		  */
		void midiHeader();

		/** Set the midi track name string
		  */
		void midiTrackName(const std::string& title);

		/** [broken] Set midi tempo in beats per minute (bpm)
		  */
		void midiTempo(const uint16_t& bpm = 120);

		/** Set the midi time signature
		  * @param nn Time signature numerator
		  * @param dd Time signature denominator
		  * @param cc Number of midi clocks per metronome tick
		  * @param bb Number of 1/32 notes per 24 midi clock ticks
		  */
		void midiTimeSignature(const uint8_t& nn = 4, const uint8_t& dd = 4, const uint8_t& cc = 24, const uint8_t& bb = 8);

		/** Set the midi key signature
		  * @param sf Number of sharps or flats (0 is C key)
		  * @param minor Is this a minor key
		  */
		void midiKeySignature(const uint8_t& sf = 0, bool minor = false);

		/** Finalize the midi track chunk
		  * This flag is required by the midi format
		  */
		void midiEndOfTrack();
	}; // class MidiFileReader

	class MidiFilePlayer : public MidiFile {
	public:
		MidiFilePlayer() :
			MidiFile(),
			dPlaybackTime(0),
			event(),
			noteBuffer()
		{
		}

		/** Get a pointer to the time-sorted midi note message buffer
		  */
		std::deque<NoteData>& operator () () {
			return noteBuffer;
		}

		/** Check the time of the next note message in the buffer against the current midi playback time
		  * @return True if the note message buffer is not empty and the time of the next message is less than or equal to the current playback time
		  */
		bool checkTime() const;

		/** Read an input midi file with the specified filename
		  * @param filename Path to midi file (uses sFilename if filename not specified)
		  */
		bool read(const std::string& filename);

		/** Update the current playback time (in seconds)
		  */
		void update(const double& dt) {
			dPlaybackTime += dt;
		}

	protected:
		double dPlaybackTime; ///< Current midi playback time (in seconds)

		TrackEvent event; ///< 

		std::deque<NoteData> noteBuffer; ///< Buffer containing time-sorted midi note messages

		/** Read midi file header (MThd=0x4d546864)
		  */
		bool readHeaderChunk(MidiChunk& hdr);

		/** Read midi file track chunk (MTrk=0x4d54726b)
		  */
		bool readTrackChunk(MidiChunk& chunk);
	}; // class MidiFilePlayer

} // namespace ott

#endif

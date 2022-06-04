#include <iostream>
#include <cstring>
#include <cmath>

#include "MidiFile.hpp"
#include "PianoKeys.hpp"

/** Reverse the order of bytes of a 16-bit integer and return the result
  */
uint16_t reverseByteOrder(const uint16_t& input){
	uint16_t retval = 0;
	for(int32_t i = 0; i < 2; i++){
		retval += ((input & (0xff << (8 * i))) >> (8 * i)) << (16 - 8 * (i + 1));
	}
	return retval;
}

/** Reverse the order of bytes of a 32-bit integer and return the result
  */
uint32_t reverseByteOrder(const uint32_t& input){
	uint32_t retval = 0;
	for(int32_t i = 0; i < 4; i++){
		retval += ((input & (0xff << (8 * i))) >> (8 * i)) << (32 - 8 * (i + 1));
	}
	return retval;
}

/** Reverse the order of bytes of an input N-bit integer and return the result
  */
uint32_t reverseByteOrder(const uint32_t& input, const char& N){
	uint32_t retval = 0;
	for(int32_t i = 0; i < 4; i++){
		retval += ((input & (0xff << (8 * i))) >> (8 * i)) << (N - 8 * (i + 1));
	}
	return retval;
}

bool ott::MidiKey::operator == (const MidiKey& rhs) const {
	return (
		bPressed == rhs.bPressed &&
		nChannel == rhs.nChannel &&
		nKeyNumber == rhs.nKeyNumber
	);
}

void ott::MidiKey::setNote(const uint32_t& t, const uint8_t& ch, const uint8_t& key, const uint8_t& velocity/* = 0x40*/) {
	nChannel = (ch & 0xf); // 0 to 15
	nKeyNumber = key;
	nVelocity = velocity;
	nTime = t;
}

void ott::MidiKey::setNote(const MidiKey& key) {
	nChannel = key.getChannel();
	nKeyNumber = key.getKeyNumber();
	nVelocity = key.getKeyVelocity();
	nTime = key.getTime();
}

void ott::MidiKey::writeTrackEvent(MidiChunk& trk, const uint32_t& t){
	MidiMessage msg(*this, t);
	if (bPressed)
		msg.press();
	else
		msg.release();
	std::cout << " dt=" << msg.getDeltaTime() << std::endl;
	msg.write(trk);
}

bool ott::MidiChunk::getUChar(uint8_t& val) {
	if(nIndex + 1 >= nLength)
		return false;
	val = data[nIndex++];
	return true;
}

bool ott::MidiChunk::getUShort(uint16_t& val) {
	if(nIndex + 2 >= nLength)
		return false;
	memcpy(static_cast<void*>(&val), static_cast<void*>(&data[nIndex]), 2);
	val = reverseByteOrder(val); // High byte first
	nIndex += 2;
	return true;
}

bool ott::MidiChunk::getUInt(uint32_t& val) {
	if(nIndex + 4 >= nLength)
		return false;
	memcpy(static_cast<void*>(&val), static_cast<void*>(&data[nIndex]), 4);
	val = reverseByteOrder(val); // High byte first
	nIndex += 4;
	return true;
}
		
bool ott::MidiChunk::getString(std::string& str, const uint32_t& len){
	if(nIndex + len >= nLength)
		return false;
	str = std::string(len, ' '); // Reserve space in string
	memcpy(static_cast<void*>(&str[0]), static_cast<void*>(&data[nIndex]), len);
	nIndex += len;
	return true;
}

bool ott::MidiChunk::copyMemory(void* dest, const uint32_t& len){
	if(nIndex + len >= nLength)
		return false;
	memcpy(dest, static_cast<void*>(&data[nIndex]), len);
	nIndex += len;
	return true;
}
		
void ott::MidiChunk::pushUChar(const uint8_t& val){
	data.push_back(val);
	nLength++;
}
		
void ott::MidiChunk::pushUShort(const uint16_t& val){
	// MSB first!
	data.push_back((uint8_t)((val & 0xff00) >> 8));
	data.push_back((uint8_t)(val & 0x00ff));
	nLength += 2;
}

void ott::MidiChunk::pushUInt(const uint32_t& val){
	// MSB first!
	data.push_back((uint8_t)((val & 0xff000000) >> 24));
	data.push_back((uint8_t)((val & 0x00ff0000) >> 16));
	data.push_back((uint8_t)((val & 0x0000ff00) >> 8));
	data.push_back((uint8_t)(val & 0x000000ff));
	nLength += 4;
}

void ott::MidiChunk::pushString(const std::string& str){
	for(size_t i = 0; i < str.length(); i++){
		data.push_back((uint8_t)str[i]);
		nLength++;
	}
}

void ott::MidiChunk::pushMemory(const uint32_t& src, const uint32_t& len){
	//data.push_back((uint8_t)((src & 0xff000000) >> 24));
	data.push_back((uint8_t)((src & 0x00ff0000) >> 16));
	data.push_back((uint8_t)((src & 0x0000ff00) >> 8));
	data.push_back((uint8_t)(src & 0x000000ff));
	nLength += len;
}

void ott::MidiChunk::pushVariableSize(const uint32_t& val){
	// Maximum size is 28-bits (0x0fffffff)
	uint8_t nibs[4] = { // MSB first
		(uint8_t)((val & 0x0fe00000) >> 21),
		(uint8_t)((val & 0x001fc000) >> 14),
		(uint8_t)((val & 0x00003f80) >> 7),
		(uint8_t)((val & 0x0000007f))
	};
	if(nibs[3] != 0){ // Write 4 bytes
		pushUChar(nibs[0] | 0x80);
		pushUChar(nibs[1] | 0x80);
		pushUChar(nibs[2] | 0x80);
		pushUChar(nibs[3] & 0x7f);
	}
	else if(nibs[2] != 0){ // 3 bytes
		pushUChar(nibs[0] | 0x80);
		pushUChar(nibs[1] | 0x80);
		pushUChar(nibs[2] & 0x7f);
	}
	else if(nibs[1] != 0){ // 2 bytes
		pushUChar(nibs[0] | 0x80);
		pushUChar(nibs[1] & 0x7f);
	}
	else{ // 1 byte
		pushUChar(nibs[0] & 0x7f);
	}
	// This is less clear and only slightly shorter
	/*int32_t lastByte = 0;
	for(int32_t i = 3; i > 0; i--){ // Find last non-zero byte
		if(nibs[i]){
			lastByte = i;
			break;
		}
	}
	for(int32_t i = 0; i <= lastByte; i++){ // Write the bytes
		if(i != lastByte)
			pushUChar(nibs[i] & 0x7f);
		else
			pushUChar(nibs[i] | 0x80);
	}*/
}

bool ott::MidiChunk::readMidiChunk(std::ifstream& f){
	f.read((char*)&sType[0], 4); // Read chunk type
	f.read((char*)&nLength, 4); // Read header length, MSB first
	nLength = reverseByteOrder(nLength);
	if(f.eof() || !nLength)
		return false;
	data.reserve(nLength);
	f.read((char*)data.data(), nLength);
	nIndex = 0; // Reset index
	return (!f.eof());
}

bool ott::MidiChunk::writeMidiChunk(std::ofstream& f){
	if(!f.good())
		return false;
	f.write(sType.data(), 4); // Write chunk type
	uint8_t lengthBytes[4] = {
		(uint8_t)((nLength & 0xff000000) >> 24),
		(uint8_t)((nLength & 0x00ff0000) >> 16),
		(uint8_t)((nLength & 0x0000ff00) >> 8),
		(uint8_t)(nLength & 0x000000ff)
	};
	for(int32_t i = 0; i < 4; i++){ // Write header length, MSB first
		f.write((char*)&lengthBytes[i], 1);
	}
	f.write((char*)data.data(), nLength); // Write chunk body
	return true;
}

void ott::MidiChunk::clear(){
	sType = "    ";
	nLength = 0;
	data.clear();
}

uint32_t ott::MidiChunk::readVariableLength(MidiChunk& chunk){
	// MSB first
	uint32_t retval = 0;
	uint8_t bytes[4] = { 0xff, 0xff, 0xff, 0xff };
	for (int32_t i = 0; i < 4; i++) { // Four bytes maximum (28 bits of data)
		if (!chunk.getUChar(bytes[i])) // Not enough data
			return 0;
		if ((bytes[i] & 0x80) == 0) { // Read bytes until one is encountered with its 7th bit cleared
			break;
		}
	}
	if ((bytes[0] & 0x80) == 0) { // 7 bit number
		retval = bytes[0];
	}
	else if ((bytes[1] & 0x80) == 0) { // 14 bit number
		retval = bytes[1] + ((bytes[0] & 0x7f) << 7);
	}
	else if ((bytes[2] & 0x80) == 0) { // 21 bit number
		retval = bytes[2] + ((bytes[1] & 0x7f) << 7) + ((bytes[0] & 0x7f) << 14);
	}
	else if ((bytes[3] & 0x80) == 0) { // 28 bit number
		retval = bytes[3] + ((bytes[2] & 0x7f) << 7) + ((bytes[1] & 0x7f) << 14) + ((bytes[0] & 0x7f) << 21);
	}
	return retval;
}
	
bool ott::MidiMessage::read(MidiChunk& chunk, const uint8_t& nRunningStatus){
	nChannel = (nRunningStatus & 0xf);
	switch((nRunningStatus & 0x70) >> 4){
	case 0: // Note released
		chunk.getUChar(nKeyNumber);
		chunk.getUChar(nVelocity);
		nStatus = MidiStatusType::RELEASED;
		break;
	case 1: // Note pressed
		chunk.getUChar(nKeyNumber);
		chunk.getUChar(nVelocity);
		nStatus = MidiStatusType::PRESSED;
		break;
	case 2: // Polyphonic key pressure
		chunk.getUChar(nKeyNumber);
		chunk.getUChar(nVelocity);
		nStatus = MidiStatusType::POLYPRESSURE;
		break;
	case 3: // Control change
		chunk.skipBytes(2);
		nStatus = MidiStatusType::CONTROLCHANGE;
		break;
	case 4: // Program change
		chunk.skipBytes(1);
		nStatus = MidiStatusType::PROGRAMCHANGE;
		break;
	case 5: // Channel pressure
		chunk.skipBytes(1);
		nStatus = MidiStatusType::CHANPRESSURE;
		break;
	case 6: // Pitch wheel change
		chunk.skipBytes(2);
		nStatus = MidiStatusType::PITCHCHANGE;
		break;
	default:
		return false;
	};		
	return true;	
}

bool ott::MidiMessage::write(MidiChunk& chunk){
	// Write delta-time
	chunk.pushVariableSize(nDeltaTime);
	switch(nStatus){
	case MidiStatusType::RELEASED: // Note released
		std::cout << "release: " << nDeltaTime << " " << (int32_t)(0x80 | (0x0 << 4) | nChannel) << " " << (int32_t)nKeyNumber << " " << (int32_t)nVelocity << std::endl;
		chunk.pushUChar(0x80 | (0x0 << 4) | nChannel); // Status byte
		chunk.pushUChar(nKeyNumber);
		chunk.pushUChar(nVelocity);
		break;
	case MidiStatusType::PRESSED: // Note pressed
		std::cout << "press: " << nDeltaTime << " " << (int32_t)(0x80 | (0x1 << 4) | nChannel) << " " << (int32_t)nKeyNumber << " " << (int32_t)nVelocity << std::endl;
		chunk.pushUChar(0x80 | (0x1 << 4) | nChannel); // Status byte
		chunk.pushUChar(nKeyNumber);
		chunk.pushUChar(nVelocity);
		break;
	case MidiStatusType::POLYPRESSURE: // Polyphonic key pressure
		chunk.pushUChar(0x80 | (0x2 << 4) | nChannel); // Status byte
		chunk.pushUChar(nKeyNumber);
		chunk.pushUChar(nVelocity);
		break;
	case MidiStatusType::CONTROLCHANGE: // Control change
		chunk.pushUChar(0x80 | (0x3 << 4) | nChannel); // Status byte
		chunk.pushUShort(0);
		break;
	case MidiStatusType::PROGRAMCHANGE: // Program change
		chunk.pushUChar(0x80 | (0x4 << 4) | nChannel); // Status byte
		chunk.pushUChar(0);
		break;
	case MidiStatusType::CHANPRESSURE: // Channel pressure
		chunk.pushUChar(0x80 | (0x5 << 4) | nChannel); // Status byte
		chunk.pushUChar(0);
		break;
	case MidiStatusType::PITCHCHANGE: // Pitch wheel change
		chunk.pushUChar(0x80 | (0x6 << 4) | nChannel); // Status byte
		chunk.pushUShort(0);
		break;
	default:
		return false;
	};	
	return true;
}

bool ott::MidiSysExclusive::read(MidiChunk& chunk, const uint8_t& nRunningStatus){
	nLength = MidiChunk::readVariableLength(chunk);
	chunk.skipBytes(nLength);
	return true;
}
	
bool ott::MidiMetaEvent::read(MidiChunk& chunk, const uint8_t& nRunningStatus){
	uint8_t byte = 0;
	std::string message;
	chunk.getUChar(nType);
	nLength = MidiChunk::readVariableLength(chunk);
	switch(nType){
	case 0x00: // Sequence number
		chunk.skipBytes(2);
		break;
	case 0x01: // Text event
		chunk.getString(message, nLength);
		std::cout << " Text=" << message << std::endl;
		break;
	case 0x02: // Copyright
		chunk.getString(message, nLength);
		std::cout << " Copyright=" << message << std::endl;
		break;
	case 0x03: // Sequencer/Track name
		chunk.getString(message, nLength);
		std::cout << " Track=" << message << std::endl;
		break;
	case 0x04: // Instrument name
		chunk.getString(message, nLength);
		std::cout << " Instrument=" << message << std::endl;
		break;
	case 0x05: // Lyrics
		chunk.getString(message, nLength);
		std::cout << " Lyrics=" << message << std::endl;
		break;
	case 0x06: // Marker
		chunk.getString(message, nLength);
		std::cout << " Marker=" << message << std::endl;
		break;
	case 0x07: // Cue point
		chunk.getString(message, nLength);
		std::cout << " Cue=" << message << std::endl;
		break;
	case 0x20: // Midi channel prefix
		chunk.skipBytes(1);
		break;
	case 0x2f: // End of track
		std::cout << " END OF TRACK\n";
		break;
	case 0x51: // Tempo
		// 500000 is default
		chunk.copyMemory(static_cast<void*>(&nTempo), 3); // 24-bit (microseconds / quarter-note)
		nTempo = reverseByteOrder(nTempo, 24); // High byte first
		std::cout << " Tempo=" << nTempo << " (" << 60.f / (nTempo * 1E-6) << " bps)" << std::endl;
		break;
	case 0x54: // SMTPE offset
		chunk.skipBytes(5);
		break;
	case 0x58: // Time signature
		std::cout << " Time Signature:\n";
		chunk.getUChar(byte);
		std::cout << "  " << (int32_t)byte << "/";
		chunk.getUChar(byte);
		std::cout << std::pow(2, byte) << std::endl;
		chunk.getUChar(byte);
		std::cout << "  " << (int32_t)byte << " midi clocks per metronome tick\n";
		chunk.getUChar(byte);
		std::cout << "  " << (int32_t)byte << " 32nd notes per 24 midi clocks\n";
		break;
	case 0x59: // Key signature
		chunk.skipBytes(2);
		break;
	case 0x7f: // Sequencer meta event
		chunk.skipBytes(nLength);
		break;
	default:
		chunk.skipBytes(nLength);
		break;
	};
	return true;
}

bool ott::TrackEvent::read(MidiChunk& chunk, const uint32_t& t){
	// Read a byte
	if(chunk.getBytesRemaining() < 2)
		return false;
	nPrevTime = t;
	nDeltaTime = MidiChunk::readVariableLength(chunk);
	nStartTime = nPrevTime + nDeltaTime;
	uint8_t byte = chunk.peekByte();
	if(byte == 0xff){ // Meta-event
		nStatusByte = byte;
		mMeta.read2(chunk);
		dSecondsPerTick = mMeta.getTempo() * 1E-6 / 24; // (us / quarter) * (s / us) * (quarter / tick) = s / tick
		std::cout << " Seconds per tick = " << dSecondsPerTick << std::endl;
	}
	else if(byte == 0xf0){ // Sys-exclusive message
		nStatusByte = byte;
		mSys.read2(chunk);
	}
	else if((byte & 0x80) != 0){ // Midi message
		nStatusByte = byte;
		mMsg.read2(chunk);
		if (mMsg.getStatus() == MidiStatusType::PRESSED) {
			messages.push({ true, mMsg.getChannel(), mMsg.getKeyNumber(), mMsg.getKeyVelocity(), dSecondsPerTick * nStartTime });
			//std::cout << "  PRESS: " << dSecondsPerTick * nStartTime << std::endl; // seconds
		}
		else if (mMsg.getStatus() == MidiStatusType::RELEASED) {
			messages.push({ false, mMsg.getChannel(), mMsg.getKeyNumber(), mMsg.getKeyVelocity(), dSecondsPerTick * nStartTime });
			//std::cout << "  RELEASE: " << dSecondsPerTick * nStartTime << std::endl; // seconds
		}
	}
	else{ // Running status (use status from previous event)
		if (nStatusByte == 0xff) { // Meta-event
			mMeta.read(chunk, nStatusByte);
			dSecondsPerTick = mMeta.getTempo() * 1E-6 / 24; // (us / quarter) * (s / us) * (quarter / tick) = s / tick
			std::cout << " Seconds per tick = " << dSecondsPerTick << std::endl;
		}
		else if (nStatusByte == 0xf0) { // Sys-exclusive message
			mSys.read(chunk, nStatusByte);
		}
		else if ((nStatusByte & 0x80) != 0) { // Midi message
			mMsg.read(chunk, nStatusByte);
			if (mMsg.getStatus() == MidiStatusType::PRESSED) {
				messages.push({ true, mMsg.getChannel(), mMsg.getKeyNumber(), mMsg.getKeyVelocity(), dSecondsPerTick * nStartTime });
				//std::cout << "  PRESS: " << dSecondsPerTick * nStartTime << std::endl; // seconds
			}
			else if (mMsg.getStatus() == MidiStatusType::RELEASED) {
				messages.push({ false, mMsg.getChannel(), mMsg.getKeyNumber(), mMsg.getKeyVelocity(), dSecondsPerTick * nStartTime });
				//std::cout << "  RELEASE: " << dSecondsPerTick * nStartTime << std::endl; // seconds
			}
		}
	}
	return true;
}

/////////////////////////////////////////////////////////////////////
// class MidiFile
/////////////////////////////////////////////////////////////////////

void ott::MidiFile::print() {
	std::cout << " Format: " << nFormat << std::endl;
	std::cout << " Tracks: " << nTracks << std::endl;
	std::cout << " Division: " << nDivision << std::endl;
	if (bSMPTE)
		std::cout << " Ticks: " << (int32_t)nTimeFormat << " x " << (int32_t)nTicksPerFrame << " (" << nTimeFormat * nTicksPerFrame << " per second)" << std::endl;
	else
		std::cout << " Ticks: " << nDeltaTicksPerQuarter << " per quarter note" << std::endl;
}

/////////////////////////////////////////////////////////////////////
// class MidiFileRecorder
/////////////////////////////////////////////////////////////////////

ott::MidiFileRecorder::MidiFileRecorder() :
	MidiFile(),
	bFirstNote(true),
	bFinalized(false),
	bOneNotePerChannel(false),
	nMinimumDuration(3),
	fClockMultiplier(1.f),
	currentTrack(0x0),
	vKeyBuffer()
{
	// Add the initial midi track
	addNewTrack("Track");
}

ott::MidiFileRecorder::MidiFileRecorder(const std::string& filename, const std::string& title/* = "Track"*/) :
	MidiFileRecorder()
{
	// Add the initial midi track
	addNewTrack(title);
}

bool ott::MidiFileRecorder::findNote(const MidiKey& key) const {
	if (vKeyBuffer.empty()) // No notes in buffer
		return false;
	auto search = std::find(vKeyBuffer.begin(), vKeyBuffer.end(), key);
	return (search != vKeyBuffer.end());
}

void ott::MidiFileRecorder::updateMidiClock(const uint32_t& t) {
	// Check the current time
	uint32_t newTime = (uint32_t)(t * fClockMultiplier);
	if (bFirstNote) { // First recorded midi note
		bFirstNote = false;
		nPrevTime = newTime;
	}
	else if (newTime <= nTime) // Clock has not ticked yet
		return; 

	// Update the clock time
	nTime = newTime;

	while (!vKeyBuffer.empty()) {
		MidiKey* key = &vKeyBuffer.front();
		if (key->deltaTime(nTime) >= nMinimumDuration) { // Write a new midi note event
			std::cout << "ch=" << (int32_t)key->getChannel() << ", t=" << key->getTime() << ", prev=" << nPrevTime << ", dT=" << key->getTime() - nPrevTime << (key->isPressed() ? " PRESS" : " RELEASE") << std::endl;
			key->writeTrackEvent(*currentTrack, nPrevTime); // Write event to track chunk
			nPrevTime = key->getTime(); // Update the time of the last event
			vKeyBuffer.pop_front();
		}
		else { // Events are time ordered, no further events will be older than the minimum time duration
			break;
		}
	}
}

void ott::MidiFileRecorder::press(MidiKey& key) {
	uint8_t ch = key.getChannel();
	if (ch >= 16)
		return;

	if (bOneNotePerChannel) // Press / release event already on this channel, release it first
		release(key);

	// Add the note press event to the buffer
	key.press();
	key.setTime(nTime);
	vKeyBuffer.push_back(key);
}

void ott::MidiFileRecorder::release(MidiKey& key) {
	uint8_t ch = key.getChannel();
	if (ch >= 16)
		return;

	// Search the event buffer to check whether this channel is already playing a note.
	// If it is, it might need to be removed if the delta time is too short.
	if (bOneNotePerChannel) {
		for (std::deque<MidiKey>::iterator iter = vKeyBuffer.begin(); iter != vKeyBuffer.end(); iter++) {
			if ((iter->getChannel() != key.getChannel()) || !iter->isPressed())
				continue;
			if (iter->deltaTime(nTime) >= nMinimumDuration) {
				// Its duration is long enough so simply add a release event		
				key.release();
				vKeyBuffer.push_back(key);
			}
			else {
				// Its duration is too short, manually remove it from the event buffer
				std::cout << " PRESS: Note duration is too short! Removing it: delta=" << iter->deltaTime(nTime) << std::endl;
				vKeyBuffer.erase(iter);
			}
			break;
		}
	}

	// Add the note release event to the buffer
	key.release();
	key.setTime(nTime);
	vKeyBuffer.push_back(key);
}

void ott::MidiFileRecorder::setMidiInstrument(const uint8_t& ch, const uint8_t& nPC) {
	// Write midi program change event 
	if (ch >= 16)
		return;
	currentTrack->pushUChar(0x00); // Delta-time
	currentTrack->pushUChar(0xc0 | ch); // Program change (for channel=ch)
	currentTrack->pushUChar(nPC & 0x7f); // Instrument program number [0, 127]
}

void ott::MidiFileRecorder::finalize(const uint32_t& t) {
	while (!vKeyBuffer.empty()) { // Release any notes which are currently down
		MidiKey* key = &vKeyBuffer.front();
		if (key->isPressed()) {
			key->release();
			key->writeTrackEvent(*currentTrack, nPrevTime); // Write event to track chunk
			nPrevTime = key->getTime(); // Update the time of the last event
		}
		vKeyBuffer.pop_front();
	}
	midiEndOfTrack(); // Finalize track chunk (REQUIRED by midi format)
	bFinalized = true;
}

bool ott::MidiFileRecorder::write(const std::string& filename){
	if (!bFinalized)
		finalize(nTime);
	print(); // Print midi file header information
	midiHeader(); // Setup midi header chunk
	std::ofstream file(filename.c_str(), std::ios::binary);
	if(!file.good())
		return false;
	header.writeMidiChunk(file); // Write header chunk to file
	for (auto trk = tracks.begin(); trk != tracks.end(); trk++) {
		trk->writeMidiChunk(file); // Write track chunk to file
	}
	file.close();
	bFirstNote = true; // Reset in case we record more audio
	bFinalized = false;
	header.clear();
	tracks.clear();
	nTracks = 0;
	return true;
}

void ott::MidiFileRecorder::addNewTrack(const std::string& title) {
	// Close out the current track (if one exists)
	if (nTracks && !bFinalized)
		finalize(nTime);
	bFinalized = false;

	// Add a new track chunk
	tracks.push_back(MidiChunk("MTrk"));
	currentTrack = &tracks.front();

	// Setup new track chunk
	midiTrackName(title); // Midi track title
	if (!nTracks) { // Temporal data in first track chunk only
		midiTimeSignature(4, 4, 24, 8); // 4/4 
		midiKeySignature(0, false); // C Major
		midiTempo(120);
	}
	if (++nTracks > 1) // Multi-track midi file
		nFormat = 1;
}

void ott::MidiFileRecorder::midiHeader() {
	header.pushUShort(nFormat); // Midi format [0: Single track, 1: Simultaneous tracks, 2: Independent tracks]
	header.pushUShort(nTracks); // Number of tracks (equal to 1 for format 0)
	header.pushUShort(nDivision); // Number of midi clock ticks per quarter note
}

void ott::MidiFileRecorder::midiTrackName(const std::string& title) {
	// Write track title
	currentTrack->pushUChar(0x00); // Delta-time
	currentTrack->pushUChar(0xff);
	currentTrack->pushUChar(0x03);
	currentTrack->pushUChar((uint8_t)title.length());
	currentTrack->pushString(title);
}

void ott::MidiFileRecorder::midiTempo(const uint16_t& bpm/*=120*/) {
	// Write tempo ff 51 03 tt tt tt (microseconds / quarter notes)
	// Default tempo = 120 bpm (500000=0x7a120)
	uint32_t usPerQuarterNote = (uint32_t)((60.0 / bpm) * 1E6); // Microseconds per beat (quarter note)
	currentTrack->pushUChar(0x00); // Delta-time
	currentTrack->pushUChar(0xff);
	currentTrack->pushUChar(0x51);
	currentTrack->pushUChar(0x03);
	currentTrack->pushMemory(usPerQuarterNote, 3);
}

void ott::MidiFileRecorder::midiTimeSignature(const uint8_t& nn/*=4*/, const uint8_t& dd/*=4*/, const uint8_t& cc/*=24*/, const uint8_t& bb/*=8*/) {
	// Write time signature ff 58 04 nn dd cc bb
	currentTrack->pushUChar(0x00); // Delta-time
	currentTrack->pushUChar(0xff);
	currentTrack->pushUChar(0x58);
	currentTrack->pushUChar(0x04);
	currentTrack->pushUChar(nn); // Numerator
	currentTrack->pushUChar((uint8_t)std::log2(dd)); // Denominator
	currentTrack->pushUChar(cc); // 24 midi clocks per metronome tick
	currentTrack->pushUChar(bb); // 8 32nd notes per 24 midi clock ticks (1 quarter note)
}

void ott::MidiFileRecorder::midiKeySignature(const uint8_t& sf/*=0*/, bool minor/*=false*/) {
	// Write key signature ff 59 02 sf mi
	currentTrack->pushUChar(0x00); // Delta-time
	currentTrack->pushUChar(0xff);
	currentTrack->pushUChar(0x59);
	currentTrack->pushUChar(0x02);
	currentTrack->pushUChar(sf);
	currentTrack->pushUChar(minor ? 1 : 0);
}

void ott::MidiFileRecorder::midiEndOfTrack() {
	// Write end of track flag ff 2f 00 (required)
	currentTrack->pushUChar(0x00); // Delta-time
	currentTrack->pushUChar(0xff);
	currentTrack->pushUChar(0x2f);
	currentTrack->pushUChar(0x00);
}

/////////////////////////////////////////////////////////////////////
// class MidiFilePlayer
/////////////////////////////////////////////////////////////////////

bool ott::MidiFilePlayer::checkTime() const {
	if (noteBuffer.empty())
		return false;
	return (noteBuffer.front().dTime <= dPlaybackTime);
}

bool ott::MidiFilePlayer::read(const std::string& filename) {
	std::ifstream file(filename.c_str(), std::ios::binary);
	if (!file.good())
		return false;
	header.clear();
	tracks.clear();
	header.readMidiChunk(file); // Read header data
	readHeaderChunk(header); // Process header data
	print();
	tracks = std::vector<MidiChunk>(nTracks, MidiChunk());
	for (auto trk = tracks.begin(); trk != tracks.end(); trk++) { // Over all tracks
		nTime = 0; // Reset midi clock
		trk->readMidiChunk(file); // Read track data
		readTrackChunk(*trk); // Process track data
	}
	while (!event.empty()) { // Add any note messages to the buffer
		noteBuffer.push_back(*event());
		event.pop();
	}
	std::sort(
		noteBuffer.begin(),
		noteBuffer.end(),
		[](const NoteData& a, const NoteData& b) {
			return (a.dTime < b.dTime);
		}
	); // Sort all note messages
	std::cout << " size=" << noteBuffer.size() << std::endl;
	file.close();
	return true;
}

bool ott::MidiFilePlayer::readHeaderChunk(MidiChunk& hdr) {
	if ((hdr != "MThd") || hdr.getLength() < 6)
		return false;

	// Read 6 header bytes
	hdr.getUShort(nFormat);
	hdr.getUShort(nTracks);
	hdr.getUShort(nDivision);

	// Division code
	bSMPTE = ((nDivision & 0x8000) == 0x8000);
	if (bSMPTE) { // SMPTE format
		nTicksPerFrame = (nDivision & 0xff);
		nTimeFormat = ((nDivision & 0x7f00) >> 8);
	}
	else { // Metric time
		nDeltaTicksPerQuarter = nDivision & 0x7fff; // Delta-time format
	}

	return true;
}

bool ott::MidiFilePlayer::readTrackChunk(MidiChunk& chunk) {
	if ((chunk != "MTrk") || chunk.empty())
		return false;
	while (event.read(chunk, nTime)) {
		nTime = event.getStartTime();
	}
	return true;
}
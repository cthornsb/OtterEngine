# Compile source files
add_library(
	OtterAudioObj 
	OBJECT 
	AudioSampler.cpp
	MidiFile.cpp
	PianoKeys.cpp
	SoundBuffer.cpp
	SoundManager.cpp
	SoundMixer.cpp
	Synthesizers.cpp
	UnitTimer.cpp
	WavFile.cpp
)

target_compile_definitions(
	OtterAudioObj 
	PRIVATE 
	ENABLE_PORT_AUDIO
)

# Generate the audio library
if(NOT WIN32)
	add_library(
		OtterAudio
		SHARED 
		$<TARGET_OBJECTS:OtterAudioObj>
	)
else()
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library(
		OtterAudio
		STATIC 
		$<TARGET_OBJECTS:OtterAudioObj>
	)
endif()

if(INSTALL_LIBRARIES)
	install(
		TARGETS OtterAudio
		DESTINATION lib
	)
endif()

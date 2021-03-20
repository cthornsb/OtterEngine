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
add_library(
	OtterAudioStatic 
	STATIC 
	$<TARGET_OBJECTS:OtterAudioObj>
)

if(INSTALL_STATIC)
	install(TARGETS OtterAudioStatic DESTINATION lib)
endif(INSTALL_STATIC)

if(INSTALL_SHARED)
	add_library(
		OtterAudio
		SHARED
		$<TARGET_OBJECTS:OtterAudioObj>
	)
	install(TARGETS OtterAudio DESTINATION lib)
endif(INSTALL_SHARED)


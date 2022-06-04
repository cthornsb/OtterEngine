# Compile source files
add_library( OtterAudioObj 
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

# Target include directory
target_include_directories(OtterAudioObj PUBLIC ..)

target_compile_definitions( OtterAudioObj 
	PRIVATE 
	ENABLE_PORT_AUDIO
)

# Generate the audio library
if(WIN32 OR INSTALL_STATIC_LIBRARIES)
	# Building dlls on Windows is a nightmare, so we just won't do it
	add_library( OtterAudio
		STATIC 
		$<TARGET_OBJECTS:OtterAudioObj>
	)
else()
	add_library( OtterAudio
		SHARED 
		$<TARGET_OBJECTS:OtterAudioObj>
	)
endif()

if(INSTALL_LIBRARIES)
	# Install libraries
	install(
		TARGETS OtterAudio
		EXPORT OtterConfig
		LIBRARY DESTINATION ${INSTALL_LIB_DIR}
		ARCHIVE DESTINATION ${INSTALL_LIB_DIR}
		RUNTIME DESTINATION ${INSTALL_BIN_DIR}
		INCLUDES DESTINATION ${INSTALL_INCLUDE_DIR}
	)
endif()


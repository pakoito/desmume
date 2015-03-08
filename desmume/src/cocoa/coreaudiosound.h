/*
	Copyright (C) 2012-2015 DeSmuME team

	This file is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 2 of the License, or
	(at your option) any later version.

	This file is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with the this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _COREAUDIOSOUND_
#define _COREAUDIOSOUND_

#include <CoreServices/CoreServices.h>
#include <AudioUnit/AudioUnit.h>
#include <AudioToolbox/AudioToolbox.h>
#include <libkern/OSAtomic.h>
#include "ringbuffer.h"


typedef void (*CoreAudioInputHardwareGainChangedCallback)(float normalizedGain, void *inParam1, void *inParam2);

class CoreAudioInput
{
private:
	OSSpinLock *_spinlockAUHAL;
	
	CoreAudioInputHardwareGainChangedCallback _hwGainChangedCallbackFunc;
	void *_hwGainChangedCallbackParam1;
	void *_hwGainChangedCallbackParam2;
	
	AUGraph _auGraph;
	AUNode _auFormatConverterNode;
	AUNode _auOutputNode;
	
	AudioUnit _auFormatConverterUnit;
	
	float _inputGainNormalized;
	AudioUnitElement _inputElement;
	
	bool _isMuted;
	bool _isPaused;
	bool _isHardwareEnabled;
	bool _isHardwareLocked;
	
	OSStatus InitInputAUHAL(UInt32 deviceID);
	
public:
	AudioUnit _auHALInputDevice;
	AudioUnit _auOutputUnit;
	AudioTimeStamp _timeStamp;
	AudioBufferList *_captureBufferList;
	AudioBufferList *_convertBufferList;
	UInt32 _captureFrames;
	RingBuffer *_samplesCaptured;
	RingBuffer *_samplesConverted;
	
	CoreAudioInput();
	~CoreAudioInput();
	
	void Start();
	void Stop();
	size_t Pull();
	bool IsHardwareEnabled() const;
	bool IsHardwareLocked() const;
	bool GetMuteState() const;
	void SetMuteState(bool muteState);
	bool GetPauseState() const;
	void SetPauseState(bool pauseState);
	float GetGain() const;
	void SetGain(float normalizedGain);
	void SetGain_ValueOnly(float normalizedGain);
	void UpdateHardwareLock();
	void SetCallbackHardwareGainChanged(CoreAudioInputHardwareGainChangedCallback callbackFunc, void *inParam1, void *inParam2);
	void RunHardwareGainChangedCallback(float normalizedGain);
};

class CoreAudioOutput
{
private:
	AudioUnit _au;
	RingBuffer *_buffer;
	OSSpinLock *_spinlockAU;
	float _volume;
	
public:
	CoreAudioOutput(size_t bufferSamples, size_t sampleSize);
	~CoreAudioOutput();
	
	void start();
	void stop();
	void writeToBuffer(const void *buffer, size_t numberSampleFrames);
	void clearBuffer();
	size_t getAvailableSamples() const;
	void mute();
	void unmute();
	void pause();
	void unpause();
	float getVolume() const;
	void setVolume(float vol);
};

OSStatus CoreAudioInputCaptureCallback(void *inRefCon,
									   AudioUnitRenderActionFlags *ioActionFlags,
									   const AudioTimeStamp *inTimeStamp,
									   UInt32 inBusNumber,
									   UInt32 inNumberFrames,
									   AudioBufferList *ioData);

OSStatus CoreAudioInputReceiveCallback(void *inRefCon,
									   AudioUnitRenderActionFlags *ioActionFlags,
									   const AudioTimeStamp *inTimeStamp,
									   UInt32 inBusNumber,
									   UInt32 inNumberFrames,
									   AudioBufferList *ioData);

OSStatus CoreAudioInputConvertCallback(void *inRefCon,
									   AudioUnitRenderActionFlags *ioActionFlags,
									   const AudioTimeStamp *inTimeStamp,
									   UInt32 inBusNumber,
									   UInt32 inNumberFrames,
									   AudioBufferList *ioData);

OSStatus CoreAudioInputDeviceChanged(AudioObjectID inObjectID,
									 UInt32 inNumberAddresses,
									 const AudioObjectPropertyAddress inAddresses[],
									 void *inClientData);

void CoreAudioInputAUHALChanged(void *inRefCon,
								AudioUnit inUnit,
								AudioUnitPropertyID inID,
								AudioUnitScope inScope,
								AudioUnitElement inElement);

OSStatus CoreAudioOutputRenderCallback(void *inRefCon,
									   AudioUnitRenderActionFlags *ioActionFlags,
									   const AudioTimeStamp *inTimeStamp,
									   UInt32 inBusNumber,
									   UInt32 inNumberFrames,
									   AudioBufferList *ioData);

void CoreAudioInputDefaultHardwareGainChangedCallback(float normalizedGain, void *inParam1, void *inParam2);

bool CreateAudioUnitInstance(AudioUnit *au, ComponentDescription *auDescription);
#if defined(MAC_OS_X_VERSION_10_6) && MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_6
bool CreateAudioUnitInstance(AudioUnit *au, AudioComponentDescription *auDescription);
#endif
void DestroyAudioUnitInstance(AudioUnit *au);

#endif

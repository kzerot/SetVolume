/**

  Copyright (c) 2016, Dawid Ciecierski

  Based on and inspired by code shared by Larry Osterman of Microsoft at
  https://blogs.msdn.microsoft.com/larryosterman/2007/03/06/how-do-i-change-the-master-volume-in-windows-vista/

  This program and the accompanying materials
  are licensed and made available under the terms and conditions of the BSD License
  which accompanies this distribution.  The full text of the license may be found at
  http://opensource.org/licenses/bsd-license.php

  THE PROGRAM IS DISTRIBUTED UNDER THE BSD LICENSE ON AN "AS IS" BASIS,
  WITHOUT WARRANTIES OR REPRESENTATIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED.

**/

//#define WINVER       _WIN32_WINNT_VISTA
//#define _WIN32_WINNT _WIN32_WINNT_VISTA
//
//#define VERSION_MAJOR 2
//#define VERSION_MINOR 0

#include <cstdio>
#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include "SetVolume.h"
#include <iostream>


void InitializeAudioEndpoint(IAudioEndpointVolume **audioEndpoint) 
{
	HRESULT hr;

	// Initialize the COM library
	CoInitialize(nullptr);

	// Get audio device enumerator
	IMMDeviceEnumerator *deviceEnumerator = nullptr;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), nullptr, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	if (hr != S_OK) {
		printf("Unable to create instance of MMDeviceEnumerator (error code: 0x%08lx)\n", hr);
		CoUninitialize();
		exit(-1);
	}

	// Ask device enumerator for the default audio renderer
	IMMDevice *defaultDevice = nullptr;
	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);
	deviceEnumerator->Release();
	deviceEnumerator = nullptr;
	if (hr != S_OK) {
		printf("Unable to get default audio endpoint (error code: 0x%08lx)\n", hr);
		CoUninitialize();
		exit(-1);
	}
	
	// Ask default audio renderer for volume controller
	//IAudioEndpointVolume *endpointVolume = nullptr;
	hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, nullptr, (LPVOID *)audioEndpoint);
	defaultDevice->Release();
	defaultDevice = nullptr;
	if (hr != S_OK) {
		printf("Unable to get default audio volume controller (error code: 0x%08lx)\n", hr);
		CoUninitialize();
		exit(-1);
	}
}

void DestroyAudioEndpoint(IAudioEndpointVolume *endpointVolume) 
{
	endpointVolume->Release();
	CoUninitialize();
}

void SetVolume(IAudioEndpointVolume *endpoint, float volume)
{
	HRESULT hr;
	printf("Setting volume to: %.0f%%\n", volume * 100);
	hr = endpoint->SetMasterVolumeLevelScalar(volume, nullptr);
	if (hr != S_OK) {
		printf("Unable to set master volume (error code: 0x%08lx)\n", hr);
		DestroyAudioEndpoint(endpoint);
		exit(-1);
	}
}

float GetVolume(IAudioEndpointVolume *endpoint)
{
	HRESULT hr;
	float volume = 0;
	hr = endpoint->GetMasterVolumeLevelScalar(&volume);
	if (hr != S_OK) {
		printf("Unable to get master volume (error code: 0x%08lx)\n", hr);
		DestroyAudioEndpoint(endpoint);
		exit(-1);
	}
	return volume;
}

void SetMute(IAudioEndpointVolume *endpoint, BOOL newValue)
{
	HRESULT hr;
	hr = endpoint->SetMute(newValue, nullptr);
	if (hr != S_OK) {
		printf("Unable to set mute (error code: 0x%08lx)\n", hr);
		DestroyAudioEndpoint(endpoint);
		exit(-1);
	}
}

BOOL GetMute(IAudioEndpointVolume *endpoint)
{
	HRESULT hr;
	BOOL value;
	hr = endpoint->GetMute(&value);
	if (hr != S_OK) {
		printf("Unable to get mute status (error code: 0x%08lx)\n", hr);
		DestroyAudioEndpoint(endpoint);
		exit(-1);
	}
	return value;
}


void ChangeVolume(float newVolume)
{
	HRESULT hr;
	IAudioEndpointVolume* endpointVolume = nullptr;


	// Find devices for manipulating mixer volumes
	InitializeAudioEndpoint(&endpointVolume);

	if (newVolume < 0) newVolume = 0;
	if (newVolume > 1) newVolume = 1;
	SetVolume(endpointVolume, newVolume);

	// Cleanup
	DestroyAudioEndpoint(endpointVolume);

}

void Mute(bool mute) {

	HRESULT hr;
	IAudioEndpointVolume* endpointVolume = nullptr;


	// Find devices for manipulating mixer volumes
	InitializeAudioEndpoint(&endpointVolume);

	if (mute) {
		BOOL currentValue = GetMute(endpointVolume);
		if (currentValue != TRUE)
			SetMute(endpointVolume, TRUE);
	}
	else {
		BOOL currentValue = GetMute(endpointVolume);
		if (currentValue != FALSE)
			SetMute(endpointVolume, FALSE);
	}

	DestroyAudioEndpoint(endpointVolume);
}

void Test() {
	std::cout << "Test" << std::endl;
}
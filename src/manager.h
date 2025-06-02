#pragma once

#include "shared.h"

void InitializeSharedData(SharedData& shared, int arraySize, int markerCount);
void CleanupSharedData(SharedData& shared);
void StartAllMarkers(SharedData& shared);
void WaitAllMarkersBlocked(SharedData& shared);
void ResumeAllExcept(SharedData& shared, int excludeId);
void TerminateMarker(SharedData& shared, int threadId);
void WaitMarkerTerminated(SharedData& shared, int threadId);
void PrintArray(SharedData& shared);

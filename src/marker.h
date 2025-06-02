#pragma once

#include "shared.h"

DWORD WINAPI MarkerThreadProc(LPVOID lpParam);

void InitializeMarkerThread(MarkerThreadContext& context, int threadId, SharedData* sharedData);

bool TryMarkIndex(SharedData* sharedData, int threadId, int index);
void UnmarkIndices(SharedData* sharedData, const std::vector<int>& indices);

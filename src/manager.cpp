#include "manager.h"
#include "marker.h"
#include <iostream>
#include <windows.h>

void InitializeSharedData(SharedData& shared, int arraySize, int markerCount) {
    shared.size = arraySize;
    shared.array = new int[arraySize] {};
    InitializeCriticalSection(&shared.arrayLock);
    InitializeCriticalSection(&shared.logLock);
    shared.markers.resize(markerCount);

    for (int i = 0; i < markerCount; ++i) {
        InitializeMarkerThread(shared.markers[i], i + 1, &shared);
    }
}

void CleanupSharedData(SharedData& shared) {
    for (auto& marker : shared.markers) {
        CloseHandle(marker.canStartEvent);
        CloseHandle(marker.canContinueEvent);
        CloseHandle(marker.terminateEvent);
        CloseHandle(marker.blockedEvent);
        CloseHandle(marker.threadHandle);
    }

    DeleteCriticalSection(&shared.arrayLock);
    DeleteCriticalSection(&shared.logLock);
    delete[] shared.array;
    shared.array = nullptr;
    shared.size = 0;
    shared.markers.clear();
}

void StartAllMarkers(SharedData& shared) {
    for (auto& marker : shared.markers) {
        SetEvent(marker.canStartEvent);
    }
}

void WaitAllMarkersBlocked(SharedData& shared) {
    std::vector<HANDLE> events;
    for (auto& marker : shared.markers) {
        if (!marker.terminated) {
            events.push_back(marker.blockedEvent);
        }
    }
    WaitForMultipleObjects(static_cast<DWORD>(events.size()), events.data(), TRUE, INFINITE);
}

void ResumeAllExcept(SharedData& shared, int excludeId) {
    for (auto& marker : shared.markers) {
        if (!marker.terminated && marker.threadId != excludeId) {
            ResetEvent(marker.blockedEvent);
            SetEvent(marker.canContinueEvent);
        }
    }
}

void TerminateMarker(SharedData& shared, int threadId) {
    for (auto& marker : shared.markers) {
        if (marker.threadId == threadId && !marker.terminated) {
            SetEvent(marker.terminateEvent);
            return;
        }
    }
}

void WaitMarkerTerminated(SharedData& shared, int threadId) {
    for (auto& marker : shared.markers) {
        if (marker.threadId == threadId) {
            WaitForSingleObject(marker.threadHandle, INFINITE);
            return;
        }
    }
}

void PrintArray(SharedData& shared) {
    EnterCriticalSection(&shared.arrayLock);
    std::cout << "[Array] ";
    for (int i = 0; i < shared.size; ++i) {
        std::cout << shared.array[i] << " ";
    }
    std::cout << "\n";
    LeaveCriticalSection(&shared.arrayLock);
}

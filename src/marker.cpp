#include "marker.h"
#include <windows.h>
#include <iostream>
#include <thread>
#include <chrono>
#include <random>

static SharedData* g_shared = nullptr;

void InitializeMarkerThread(MarkerThreadContext& context, int threadId, SharedData* sharedData) {
    context.threadId = threadId;
    context.canStartEvent = CreateEvent(nullptr, TRUE, FALSE, nullptr);
    context.canContinueEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    context.terminateEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
    context.blockedEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);

    g_shared = sharedData;

    context.threadHandle = CreateThread(
        nullptr,
        0,
        MarkerThreadProc,
        &context,
        0,
        nullptr
    );
}

bool TryMarkIndex(SharedData* sharedData, int threadId, int index) {
    EnterCriticalSection(&sharedData->arrayLock);
    bool success = false;
    if (sharedData->array[index] == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        sharedData->array[index] = threadId;
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        success = true;
    }
    LeaveCriticalSection(&sharedData->arrayLock);
    return success;
}
void UnmarkIndices(SharedData* sharedData, const std::vector<int>& indices) {
    EnterCriticalSection(&sharedData->arrayLock);
    for (int idx : indices) {
        sharedData->array[idx] = 0;
    }
    LeaveCriticalSection(&sharedData->arrayLock);
}

DWORD WINAPI MarkerThreadProc(LPVOID lpParam) {
    auto* context = static_cast<MarkerThreadContext*>(lpParam);
    int id = context->threadId;
    srand(id);

    WaitForSingleObject(context->canStartEvent, INFINITE);

    while (true) {
        int rnd = rand();
        int index = rnd % g_shared->size;

        if (TryMarkIndex(g_shared, id, index)) {
            context->markedIndices.push_back(index);
            continue;
        }
        else {
            EnterCriticalSection(&g_shared->arrayLock);
            int markedCount = static_cast<int>(context->markedIndices.size());
            LeaveCriticalSection(&g_shared->arrayLock);
            EnterCriticalSection(&g_shared->logLock);
            std::cout << "[Marker " << id << "] Blocked after " << markedCount << " marks at index " << index << "\n";
            LeaveCriticalSection(&g_shared->logLock);
            SetEvent(context->blockedEvent);

            HANDLE waitEvents[] = { context->canContinueEvent, context->terminateEvent };
            DWORD result = WaitForMultipleObjects(2, waitEvents, FALSE, INFINITE);

            if (result == WAIT_OBJECT_0 + 1) {
                UnmarkIndices(g_shared, context->markedIndices);
                context->markedIndices.clear();
                context->terminated = true;
                return 0;
            }
            else {
                ResetEvent(context->canContinueEvent);
                continue;
            }
        }
    }

    return 0;
}

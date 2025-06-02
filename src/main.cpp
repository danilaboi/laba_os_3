#include "manager.h"
#include <iostream>

int main() {
    SharedData shared;

    int arraySize = 0;
    std::cout << "Enter array size: ";
    std::cin >> arraySize;

    int threadCount = 0;
    std::cout << "Enter number of marker threads: ";
    std::cin >> threadCount;

    InitializeSharedData(shared, arraySize, threadCount);
    StartAllMarkers(shared);

    while (true) {
        WaitAllMarkersBlocked(shared);
        PrintArray(shared);

        std::cout << "Enter thread ID to terminate: ";
        int idToTerminate;
        std::cin >> idToTerminate;

        TerminateMarker(shared, idToTerminate);
        WaitMarkerTerminated(shared, idToTerminate);

        PrintArray(shared);

        ResumeAllExcept(shared, idToTerminate);

        bool allTerminated = true;
        for (const auto& marker : shared.markers) {
            if (!marker.terminated) {
                allTerminated = false;
                break;
            }
        }

        if (allTerminated) break;
    }

    CleanupSharedData(shared);
    return 0;
}

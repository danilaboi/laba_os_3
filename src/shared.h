#pragma once

#include <windows.h>
#include <vector>

struct MarkerThreadContext {
    int threadId;
    HANDLE threadHandle; 
    HANDLE canStartEvent; 
    HANDLE canContinueEvent;
    HANDLE terminateEvent;               
    HANDLE blockedEvent;   
    std::vector<int> markedIndices; 
    bool terminated = false;             
};
    
struct SharedData {
    int* array = nullptr;                
    int size = 0;                         
    CRITICAL_SECTION arrayLock;  
    CRITICAL_SECTION logLock;
    std::vector<MarkerThreadContext> markers; 
};


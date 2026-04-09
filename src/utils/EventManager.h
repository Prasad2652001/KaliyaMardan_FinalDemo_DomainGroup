#pragma once
#include <unordered_map>
#include <algorithm>

#include "./common.h"

struct events_t
{
    float start;
    float duration;
    float deltaT; // interpolation factor always between 0.0 to 1.0
};

class EventManager
{
private:
    float currentTime; // current time of scene or duration of scene
    float enableEasing;
    std::unordered_map<unsigned, events_t *> eventList;

public:
    EventManager(std::vector<std::pair<unsigned, std::pair<float, float>>> events);
    EventManager(std::vector<std::pair<unsigned, std::pair<float, float>>> events, BOOL _enableEasing);
    void recalculatetTs();
    void resetT();
    float &operator[](unsigned index);
    void increment();
    friend EventManager &operator+=(EventManager &e, float f);
    friend EventManager &operator-=(EventManager &e, float f);
    float getT();
    void setT(float t);
    float getEventTime(unsigned index);
    float easeInOutQuad(float t);
    BOOL isEventStarted(unsigned index);
    BOOL isEventComplete(unsigned index);
    BOOL isEventInProgress(unsigned index);
    ~EventManager();
};

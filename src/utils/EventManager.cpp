#include "./EventManager.h"

template<typename T> T custom_clamp(T value, T low, T high) {
    return std::min(high, std::max(low, value));
}

EventManager::EventManager(std::vector<std::pair<unsigned, std::pair<float, float>>> events)
{
    currentTime = 0.0f;
    enableEasing = false;
    for (std::pair<unsigned, std::pair<float, float>> p : events)
    {
        events_t* e = new events_t;
        e->start = p.second.first;
        e->duration = p.second.second;
        e->deltaT = 0.0f;
        this->eventList[p.first] = e;
    }
}
EventManager::EventManager(std::vector<std::pair<unsigned, std::pair<float, float>>> events, BOOL _enableEasing)
{
    currentTime = 0.0f;
    enableEasing = _enableEasing;
    for (std::pair<unsigned, std::pair<float, float>> p : events)
    {
        events_t* e = new events_t;
        e->start = p.second.first;
        e->duration = p.second.second;
        e->deltaT = 0.0f;
        this->eventList[p.first] = e;
    }
}

void EventManager::recalculatetTs()
{
    // update deltaT of all events and custom_clamp between 0.0 to 1.0
    for (std::pair<unsigned, events_t*> ev : this->eventList)
    {
        ev.second->deltaT = custom_clamp((this->currentTime - ev.second->start) / ev.second->duration, 0.0f, 1.0f);

        // Apply easing function
        if (enableEasing)
            ev.second->deltaT = easeInOutQuad(ev.second->deltaT);
    }
}

float EventManager::easeInOutQuad(float t)
{
    return t < 0.5 ? 2 * t * t : -1 + (4 - 2 * t) * t;
}

void EventManager::resetT()
{
    this->currentTime = 0.0f;
}

float &EventManager::operator[](unsigned index)
{
    if (this->eventList.count(index) == 0)
    {
        PrintLog("Invalid Event\n");
    }
    return this->eventList[index]->deltaT;
}

void EventManager::increment()
{
    this->currentTime += (float)gDeltaTime;
    this->recalculatetTs();
}

float EventManager::getT()
{
    return this->currentTime;
}

void EventManager::setT(float t)
{
    this->currentTime += t;
    this->recalculatetTs();
}

float EventManager::getEventTime(unsigned index)
{
    return this->eventList[index]->deltaT;
}

BOOL EventManager::isEventStarted(unsigned index)
{
    return this->eventList[index]->deltaT > 0.0f;
}
BOOL EventManager::isEventComplete(unsigned index)
{
    return this->eventList[index]->deltaT >= 1.0f;
}

BOOL EventManager::isEventInProgress(unsigned index)
{
    return ((this->eventList[index]->deltaT > 0.0) && !(this->eventList[index]->deltaT >= 1.0));
}

EventManager::~EventManager()
{
    this->eventList.clear();
}

EventManager &operator+=(EventManager &e, float t)
{
    e.currentTime += t;
    e.recalculatetTs();
    return e;
}

EventManager &operator-=(EventManager &e, float t)
{
    e.currentTime -= t;
    e.recalculatetTs();
    return e;
}

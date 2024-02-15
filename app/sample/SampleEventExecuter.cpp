#include "SampleEventExecuter.h"
#include <ctime>
#include <iostream>
#include <iomanip>
#include "SetupN.h"

SampleEventExecuter::SampleEventExecuter()
:AbstractThreadCondition(0)
,_mask(static_cast<uint32_t>(EEventExecuterMask::EmailMask))
{
    SetupN::instance()->addObserver(_emailCate.index(), this);
}
SampleEventExecuter::~SampleEventExecuter()
{
}
void SampleEventExecuter::worker()
{
    {
        Event event;
        EmailCategoryN emailCate;
        {
            std::unique_lock<std::mutex> m(_mutex);
            if(!_queue.empty()) {
                event  = _queue.front();
                _queue.pop();
            }
            //_emailCate.get(&emailCate); 
            emailCate.set(_emailCate); 
            //printf("%s %s %d,serverName %s!!\n", __FILE__, __func__, __LINE__, emailCate._serverAddress.c_str());
        }
        printf("%s, %s, %d event %s extId %d!!\n", __FILE__, __func__, __LINE__, 
                event.name().c_str(), event.extId());
        auto tick = event.dateTime();
        std::cout << "tick" << std::chrono::time_point_cast<std::chrono::milliseconds>(tick).time_since_epoch().count() << std::endl;
        std::time_t t = std::chrono::system_clock::to_time_t(event.dateTime());
        std::cout << "현재 시간은 : " << std::put_time(std::localtime(&t), "%F %T %z")
            << '\n';
    }
}   

void SampleEventExecuter::execute(Event* item)
{
    {
        std::lock_guard<std::mutex> lock(_mutex);
        Event event(item);
        _queue.push(event); 
    }
    wakeUp();
} 

void SampleEventExecuter::update(AbstractSetupCategoryN* cate)
{
    std::lock_guard<std::mutex> lock(_mutex);
    if(cate->index() == _emailCate.index()){
        cate->get(&_emailCate);
    }
}

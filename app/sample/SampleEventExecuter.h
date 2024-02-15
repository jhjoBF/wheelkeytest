#pragma once
#include <queue>
#include "AbstractThreadCondition.h"
#include "IEventObserver.h"
#include "Event.h"
#include "const.h"
#include "EmailCategoryN.h"

class SampleEventExecuter: public AbstractThreadCondition , public IEventObserver, public ISetupObserver
{
public:
    SampleEventExecuter();
    virtual ~SampleEventExecuter();
  
private:
    std::queue<Event> _queue;
    uint32_t _mask;
    EmailCategoryN _emailCate;
protected:
    void worker();

public://ISetupObserver
    void update(AbstractSetupCategoryN* cate);

public://IEventObserver
    void execute(Event* item);
    uint32_t mask() { return _mask; }
};


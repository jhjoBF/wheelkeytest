#pragma once
#include "AbstractThread.h"
#include "SystemCategoryN.h"
#include "VideoCategoryN.h"

class SampleThread: public AbstractThread, public ISetupObserver {
public:
    SampleThread();
    virtual ~SampleThread();
    
protected:
    void worker();
private:
    SystemCategoryN _systemCate;
    VideoCategoryN _vCate;
public:
    void update(AbstractSetupCategoryN* cate);
};


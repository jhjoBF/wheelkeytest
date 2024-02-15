#pragma once
#include <vector>
#include "AbstractThread.h"
#include "Model.h" 
#include "OemFactory.h"
#include "hal/Platform.h"

class MediaManager;
class SetupN;
class Sample : public AbstractThread
{
public:
    Sample();
    virtual ~Sample();
private:
    std::vector<IThread*> _threads;
    SetupN* _setup;// Setup Object
    std::unique_ptr<const Model> _model;
    std::unique_ptr<AbstractOem> _oem;
    Hal::Platform _platform; 
    MediaManager* _mediaManager;
protected:
    void worker();

public:
    void initialize();
    void finalize();
    void forceRun();

private:
    void makeThreads();
    void startThreads();
    void setSystemConfig();
    void loadSetup();
    void defaultSetup();
    void initHal();
    void getHwSpec();
};

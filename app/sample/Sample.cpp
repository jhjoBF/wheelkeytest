#include "Sample.h"
#include <stdio.h>
#include <chrono>
#include "Spec.h"
#include "SetupN.h"
#include "SampleProducer.h"
#include "SampleConsumer.h"
#include "SampleConsumerQueue.h"
#include "SampleStateMachine.h"
#include "SampleThread.h"
#include "EventManager.h"
#include <unistd.h>
#include "hal/Osd.h"
#include <iostream>
#include <ctime>
#include <iomanip>
#include <sstream>
#include "MediaManager.h"

Sample::Sample()
:AbstractThread(300)
{
}

Sample::~Sample()
{
}
void Sample::makeThreads()
{
    _threads.push_back(new SampleThread());
    //_threads.push_back(new EventManager());
    //_threads.push_back(new VideoEncoder(0));
    /* 
    VideoEncoder* venc = new VideoEncoder(0);
    _threads.push_back(venc);
    
    FileMediaConsumer* fcon = new FileMediaConsumer(venc);
    _threads.push_back(fcon);
    */
    
    _mediaManager = new MediaManager();
    _mediaManager->init();
#if 0 
    SampleProducer* sProducer = new SampleProducer();
    _threads.push_back(sProducer);
    
    SampleConsumer* sConsumer = new SampleConsumer(sProducer);
    _threads.push_back(sConsumer);
   
    SampleConsumerQueue* sConsumerQueue = new SampleConsumerQueue(sProducer);
    _threads.push_back(sConsumerQueue);
    
    SampleStateMachine* sStateMachine = new SampleStateMachine();
    sProducer->addListener(sStateMachine);
    _threads.push_back(sStateMachine);
#endif 
}
void Sample::startThreads()
{
    for(const auto& thread : _threads)
        thread->start();
}

void Sample::setSystemConfig()
{
    // oem spec
    std::unique_ptr<const Model> model(new Model());
    _model = std::move(model);
    
    std::unique_ptr<AbstractOem> oem(OemFactory::createOem());
    _oem = std::move(oem);
    _oem->init(_model->model());
    
    Spec::instance()->check(_platform);

}

void Sample::defaultSetup()
{
    _setup->setDefault();
    _oem->setOemDefault();
    _setup->saveSetup();
}

void Sample::loadSetup()
{
    _setup->loadSetup();
}

void Sample::initHal()
{
    //Hal::initHal();    
    _platform.init();
}

void Sample::getHwSpec()
{
    //getHwSpec
}

void Sample::initialize()
{
    initHal();
    getHwSpec();
    setSystemConfig();  
    _setup = SetupN::instance();
    _setup->init();
    if(!_setup->isValidFile())
        defaultSetup();
    //return;
    makeThreads();
    loadSetup();
    startThreads();
    
    
    printf("munufacuturer %s\n", Spec::instance()->manufacturer().c_str());
    printf("%s, %d !!\n", __func__, __LINE__);
}

void Sample::finalize()
{
    for(const auto& thread : _threads) {
        thread->stop();
    }
    usleep(30 * 1000); 
    for(const auto& thread : _threads) {
        thread->join();
    }
    _mediaManager->cleanup();
    
    delete _mediaManager;
    _platform.exit(); 
    for(const auto& thread : _threads) {
        delete thread;
    }
    printf("%s, %d !!\n", __func__, __LINE__);
}

void Sample::worker()
{
    static auto start = std::chrono::steady_clock::now();
    auto now = std::chrono::steady_clock::now(); 
    auto diff = now - start;
    int sec = std::chrono::duration_cast<std::chrono::seconds>(diff).count();
    static int _sec;
    //if(0) {
    if(sec != _sec) {
        _sec = sec;
        std::time_t t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        //std::cout << "현재 시간은 : " << std::put_time(std::localtime(&t), "%F %T %z") << '\n';
        std::cout << std::put_time(std::localtime(&t), "%T") << '\n';
        std::ostringstream os;
        os << (std::put_time(std::localtime(&t), "%T"));
        Hal::osdText(os.str(), 1, 1, 1);
        //Hal::osdText(os.str(), 1, 1, Hal::WHITE);
    }
    //if(sec > 1 * 30) {
    //if(0) {
#if 0
    if(sec > 1 * 10) {
        char* ptr = 0;
        *(ptr) = 1;
    }
#endif
#if 0
    if(sec > 1 * 30 * 2) {
        printf("%s, %d sec %d!!\n", __func__, __LINE__, sec);
        for(const auto& thread : _threads) {
            thread->stop();
        }
        usleep(30 * 1000); 
        for(const auto& thread : _threads) {
            thread->join();
        }
        _mediaManager->cleanup();
        stop();
    }
#endif
}
void Sample::forceRun()
{
    _stop = false;
    AbstractThread::run();
}

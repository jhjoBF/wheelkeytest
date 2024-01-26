#ifndef IMANAGER_H_
#define IMANAGER_H_

class IManager {
public:
    IManager() {};
    virtual ~IManager() {};
public:
    virtual void init() = 0;
    virtual void start() = 0;
    virtual void cleanup() = 0;
};

#endif  // IMANAGER_H_

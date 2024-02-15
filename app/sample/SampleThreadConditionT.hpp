//#include <SampleThreadConditionT.h>

template <typename T> 
SampleThreadConditionT<T>::SampleThreadConditionT()
:AbstractThreadCondition(0)
{
    printf("%s, %s, %d !!\n", __FILE__, __func__, __LINE__);
}
template <typename T> 
SampleThreadConditionT<T>::~SampleThreadConditionT()
{
    printf("%s, %s, %d !!\n", __FILE__, __func__, __LINE__);
}
template <typename T> 
void SampleThreadConditionT<T>::worker()
{
    printf("%s, %s, %d !!\n", __FILE__, __func__, __LINE__);
    /*
    auto data = _data;
    {    
        std::lock_guard<std::mutex> lock(_mutex);
        data = _data;
    }
    static int cnt;
    cnt++;
    if (cnt > 3) {
        printf("Test Thread Stop!!\n");
        //_stop = true;
        stop();
    }
    */
    std::unique_lock<std::mutex> m(_mutex);
    if(!_queue.empty()) {
        T data  = _queue.front();
        _queue.pop();
    }
}   

template <typename T> 
void SampleThreadConditionT<T>::setData(int data)
{
    printf("%s, %s, %d data %d!!\n", __FILE__, __func__, __LINE__, data);
    {
        std::lock_guard<std::mutex> lock(_mutex);
        _data = data;
        //_queue.push(data); 
    }
    //wakeUp();
} 

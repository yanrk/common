#include <iostream>

#include "test_base.h"
#include "base_time.h"
#include "base_timer.h"

USING_NAMESPACE_COMMON

class TimerSink : public ITimerSink
{
public:
    TimerSink();

public:
    virtual void on_timer(bool first_time, size_t index);

private:
    size_t      m_count;
    BaseTimer   m_timer;
};

TimerSink::TimerSink()
    : m_count(0)
    , m_timer()
{
    m_timer.init(this, 100);
}

void TimerSink::on_timer(bool first_time, size_t index)
{
    if (first_time)
    {
        std::cout << "...on timer..." << std::endl;
    }
    std::cout << "index: " << index << " --- "
              << "count: " << m_count++ << std::endl;
}

void test_base_timer(void)
{
    std::cout << "sleep 1000ms begin..." << std::endl;
    {
        TimerSink sink;
        base_millisecond_sleep(1000);
    }
    std::cout << "sleep 1000ms end..." << std::endl;
}

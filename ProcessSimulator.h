/*
 *  CSOPESY Semi-Major Output 2  —  Desktop-Style OS Mock-up
 *  Requirement C (support): live process model
 *
 *  Feeds the Task Manager table with placeholder-but-plausible process rows.
 *  Values are driven by a seeded linear congruential generator so that a
 *  black-box test run is reproducible: the same rng-seed always produces the
 *  same sequence of CPU and memory readings.
 */

#pragma once
#include <cstdint>
#include <string>
#include <vector>

enum class ProcStatus { Running, Idle, Suspended };

struct Process {
    int         pid    = 0;
    std::string name;
    ProcStatus  status = ProcStatus::Running;
    float       cpu    = 0.0f;   // percent of one core
    int         memKb  = 0;
};

class ProcessSimulator {
public:
    void init(int processCount, unsigned seed, int minMemKb, int maxMemKb,
              float minCpu, float maxCpu, int updateIntervalMs, int totalMemKb);

    // Advance the model.  deltaSeconds is the real frame delta.
    void update(double deltaSeconds);

    const std::vector<Process>& processes() const { return m_processes; }
    const std::vector<std::string>& eventLog() const { return m_events; }

    float totalCpuPercent() const;
    int   usedMemoryKb()   const;
    int   totalMemoryKb()  const { return m_totalMemKb; }
    int   runningCount()   const;

    static const char* statusName(ProcStatus s);

private:
    uint32_t nextRandom();
    float    nextFloat(float lo, float hi);
    void     seedProcess(Process& p);
    void     pushEvent(const std::string& text);

    std::vector<Process>   m_processes;
    std::vector<std::string> m_events;

    uint32_t m_random  = 1u;
    int      m_minMemKb = 4096;
    int      m_maxMemKb = 65536;
    int      m_totalMemKb = 262144;
    float    m_minCpu   = 0.0f;
    float    m_maxCpu   = 40.0f;
    int      m_updateIntervalMs = 200;
    float    m_accumulator = 0.0f;
    int      m_nextPid  = 1000;
};

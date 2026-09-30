#include "ProcessSimulator.h"
#include <algorithm>
#include <cmath>

namespace {

// Placeholder service names for the mocked-up shell.
const char* const kProcessNames[] = {
    "csopesy-init",   "csopesy-sched",  "csopesy-memmgr", "csopesy-compositor",
    "marquee-svc",    "config-svc",     "taskmgr-svc",    "clock-svc",
    "shell-ui",       "net-stub",       "disk-stub",      "log-svc",
    "pager-svc",      "font-svc",       "input-svc",      "power-svc"
};
const int kProcessNameCount = sizeof(kProcessNames) / sizeof(kProcessNames[0]);

float clampf(float v, float lo, float hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

} // namespace

const char* ProcessSimulator::statusName(ProcStatus s) {
    switch (s) {
        case ProcStatus::Running:   return "Running";
        case ProcStatus::Idle:      return "Idle";
        case ProcStatus::Suspended: return "Suspended";
    }
    return "Unknown";
}

// Numerical Recipes LCG — deterministic for a given seed.
uint32_t ProcessSimulator::nextRandom() {
    m_random = m_random * 1664525u + 1013904223u;
    return m_random;
}

float ProcessSimulator::nextFloat(float lo, float hi) {
    const float unit = static_cast<float>(nextRandom() >> 8) / 16777216.0f; // [0,1)
    return lo + unit * (hi - lo);
}

void ProcessSimulator::pushEvent(const std::string& text) {
    m_events.insert(m_events.begin(), text);
    if (m_events.size() > 8) m_events.pop_back();
}

void ProcessSimulator::seedProcess(Process& p) {
    p.pid   = m_nextPid++;
    p.name  = kProcessNames[p.pid % kProcessNameCount];
    p.status = ProcStatus::Running;
    p.cpu   = nextFloat(m_minCpu, m_maxCpu * 0.6f);
    p.memKb = nextFloat(static_cast<float>(m_minMemKb), static_cast<float>(m_maxMemKb) * 0.5f);
}

void ProcessSimulator::init(int processCount, unsigned seed, int minMemKb, int maxMemKb,
                            float minCpu, float maxCpu, int updateIntervalMs, int totalMemKb) {
    m_random          = (seed == 0u ? 1u : seed);
    m_minMemKb        = std::max(64, minMemKb);
    m_maxMemKb        = std::max(m_minMemKb, maxMemKb);
    m_totalMemKb      = std::max(m_maxMemKb, totalMemKb);
    m_minCpu          = std::max(0.0f, minCpu);
    m_maxCpu          = std::max(m_minCpu, maxCpu);
    m_updateIntervalMs = std::max(16, updateIntervalMs);
    m_accumulator     = 0.0f;
    m_nextPid         = 1000;
    m_processes.clear();
    m_events.clear();

    m_processes.reserve(static_cast<size_t>(processCount));
    for (int i = 0; i < processCount; ++i) {
        Process p;
        seedProcess(p);
        m_processes.push_back(p);
    }

    pushEvent("csopesy-init: system ready, " + std::to_string(processCount) + " processes spawned");
}

void ProcessSimulator::update(double deltaSeconds) {
    m_accumulator += static_cast<float>(deltaSeconds) * 1000.0f;

    // Bound the catch-up so a stalled frame cannot spin.
    int steps = 0;
    while (m_accumulator >= static_cast<float>(m_updateIntervalMs) && steps < 4) {
        m_accumulator -= static_cast<float>(m_updateIntervalMs);
        ++steps;

        for (Process& p : m_processes) {
            // Status transitions — rare, so the table does not flicker.
            const uint32_t roll = nextRandom() % 1000u;
            if (roll < 12u) {
                const ProcStatus next = (p.status == ProcStatus::Running) ? ProcStatus::Idle
                                     : (p.status == ProcStatus::Idle)    ? ProcStatus::Suspended
                                                                          : ProcStatus::Running;
                p.status = next;
                pushEvent(p.name + " (" + std::to_string(p.pid) + ") is now " + statusName(next));
            }

            if (p.status == ProcStatus::Running) {
                const float target = nextFloat(m_minCpu, m_maxCpu);
                p.cpu += (target - p.cpu) * 0.35f;
                p.cpu += nextFloat(-1.2f, 1.2f);
            } else if (p.status == ProcStatus::Idle) {
                p.cpu += (0.0f - p.cpu) * 0.6f;
            } else {
                p.cpu = 0.0f;
            }
            p.cpu = clampf(p.cpu, 0.0f, m_maxCpu);

            // Memory creeps in both directions, quantised to 4 KB.
            const float memStep = nextFloat(-2048.0f, 2560.0f);
            p.memKb = static_cast<int>(clampf(static_cast<float>(p.memKb) + memStep,
                                              static_cast<float>(m_minMemKb),
                                              static_cast<float>(m_maxMemKb)));
            p.memKb = (p.memKb / 4) * 4;
        }
    }
    if (m_accumulator > static_cast<float>(m_updateIntervalMs) * 4.0f)
        m_accumulator = 0.0f;
}

float ProcessSimulator::totalCpuPercent() const {
    float sum = 0.0f;
    for (const Process& p : m_processes) sum += p.cpu;
    return sum;
}

int ProcessSimulator::usedMemoryKb() const {
    int sum = 0;
    for (const Process& p : m_processes) sum += p.memKb;
    return sum;
}

int ProcessSimulator::runningCount() const {
    int n = 0;
    for (const Process& p : m_processes)
        if (p.status == ProcStatus::Running) ++n;
    return n;
}

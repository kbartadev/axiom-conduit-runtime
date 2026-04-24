#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "conduit/core.hpp"

using namespace cre;

// ============================================================================
// 1. COMPOSITION: The Layered Event
// ============================================================================
struct network_layer  { std::string ip; };
struct security_layer { bool authorized = false; double threat_level = 0.0; };
struct business_layer { int order_id; double amount; };

// One event, three distinct responsibility layers
struct integrated_event : allocated_event<integrated_event, 128> {
    network_layer  net;
    security_layer sec;
    business_layer biz;

    integrated_event(const char* ip, double threat, int id, double amt)
        : net{ ip }, sec{ false, threat }, biz{ id, amt } {}
};

// ============================================================================
// 2. CONCEPTS: Generic Structural Discovery
// ============================================================================
template <typename T> concept IsNetworkAware = requires(T a) { a.net; };

// ============================================================================
// 3. HANDLERS: Diverse Processing Patterns
// ============================================================================

// Stage 1: Generic Auditor (Uses C++20 Concepts to match structural patterns)
struct auditor_stage : handler_base<auditor_stage> {
    template <IsNetworkAware E>
    void on(event_ptr<E>& ev) {
        if (ev) std::cout << "[Stage 1] Auditing IP: " << ev->net.ip << "\n";
    }
};

// Stage 2: Processor (Uses Manual Cascading to separate layer logic)
struct processor_stage : handler_base<processor_stage> {
    
    // Internal logic for security
    void on(security_layer& s) {
        if (s.threat_level < 0.5) {
            s.authorized = true;
            std::cout << "[Stage 2.a] Security: Authorized (Threat: " << s.threat_level << ")\n";
        }
    }

    // Internal logic for business (Mutation)
    void on(business_layer& b) {
        b.amount *= 1.10; // Apply 10% processing fee
        std::cout << "[Stage 2.b] Business: Fee applied. New Amount: $" << b.amount << "\n";
    }

    // Entry point: Decomposes the complex event into simple layer calls
    void on(event_ptr<integrated_event>& ev) {
        if (!ev) return;
        on(ev->sec); // Cascade to internal security overload
        on(ev->biz); // Cascade to internal business overload
    }
};

// Stage 3: Reporter (Observes the final mutated state)
struct reporter_stage : handler_base<reporter_stage> {
    void on(event_ptr<integrated_event>& ev) {
        if (ev) {
            std::cout << "[Stage 3] Final State -> Auth: " << (ev->sec.authorized ? "YES" : "NO") 
                      << " | Total: $" << std::fixed << std::setprecision(2) << ev->biz.amount << "\n";
        }
    }
};

// ============================================================================
// MAIN: Orchestrating the Synthesis
// ============================================================================
int main() {
    std::cout << "--- Conduit Ultimate Logic Synthesis Example ---\n\n";

    // 1. Setup the memory domain for our integrated events
    runtime_domain<integrated_event> domain;

    // 2. Initialize our specialized handlers
    auditor_stage   auditor;
    processor_stage processor;
    reporter_stage  reporter;

    // 3. Define the Pipeline (Full Dispatch Chain)
    // Execution flow: Audit -> Process (Cascade/Mutate) -> Report
    pipeline<auditor_stage, processor_stage, reporter_stage> pipe(auditor, processor, reporter);

    // 4. Create and dispatch the event
    std::cout << "Dispatching Event: { IP: 192.168.1.1, Threat: 0.1, Order: 5005, Amount: $100.00 }\n";
    std::cout << "----------------------------------------------------------------------\n";

    auto ev = domain.make<integrated_event>("192.168.1.1", 0.1, 5005, 100.0);
    pipe.dispatch(ev);

    std::cout << "----------------------------------------------------------------------\n";
    std::cout << "Processing complete. Event reclaimed automatically by scope exit.\n";

    return 0;
}
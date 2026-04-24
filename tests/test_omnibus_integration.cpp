#include <gtest/gtest.h>
#include <string>
#include <vector>
#include "conduit/core.hpp"

using namespace cre;

// ============================================================================
// 1. COMPOSITION: Structural Layers (from test_hierarchical_execution_order)
// ============================================================================
struct network_layer { std::string ip; };
struct security_layer { bool authorized = false; double threat_level = 0.0; };
struct business_layer { int order_id; double amount; };

// Integrated event containing all three layers (from test_single_handler_cascade)
struct integrated_event : allocated_event<integrated_event, 100> {
    network_layer net;
    security_layer sec;
    business_layer biz;

    integrated_event(const char* ip, double threat, int id, double amt)
        : net{ ip }, sec{ false, threat }, biz{ id, amt } {
    }
};

// ============================================================================
// 2. CONCEPTS: Zero-Cost Structural Matching (from hierarchical_execution_order)
// ============================================================================
template <typename T> concept IsNetworkAware = requires(T a) { a.net; };
template <typename T> concept IsSecurityAware = requires(T a) { a.sec; };

// ============================================================================
// 3. HANDLERS: Combining Pipeline Stages with Internal Cascading
// ============================================================================

// Stage 1: General Auditor (Hierarchical Concept Matching)
struct global_auditor : handler_base<global_auditor> {
    std::vector<std::string>& log;
    global_auditor(std::vector<std::string>& l) : log(l) {}

    template <IsNetworkAware E>
    void on(event_ptr<E>& ev) {
        if (ev) log.push_back("audited_ip:" + ev->net.ip);
    }
};

// Stage 2: Security & Business Processor (Single Handler Cascade)
struct integrated_processor : handler_base<integrated_processor> {
    std::vector<std::string>& log;
    integrated_processor(std::vector<std::string>& l) : log(l) {}

    // Internal Layer Overloads
    void on(security_layer& s) {
        if (s.threat_level < 0.5) {
            s.authorized = true;
            log.push_back("sec:authorized");
        }
    }

    void on(business_layer& b) {
        b.amount *= 1.10; // Apply 10% fee (Mutation propagation)
        log.push_back("biz:fee_applied");
    }

    // Top-level entry point (The Manual Cascade)
    void on(event_ptr<integrated_event>& ev) {
        if (!ev) return;
        on(ev->sec); // Cascade to security logic
        on(ev->biz); // Cascade to business logic
    }
};

// Stage 3: Final Observer (Full Dispatch Chain - Mutation verification)
struct final_observer : handler_base<final_observer> {
    std::string last_ip;
    bool was_authorized = false;
    double final_amt = 0.0;

    void on(event_ptr<integrated_event>& ev) {
        if (ev) {
            last_ip = ev->net.ip;
            was_authorized = ev->sec.authorized;
            final_amt = ev->biz.amount;
        }
    }
};

// ============================================================================
// 4. TEST CASE: Verifying the Integrated Invariants
// ============================================================================
TEST(UltimateIntegration, verifies_all_three_logic_patterns) {
    runtime_domain<integrated_event> domain;
    std::vector<std::string> execution_log;

    global_auditor auditor(execution_log);
    integrated_processor processor(execution_log);
    final_observer observer;

    // PIPELINE: Defines the strict, inlined execution order [cite: 322]
    // Sequence: Audit -> Integrated (Security+Biz) -> Observer
    pipeline<global_auditor, integrated_processor, final_observer> pipe(auditor, processor, observer);

    auto ev = domain.make<integrated_event>("192.168.1.1", 0.1, 5005, 100.0);

    // DISPATCH: Execute the chain [cite: 322, 345]
    pipe.dispatch(ev);

    // PROOF 1: Hierarchical Concept Matching (Auditor found the IP)
    ASSERT_GT(execution_log.size(), 0);
    EXPECT_EQ(execution_log[0], "audited_ip:192.168.1.1");

    // PROOF 2: Single Handler Cascade (Internal overloads triggered in order)
    ASSERT_GE(execution_log.size(), 3);
    EXPECT_EQ(execution_log[1], "sec:authorized");
    EXPECT_EQ(execution_log[2], "biz:fee_applied");

    // PROOF 3: Full Dispatch Chain & Mutation (Observer sees final values) 
    EXPECT_TRUE(observer.was_authorized);
    EXPECT_EQ(observer.last_ip, "192.168.1.1");
    // Mutation: 100.0 * 1.10 = 110.0
    EXPECT_DOUBLE_EQ(observer.final_amt, 110.0);

    // PROOF 4: Deterministic Reclamation [cite: 317]
    // Event is reclaimed at the end of the domain/scope.
}
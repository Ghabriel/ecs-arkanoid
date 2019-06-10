#pragma once

#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include "NullState.hpp"
#include "State.hpp"

namespace state {
    class StateMachine {
     public:
        StateMachine();

        void clear();
        void registerState(const std::string& name, std::unique_ptr<State>);
        void pushState(const std::string& stateName);
        void popState();
        State& getState();

     private:
        std::stack<State*> states;
        std::unordered_map<std::string, std::unique_ptr<State>> registeredStates;
    };

    inline StateMachine::StateMachine() {
        registeredStates.insert({"", std::make_unique<NullState>()});
        states.push(registeredStates.at("").get());
    }

    inline void StateMachine::clear() {
        while (states.size() > 1) {
            states.pop();
        }

        auto it = registeredStates.begin();
        while (it != registeredStates.end()) {
            if (it->first != "") {
                it = registeredStates.erase(it);
            } else {
                ++it;
            }
        }
    }

    inline void StateMachine::registerState(
        const std::string& name,
        std::unique_ptr<State> state
    ) {
        registeredStates.insert({name, std::move(state)});
    }

    inline void StateMachine::pushState(const std::string& stateName) {
        states.top()->onExit();
        states.push(registeredStates.at(stateName).get());
        states.top()->onEnter();
    }

    inline void StateMachine::popState() {
        if (states.size() > 1) {
            states.top()->onExit();
            states.pop();
            states.top()->onEnter();
        }
    }

    inline State& StateMachine::getState() {
        return *states.top();
    }
}

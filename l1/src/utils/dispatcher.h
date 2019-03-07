#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <experimental/tuple>
#include <functional>
#include <vector>
#include <tuple>


/**
 * @brief A generic algorithm to dispatch data
 */
template<typename... func_args>
class Dispatcher
{
    using Callback = std::function<void(func_args...)>;
    std::vector<Callback> observers;
    std::tuple<func_args...> data;

public:
    /**
     * @brief addObserver Adds an observer to be informed
     * about an incoming message
     * @param function function with func_args arguments
     */
    void addObserver(Callback function)
    {
        observers.push_back(function);
    }

    /**
     * @brief notify Notifies all register observers
     */
    void notify()
    {
        for (auto &observer : observers)
        {
            std::experimental::apply(observer, data);
        }
    }

    /**
     * @brief dispatch Stores data to be dispatched. To dispatch it
     * among all observers notify() must be called.
     * @param args arguments func_args
     */
    void dispatch(func_args... args)
    {
        data = std::make_tuple(args...);
    }
};

#endif // DISPATCHER_H

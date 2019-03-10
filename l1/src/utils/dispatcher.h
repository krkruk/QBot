#ifndef DISPATCHER_H
#define DISPATCHER_H
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/lexical_cast.hpp>
#include <experimental/tuple>
#include <unordered_map>
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

/**
 * @brief A generic algorithm to dispatch data
 */

/**
 * MappedDispatcher provides a generic algorithm for
 * dispatching data to valid receivers.
 *
 * The template requires two parameters:
 *  * InputType defines a type of raw input. Usually, std::string
 *  * Processor processes received input of type InputType. It
 *      is assumed Processor provides two traits: callback_type
 *      and id_type. Thanks to these traits MappedDispatcher can
 *      trigger a valid callback with correct inputs.
 */
template<typename InputType, typename Processor>
class MappedDispatcher
{
public:
    using Callback = typename Processor::callback_type;
    using IdType = typename Processor::id_type;

    /**
     * @brief addObserver Adds an observer with an id of IdType
     * @param id identification label
     * @param function callback to be raised once input is obtained.
     */
    void addObserver(IdType id, Callback function)
    {
        observers[id] = function;
    }

    /**
     * @brief delObserver Deletes an observer
     * @param id id of the observer to be deleted
     */
    void delObserver(IdType id)
    {
        observers.erase(id);
    }

    /**
     * @brief notify Notifies all register observers
     */
    void notify()
    {
        for (const auto &observer : observers)
        {
            IdType id = observer.first;
            if (processor.isValid(id))
            {
                const auto data = processor.get(id);
                observer.second(id, data);
            }
        }
    }

    /**
     * @brief dispatch Stores data to be dispatched. To dispatch it
     * among all observers notify() must be called.
     * @param data input data
     */
    void dispatch(const InputType &data)
    {
        processor.process(data);
    }

private:
    Processor processor;
    std::unordered_map<IdType, Callback> observers;
};

#endif // DISPATCHER_H

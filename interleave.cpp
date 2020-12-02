#include <iostream>
#include <ranges>
#include <vector>
#include <memory>
#include <boost/format.hpp>
#include <iterator>
#include <tuple>
#include <utility>
#include <list>

using std::views::filter;

template <typename T>
using iterator_t = decltype(std::begin(std::declval<T&>()));

template <typename T>
using range_reference_t = decltype(*std::begin(std::declval<T&>()));

template <typename T, size_t ... indices>
constexpr auto values_to_tuples(T tpl, std::index_sequence<indices...>) {
    return std::make_tuple(std::begin(std::get<indices>(tpl))...);
}

template <typename... R>
class view_interleave {
    std::tuple<R...> _ranges;
    std::tuple<decltype(values_to_tuples(
        _ranges,
        std::make_index_sequence<std::tuple_size<std::tuple<R...>>::value>{}))>
        iterators;

    struct iterator_type : iterator_t<decltype(std::get<0>(iterators))> {
        void begin() { }

    };

public:
    view_interleave(R&& ...rr) : _ranges(std::forward_as_tuple(rr...)),
    iterators(values_to_tuples(
        _ranges,
        std::make_index_sequence<std::tuple_size<std::tuple<R...>>::value>{})) {}
};

template <typename... R>
view_interleave(R && ...)->view_interleave<R...>;


int main() {
    std::vector<int> v{3, 5};
    std::list<int> l{3, 6};
    view_interleave a(v, l);
}

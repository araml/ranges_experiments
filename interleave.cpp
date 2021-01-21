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

template <typename T, size_t ... indices>
constexpr auto values_to_tuples2(T tpl, std::index_sequence<indices...>) {
    return std::make_tuple(std::end(std::get<indices>(tpl))...);
}


template <typename... R>
class view_interleave {
    std::tuple<R...> _ranges;

    using it_type = decltype(values_to_tuples(
        _ranges,
        std::make_index_sequence<std::tuple_size<std::tuple<R...>>::value>{}));

    std::tuple<it_type> iterators_ends;
    std::tuple<it_type> iterators;


    struct iterator_type {
        size_t current_iterator{0};
        std::tuple<decltype(values_to_tuples(
            _ranges,
            std::make_index_sequence<std::tuple_size<std::tuple<R...>>::value>{}))>
            iterators;

        using reference = int; //decltype(*(std::get<0>(iterators)));

        iterator_type(std::tuple<it_type> its)
            : iterators(its) {
            static_assert(std::tuple_size<std::tuple<R...>>::value == 2, "hmm");
        }

        /* Super broken for now.. */
        bool operator==(const iterator_type& rhs) { return iterators == rhs.iterators; }
        /* Super broken for now.. */
        bool operator!=(const iterator_type& rhs) { return !(*this == rhs); }

        iterator_type operator++(int) {
            switch (current_iterator) {
                case 0:
                    std::get<0>(iterators)++;
                case 1:
                    std::get<1>(iterators)++;
                break;
            }

            current_iterator++;
            if (current_iterator == std::tuple_size(iterators)) {
                current_iterator = 0;
            }
            return *this;
        }

        iterator_type &operator++() {
            ++(*this);
            return *this;
        }

        int operator*() {
            switch (current_iterator) {
                case 1:
                    return *(std::get<0>(iterators));
                break;
                case 2:
                    return *(std::get<1>(iterators));
                break;
            }
        }
    };

public:
    view_interleave(R&& ...rr) : _ranges(std::forward_as_tuple(rr...)) {}

    using iterator = iterator_type;

    iterator begin() {
        return iterator(iterators);
    }

    iterator end() {
        return iterator(iterators_ends);
    }
};

template <typename... R>
view_interleave(R && ...)->view_interleave<R...>;


int main() {
    std::vector<int> v{3, 5};
    std::list<int> l{3, 6};
    view_interleave vi(v, l);
    for (auto e : vi)
        std::cout << e << std::endl;
}

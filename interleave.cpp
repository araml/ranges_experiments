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
constexpr auto range_to_begin_it(T tpl, std::index_sequence<indices...>) {
    return std::make_tuple(std::begin(std::get<indices>(tpl))...);
}

template <typename T, size_t ... indices>
constexpr auto range_to_end_it(T tpl, std::index_sequence<indices...>) {
    return std::make_tuple(std::end(std::get<indices>(tpl))...);
}


template <typename... R>
class view_interleave {
    struct iterator_type;

public:
    view_interleave(R&& ...rr) : _ranges(rr...) {}
    using iterator = iterator_type;

    iterator begin() {
        return iterator(_ranges);
    }

    iterator end() {
        return iterator(_ranges, true);
    }

private:

    std::tuple<R...> _ranges;

    using it_type = decltype(range_to_begin_it(
        _ranges,
        std::make_index_sequence<std::tuple_size<std::tuple<R...>>::value>{}));

    struct iterator_type {
        it_type iterators;
        size_t current_iterator{0};

        using reference = int; //decltype(*(std::get<0>(iterators)));

        iterator_type(std::tuple<R...> _ranges)
            : iterators(range_to_begin_it(_ranges,
                        std::index_sequence_for<R...>{})) {
            static_assert(std::tuple_size<std::tuple<R...>>::value == 2, "hmm");
        }

        iterator_type(std::tuple<R...> _ranges, bool)
            : iterators(range_to_end_it(_ranges,
                        std::index_sequence_for<R...>{})) {
        }


        /* Super broken for now.. */
        bool operator==(const iterator_type& rhs) const { return iterators == rhs.iterators; }
        /* Super broken for now.. */
        bool operator!=(const iterator_type& rhs) const { return !(*this == rhs); }

        iterator_type operator++(int) {
            switch (current_iterator) {
                case 0:
                    std::get<0>(iterators)++;
                    break;
                case 1:
                    std::get<1>(iterators)++;
                    break;
            }

            current_iterator++;
            if (current_iterator == std::tuple_size_v<it_type>) {
                current_iterator = 0;
            }
            return *this;
        }

        iterator_type &operator++() {
            switch (current_iterator) {
                case 0:
                    ++std::get<0>(iterators);
                    break;
                case 1:
                    ++std::get<1>(iterators);
                    break;
            }

            current_iterator++;
            if (current_iterator == std::tuple_size_v<it_type>) {
                current_iterator = 0;
            }
            return *this;
        }

        auto& operator*() {
            switch (current_iterator) {
                case 0:
                    return *std::get<0>(iterators);
                break;
                case 1:
                    return *std::get<1>(iterators);
                break;
            }
        }
    };
};

template <typename... R>
view_interleave(R && ...)->view_interleave<R...>;

struct interleave_fn {
    template <typename... R>
    auto operator()(R&& ...r) {
        return view_interleave{std::forward<R>(r)...};
    }
};

namespace views {
    interleave_fn interleave;
}

int main() {
    std::vector<int> v{3, 5};
    std::list<int> l{3, 6};
    auto vi = views::interleave(v, l);
    for (auto e : vi)
        std::cout << e << std::endl;
}

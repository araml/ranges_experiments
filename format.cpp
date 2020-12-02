#include <iostream>
#include <ranges>
#include <vector>
#include <memory>
#include <boost/format.hpp>

using std::views::filter;

template <typename T>
using iterator_t = decltype(std::begin(std::declval<T&>()));

template <typename T>
using range_reference_t = decltype(*std::begin(std::declval<T&>()));

template <typename R>
class view_format : public std::ranges::view_base {
private:
    R _range;
    std::string fmt;

    struct iterator_type : iterator_t<R> {
        using base_iterator = iterator_t<R>;
        using reference = std::string;
        boost::format fmt;

        iterator_type() = default;
        iterator_type(const base_iterator &b, std::string fmt)
            : base_iterator{b}, fmt(fmt) {}

        iterator_type operator++(int) {
            return static_cast<base_iterator &>(*this)++;
        }

        iterator_type &operator++() {
            ++static_cast<base_iterator &>(*this);
            return *this;
        }

        reference operator*() {
            return str(fmt % (*static_cast<base_iterator>(*this)));
        }
    };

public:
    using reference = std::string;
    using const_reference = std::string;
    using value_type = std::string;

    using iterator = iterator_type;
    using const_iterator = iterator_type;

    view_format() = default;
    constexpr view_format(view_format &&rhs) = default;
    constexpr view_format(const view_format &rhs) = default;
    constexpr view_format &operator=(view_format &&rhs) = default;
    constexpr view_format &operator=(const view_format &rhs) = default;
    ~view_format() = default;

    view_format(const char *fmt, R &&range)
        : fmt(fmt),
          _range(std::move(range)) {}

    iterator begin() const {
        return iterator_type(std::begin(_range), fmt);
    }

    iterator end() const {
        return iterator_type(std::end(_range), fmt);
    }

    iterator cbegin() const {
        return begin();
    }

    iterator cend() const {
        return end();
    }

};

template <typename R>
view_format(const char *, R &&)->view_format<R>;

struct format_fn {
    std::string fmt;

    auto operator()(const char *fmt) {
        this->fmt = fmt;
        return *this;
    }

    template <typename R>
    auto operator()(R &&urange, const char *fmt) const {
        return view_format{fmt, std::views::all(std::forward<R>(urange))};
    }

    template <typename R>
    friend auto operator|(R &&urange, const format_fn &f) {
        return f(std::views::all(std::forward<R>(urange)), f.fmt.c_str());
    }
};

namespace views {
    format_fn format;
}

int main() {

    std::vector<int> numbers = {1, 2, 3, 4, 5, 6};

    auto fmt = views::format;

    for (auto v : numbers | fmt("%|10t|%1%"))
        std::cout << v << " "; // 4 8 12
}

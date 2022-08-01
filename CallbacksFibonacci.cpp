#include <iostream>
#include <functional>
#include <system_error>
#include <unordered_map>
#include <cassert>


class Fibonacci {
public:
    using IntType = unsigned long long;
    // Callback arguments -> error, result
    using Callback = std::function<void(std::error_code, IntType)>;

    static void Calculate(IntType input, Callback cb) {
        if (input < 0) {
            cb(std::make_error_code(std::errc::invalid_argument), 0);
        } else if (input == 0 || input == 1) {
            cb(std::error_code{}, 1);
        } else if (_resultMap.count(input) != 0) {
            IntType endResult = _resultMap[input];
            cb(std::error_code{}, endResult);
        } else {
            auto fstCallback = [&input, &cb](std::error_code ec, IntType result) {
                assert(!ec);
                auto sndCallback = [&input, &cb, &result](std::error_code ec, IntType sndResult) {
                    assert(!ec);
                    IntType endResult = result + sndResult;
                    _resultMap[input] = endResult;
                    cb(std::error_code{}, endResult);
                };
                Calculate(input - 2, sndCallback);
            };
            Calculate(input - 1, fstCallback);
        }
    }
private:
    static std::unordered_map<IntType, IntType> _resultMap;
};

std::unordered_map<Fibonacci::IntType, Fibonacci::IntType>
Fibonacci::_resultMap = {};

int main()
{
    auto calculateCb = [](std::error_code ec, Fibonacci::IntType result) {
        ec ? std::cout << ec.message() : std::cout << result;
    };
    Fibonacci::Calculate(120u, calculateCb);
    return 0;
}

#include "pch.h"

#include "number_converter.h"

#include <unordered_set>

namespace MathLib {

#pragma region Defaults
const char* NumberConverter::kDefaultDigits =
    "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
const int NumberConverter::kDefaultPrecision = 6;
#pragma endregion

#pragma region Limits
const int NumberConverter::kMinBase = 2;
const int NumberConverter::kMaxIntPartLength = 250;
const int NumberConverter::kMaxFracPartLength = 250;
#pragma endregion

#pragma region Conversion
bool NumberConverter::Convert(std::string_view str_number, int base_from,
                              std::string& str_converted_number, 
                              int base_to, int precision) const {
  long double decimal_number;
  return ConvertTo10(str_number, base_from, decimal_number) &&
         ConvertFrom10(decimal_number, str_converted_number, base_to, precision);
}

bool NumberConverter::ConvertTo10(std::string_view str_number, int base_from,
                                  long double& converted_number) const {
  auto number_length = str_number.length();
  if (base_from < kMinBase || static_cast<size_t>(base_from) > digits_.size() ||
      number_length == 0)
    return false;

  size_t number_start_idx = 0;
  bool is_positive = true;
  switch (str_number[0]) {
    case '-':
      is_positive = false;
      [[fallthrough]];
    case '+':
      ++number_start_idx;
      break;
    default:
      break;
  }
  if (number_start_idx >= number_length) 
    return false;

  auto valid_digits = digits_.substr(0, base_from);

  auto separator_pos = str_number.find_first_of(".,");
  auto str_int_part =
      str_number.substr(number_start_idx, separator_pos - number_start_idx);
  std::string_view str_frac_part;
  if (separator_pos != std::string_view::npos) {
    str_frac_part = str_number.substr(separator_pos + 1);
  }

  long double int_part = 0;
  for (char c_digit : str_int_part) {
    auto digit_value = valid_digits.find(static_cast<char>(toupper(c_digit)));
    if (digit_value == std::string_view::npos) 
      return false;
    int_part = int_part * base_from + digit_value;
  }

  long double frac_part = 0;
  for (auto it_digit = str_frac_part.rbegin(); it_digit != str_frac_part.rend(); ++it_digit) {
    auto digit_value = valid_digits.find(static_cast<char>(toupper(*it_digit)));
    if (digit_value == std::string_view::npos) 
      return false;
    frac_part = (frac_part + digit_value) / base_from;
  }

  converted_number = (is_positive ? 1 : -1) * (int_part + frac_part);
  return true;
}

bool NumberConverter::ConvertFrom10(long double number,
                                    std::string& str_converted_number,
                                    int base_to, int precision) const {
  if (base_to < kMinBase) 
    return false;

  bool is_negative = number < 0;
  if (is_negative) {
    number *= -1;
  }

  double int_part;
  double frac_part = modf(number, &int_part);

  auto n_valid_digits = digits_.size();

  str_converted_number = "";
  int int_part_digits_counter = 0;
  do {
    if (++int_part_digits_counter > kMaxIntPartLength)
      return false;
    int digit = static_cast<int>(fmod(int_part, base_to));
    if (static_cast<size_t>(digit) >= n_valid_digits) 
      return false;
    str_converted_number = digits_[digit] + str_converted_number;
    int_part = floor(int_part / base_to);
  } while (int_part > 0);

  if (frac_part > 0) {
    str_converted_number += '.';
    int frac_part_digits_counter = 0;
    while (frac_part_digits_counter < precision && frac_part > 0) {
      if (++frac_part_digits_counter > kMaxFracPartLength) 
        return false;
      frac_part *= base_to;
      int digit = static_cast<int>(frac_part);
      if (static_cast<size_t>(digit) >= n_valid_digits) 
        return false;
      str_converted_number += digits_[digit];
      frac_part -= digit;
    }
  }

  if (is_negative) {
    str_converted_number = "-" + str_converted_number;
  }

  return true;
}
#pragma endregion

#pragma region Setters & Getters
void NumberConverter::set_digits(std::string_view digits) {
  std::string distinct_digits = "";
  distinct_digits.reserve(digits.size());

  std::unordered_set<char> seen;  // Provides more efficient search than string.
  for (char digit : digits) {
    digit = static_cast<char>(toupper(digit));
    if (!seen.contains(digit)) {
      distinct_digits += digit;
      seen.insert(digit);
    }
  }
  digits_ = distinct_digits;
}
std::string NumberConverter::digits() const { return digits_; }
#pragma endregion

}  // namespace MathLib
#ifndef MATH_LIB_NUMBER_CONVERTER_H
#define MATH_LIB_NUMBER_CONVERTER_H

#include <string>

namespace MathLib {

class NumberConverter {
 public:
#pragma region Defaults
  static const char* kDefaultDigits;
  static const int kDefaultPrecision;
#pragma endregion

#pragma region Limits
  static const int kMinBase;
  static const int kMaxIntPartLength;
  static const int kMaxFracPartLength;
#pragma endregion

#pragma region Conversion
  bool Convert(std::string_view str_number, int base_from,
               std::string& str_converted_number, int base_to,
               int precision = kDefaultPrecision) const;

  bool ConvertTo10(std::string_view str_number, int base_from,
                   long double& converted_number) const;

  bool ConvertFrom10(long double number, std::string& str_converted_number,
                     int base_to, int precision = kDefaultPrecision) const;
#pragma endregion

#pragma region Setters & Getters
  void set_digits(std::string_view digits);
  std::string digits() const;
#pragma endregion

 private:
  std::string digits_ = kDefaultDigits;
};

}  // namespace MathLib

#endif  // MATH_LIB_NUMBER_CONVERTER_H
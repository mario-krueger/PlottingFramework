// Plotting Framework
//
// Copyright (C) 2019-2020  Mario Krüger
// Contact: mario.kruger@cern.ch
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef PlottingFrameworkHelpers_h
#define PlottingFrameworkHelpers_h

#include "PlottingFramework.h"

namespace PlottingFramework {

inline constexpr bool str_contains(const std::string &str1, const std::string &str2)
{
  return (str1.find(str2) != string::npos);
}

template<typename T>
struct is_vector : public std::false_type {};

template<typename T, typename A>
struct is_vector<std::vector<T, A>> : public std::true_type {};

template<typename T>
string vector_to_string(vector<T> numbers)
{
  string numberString;
  for(auto& number : numbers)
  {
    numberString += std::to_string(number);
    if(&number != &numbers.back()) numberString += ",";
  }
  return numberString;
}

template<typename T>
vector<T> string_to_vector(string numberString)
{
  // savety in case user put some blank spaces between numbers
  std::remove_if(numberString.begin(), numberString.end(), ::isspace);
  vector<T> numbers;
  string curNumStr;
  std::istringstream stream(numberString);
  while(std::getline(stream, curNumStr, ',')) {
    numbers.push_back(std::stoi(curNumStr));
  }
  return numbers;
}

template<typename T>
void put_in_tree_optional(ptree& tree, const optional<T>& var, const string& lable)
{
   if constexpr (is_vector<T>{}) // vectors are stored as comma separated strings
   {
     if(var) tree.put(lable, vector_to_string(*var));
   }
   else
   {
     if(var) tree.put(lable, *var);
   }
}

template<typename T>
void read_from_tree_optional(const ptree& tree, optional<T>& var, const string& lable)
{
  if constexpr (is_vector<T>{}) // vectors are stored as comma separated strings
  {
    if(auto tmp = tree.get_optional<string>(lable)) var = string_to_vector<typename T::value_type>(*tmp);
  }
  else if constexpr (std::is_enum<T>::value) // in case using enum types of the framework
  {
    if(auto tmp = tree.get_optional<typename std::underlying_type<T>::type>(lable)) var = static_cast<T>(*tmp);
  }
  else
  {
    if(auto tmp = tree.get_optional<T>(lable)) var = *tmp;
  }
}

} // end namespace PlottingFramework
#endif /* PlottingFrameworkHelpers_h */
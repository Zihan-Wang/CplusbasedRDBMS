//
//  Value.hpp
//  RGAssignment5
//
//  Created by rick gessner on 4/26/20.
//  Copyright © 2020 rick gessner. All rights reserved.
//

#ifndef Value_h
#define Value_h

#include <variant>
#include <optional>
#include <string>
#include <map>
#include <cstdint>
#include <variant>
#include "Errors.hpp"

namespace ECE141 {
  using ValueType = std::variant<uint32_t, float, bool, std::string>;

  using KeyValues = std::map<const std::string, ValueType>;

}

#endif /* Value_h */
